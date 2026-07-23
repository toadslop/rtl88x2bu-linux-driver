# Rust migration test plan (no hardware required)

Most incremental PRs will not be runnable on a real RTL8822BU dongle. This plan defines **offline gates** that must pass before merge, plus rarer optional hardware milestones.

## Existing project coverage

**There is no automated regression test suite to reuse.** This tree has no `tests/`, no `.github/workflows`, no KUnit/selftests, and no host unit tests.

What exists instead (not a drop-in CI suite):

- Ad-hoc bring-up scripts: `runwpa`, `wlan0dhcp`, `clean` (need a live interface)
- **MP / manufacturing mode** (`CONFIG_MP_INCLUDED`, `core/rtw_mp.c`, `os_dep/linux/ioctl_mp.c`) — factory RF/ioctl tooling on real hardware, not vectorized offline tests
- Manual debug via `/proc/net/rtl88x2bu/` and module params

So the L0–L2 harness in this document is **new infrastructure**, not a port of an existing suite. The closest “reuse” is treating current **C crypto `.c` files as oracles** in host differential tests until each file is fully swapped to Rust.

Architecture (domain types, unsafe edges): [`architecture.md`](architecture.md).

## Principles

1. **Characterize, then port** — identify current C behavior, freeze it in Rust unit/host tests, then implement until tests pass.
2. **Parity tests first, spec tests later** — initially assertions mean “same as C”; intentional behavior changes update tests in the same PR.
3. **Every PR proves something without hardware** — at least build + symbol/ABI checks; pure code also gets host/unit tests.
4. **Hardware is a milestone, not the default CI** — use it when a whole wave lands or before Phase 1 exit.
5. **Keep tests near ~200 LOC chunks** — grow the harness in small issues too.

## Characterization workflow (per ported chunk)

```text
Identify I/O & errors  →  Capture from C oracle  →  Freeze Rust tests
        →  Port with domain types  →  Keep parity tests green
```

Minimum for a pure/leaf chunk (e.g. crypto):

1. List functions/symbols and their contracts (buffers in-place? return -1?).
2. Add vectors: known-answer plus edge cases (zero length, max key length, unaligned sizes if C allows).
3. Run vectors against **C** in the host harness (oracle still green).
4. Implement Rust (typed API + `extern "C"` shim if C callers remain).
5. Same vectors run against Rust; require byte/return identity with C.
6. Only after that, remove the C `.o` from the module link.

Do **not** port first and “add tests later.”

## Gate levels

| Level | When | What | Hardware? |
|-------|------|------|-----------|
| L0 Build | Every PR | Module (or affected objects) compile with pinned `KDIR` + `LLVM=1` | No |
| L1 Symbols | Every file/chunk swap C→Rust | Exported `extern "C"` symbol set matches the old `.o` | No |
| L2 Host diff | Pure / leaf units (crypto, IE helpers, chplan math) | Userspace tests: C reference vs Rust, fixed vectors | No |
| L3 Link/init | After Wave 0 scaffold; module-entry changes | `modprobe`/`insmod` in VM **without** device (or with dummy), check dmesg + clean `rmmod` | No dongle |
| L4 Hardware | Wave boundaries / Phase 1 exit | Scan, associate, ping (see smoke checklist) | Yes |

**Merge rule for Phase 1 translation PRs:** L0 + L1 always. L2 when the chunk is pure enough. L3 when touching init/USB registration. L4 only at agreed milestones.

## L0 — Build gate

```bash
make clean
make KDIR=/path/to/rust-enabled-kernel LLVM=1 -j"$(nproc)"
```

CI (when added) should run this on the pinned kernel tree or a cached build container. Distro headers without `CONFIG_RUST` are **not** sufficient.

## L1 — Symbol / ABI gate

When replacing `foo.c` with `foo.rs` (or moving a function group):

1. Build the **pre-change** `foo.o` (or keep artifacts from `master`).
2. Build the **post-change** Rust object.
3. Compare global text symbols:

```bash
./docs/rust-migration/scripts/check-symbols.sh path/to/old.o path/to/new.o
```

(Script added in test-harness issues.) Fail if any previously global symbol disappears or changes binding unexpectedly (`T`/`R`/`D` as documented per file).

Also required in the PR description:

- List of `extern "C"` symbols owned by this chunk
- Confirmation structs crossing FFI remain `#[repr(C)]` / bindgen-backed

## L2 — Host differential tests (leaf / crypto first)

For `core/crypto/*` and similar pure code:

1. **Keep the C file as the oracle** until the chunk is swapped (or compile C sources into a userspace test binary).
2. Build a small **host** test binary (no kernel) that:
   - Links or compiles the C implementation
   - Links the Rust translation compiled with `#[cfg]` host shims **or** calls the same algorithm via a tiny `cdylib`/`staticlib` built for userspace
3. Run known vectors (NIST / project-local fixtures) and random lengths; require **byte-identical** output and return codes.

Layout (target):

```text
tests/host/
  crypto/
    aes_ctr_vectors.json
    test_aes_ctr.c          # or .rs host harness
  README.md
```

Rules:

- Prefer **one harness per crypto family**, extended as files translate.
- Do not require kernel headers for L2.
- If a function is too entangled with kernel types, extract only the pure core for L2 and leave the thin wrapper to L0/L1.

## L3 — Module load without device

Once Wave 0 lands, periodically (and always for init/USB table changes):

1. Boot a QEMU VM (or CI runner) with the **same** Rust-enabled kernel.
2. `insmod 88x2bu.ko` with **no** matching USB device.
3. Expect: module loads (or fails only for documented reasons), scaffold/init logs appear, `rmmod` is clean (no WARN/Oops).

This does **not** prove TX/RX; it catches link errors, missing symbols, and init/exit regressions.

## L4 — Hardware milestones (infrequent)

Use [`docs/smoke-test.md`](../smoke-test.md) (added in W0-01) at:

- End of Wave 0 / 1 / 2
- After first STA-critical core batches (Wave 3)
- Phase 1 exit

Failures at L4 block the wave epic, not every tiny PR, if L0–L2 were green—but they must be investigated before opening the next wave’s children.

## Per-PR checklist (copy into PR template)

```markdown
## Verification
- [ ] Characterization: C behavior identified; vectors/tests added before or with the port
- [ ] L0: builds with pinned KDIR + LLVM=1
- [ ] L1: symbol check vs previous .o (attached or CI log)
- [ ] L2: host/unit parity tests passing (if pure chunk)
- [ ] L3: VM insmod/rmmod (if init/USB registration touched)
- [ ] L4: hardware smoke (only if this closes a wave/milestone)

## Architecture
- [ ] Domain types at Rust API (no new public raw pointers / mystery integers)
- [ ] `unsafe` confined to abi/os shim
- [ ] `extern "C"` shim only if C still calls this symbol
- [ ] Behavior change vs C? If yes, tests updated and called out as spec change
```

## What we will automate first (issues)

1. **T0** — Document this plan + PR checklist (this file).
2. **T1** — `check-symbols.sh` + Make target `rust-check-symbols`.
3. **T2** — Host crypto harness scaffolding + first vectors for `aes-ctr` (ties to W1-03).
4. **T3** — Optional GitHub Actions: L0 on a Rust-kernel container when available; always run L2 host tests on ubuntu-latest.

## Out of scope (for now)

- Full USB/Wi-Fi device emulation in QEMU
- Bit-exact compare of entire `.ko` binaries
- Relying on Miri for kernel `unsafe` (not applicable to RfL modules as shipped)
