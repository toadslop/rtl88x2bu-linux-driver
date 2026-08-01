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
KBUILD="/lib/modules/${KVER}/build"
SRC_DIR="/usr/src/rtl88x2bu-${VER}"

echo "==> DKMS version: ${VER} (kernel ${KVER})"

if [[ ! -d "${KBUILD}" ]]; then
  echo "Missing kernel headers for ${KVER} (expected ${KBUILD})" >&2
  echo "Install linux-headers matching your running kernel." >&2
  exit 1
fi

if [[ ! -f "${KBUILD}/.config" ]]; then
  echo "Kernel build tree lacks .config — install full linux-headers for ${KVER}." >&2
  exit 1
fi

if ! grep -q '^CONFIG_RUST=y' "${KBUILD}/.config" 2>/dev/null; then
  echo "Kernel ${KVER} lacks CONFIG_RUST=y." >&2
  echo "This migration driver links Rust objects; C-only builds omit them silently." >&2
  echo "Use Rust-enabled headers or pin a CONFIG_RUST kernel — see docs/rust-migration/dev-environment.md" >&2
  exit 1
fi

if [[ ! -f "${KBUILD}/scripts/target.json" ]]; then
  echo "Kernel headers for ${KVER} lack Rust metadata (scripts/target.json)." >&2
  echo "See docs/rust-migration/dev-environment.md#arch-linux" >&2
  exit 1
fi

# Brief Wi-Fi outage if the temporary insmod'd module is loaded.
while read -r iface _; do
  ip link set "${iface}" down 2>/dev/null || true
done < <(ip -br link 2>/dev/null | awk '/^wl/ {print $1}')
rmmod 88x2bu 2>/dev/null || true

# Replace AUR C-only package if present.
if pacman -Q rtl88x2bu-dkms-git &>/dev/null; then
  echo "==> Removing AUR rtl88x2bu-dkms-git"
  pacman -R --noconfirm rtl88x2bu-dkms-git
fi

# Remove every prior rtl88x2bu DKMS registration (AUR, old migration copies, etc.).
if dkms status rtl88x2bu &>/dev/null; then
  mapfile -t OLD_VERS < <(
    dkms status rtl88x2bu 2>/dev/null | sed -n 's/^rtl88x2bu,\([^,]*\),.*/\1/p'
  )
  for old_ver in "${OLD_VERS[@]}"; do
    echo "==> Removing prior DKMS rtl88x2bu/${old_ver}"
    dkms remove -m rtl88x2bu -v "${old_ver}" --all 2>/dev/null || true
    rm -rf "/usr/src/rtl88x2bu-${old_ver}"
  done
fi

# Keep in-tree rtw88 from claiming the USB stick.
mkdir -p /etc/modprobe.d
if [[ ! -f /etc/modprobe.d/rtw8822bu.conf ]] ||
   ! grep -q 'blacklist rtw88_8822bu' /etc/modprobe.d/rtw8822bu.conf 2>/dev/null; then
  echo "blacklist rtw88_8822bu" >>/etc/modprobe.d/rtw8822bu.conf
fi

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
# Arch: pacman rustc + libclang layout (no LLVM=1 — GCC-built kernel).
sed -i 's|@MAKE_ENV@|env PATH=/usr/bin:/bin LIBCLANG_PATH=/usr/lib |' "${SRC_DIR}/dkms.conf"

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
