# Dev environment notes (Wave 0 lessons)

Practical gotchas from bringing up **L0** (`make KDIR=… LLVM=1`) and **L3** (VM `insmod`/`rmmod`) for this out-of-tree driver. Read this before spending a long time rediscovering the same failures.

Normative build contract: [`../rust-migration.md`](../rust-migration.md). Gates: [`test-plan.md`](test-plan.md).

## What is *not* enough

| Tempting shortcut | Why it fails |
|-------------------|--------------|
| Distro `linux-headers-$(uname -r)` | Almost never ships `CONFIG_RUST=y` or Rust metadata (`scripts/target.json`, `rust/*.rmeta`). |
| `make KSRC=…` alone for migration | Pre-W0-02 habit. Migration builds must use **`KDIR`** + **`LLVM=1`**. |
| Building the `.ko` against kernel A, `insmod` on host kernel B | Module vermagic / CRCs will not match. L3 needs the **same** kernel image that built the module. |
| `make … \| tee log; echo $?` | `$?` is often **tee’s** exit (0). Use `echo EXIT:${PIPESTATUS[0]}` or avoid the pipe when checking success. |

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
# Expect: RUSTC [M] .../rust/kbuild_stub.o .../rust/scaffold.o .../rust/bindings.o
# then  LD [M] .../88x2bu.ko
nm 88x2bu.ko | grep -E 'rtw_rust_kbuild_probe|rtw_rust_scaffold_init|rtw_rust_bindings_probe'
```

Regenerate allowlisted crypto FFI (after editing `rust/bindings/bindgen_helper.h` or the script allowlist):

```bash
export LIBCLANG_PATH=/usr/lib/llvm-18/lib
KDIR=/path/to/linux ./scripts/bindgen_rtw.sh
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

## When to extend this doc

Add a short bullet when a new Wave hits a **recurring** environment failure (bindgen skew, RfL API break on kernel bump, CI image gap). Keep recipes copy-pasteable; link issue IDs once GitHub Issues are enabled.
