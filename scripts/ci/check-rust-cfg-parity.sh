#!/usr/bin/env bash
# T21: verify the default L0 build uses rust/generated_cfgs and it is non-empty.
set -euo pipefail

root="$(cd "$(dirname "$0")/../.." && pwd)"
gen="${root}/rust/generated_cfgs"
cmd="${root}/rust/.scaffold.o.cmd"

if [ ! -f "$gen" ]; then
	echo "check-rust-cfg-parity: build module first (missing ${gen})" >&2
	exit 1
fi
if [ ! -f "$cmd" ]; then
	echo "check-rust-cfg-parity: missing ${cmd} (run L0 build)" >&2
	exit 1
fi

if ! grep -q "@${root}/rust/generated_cfgs" "$cmd"; then
	echo "check-rust-cfg-parity: rustc cmd does not load ${gen}" >&2
	exit 1
fi

mapfile -t gen_cfgs < <(sed -n 's/^--cfg=//p' "$gen" | LC_ALL=C sort -u)
if [ "${#gen_cfgs[@]}" -lt 10 ]; then
	echo "check-rust-cfg-parity: suspiciously small generated cfg set (${#gen_cfgs[@]})" >&2
	exit 1
fi

echo "check-rust-cfg-parity: OK (${#gen_cfgs[@]} cfgs in generated_cfgs, wired into rustc)"
