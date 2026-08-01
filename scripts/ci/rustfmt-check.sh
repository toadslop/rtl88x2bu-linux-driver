#!/usr/bin/env bash
# T16: verify rust/** sources match rustfmt (edition 2021).
set -euo pipefail

edition="${RUSTFMT_EDITION:-2021}"
toolchain="${RUSTUP_TOOLCHAIN:-1.83.0}"

mapfile -d '' files < <(find rust -name '*.rs' \
	! -path 'rust/bindings/generated.rs' \
	! -path 'rust/rtw_sta_mgt.rs' \
	-print0 | sort -z)

if [ "${#files[@]}" -eq 0 ]; then
	echo "rustfmt-check: no rust/*.rs files found" >&2
	exit 1
fi

if ! command -v rustfmt >/dev/null 2>&1; then
	rustup toolchain install "${toolchain}" --profile minimal --component rustfmt
fi

if ! rustfmt --edition "${edition}" --check "${files[@]}"; then
	echo >&2
	echo "rustfmt-check: formatting drift detected. Run locally:" >&2
	echo "  find rust -name '*.rs' ! -path 'rust/bindings/generated.rs' ! -path 'rust/rtw_sta_mgt.rs' -print0 | xargs -0 rustfmt --edition ${edition}" >&2
	exit 1
fi

echo "rustfmt-check: OK (${#files[@]} files)"
