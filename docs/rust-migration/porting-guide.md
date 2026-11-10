# C → Rust porting guide (FFI rules and known pitfalls)

Normative for every translation PR, alongside [`architecture.md`](architecture.md)
(layering, domain types) and [`test-plan.md`](test-plan.md) (L0–L4 gates).

Sources:

- Mainmatter, [*The C to Rust Migration Book*](https://mainmatter.com/c-to-rust-migration-book/)
  (course: [mainmatter/migrating-c-to-rust](https://github.com/mainmatter/migrating-c-to-rust)).
  Section headings below follow its chapters.
- Kernel [Rust coding guidelines](https://docs.kernel.org/rust/coding-guidelines.html).
- This repo's 2026-11 port review (epic [E14](issues/epic-14-port-regression-fixes.md)).
  Each rule cites the bug it would have prevented (`FX-NN`).

## TL;DR — the checks reviewers will ask about

1. **Do not retype C facts.** Take constants, enum values, struct offsets and
   prototypes from C: bindgen, or `offsetof`/`sizeof` exported from C.
2. **Each `#[cfg]` must match the C macro.** Derive it from what C actually sees,
   not from a Makefile variable or an `autoconf.h` grep.
3. **Where C wrapped, write `wrapping_*`.** With `CONFIG_RUST_OVERFLOW_CHECKS=y`,
   plain `+`, `-`, `<<` and indexing panic in the kernel.
4. **Copy C's NULL rules exactly.** Don't add a NULL check C didn't have, and don't
   drop one it did.
5. **Test the shipped code against the original C.** The oracle is
   `git show <port>^:<file>.c`, and the code under test is the kernel Rust function.
6. **Keep observable behaviour, including logs.** Format strings, counters, errno
   values and `rtw_warn_on` are part of the contract.

---

## 1. Foreign function interfaces — a binding is a promise nobody checks

The book's point: an `extern "C"` block is a claim about the other side's ABI.
Neither `rustc` nor `clang` sees both sides, so a wrong argument width, return
type or variadic flag compiles and links cleanly.

Rules for this repo:

- Declare each import **exactly** as its C definition. Use `core::ffi::c_ulong` for
  `systime` / `unsigned long`, `u32` for `u32` (not `u8` "because the value is
  small"), `bool` for C `bool`, and the right return type even when it is ignored.
- **Narrow integer arguments are not portable.** AArch64 leaves the upper bits of a
  `u8`/`u16` argument unspecified, so passing `u8` where C takes `u32` or an enum
  works on x86-64 and breaks on arm64 (FX-17: `rtw_rust_hal_com_warn_c2h_short_len`,
  `MGN_RATE_STR`).
- **Declare a symbol once.** When several crates import it, put the declaration in
  one shared `#[path]` include. The review found six symbols declared
  inconsistently, e.g. `rtw_set_rpwm` returning `u8` in one file and `()` in
  another, and `rtw_malloc(u32)` vs `rtw_malloc(usize)` (FX-17).
- **Macros are not symbols.** `rtw_malloc`, `rtw_warn_on` and friends are C
  macros: call the real function (`_rtw_malloc`) or add a C shim (FX-09).
- **`systime` is `unsigned long`.** A shim that returns `u32` breaks
  `rtw_get_passing_time_ms` once jiffies passes 2^32 (about 5 minutes after boot on
  64-bit), and the result is wrong on HZ=300 kernels (FX-01, #1594). Add
  `_Static_assert(sizeof(shim()) == sizeof(systime))` next to time shims.

## 2. Types and data across the boundary — generate, don't transcribe

The book recommends `bindgen` for C→Rust (and a header generator for Rust→C) so
layouts and signatures cannot drift.

- **No literal struct offsets.** `_ADAPTER` contains spinlocks, timers, mutexes
  and tasklets whose size depends on kernel config (`CONFIG_DEBUG_SPINLOCK`,
  lockdep, `PREEMPT_RT`) and on driver options (`CONFIG_RTW_80211K`, multi-AP,
  …). Export `const size_t rtw_rust_off_* = offsetof(...)` from C, as
  `core/rtw_wapi_sms4_frame_rust_acc.c` does, or read the field through a C
  accessor (FX-05).
- **No copied constants or enum values.** `PS_MODE_NUM` was retyped as 9; the C
  enum gives 11, so two valid power modes are rejected (FX-02). Take the value from
  bindgen or a C export, and never copy it into a host test header.
- Byte order: read `__le16` fields with `u16::from_le_bytes`, not through a native
  `*const u16`.

## 3. FFI-safe types — make the C contract visible in the signature

- Use `Option<NonNull<T>>` / references **only where C really forbids NULL**.
  Where NULL means something, model it. In this driver `RTW_DBGDUMP` is `0`, and a
  NULL `sel` means "print to the kernel log". Rejecting it silenced
  `dump_adapters_status` on every anomaly path (FX-03).
- **Never offset a pointer before checking it.** `ptr.add(n)` on NULL is UB, and
  the result is no longer NULL, so a later null check passes. Copy C's
  `if (mac) memcpy(mac + 2, …)` as `if !mac.is_null() { … mac.add(2) … }`
  (FX-12).
- Wrap bare integers with meaning (flags, modes, rate codes) in newtypes or enums
  at the boundary, with an explicit policy for unknown bits and values
  ([`architecture.md`](architecture.md) "Domain typing rules").

## 4. FFI validation — every `extern "C"` fn is a trust boundary

- Validate and narrow inputs **once**, at the boundary, then hand typed values
  inward.
- **Do not change the C contract while validating.** Rejecting input C accepted
  is a behaviour change: `%u` where C parsed `%d` rejects the documented `-1`
  ("auto") for the AMPDU procs (FX-13). So is returning `-EFAULT` where C returned
  `count`. Either way, mark it as a spec change.
- **No panics reachable from C.** The kernel Rust guidelines say panicking "should
  be very rare and used only with a good reason", and
  [`deprecated.rst`](https://docs.kernel.org/process/deprecated.html#bug-and-bug-on)
  says to use `WARN()` rather than `BUG()`. With
  `CONFIG_RUST_OVERFLOW_CHECKS=y` the following become a kernel BUG:
  - `a + b` / `a - b` on C-supplied integers. Use `wrapping_*` where C relied on
    modular arithmetic (jiffies, byte counters, toggles) and `checked_*` where
    overflow is a real error (FX-11).
  - `x << n` with `n >= bits` (FX-15).
  - `slice[i]` / `&slice[a..b]` with C-supplied indices. Use `get()` and return
    the C error.
  - `/` or `%` by a C-supplied divisor.
  Check with `llvm-objdump -dr rust/<file>.o | grep panic` (T23 automates this).
- Keep **observable diagnostics**: dropping `RTW_INFO` / `RTW_WARN` /
  `rtw_warn_on` or a debug counter is a regression (FX-14, FX-15). Route the
  format string through a C shim so `%p` hashing and `RTW_PRINT_SEL` prefixes stay
  identical.

## 5. Error handling and API design — map errors once, keep the surface small

- `_SUCCESS` / `_FAIL`, errno values and `bool`-as-`u8` returns are ABI; see
  [`architecture.md`](architecture.md) "Status codes are ABI".
- Convert Rust errors to the C code at the outermost `extern "C"` fn, in one
  place. Don't spread `return -EINVAL` through the logic.
- Prefer coarse C shims (one call per operation) over dozens of per-field
  getters and setters. Each extra shim is one more hand-written prototype that
  can drift.

## 6. Production-quality FFI — build integration and automated checks

- **cfg parity.** C sees `ccflags-y`, `include/autoconf.h`, `drv_conf.h`,
  `hal_ic_cfg.h` and kernel `CONFIG_*`. Each Rust `#[cfg(config_foo)]` must be
  enabled under exactly the same conditions. Known mismatches:
  - `--cfg` from a Makefile variable while C gets `-D` from another path
    (`AP_WOWLAN` forces `AP_MODE`).
  - Flags never passed at all (USB interrupt-in pipe, prealloc skb).
  - cfgs passed with no C counterpart (`config_lps_pg`, `config_rf_shadow_rw`).
  All found in FX-07; T21 replaces hand derivation with a generated list.
- **Link safety.** Whenever a Rust object can be linked, its C shims must be
  defined. Gate the object or the export on the same macro as the shim
  (FX-07 row 9, FX-08). Never change the C config so the Rust code links (FX-06).
- **Fallbacks.** When you delete a C body, either keep a
  `#if !defined(CONFIG_RUST) || …` fallback or record that the TU is Rust-only
  (T22 decides the policy). Don't leave headers declaring symbols that are
  `static` in the fallback (FX-08).
- **Document `unsafe`.** Write `// SAFETY:` on every `unsafe` block and
  `/// # Safety` on every `unsafe fn`, naming the C invariant relied on (kernel
  guideline; clippy `undocumented_unsafe_blocks` in T20).

## Testing the port (what L2 must actually prove)

- **Oracle = the original C.** Build it from `git show <port>^:<file>.c`; don't
  rewrite it from memory, and don't share constants between oracle and Rust.
- **Code under test = the kernel function.** Host builds stub the C shims; they
  don't swap in a `#[cfg(host_*)]` rewrite of the logic (T24).
- **Time and counters at the edges.** Fake clocks are `unsigned long` starting
  just below 2^32 and are checked at HZ=100/250/300/1000. Counters are tested at
  `MAX`.
- **Configs.** Build the object with each feature combination its cfgs mention
  (T22).

## Reviewer checklist (copy into review comments as needed)

- [ ] Every new `extern "C"` import matches the C definition (width, sign, return, `systime`)
- [ ] No literal offsets or copied constants; values come from C or bindgen
- [ ] Every new `#[cfg]` maps to the same C macro condition; the object links in all configs the cfg allows
- [ ] No `+ - << [] /` on C-supplied values without `wrapping_*`/`checked_*`/`get()` (objdump shows no new `panic_*`)
- [ ] NULL handling identical to C; no pointer arithmetic before the null check
- [ ] Log lines, counters, errno and `rtw_warn_on` preserved (or called out as spec change)
- [ ] L2 oracle is `commit^` C and exercises the kernel Rust function
