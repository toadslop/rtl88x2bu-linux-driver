# AGENTS.md

## Cursor Cloud specific instructions

This repo is an **out-of-tree Linux kernel module** (`88x2bu`, the RTL88x2BU USB
Wi-Fi driver) mid-way through a C→Rust migration. There is no web/app server: the
"application" is the `88x2bu.ko` module. The canonical dev workflow, gates, and
gotchas already live in `docs/rust-migration/` (especially
[`dev-environment.md`](docs/rust-migration/dev-environment.md),
[`test-plan.md`](docs/rust-migration/test-plan.md)) and
[`docs/smoke-test.md`](docs/smoke-test.md). Read those first; notes below only
capture what is specific to the pre-provisioned cloud VM.

### What is already provisioned (baked into the snapshot)

- Toolchain: `clang-18` + `lld`, with **unsuffixed** `ld.lld`/`llvm-*` symlinks in
  `/usr/bin` (kernel `LLVM=1` requires unsuffixed names). `rustc`/`cargo` 1.83 with
  the `rust-src` component, and `bindgen` 0.65.1 on `PATH`.
- A **pinned, pre-built Rust-enabled kernel tree at `/opt/linux`** (stable
  `v6.12.9`, `CONFIG_RUST=y`, built with `LLVM=1`). Use it as `KDIR`. Do not rebuild
  it unless you intentionally bump the pin — a full rebuild takes several minutes.

### Building the module (L0 gate)

Run from the repo root. `LIBCLANG_PATH` must point at the clang-18 libs or bindgen
fails:

```bash
export LIBCLANG_PATH=/usr/lib/llvm-18/lib
make clean
make KDIR=/opt/linux LLVM=1 -j"$(nproc)"
```

Because the pinned kernel has `CONFIG_RUST=y`, the build links the Rust object
(`RUSTC [M] rust/kbuild_stub.o`) into `88x2bu.ko`. Confirm with
`nm 88x2bu.ko | grep rtw_rust_kbuild_probe`.

### Running / loading the module (L3 gate)

- **Do not `insmod` on the cloud host kernel.** The host runs a different kernel
  (`uname -r` ~ `6.12.94+`) with no matching build tree, and this container cannot
  load modules; vermagic will not match `/opt/linux` either.
- Load/unload in **QEMU (TCG, no `/dev/kvm`)** booting the same `/opt/linux`
  `bzImage` with a busybox initramfs. The copy-paste recipe is in
  `docs/rust-migration/dev-environment.md` ("Reliable fallback: busybox initramfs +
  QEMU (TCG)"). A clean pass shows `registered new interface driver rtl88x2bu` on
  `insmod` and `deregistering interface driver rtl88x2bu` on `rmmod`, with no
  Oops/WARN.

### Gates not yet wired up

- **L1** (`docs/rust-migration/scripts/check-symbols.sh`) and **L2** (host crypto
  harness under `tests/`) are referenced by the plan but **do not exist in the tree
  yet** — there is no `tests/` dir and no CI workflow. Don't assume `make`
  test/lint targets exist.
- **L4** (hardware STA smoke, `docs/smoke-test.md`) needs a real USB RTL8822BU
  dongle and cannot run in this VM.
