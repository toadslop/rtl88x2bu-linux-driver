#!/bin/bash
# Build the pinned Rust-enabled kernel tree for L0 CI (T6).
# See docs/rust-migration/dev-environment.md#pinning-and-building-a-rust-enabled-kernel
set -euo pipefail

KERNEL_TAG="${KERNEL_TAG:-v6.12.9}"
INSTALL_DIR="${INSTALL_DIR:-/opt/linux}"

if [ -f "${INSTALL_DIR}/.config" ] && [ -f "${INSTALL_DIR}/vmlinux" ]; then
	echo "Pinned kernel already present at ${INSTALL_DIR}; skipping build."
	exit 0
fi

if [ -e "${INSTALL_DIR}" ]; then
	rm -rf "${INSTALL_DIR}"
fi

WORKDIR="$(mktemp -d)"
trap 'rm -rf "${WORKDIR}"' EXIT

echo "Cloning Linux ${KERNEL_TAG}..."
git clone --depth 1 --branch "${KERNEL_TAG}" \
	https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git \
	"${WORKDIR}/linux"

cd "${WORKDIR}/linux"

# Driver Rust uses edition 2024; upstream v6.12.9 kbuild defaults to 2021.
sed -i 's/--edition=2021/--edition=2024/' Makefile

make LLVM=1 defconfig
./scripts/config --enable CONFIG_RUST
./scripts/config --enable CONFIG_MODULES
./scripts/config --enable CONFIG_MODULE_UNLOAD
# Driver L0/L3 (USB wifi registration path):
./scripts/config --enable CONFIG_NET
./scripts/config --enable CONFIG_NETDEVICES
./scripts/config --enable CONFIG_USB
./scripts/config --enable CONFIG_CFG80211
./scripts/config --enable CONFIG_WLAN
# L3 helpers (busybox/QEMU or virtme):
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

mkdir -p "$(dirname "${INSTALL_DIR}")"
mv "${WORKDIR}/linux" "${INSTALL_DIR}"
trap - EXIT
rm -rf "${WORKDIR}"

echo "Pinned kernel installed at ${INSTALL_DIR}"
