#!/bin/bash
# L3 gate: boot pinned bzImage in QEMU (TCG) and validate insmod/rmmod serial log (T8 PR2).
# See docs/rust-migration/test-plan.md#l3--module-load-without-device
set -euo pipefail

KERNEL="${KDIR:-/opt/linux}/arch/x86/boot/bzImage"
INITRD=""
KO=""
TIMEOUT=180
LOG=""
BUILD_INITRD=0

REQUIRED_MARKERS=(
	"=== L3 start ==="
	"rust scaffold: rtw_rust_scaffold_init ret=0"
	"module init ret=0"
	"registered new interface driver rtl88x2bu"
	"module exit success"
	"=== L3_PASS ==="
)

FORBIDDEN_PATTERNS=(
	"=== L3_FAIL"
	"Oops"
	"BUG:"
	"WARNING:"
)

usage() {
	cat <<'EOF'
Usage: run-l3-qemu.sh [options]

Boot the pinned kernel in QEMU (TCG), run the L3 initramfs test, and validate
the serial console log.

Options:
  --kernel <path>     bzImage path (default: $KDIR/arch/x86/boot/bzImage)
  --initrd <path>     initramfs cpio (required unless --ko is given)
  --ko <path>         Build initrd from 88x2bu.ko via build-l3-initrd.sh
  --timeout <sec>     QEMU timeout (default: 180)
  --log <path>        Write serial log to file (default: temp file)
  -h, --help          Show this help

On failure, see docs/rust-migration/test-plan.md#l3--module-load-without-device
EOF
}

script_dir() {
	cd "$(dirname "${BASH_SOURCE[0]}")" && pwd
}

while [ "$#" -gt 0 ]; do
	case "$1" in
	--kernel)
		KERNEL="${2:-}"
		shift 2
		;;
	--initrd)
		INITRD="${2:-}"
		shift 2
		;;
	--ko)
		KO="${2:-}"
		BUILD_INITRD=1
		shift 2
		;;
	--timeout)
		TIMEOUT="${2:-}"
		shift 2
		;;
	--log)
		LOG="${2:-}"
		shift 2
		;;
	-h | --help)
		usage
		exit 0
		;;
	*)
		echo "run-l3-qemu: unknown argument: $1" >&2
		usage >&2
		exit 1
		;;
	esac
done

if ! command -v qemu-system-x86_64 >/dev/null 2>&1; then
	echo "run-l3-qemu: qemu-system-x86_64 not found (install qemu-system-x86)" >&2
	exit 1
fi

if [ ! -f "${KERNEL}" ]; then
	echo "run-l3-qemu: kernel image not found: ${KERNEL}" >&2
	exit 1
fi

if [ "${BUILD_INITRD}" = 1 ]; then
	if [ -z "${KO}" ] || [ ! -f "${KO}" ]; then
		echo "run-l3-qemu: --ko requires a valid 88x2bu.ko path" >&2
		exit 1
	fi
	INITRD="$(mktemp /tmp/l3-initrd.XXXXXX.cpio)"
	trap 'rm -f "${INITRD}"' EXIT
	"$(script_dir)/build-l3-initrd.sh" --ko "${KO}" --output "${INITRD}"
elif [ -z "${INITRD}" ] || [ ! -f "${INITRD}" ]; then
	echo "run-l3-qemu: --initrd or --ko is required" >&2
	usage >&2
	exit 1
fi

if [ -z "${LOG}" ]; then
	LOG="$(mktemp /tmp/l3-serial.XXXXXX.log)"
	trap 'rm -f "${LOG}"' EXIT
fi

echo "run-l3-qemu: kernel=${KERNEL} initrd=${INITRD} log=${LOG}"

set +e
timeout "${TIMEOUT}" qemu-system-x86_64 -cpu qemu64 -m 1G -nographic \
	-kernel "${KERNEL}" \
	-initrd "${INITRD}" \
	-append 'console=ttyS0 earlyprintk=serial,ttyS0 ignore_loglevel rdinit=/init' \
	>"${LOG}" 2>&1
qemu_status=$?
set -e

if [ "${qemu_status}" -eq 124 ]; then
	echo "run-l3-qemu: QEMU timed out after ${TIMEOUT}s" >&2
elif [ "${qemu_status}" -ne 0 ]; then
	echo "run-l3-qemu: QEMU exited with status ${qemu_status}" >&2
fi

validate_log() {
	local log="$1"
	local marker pattern

	for marker in "${REQUIRED_MARKERS[@]}"; do
		if ! grep -Fq "${marker}" "${log}"; then
			echo "run-l3-qemu: missing required marker: ${marker}" >&2
			return 1
		fi
	done

	for pattern in "${FORBIDDEN_PATTERNS[@]}"; do
		if grep -Fq "${pattern}" "${log}"; then
			echo "run-l3-qemu: forbidden pattern in log: ${pattern}" >&2
			return 1
		fi
	done

	return 0
}

if ! validate_log "${LOG}"; then
	echo "run-l3-qemu: serial log tail:" >&2
	tail -80 "${LOG}" >&2
	echo "run-l3-qemu: see docs/rust-migration/test-plan.md#l3--module-load-without-device" >&2
	exit 1
fi

if [ "${qemu_status}" -ne 0 ] && [ "${qemu_status}" -ne 124 ]; then
	echo "run-l3-qemu: log markers passed but QEMU status was ${qemu_status}" >&2
	exit 1
fi

echo "run-l3-qemu: OK (log=${LOG})"
