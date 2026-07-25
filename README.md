# RTL88x2BU driver — C→Rust migration

This repository is an **incremental port** of the out-of-tree Linux kernel module `88x2bu` (Realtek RTL8812BU / RTL8822BU USB Wi-Fi) from C to [Rust for Linux](https://rust-for-linux.com/). The goal is **behavior parity first**, then idiomatic Rust once the mixed C/Rust module is complete.

The tree still builds the same `88x2bu.ko` driver, but the primary purpose here is **migration work**: small, test-gated PRs, typed domain APIs, and offline verification—not a general-purpose distro driver fork.

## What lives here

| Area | Location |
|------|----------|
| Migration plan, ABI rules, build contract | [`docs/rust-migration.md`](docs/rust-migration.md) |
| Architecture (layers, domain types) | [`docs/rust-migration/architecture.md`](docs/rust-migration/architecture.md) |
| Test gates L0–L4 | [`docs/rust-migration/test-plan.md`](docs/rust-migration/test-plan.md) |
| Toolchain, pinned kernel, QEMU L3 | [`docs/rust-migration/dev-environment.md`](docs/rust-migration/dev-environment.md) |
| Hardware STA smoke checklist | [`docs/smoke-test.md`](docs/smoke-test.md) |
| Host L2 crypto harness | [`tests/host/README.md`](tests/host/README.md) |
| Work tracker (until GitHub Issues are enabled) | [`docs/rust-migration/issues/README.md`](docs/rust-migration/issues/README.md) |
| Rust sources | [`rust/`](rust/) |

## Migration status (Phase 1)

Work proceeds in waves of ~200-line PRs. Completed so far:

- **Wave 0** — docs, Kbuild `.rs` integration, scaffold init hook
- **Wave 1** — bindgen/FFI seam, domain-type seed, `aes-ctr` pilot
- **Wave 2 (in progress)** — leaf crypto: W2-01…W2-05 landed (`aes-omac1` … `sha256-internal`); W2-06+ remaining

Remaining Phase 1 scope: core logic, HAL, `os_dep`, and eventually a Rust `module!` entry. See the wave map in [`docs/rust-migration.md`](docs/rust-migration.md).

## Building (contributors)

Migration builds require a **Rust-enabled kernel tree** (`CONFIG_RUST=y`) plus **`KDIR`** and **`LLVM=1`**.

**Arch Linux:** current `linux-headers` often already have `CONFIG_RUST=y`. Install `bindgen` to match the headers’ `scripts/min-tool-version.sh`, then:

```bash
export LIBCLANG_PATH=/usr/lib
make clean
make KDIR=/lib/modules/$(uname -r)/build LLVM=1 -j"$(nproc)"
```

**Ubuntu / pinned tree / cloud VM:** distro headers usually lack Rust metadata — use a Rust-enabled kernel source tree (or `/opt/linux` on the provisioned cloud snapshot):

```bash
export LIBCLANG_PATH=/usr/lib/llvm-18/lib   # adjust for host clang
make clean
make KDIR=/path/to/rust-enabled-kernel LLVM=1 -j"$(nproc)"
```

First-time setup (Arch and Ubuntu packages, `ld.lld` symlinks, bindgen pin, pinning a kernel, L3 QEMU): [`docs/rust-migration/dev-environment.md`](docs/rust-migration/dev-environment.md).

Migrated crypto objects are linked from `rust/` only when the target kernel has `CONFIG_RUST=y`. There is no fallback to the old C objects for those units.

## Verification gates

Every translation PR should pass the offline gates that apply to its scope:

| Gate | What |
|------|------|
| **L0** | Module builds with pinned `KDIR` + `LLVM=1` |
| **L1** | Exported `extern "C"` symbols match the replaced `.o` |
| **L2** | Host differential tests (C oracle vs Rust) for pure/leaf code |
| **L3** | `insmod` / `rmmod` in a VM (no USB dongle required) |
| **L4** | Hardware STA smoke at wave milestones |

Details and commands: [`docs/rust-migration/test-plan.md`](docs/rust-migration/test-plan.md).

```bash
# Example: L2 host crypto harness
make -C tests/host/crypto test
```

## Contributing

1. Read [`docs/rust-migration.md`](docs/rust-migration.md) and [`docs/rust-migration/architecture.md`](docs/rust-migration/architecture.md).
2. **Characterize C behavior → freeze tests → port** (do not port first and add tests later).
3. Keep PRs to one coherent chunk (~200 LOC of meaningful change).
4. Use domain types at Rust APIs; confine `unsafe` and raw pointers to ABI/OS shims.

Work items are tracked in [`docs/rust-migration/issues/`](docs/rust-migration/issues/) until GitHub Issues are enabled on this repository.

## Driver background

Underneath the migration, this is still the **RTL88x2BU** USB driver for Realtek **RTL8812BU** and **RTL8822BU** chipsets (802.11ac). It does **not** support newer 802.11ax (Wi-Fi 6) parts such as RTL8852BU.

**Current driver version:** 5.13.1-30

On Linux 5.18+, some distributions ship experimental in-tree **rtw88** USB support that can conflict with this out-of-tree module. If `lsmod` shows `rtw88_*` instead of `88x2bu`, blacklist the in-tree driver before hardware testing:

```bash
echo "blacklist rtw88_8822bu" | sudo tee /etc/modprobe.d/rtw8822bu.conf
```

Hardware bring-up steps: [`docs/smoke-test.md`](docs/smoke-test.md).

For device lists, DKMS install, USB 3.0 mode (`rtw_switch_usb_mode`), debug logging (`rtw_drv_log_level` / `/proc/net/rtl88x2bu/log_level`), and unsupported USB ID troubleshooting, see [RinCat/RTL88x2BU-Linux-Driver](https://github.com/RinCat/RTL88x2BU-Linux-Driver).

## Upstream lineage

This tree descends from community out-of-tree RTL88x2BU drivers (Realtek vendor sources, maintained forks such as [RinCat/RTL88x2BU-Linux-Driver](https://github.com/RinCat/RTL88x2BU-Linux-Driver)). This fork adds Rust-for-Linux integration, migration documentation, and offline test infrastructure on top of that base.

## License

GPL-2.0 — see [`LICENSE`](LICENSE).
