#!/usr/bin/env bash
# CONFIG_USE_USB_BUFFER_ALLOC_RX is #define'd in include/autoconf.h only inside
# CONFIG_PLATFORM_ACTIONS_ATM702X. The default I386_PC build does not define it.
# Rust must not receive the cfg from that inactive line: usb_free_recv_priv
# would then skip rtw_skb_queue_purge of the preallocated RX skbs.
set -euo pipefail

root=$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)
script="${root}/scripts/rust_cfgs.sh"
cc="${CC:-}"
if [ -z "$cc" ]; then
	if command -v clang-18 >/dev/null 2>&1; then
		cc=clang-18
	elif command -v clang >/dev/null 2>&1; then
		cc=clang
	else
		cc=gcc
	fi
fi

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

"$script" --out "${tmp}/default.cfg" --srcdir "$root" --cc "$cc" --
if grep -qx -- '--cfg=CONFIG_USE_USB_BUFFER_ALLOC_RX' "${tmp}/default.cfg"; then
	echo "test-rust-cfgs-active-macros: inactive autoconf.h #define became a cfg" >&2
	exit 1
fi

"$script" --out "${tmp}/forced.cfg" --srcdir "$root" --cc "$cc" -- \
	-DCONFIG_USE_USB_BUFFER_ALLOC_RX
if ! grep -qx -- '--cfg=CONFIG_USE_USB_BUFFER_ALLOC_RX' "${tmp}/forced.cfg"; then
	echo "test-rust-cfgs-active-macros: -DCONFIG_USE_USB_BUFFER_ALLOC_RX did not emit the cfg" >&2
	exit 1
fi

echo "test-rust-cfgs-active-macros: OK"
