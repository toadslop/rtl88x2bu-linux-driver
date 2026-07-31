# Dev environment notes (Wave 0 lessons)

Practical gotchas from bringing up **L0** (`make KDIR=… LLVM=1`) and **L3** (VM `insmod`/`rmmod`) for this out-of-tree driver. Read this before spending a long time rediscovering the same failures.

Normative build contract: [`../rust-migration.md`](../rust-migration.md). Gates: [`test-plan.md`](test-plan.md).

## What is *not* enough

| Tempting shortcut | Why it fails |
|-------------------|--------------|
| Distro `linux-headers-$(uname -r)` | Usually lacks `CONFIG_RUST=y` or Rust metadata (`scripts/target.json`, `rust/*.rmeta`). **Exception:** some Arch `linux-headers` packages do ship them — verify before relying on them (see [Arch Linux](#arch-linux)). |
| `make KSRC=…` alone for migration | Pre-W0-02 habit. Migration builds must use **`KDIR`**. Add **`LLVM=1`** only when the target kernel was Clang-built (pinned Ubuntu / cloud `/opt/linux`); omit it for GCC-built Arch headers. |
| `LLVM=1` against a GCC-built distro kernel | Clang inherits GCC-only `CFLAGS` and fails (`-mindirect-branch=…`, etc.). Match the kernel’s C compiler. |
| Building the `.ko` against kernel A, `insmod` on host kernel B | Module vermagic / CRCs will not match. L3 needs the **same** kernel image that built the module. |
| `make … \| tee log; echo $?` | `$?` is often **tee’s** exit (0). Use `echo EXIT:${PIPESTATUS[0]}` or avoid the pipe when checking success. |

## Arch Linux

Arch `linux` / `linux-headers` often ship with `CONFIG_RUST=y` and the `rust/` metadata needed for out-of-tree RfL modules. When that holds, you can build against the running kernel instead of pinning a separate tree.

**Do not pass `LLVM=1` against stock Arch kernels.** They are built with GCC (`CONFIG_CC_IS_GCC=y`). `LLVM=1` switches the module build to Clang, which then inherits GCC-only flags (`-mindirect-branch=…`, `-mpreferred-stack-boundary=3`, …) and fails. RfL still compiles the `.rs` objects via `rustc` under a GCC C toolchain.

### Verify headers are Rust-capable

```bash
K=/lib/modules/$(uname -r)/build
grep '^CONFIG_RUST=y' "$K/.config"   # or: grep CONFIG_RUST "$K/include/config/auto.conf"
grep '^CONFIG_CC_IS_GCC=y' "$K/include/config/auto.conf"
test -f "$K/scripts/target.json" && ls "$K/rust"/*.rmeta >/dev/null
```

If `CONFIG_RUST` / metadata checks fail, fall back to [pinning a Rust-enabled kernel](#pinning-and-building-a-rust-enabled-kernel) (that path uses `LLVM=1` because the pin is Clang-built).

### Packages, bindgen, and which rustc

```bash
sudo pacman -S --needed linux-headers base-devel bc clang lld llvm rust

K=/lib/modules/$(uname -r)/build
# Match the headers tree — do not blindly use the Ubuntu 6.12.x pin (0.65.1):
"$K/scripts/min-tool-version.sh" rustc     # e.g. 1.85.0 on 7.x
"$K/scripts/min-tool-version.sh" bindgen   # e.g. 0.71.1 on 7.x
cargo install bindgen-cli --version "$("$K/scripts/min-tool-version.sh" bindgen)" --locked

# Arch libclang lives in /usr/lib (not /usr/lib/llvm-N/lib)
export LIBCLANG_PATH=/usr/lib

# Kernel rmeta was built with pacman rustc. rustup's rustc often shares the
# same version string but a different LLVM and fails with E0514 ("incompatible
# version of rustc"). Prefer /usr/bin ahead of ~/.cargo/bin:
export PATH="/usr/bin:$PATH"
rustc --version   # must include "(Arch Linux rust …)" matching auto.conf

make -C "$K" rustavailable   # must print: Rust is available!
```

`bc` is required by this driver's Makefile (GCC version probe); without it you get `/bin/sh: bc: command not found` noise (and may miss `-Wno-date-time`).

### Build (L0) and load on the host

```bash
export PATH="/usr/bin:$PATH"
export LIBCLANG_PATH=/usr/lib
cd /path/to/rtl88x2bu-linux-driver
make clean
make KDIR=/lib/modules/$(uname -r)/build -j"$(nproc)"   # no LLVM=1
nm 88x2bu.ko | grep -E 'rtw_rust_kbuild_probe|rtw_rust_scaffold_init|aes_ctr_encrypt'
```

Because `KDIR` matches `uname -r`, `insmod` / `modprobe` on this host is valid (unlike the cloud `/opt/linux` + QEMU path, where vermagic will not match the host).

```bash
sudo insmod ./88x2bu.ko
# or: sudo make install && sudo modprobe 88x2bu
```

### Pitfalls (Arch)

| Symptom | Cause / fix |
|---------|-------------|
| `unknown argument: '-mindirect-branch=…'` / `-mpreferred-stack-boundary=3` under Clang | Dropped `LLVM=1`; use GCC to match the distro kernel. |
| `E0514: found crate core compiled by an incompatible version of rustc` | rustup `rustc` on `PATH` ahead of `/usr/bin/rustc`. Put `/usr/bin` first. |
| `/bin/sh: bc: command not found` | `sudo pacman -S bc` |

## Host packages (Ubuntu-like)

Kernel config/build (LLVM path):

```bash
sudo apt-get install -y \
  flex bison bc libelf-dev libssl-dev dwarves cpio \
  clang lld llvm \
  libclang-dev
```

Rust side (versions must satisfy the pinned kernel’s `scripts/min-tool-version.sh`; for Linux 6.12.x that was **rustc ≥ 1.78**, **bindgen 0.65.1**):

```bash
rustup component add rust-src rustfmt clippy
# Prefer --locked; unbound `cargo install bindgen-cli` can fail on newer
# crates that need edition2024 while the host Cargo is older.
cargo install bindgen-cli --version 0.65.1 --locked
export LIBCLANG_PATH=/usr/lib/llvm-$(llvm-config --version | cut -d. -f1)/lib   # adjust
make -C /path/to/linux LLVM=1 rustavailable   # must print: Rust is available!
```

### Distro LLVM is version-suffixed

Packages often provide `llvm-ar-18`, `ld.lld-18`, not bare `llvm-ar` / `ld.lld`. Kernel `LLVM=1` looks for **unsuffixed** names. If configure fails with `linker 'ld.lld' not found` or `llvm-ar: not found`:

```bash
sudo update-alternatives --install /usr/bin/ld.lld ld.lld /usr/bin/ld.lld-18 100
for f in /usr/bin/llvm-*-18; do
  base=$(basename "$f" -18)
  [ -e "/usr/bin/$base" ] || sudo ln -sf "$f" "/usr/bin/$base"
done
```

(Adjust `-18` to whatever `apt` installed.)

## Pinning and building a Rust-enabled kernel

Shallow clone + defconfig + Rust (example used in W0-02: **v6.12.9**):

```bash
git clone --depth 1 --branch v6.12.9 \
  https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git /path/to/linux
cd /path/to/linux
make LLVM=1 defconfig
./scripts/config --enable CONFIG_RUST
./scripts/config --enable CONFIG_MODULES
./scripts/config --enable CONFIG_MODULE_UNLOAD
# Driver needs these for a meaningful L0/L3 (USB wifi registration path):
./scripts/config --enable CONFIG_NET
./scripts/config --enable CONFIG_NETDEVICES
./scripts/config --enable CONFIG_USB
./scripts/config --enable CONFIG_CFG80211
./scripts/config --enable CONFIG_WLAN
# L3 helpers (busybox/qemu or virtme):
./scripts/config --enable CONFIG_DEVTMPFS
./scripts/config --enable CONFIG_DEVTMPFS_MOUNT
./scripts/config --enable CONFIG_TMPFS
./scripts/config --enable CONFIG_OVERLAY_FS
./scripts/config --enable CONFIG_VIRTIO
./scripts/config --enable CONFIG_VIRTIO_PCI
./scripts/config --enable CONFIG_VIRTIO_CONSOLE
./scripts/config --enable CONFIG_NET_9P
./scripts/config --enable CONFIG_NET_9P_VIRTIO
./scripts/config --enable CONFIG_9P_FS
make LLVM=1 olddefconfig
make LLVM=1 rustavailable
make LLVM=1 -j"$(nproc)"
```

Expect a long first build (full `defconfig`). Incremental rebuilds after `scripts/config` tweaks are much faster.

Out-of-tree driver:

```bash
cd /path/to/rtl88x2bu-linux-driver
make clean
make KDIR=/path/to/linux LLVM=1 -j"$(nproc)"
# Expect: RUSTC [M] .../rust/kbuild_stub.o .../rust/scaffold.o .../rust/ffi.o .../rust/domain_types.o .../rust/aes_ctr.o
# then  LD [M] .../88x2bu.ko
nm 88x2bu.ko | grep -E 'rtw_rust_kbuild_probe|rtw_rust_scaffold_init|rtw_rust_bindings_probe|rtw_rust_domain_types_probe|rtw_rust_aes_ctr_probe|aes_ctr_encrypt'
```

Regenerate allowlisted crypto FFI (after editing `rust/bindings/bindgen_helper.h` or the script allowlist):

```bash
export LIBCLANG_PATH=/usr/lib/llvm-18/lib
KDIR=/path/to/linux ./scripts/bindgen_rtw.sh
# CI (module-l0.yml) runs ./scripts/bindgen_rtw.sh --check before L0 build
```

L3 should show a dmesg breadcrumb from C init after the Rust call, e.g. `rust scaffold: rtw_rust_scaffold_init ret=0` (W0-03).

### Clang vs this C tree

Modern kernels compile external modules with Clang `-Werror` on several diagnostics. This Realtek C tree trips **missing prototypes** and **implicit fallthrough** immediately. The Makefile adds `-Wno-*` for those **only when `LLVM=1`** (do not make them unconditional — Clang accepts `-Wno-frame-larger-than=` which GCC rejects). If a new Clang warning blocks L0, prefer a narrow flag inside that `ifeq ($(LLVM),1)` block over mass C edits unless the warning is a real bug.

Under `LLVM=1`, platform `-mhard-float` / `-mfloat-abi=hard` is removed via `ccflags-remove-y` — those flags fight the kernel code model.

## L3 — VM `insmod`/`rmmod` (no USB)

Goal: load `88x2bu.ko` built for the pinned kernel, then `rmmod`, with **no** adapter. Pass = clean init/exit, no Oops/WARN.

### virtme-ng pitfalls (W0-02)

`virtme-ng` / `virtme-run` are convenient when they work; they failed in constrained agents for several reasons:

1. **Needs a real PTY** — `ERROR: not a valid pts, try to run vng inside tmux or screen`. Run inside `tmux`/`screen`, not a raw redirected pipe.
2. **`--script-sh` needs virtio-serial** — without working script I/O: `cannot find script I/O ports; make sure virtio-serial is available`, then power-off with **no** script run. Enable `CONFIG_VIRTIO_CONSOLE` (and friends above) and prefer `--show-boot-console` while debugging.
3. **Overlay mounts** — guest init may need `CONFIG_OVERLAY_FS`; otherwise `/etc` overlays fail and the guest is half-broken.
4. **No `/dev/kvm`** — `-enable-kvm` dies with `Could not access KVM kernel module`. Use TCG (`qemu-system-x86_64` **without** `-enable-kvm`) or virtme `--disable-kvm`.

### Reliable fallback: busybox initramfs + QEMU (TCG)

This path passed W0-02 L3 without KVM or virtme script ports.

```bash
# One-time: static busybox
sudo apt-get install -y qemu-system-x86 busybox-static

WORKDIR=/tmp/l3-initrd
rm -rf "$WORKDIR" && mkdir -p "$WORKDIR"/{bin,dev,proc,sys,workspace}
cp "$(command -v busybox)" "$WORKDIR/bin/busybox"   # prefer static
for a in sh ls insmod rmmod dmesg uname sleep poweroff reboot mount cat echo; do
  ln -sf busybox "$WORKDIR/bin/$a"
done
cp /path/to/rtl88x2bu-linux-driver/88x2bu.ko "$WORKDIR/workspace/"

cat > "$WORKDIR/init" <<'EOF'
#!/bin/sh
export PATH=/bin
mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs none /dev 2>/dev/null || true
echo "=== L3 start ==="
uname -r
insmod /workspace/88x2bu.ko || { echo "=== L3_FAIL insmod ==="; dmesg | tail -80; poweroff -f; }
echo "insmod ok"
dmesg | tail -40
rmmod 88x2bu || { echo "=== L3_FAIL rmmod ==="; dmesg | tail -80; poweroff -f; }
echo "rmmod ok"
dmesg | tail -30
echo "=== L3_PASS ==="
poweroff -f
EOF
chmod +x "$WORKDIR/init"
(cd "$WORKDIR" && find . | cpio -o -H newc) > /tmp/l3-initrd.cpio

timeout 120 qemu-system-x86_64 -cpu qemu64 -m 1G -nographic \
  -kernel /path/to/linux/arch/x86/boot/bzImage \
  -initrd /tmp/l3-initrd.cpio \
  -append 'console=ttyS0 earlyprintk=serial,ttyS0 ignore_loglevel rdinit=/init'
# Expect serial log: module init ret=0, module exit success, L3_PASS
```

## Checklist before asking “why won’t it build?”

1. `make -C "$KDIR" LLVM=1 rustavailable` → **Rust is available!**
2. Unsuffixed `ld.lld`, `llvm-ar`, `clang` on `PATH`.
3. `bindgen --version` ≈ 0.65.1; `rustc` + `rust-src` match kernel mins.
4. Driver: `make KDIR="$KDIR" LLVM=1` (not only `KSRC`).
5. L3: same `bzImage` as `$KDIR`; do not `insmod` on the cloud/host kernel unless vermagic matches.
6. Agent/CI without KVM: use the busybox+QEMU recipe, not virtme `--script-sh`, until virtio-serial is proven.

## CI L0 image

GitHub Actions L0 builds use a pre-built container with the pinned kernel at `/opt/linux` and the same toolchain as above (T6, issue #151).

| Item | Value |
|------|-------|
| Kernel pin | `v6.12.9` (`CONFIG_RUST=y`, `LLVM=1`) |
| Image tags | `ghcr.io/<owner>/rtl88x2bu-l0:v6.12.9`, `:latest` |
| Publish workflow | [`.github/workflows/publish-l0-image.yml`](../../.github/workflows/publish-l0-image.yml) |
| Dockerfile | [`.github/docker/l0/Dockerfile`](../../.github/docker/l0/Dockerfile) |

### Build locally

From the repo root (expect a long first build — full kernel compile):

```bash
docker build -f .github/docker/l0/Dockerfile -t rtl88x2bu-l0:v6.12.9 .
```

Smoke the driver inside the image:

```bash
docker run --rm -v "$PWD:/driver" -w /driver rtl88x2bu-l0:v6.12.9 \
  bash -c 'make clean && make KDIR=/opt/linux LLVM=1 -j"$(nproc)"'
```

### Pull from ghcr.io

After the publish workflow runs on `master`:

```bash
docker pull ghcr.io/<owner>/rtl88x2bu-l0:v6.12.9
```

Replace `<owner>` with the GitHub org or user that owns the repo (e.g. `toadslop`).

**Package visibility:** images pushed via `GITHUB_TOKEN` default to **private** in GitHub Packages. Same-repo Actions jobs can pull them with `packages: read`, but fork PRs and external consumers cannot. After the first publish, set the `rtl88x2bu-l0` package to **public** in GitHub Packages settings (or document an inline image-build fallback for fork PRs in T6b). CI jobs should pin `ghcr.io/<owner>/rtl88x2bu-l0:v6.12.9` — avoid `:latest`, which moves on every rebuild.

### When to rebuild the image

Rebuild and re-publish when:

- The kernel pin changes (bump `KERNEL_TAG` in the Dockerfile, `build-pinned-kernel.sh`, and workflow env)
- Minimum `rustc` / `bindgen` versions change for that kernel
- Host packages required for `make LLVM=1` change materially

PR L0 verification uses [`.github/workflows/module-l0.yml`](../../.github/workflows/module-l0.yml) (T6b).

## CI L1 symbol checks

GitHub Actions L1 runs inside the same L0 container (T7, issue #152). No full `88x2bu.ko` rebuild — only `rust/*.o` via kbuild plus host-`gcc` C oracle objects where the Makefile defines them.

| Item | Value |
|------|-------|
| Workflow | [`.github/workflows/module-l1.yml`](../../.github/workflows/module-l1.yml) |
| Selftest | `make KDIR=/opt/linux LLVM=1 rust-check-symbols-selftest` |
| Unit aggregates | [`scripts/ci/run-l1-unit-checks.sh`](../../scripts/ci/run-l1-unit-checks.sh) |
| Diff scope | [`scripts/ci/l1-targets-from-diff.sh`](../../scripts/ci/l1-targets-from-diff.sh) |

### Run locally (inside L0 image)

```bash
docker run --rm -v "$PWD:/driver" -w /driver rtl88x2bu-l0:v6.12.9 \
  bash -c 'make KDIR=/opt/linux LLVM=1 rust-check-symbols-selftest && ./scripts/ci/run-l1-unit-checks.sh'
```

On failure, read the `check-symbols.sh` output and the per-unit allowlist under `docs/rust-migration/scripts/*.allow`. See [`test-plan.md`](test-plan.md#l1--symbol--abi-gate).

## CI L3 module load (QEMU)

GitHub Actions L3 runs inside the L0 container after a full module build (T8, issue #153). Uses busybox initramfs + QEMU (TCG) — no KVM required.

| Item | Value |
|------|-------|
| Workflow | [`.github/workflows/module-l3.yml`](../../.github/workflows/module-l3.yml) |
| Trigger | `push` to `master` (path-filtered driver/build inputs) |
| Initramfs | [`scripts/ci/build-l3-initrd.sh`](../../scripts/ci/build-l3-initrd.sh) |
| QEMU runner | [`scripts/ci/run-l3-qemu.sh`](../../scripts/ci/run-l3-qemu.sh) |
| Image deps | `qemu-system-x86`, `busybox-static` in [`.github/docker/l0/Dockerfile`](../../.github/docker/l0/Dockerfile) |

### Run locally (inside L0 image)

```bash
docker run --rm -v "$PWD:/driver" -w /driver rtl88x2bu-l0:v6.12.9 \
  bash -c 'make clean && make KDIR=/opt/linux LLVM=1 -j"$(nproc)" && ./scripts/ci/run-l3-qemu.sh --ko 88x2bu.ko'
```

A clean pass prints `run-l3-qemu: OK` and the serial log contains `=== L3_PASS ===`. See [`test-plan.md`](test-plan.md#l3--module-load-without-device).

## Branch protection

CI workflows exist, but merges stay unblocked until a **repo admin** enables branch protection on `master`. This section lists the exact check names to require (issue T9, #154).

### Settings (repo admin)

1. GitHub → **Settings** → **Branches** → **Add branch protection rule** (or edit existing rule for `master`).
2. Enable **Require a pull request before merging**.
3. Enable **Require status checks to pass before merging**.
4. Search and select these checks (workflow name / job id):

| Check name | When it runs |
|------------|--------------|
| `Host L2 tests / host-l2` | PRs touching `rust/**`, `core/**`, `tests/host/**`, etc. |
| `Module L0 build / module-l0` | PRs touching driver/build inputs (`core/`, `hal/`, `Makefile`, …) |
| `Module L1 symbols / module-l1` | PRs touching `rust/**`, `Makefile`, symbol scripts |

5. **Do not require** `Module L3 load / module-l3` on pull requests — that workflow runs on `push` to `master` only (post-merge health gate).
6. **Do not require** `Publish L0 CI image / build-and-push` — it runs only when Docker or kernel-pin files change.
7. **CodeQL** (if enabled via GitHub Advanced Security) is optional; it is not an in-repo required workflow.

Optional team preferences: dismiss stale pull request approvals, require linear history.

### Path-scoped PR checks (T10)

On **pull requests**, L0/L1/L2 workflows always trigger (no workflow-level `paths:` on `pull_request`). Each job uses [`dorny/paths-filter`](https://github.com/dorny/paths-filter) to decide whether to run the full gate or a no-op skip step. Out-of-scope PRs (e.g. docs-only) report **Success** on all three required checks instead of staying **Waiting for status to be reported** ([GitHub troubleshooting docs](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/collaborating-on-repositories-with-code-quality-features/troubleshooting-required-status-checks)).

**Push to `master`** still uses workflow-level `paths:` filters so post-merge CI skips unrelated commits.

Admins can require `Host L2 tests / host-l2`, `Module L0 build / module-l0`, and `Module L1 symbols / module-l1` on pull requests without an admin bypass for docs-only PRs.

### Fork pull requests

Fork PRs may fail to pull the published L0 image from ghcr.io; workflows fall back to building the image inline (see [CI L0 image](#ci-l0-image) above). Ensure the `rtl88x2bu-l0` package is **public** in GitHub Packages so fork CI can pull it without rebuilding.

Contributor overview: [`docs/contributing.md`](../contributing.md).

## When to extend this doc

Add a short bullet when a new Wave hits a **recurring** environment failure (bindgen skew, RfL API break on kernel bump, CI image gap). Keep recipes copy-pasteable; link relevant GitHub issue IDs (see [`issues/ISSUE-MAP.md`](issues/ISSUE-MAP.md)).
