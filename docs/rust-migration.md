# Rust migration (88x2bu)

Incremental port of this out-of-tree `88x2bu` USB driver from C to Rust-for-Linux (RfL). Work is tracked under [`docs/rust-migration/issues/`](rust-migration/issues/README.md) until GitHub Issues are enabled.

Normative companions:

- [`rust-migration/architecture.md`](rust-migration/architecture.md) — layers, domain types, unsafe at edges
- [`rust-migration/test-plan.md`](rust-migration/test-plan.md) — L0–L4 gates (offline-first)
- [`rust-migration/dev-environment.md`](rust-migration/dev-environment.md) — toolchain, pinned kernel, L0/L3 gotchas (Wave 0 lessons)
- [`smoke-test.md`](smoke-test.md) — L4 hardware STA checklist only

## Phases

### Phase 1 — Behavior parity (exact translation)

Port C → Rust in ~200 LOC PRs. Observable behavior must match the C oracle (characterization tests). New Rust APIs use validated **domain types**; raw `extern "C"` / `#[repr(C)]` stays only at the mixed-C ABI edge.

Exit (USB 8822B default config): no remaining `.c` objects in the module link; STA works on supported USB IDs.

### Phase 2 — Idiomatic Rust

After Phase 1 is green on hardware: RfL safe abstractions, tighter ownership, shrink `unsafe` / `extern "C"` between Rust-only units. Behavior changes only with deliberate test updates.

## Working rules

1. **Characterize → freeze tests → port** — do not port first and “add tests later.”
2. **~200 LOC per PR** — one coherent chunk; do not bundle unrelated files.
3. **Default merge gates are L0–L2** (build, symbols, host/unit parity). L3 for init/USB registration; L4 only at wave milestones. See [test-plan.md](rust-migration/test-plan.md).
4. **Architecture checklist** on every translation PR — domain types at Rust APIs; `unsafe` only in abi/os shims. See [architecture.md](rust-migration/architecture.md).

## ABI rules (mixed C/Rust era)

| Rule | Detail |
|------|--------|
| Preserve symbols | Global `extern "C"` names that C still calls must keep the same binding (`T`/`R`/`D` as documented per chunk). |
| `#[repr(C)]` | Structs crossing FFI are bindgen-backed or explicitly `#[repr(C)]`. |
| Thin shims | Rust services use domain types; convert to/from raw at the ABI edge only. |
| No duplicate defs | Never link the full original C TU **and** a Rust object that defines the same symbols. Extract/remove definitions from C first (see architecture “Multi-part file ports”). |
| Shrink over time | When the last C caller of a symbol goes away, prefer dropping the shim and calling the typed API. |

## Build: pinned Rust-enabled kernel

Distro headers without `CONFIG_RUST=y` are **not** sufficient for migration builds.

Pin a kernel tree built with Rust support, then:

```bash
make clean
make KDIR=/path/to/rust-enabled-kernel LLVM=1 -j"$(nproc)"
```

Notes:

- **`KDIR`** — path to the Rust-enabled kernel build tree (RfL out-of-tree pattern). When set, it overrides the platform `KSRC` default (`/lib/modules/$(uname -r)/build` on `CONFIG_PLATFORM_I386_PC`).
- **`LLVM=1`** — forwarded to the kernel make; required for the Clang/LLVM toolchain path used with RfL out-of-tree modules. When set, the Makefile adds Clang-quieting `ccflags-y` (e.g. `-Wno-missing-prototypes`, including Clang-only forms like `-Wno-frame-larger-than=`) so the C tree builds under Clang without Wave-0 mass churn. Default GCC builds omit that block.
- **`.rs` objects** — linked into `88x2bu.ko` only when the target kernel has `CONFIG_RUST=y` (see `rust/kbuild_stub.rs`, `rust/scaffold.rs`). Distro headers without Rust keep a C-only link (unchanged object list). `rtw_drv_entry` calls `rtw_rust_scaffold_init()` once when Rust is enabled.
- **C-only / legacy:** `make` and `make KSRC=...` still work as before when `KDIR` is unset.
- **Product config** for Phase 1 exit remains default `CONFIG_RTL8822B=y` + `CONFIG_USB_HCI=y` (module name `88x2bu`).

First-time L0/L3 setup (packages, `ld.lld` symlinks, bindgen `--locked`, QEMU without KVM): see [`rust-migration/dev-environment.md`](rust-migration/dev-environment.md).

## In-tree rtw88 blacklist

On Linux 5.18+, some distros ship experimental in-tree **rtw88** USB support (including 8822BU). That stack loads with higher priority than this out-of-tree driver. For L4 hardware smoke (and any bring-up of `88x2bu`), blacklist the conflicting module if it is present:

```bash
echo "blacklist rtw88_8822bu" | sudo tee /etc/modprobe.d/rtw8822bu.conf
# reboot, or unload rtw88_* and load 88x2bu after
```

Check with `lsmod`: `rtw88_*` means in-tree; `88x2bu` means this driver. Full STA steps: [`smoke-test.md`](smoke-test.md).

## Wave map (Phase 1)

| Wave | Focus |
|------|--------|
| 0 | Docs, Kbuild `.rs` support, trivial scaffold |
| 1 | bindgen / FFI + crypto pilot (`aes-ctr`) |
| 2 | Leaf/pure units (remaining crypto) |
| 3–6 | core → HAL → os_dep → `module!` (children filed later) |

## PR verification snippet

Copy into translation PRs. Source of truth: [test-plan.md § Per-PR checklist](rust-migration/test-plan.md#per-pr-checklist-copy-into-pr-template) (keep this block identical when editing either side):

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
