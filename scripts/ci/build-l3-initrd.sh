#!/bin/bash
# L3 gate: assemble a busybox initramfs that insmods/rmmods 88x2bu.ko (T8 PR1).
# See docs/rust-migration/dev-environment.md#reliable-fallback-busybox-initramfs--qemu-tcg
set -euo pipefail

KO=""
OUTPUT="/tmp/l3-initrd.cpio"
WORKDIR=""
KEEP_WORKDIR=0

usage() {
	cat <<'EOF'
Usage: build-l3-initrd.sh --ko <path> [options]

Build a cpio initramfs for the L3 QEMU load/unload gate.

Options:
  --ko <path>         Path to 88x2bu.ko (required)
  --output <path>     Output cpio path (default: /tmp/l3-initrd.cpio)
  --workdir <path>    Staging directory (default: temp dir, removed on exit)
  --keep-workdir      Do not remove --workdir on exit
  -h, --help          Show this help
EOF
}

while [ "$#" -gt 0 ]; do
	case "$1" in
	--ko)
		KO="${2:-}"
		shift 2
		;;
	--output)
		OUTPUT="${2:-}"
		shift 2
		;;
	--workdir)
		WORKDIR="${2:-}"
		shift 2
		;;
	--keep-workdir)
		KEEP_WORKDIR=1
		shift
		;;
	-h | --help)
		usage
		exit 0
		;;
	*)
		echo "build-l3-initrd: unknown argument: $1" >&2
		usage >&2
		exit 1
		;;
	esac
done

if [ -z "${KO}" ]; then
	echo "build-l3-initrd: --ko is required" >&2
	usage >&2
	exit 1
fi

if [ ! -f "${KO}" ]; then
	echo "build-l3-initrd: module not found: ${KO}" >&2
	exit 1
fi

BUSYBOX="$(command -v busybox 2>/dev/null || true)"
if [ -z "${BUSYBOX}" ]; then
	echo "build-l3-initrd: busybox not found (install busybox-static)" >&2
	exit 1
fi

cleanup() {
	if [ "${KEEP_WORKDIR}" = 0 ] && [ -n "${WORKDIR}" ] && [ -d "${WORKDIR}" ]; then
		rm -rf "${WORKDIR}"
	fi
}

if [ -z "${WORKDIR}" ]; then
	WORKDIR="$(mktemp -d)"
	trap cleanup EXIT
else
	mkdir -p "${WORKDIR}"
	if [ "${KEEP_WORKDIR}" = 0 ]; then
		trap cleanup EXIT
	fi
fi

mkdir -p "${WORKDIR}"/{bin,dev,proc,sys,workspace}
cp "${BUSYBOX}" "${WORKDIR}/bin/busybox"
for applet in sh ls insmod rmmod dmesg uname sleep poweroff reboot mount cat echo find cpio; do
	ln -sf busybox "${WORKDIR}/bin/${applet}"
done
cp "${KO}" "${WORKDIR}/workspace/88x2bu.ko"

cat > "${WORKDIR}/init" <<'EOF'
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
chmod +x "${WORKDIR}/init"

mkdir -p "$(dirname "${OUTPUT}")"
(
	cd "${WORKDIR}"
	find . | cpio -o -H newc
) > "${OUTPUT}"

echo "build-l3-initrd: wrote ${OUTPUT} (ko=$(readlink -f "${KO}"))"
