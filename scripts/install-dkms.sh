#!/usr/bin/env bash
# Install this migration tree as a DKMS module (Arch-oriented).
# Run from repo root: sudo ./scripts/install-dkms.sh
set -euo pipefail

if [[ ${EUID} -ne 0 ]]; then
  echo "Run as root: sudo $0" >&2
  exit 1
fi

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$REPO_ROOT"

if [[ ! -f dkms.conf || ! -f Makefile ]]; then
  echo "Not in driver tree (missing dkms.conf/Makefile)" >&2
  exit 1
fi

VER="5.13.1.migration.$(git rev-parse --short HEAD)"
KVER="$(uname -r)"
SRC_DIR="/usr/src/rtl88x2bu-${VER}"

echo "==> DKMS version: ${VER} (kernel ${KVER})"

# Brief Wi-Fi outage if the temporary insmod'd module is loaded.
ip link set wlan0 down 2>/dev/null || true
rmmod 88x2bu 2>/dev/null || true

# Replace AUR C-only package if present.
if pacman -Q rtl88x2bu-dkms-git &>/dev/null; then
  echo "==> Removing AUR rtl88x2bu-dkms-git"
  pacman -R --noconfirm rtl88x2bu-dkms-git
fi
dkms remove -m rtl88x2bu -v 5.13.1.r212.825556e --all 2>/dev/null || true
rm -rf /usr/src/rtl88x2bu-5.13.1.r212.825556e

# Keep in-tree rtw88 from claiming the USB stick.
mkdir -p /etc/modprobe.d
echo "blacklist rtw88_8822bu" >/etc/modprobe.d/rtw8822bu.conf

# Fresh source tree for DKMS (no build artifacts / VCS).
echo "==> Installing sources to ${SRC_DIR}"
rm -rf "${SRC_DIR}"
mkdir -p "${SRC_DIR}"
rsync -a \
  --exclude='.git/' \
  --exclude='.github/' \
  --exclude='.cursor/' \
  --exclude='*.ko' \
  --exclude='*.o' \
  --exclude='*.a' \
  --exclude='*.mod' \
  --exclude='*.mod.c' \
  --exclude='.*.cmd' \
  --exclude='.tmp_versions/' \
  --exclude='Module.symvers' \
  --exclude='modules.order' \
  --exclude='*.dwo' \
  --exclude='target/' \
  "${REPO_ROOT}/" "${SRC_DIR}/"
sed -i "s/@PKGVER@/${VER}/" "${SRC_DIR}/dkms.conf"

# Drop any prior copy of this exact version, then add/install.
dkms remove -m rtl88x2bu -v "${VER}" --all 2>/dev/null || true
dkms add -m rtl88x2bu -v "${VER}"
dkms install -m rtl88x2bu -v "${VER}" -k "${KVER}"
depmod -a "${KVER}"

modprobe 88x2bu

echo "==> DKMS status"
dkms status
echo "==> Loaded module"
lsmod | grep -E '88x2bu|rtw88' || true
modinfo 88x2bu | grep -E '^(filename|version|vermagic)'
ip -br link | grep -E 'wlan|wl' || true
echo "OK — permanently installed via DKMS (${VER})"
