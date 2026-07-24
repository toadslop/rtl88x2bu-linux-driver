#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0
#
# L1 gate: compare global defined symbols between a reference (C) .o and a
# replacement (Rust) .o when swapping a translation unit.
#
# Usage (from repo root):
#   ./docs/rust-migration/scripts/check-symbols.sh OLD.o NEW.o
#   ./docs/rust-migration/scripts/check-symbols.sh OLD.o NEW.o --allowlist path.allow
#   make rust-check-symbols OLD=path/to/old.o NEW=path/to/new.o
#
# Environment:
#   NM   nm/llvm-nm binary (default: nm). The Makefile target sets this when LLVM=1.
#
# Allowlist file format (one rule per line; # starts a comment):
#   drop SYMBOL            OLD symbol may be absent from NEW
#   rename OLD_NAME NEW_NAME   OLD symbol renamed in NEW (binding type must match)
#
# Exit 0 when every non-dropped OLD global symbol is present in NEW with the same
# nm type letter (T/R/D/B/…).  Extra symbols in NEW are allowed (e.g. Rust probes).
#
set -euo pipefail

NM="${NM:-nm}"
ALLOWLIST=""

usage() {
	sed -n '2,22p' "$0"
}

die() {
	echo "check-symbols: error: $*" >&2
	exit 1
}

OLD_OBJ=""
NEW_OBJ=""

while [[ $# -gt 0 ]]; do
	case "$1" in
	-h | --help)
		usage
		exit 0
		;;
	--allowlist)
		shift
		[[ $# -gt 0 ]] || die "--allowlist requires a path"
		ALLOWLIST="$1"
		shift
		;;
	--)
		shift
		while [[ $# -gt 0 ]]; do
			if [[ -z "$OLD_OBJ" ]]; then
				OLD_OBJ="$1"
			elif [[ -z "$NEW_OBJ" ]]; then
				NEW_OBJ="$1"
			else
				die "too many positional arguments (expected OLD.o NEW.o)"
			fi
			shift
		done
		break
		;;
	-*)
		die "unknown option: $1 (try --help)"
		;;
	*)
		if [[ -z "$OLD_OBJ" ]]; then
			OLD_OBJ="$1"
		elif [[ -z "$NEW_OBJ" ]]; then
			NEW_OBJ="$1"
		else
			die "too many positional arguments (expected OLD.o NEW.o)"
		fi
		shift
		;;
	esac
done

[[ -n "$OLD_OBJ" && -n "$NEW_OBJ" ]] || {
	usage >&2
	die "expected OLD.o NEW.o"
}

[[ -f "$OLD_OBJ" ]] || die "OLD object not found: $OLD_OBJ"
[[ -f "$NEW_OBJ" ]] || die "NEW object not found: $NEW_OBJ"

command -v "$NM" >/dev/null 2>&1 || die "nm not found: $NM"

declare -A DROP=()
declare -A RENAME=()

load_allowlist() {
	local line rule op sym rest
	while IFS= read -r line || [[ -n "$line" ]]; do
		line="${line%%#*}"
		line="${line#"${line%%[![:space:]]*}"}"
		line="${line%"${line##*[![:space:]]}"}"
		[[ -n "$line" ]] || continue

		rule="${line%% *}"
		rest="${line#"$rule"}"
		rest="${rest#"${rest%%[![:space:]]*}"}"

		case "$rule" in
		drop)
			[[ -n "$rest" ]] || die "allowlist: drop requires a symbol name"
			DROP["$rest"]=1
			;;
		rename)
			sym="${rest%% *}"
			rest="${rest#"$sym"}"
			rest="${rest#"${rest%%[![:space:]]*}"}"
			[[ -n "$sym" && -n "$rest" ]] || die "allowlist: rename requires OLD NEW"
			RENAME["$sym"]="$rest"
			;;
		*)
			die "allowlist: unknown rule '$rule' (expected drop or rename)"
			;;
		esac
	done <"$ALLOWLIST"
}

if [[ -n "$ALLOWLIST" ]]; then
	[[ -f "$ALLOWLIST" ]] || die "allowlist not found: $ALLOWLIST"
	load_allowlist
fi

# name -> single-letter nm type (global, defined-only).
declare -A OLD_SYMS=()
declare -A NEW_SYMS=()

read_symbols() {
	local file="$1"
	declare -n out_ref="$2"
	local name sym_type line

	while IFS= read -r line; do
		[[ -n "$line" ]] || continue
		name="${line%% *}"
		sym_type="${line#"$name "}"
		sym_type="${sym_type%% *}"
		out_ref["$name"]="$sym_type"
	done < <("$NM" -g --defined-only -P "$file")
}

read_symbols "$OLD_OBJ" OLD_SYMS
read_symbols "$NEW_OBJ" NEW_SYMS

missing=0
binding=0
checked=0

for sym in "${!OLD_SYMS[@]}"; do
	if [[ -n "${DROP[$sym]+x}" ]]; then
		continue
	fi

	checked=$((checked + 1))
	expected="$sym"
	if [[ -n "${RENAME[$sym]+x}" ]]; then
		expected="${RENAME[$sym]}"
	fi

	old_type="${OLD_SYMS[$sym]}"

	if [[ -z "${NEW_SYMS[$expected]+x}" ]]; then
		echo "check-symbols: missing: $sym (expected in NEW as '$expected', type $old_type)" >&2
		missing=1
		continue
	fi

	new_type="${NEW_SYMS[$expected]}"
	if [[ "$new_type" != "$old_type" ]]; then
		echo "check-symbols: binding changed: $sym -> $expected ($old_type -> $new_type)" >&2
		binding=1
	fi
done

if [[ "$missing" -ne 0 || "$binding" -ne 0 ]]; then
	echo "check-symbols: FAIL ($OLD_OBJ vs $NEW_OBJ)" >&2
	exit 1
fi

old_count="${#OLD_SYMS[@]}"
new_count="${#NEW_SYMS[@]}"
dropped=${#DROP[@]}
echo "check-symbols: OK — $checked required OLD global(s) satisfied ($old_count in OLD, $new_count in NEW, $dropped dropped)"
