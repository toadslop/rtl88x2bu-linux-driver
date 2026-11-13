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

if ! grep -qE "@.*/rust/generated_cfgs|generated_cfgs" "$cmd"; then
	echo "check-rust-cfg-parity: rustc cmd does not load ${gen}" >&2
	exit 1
fi

mapfile -t gen_cfgs < <(sed -n 's/^--cfg=//p' "$gen" | LC_ALL=C sort -u)
if [ "${#gen_cfgs[@]}" -lt 10 ]; then
	echo "check-rust-cfg-parity: suspiciously small generated cfg set (${#gen_cfgs[@]})" >&2
	exit 1
fi

declare -A gen_set=()
for c in "${gen_cfgs[@]}"; do
	gen_set["$c"]=1
done

mapfile -t wanted < <(python3 "${root}/scripts/rust_cfg_allowlist.py" "${root}/rust")
not_default=()
for w in "${wanted[@]}"; do
	[ -n "${gen_set[$w]+x}" ] || not_default+=("$w")
done

if [ "${#not_default[@]}" -gt 0 ]; then
	echo "check-rust-cfg-parity: note — ${#not_default[@]} Rust #[cfg] names not in default generated_cfgs" \
		"(other chips / Makefile-only / T22 matrix — expected)" >&2
fi

golden="${root}/scripts/ci/rust-cfg-default.golden"
if [ ! -f "$golden" ]; then
	echo "check-rust-cfg-parity: missing ${golden}" >&2
	exit 1
fi

mapfile -t effective < <(
	python3 - "$cmd" "$gen" <<'PY'
import re
import sys

cmd_path, gen_path = sys.argv[1:3]
cmd = open(cmd_path).read()
rest = cmd.split("rustc ", 1)[1]
at = rest.find("@")
prefix = rest[:at] if at > 0 else rest
explicit = set(re.findall(r"--cfg[=\s]([A-Za-z0-9_]+)", prefix))
for line in open(gen_path):
	line = line.strip()
	if line.startswith("--cfg="):
		explicit.add(line.split("=", 1)[1])
for name in sorted(explicit):
	print(name)
PY
)

mapfile -t expect < <(grep -v '^[[:space:]]*$' "$golden" | LC_ALL=C sort -u)
if [ "${#effective[@]}" -ne "${#expect[@]}" ] || ! diff -q <(printf '%s\n' "${effective[@]}") <(printf '%s\n' "${expect[@]}") >/dev/null; then
	echo "check-rust-cfg-parity: default rustc cfg set drifted (see scripts/ci/rust-cfg-default.golden)" >&2
	diff -u <(printf '%s\n' "${expect[@]}") <(printf '%s\n' "${effective[@]}") >&2 || true
	exit 1
fi

echo "check-rust-cfg-parity: OK (${#gen_cfgs[@]} cfgs in generated_cfgs, ${#effective[@]} rustc cfgs match golden)"
