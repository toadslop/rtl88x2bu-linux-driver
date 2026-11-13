#!/usr/bin/env bash
# Generate rustflags --cfg lines from the effective C preprocessor macro set.
# Invoked from the module Makefile (see T21); not for host L2 harnesses.
set -euo pipefail

usage() {
	echo "usage: $0 --out FILE --srcdir DIR --cc CC -- [ccflags ...]" >&2
	exit 2
}

out=""
srcdir=""
cc="clang"
while [ $# -gt 0 ]; do
	case "$1" in
	--out) out="$2"; shift 2 ;;
	--srcdir) srcdir="$2"; shift 2 ;;
	--cc) cc="$2"; shift 2 ;;
	--) shift; break ;;
	-h | --help) usage ;;
	*) echo "unknown arg: $1" >&2; usage ;;
	esac
done

[ -n "$out" ] && [ -n "$srcdir" ] || usage

cfg_dir="$(cd "$(dirname "$0")" && pwd)/rust_cfgs.d"
keep_case="$cfg_dir/keep-case.txt"
aliases="$cfg_dir/aliases.txt"
allowlist="$(mktemp)"
trap 'rm -f "$allowlist"' EXIT
python3 "$(cd "$(dirname "$0")" && pwd)/rust_cfg_allowlist.py" "$srcdir/rust" >"$allowlist"

includes=(
	"-include" "${srcdir}/include/autoconf.h"
	"-include" "${srcdir}/include/drv_conf.h"
	"-include" "${srcdir}/include/hal_ic_cfg.h"
)

mapfile -t raw < <(
	"$cc" -dM -E "$@" "${includes[@]}" -I"${srcdir}/include" -x c /dev/null 2>/dev/null \
		| sed -nE 's/^#define ([A-Za-z_][A-Za-z0-9_]*)([[:space:]]+(-?[0-9]+|".*"))?/\1 \2/p'
)

declare -A skip_prefix=(
	[CONFIG_RUST_]=1
)

macro_is_active() {
	local _rest="$1"
	case "$_rest" in
	" 0" | $'\t0') return 1 ;;
	esac
	return 0
}

to_default_cfg() {
	local macro="$1"
	local suffix="${macro#CONFIG_}"
	printf 'config_%s' "$(printf '%s' "$suffix" | tr '[:upper:]' '[:lower:]')"
}

declare -A emit=()
declare -A allowed=()
declare -A keep_upper=()
while IFS= read -r line; do
	[ -n "$line" ] || continue
	allowed["$line"]=1
done <"$allowlist"
while IFS= read -r line; do
	[ -n "$line" ] || continue
	keep_upper["$line"]=1
done <"$keep_case"

add_cfg() {
	local name="$1"
	[ -n "${allowed[$name]+x}" ] || return 0
	emit["$1"]=1
}

apply_macro() {
	local name="$1"
	for prefix in "${!skip_prefix[@]}"; do
		if [[ "$name" == "${prefix}"* ]]; then
			return
		fi
	done
	if [[ "$name" != CONFIG_* ]] && [[ "$name" != DBG_* ]] && [[ "$name" != RTW_* ]]; then
		return
	fi
	if [ -n "${keep_upper[$name]+x}" ]; then
		add_cfg "$name"
		return
	fi
	if [[ "$name" == CONFIG_* ]]; then
		add_cfg "$(to_default_cfg "$name")"
		return
	fi
	if [[ "$name" == DBG_* ]]; then
		add_cfg "$(printf '%s' "$name" | tr '[:upper:]' '[:lower:]')"
	fi
}

while IFS=' ' read -r name rest; do
	[ -n "${name:-}" ] || continue
	macro_is_active "$rest" || continue
	apply_macro "$name"
done < <(printf '%s\n' "${raw[@]}")

# Aliases only apply when the source macro is defined.
while IFS=' ' read -r name rest; do
	[ -n "${name:-}" ] || continue
	macro_is_active "$rest" || continue
	while IFS=$'\t' read -r macro cfgs; do
		[ "$name" = "$macro" ] || continue
		for c in $cfgs; do
			add_cfg "$c"
		done
	done <"$aliases"
done < <(printf '%s\n' "${raw[@]}")

# Match Makefile FX-07 probes: autoconf.h may define macros without going through -dM.
autoconf="${srcdir}/include/autoconf.h"
if [ -f "$autoconf" ]; then
	while IFS= read -r want; do
		[ -n "$want" ] || continue
		[[ "$want" != CONFIG_* ]] && continue
		grep -Eq "^[[:space:]]*#[[:space:]]*define[[:space:]]+${want}\\b" "$autoconf" \
			|| continue
		apply_macro "$want"
	done < <(cat "$allowlist" "$keep_case" | LC_ALL=C sort -u)
fi

mkdir -p "$(dirname "$out")"
{
	for c in $(printf '%s\n' "${!emit[@]}" | LC_ALL=C sort); do
		echo "--cfg=$c"
	done
} >"$out"
