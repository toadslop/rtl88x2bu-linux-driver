#!/usr/bin/env bash
# T20: advisory clippy on rust/** Kbuild crate roots (warn-only; never fails CI).
set -euo pipefail

toolchain="${RUSTUP_TOOLCHAIN:-1.83.0}"
driver=(rustup run "${toolchain}" clippy-driver)
edition=2021

# Grandfathered / FFI-heavy lints (Phase A). Tighten in a follow-up issue.
read -r -a allows <<-EOF
	-Aclippy::not_unsafe_ptr_arg_deref
	-Aclippy::undocumented_unsafe
	-Aclippy::absurd_extreme_comparisons
	-Aimproper_ctypes
	-Aclippy::result_unit_err
	-Aclippy::needless_range_loop
EOF

mapfile -d '' files < <(
	find rust -name '*.rs' \
		! -path 'rust/bindings/generated.rs' \
		! -path 'rust/rtw_sta_mgt.rs' \
		! -name '*.inc.rs' \
		-print0 | sort -z
)

if [ "${#files[@]}" -eq 0 ]; then
	echo "clippy-check: no rust/*.rs files found" >&2
	exit 0
fi

if ! command -v clippy-driver >/dev/null 2>&1; then
	rustup toolchain install "${toolchain}" --profile minimal --component clippy
fi

tmpdir=$(mktemp -d)
trap 'rm -rf "${tmpdir}"' EXIT

checked=0
skipped=0
warn_files=0
warn_total=0

summary="${tmpdir}/summary.md"
{
	echo "## Rust clippy (advisory)"
	echo
	echo "Toolchain: \`${toolchain}\` · crates attempted: **${#files[@]}**"
	echo
	echo "| Crate | Result | Warnings |"
	echo "|-------|--------|----------|"
} >"${summary}"

for f in "${files[@]}"; do
	crate=$(basename "${f}" .rs)
	out="${tmpdir}/${crate}.log"
	set +e
	"${driver[@]}" --edition "${edition}" --crate-type lib \
		-Wclippy::all --cap-lints=warn \
		"${allows[@]}" \
		"${f}" -o "${tmpdir}/${crate}.rlib" >"${out}" 2>&1
	rc=$?
	set -e

	if [ "${rc}" -ne 0 ]; then
		if grep -qE '^error(\[E| compiled)' "${out}"; then
			skipped=$((skipped + 1))
			reason=$(head -n1 "${out}" | sed 's/|/\\|/g' | cut -c1-80)
			echo "| \`${crate}\` | skip (needs kbuild cfgs/deps) | — |" >>"${summary}"
			continue
		fi
	fi

	checked=$((checked + 1))
	nwarns=$(grep -cE '^warning:' "${out}" 2>/dev/null || true)
	if [ "${nwarns}" -gt 0 ]; then
		warn_files=$((warn_files + 1))
		warn_total=$((warn_total + nwarns))
		echo "| \`${crate}\` | warnings | ${nwarns} |" >>"${summary}"
		echo "--- ${f} (${nwarns} warnings) ---" >>"${tmpdir}/warnings.txt"
		grep '^warning:' "${out}" >>"${tmpdir}/warnings.txt" || true
	else
		echo "| \`${crate}\` | clean | 0 |" >>"${summary}"
	fi
done

{
	echo
	echo "**Checked:** ${checked} · **Skipped (compile):** ${skipped} · **With warnings:** ${warn_files} · **Warning lines:** ${warn_total}"
	echo
	echo "Advisory only — job does not block merge. See T20 / \`scripts/ci/clippy-check.sh\` for allowlist."
} >>"${summary}"

cat "${summary}"
if [ -f "${tmpdir}/warnings.txt" ]; then
	echo
	echo "=== Sample warnings (first 40 lines) ==="
	head -n 40 "${tmpdir}/warnings.txt"
fi

if [ -n "${GITHUB_STEP_SUMMARY:-}" ]; then
	cat "${summary}" >>"${GITHUB_STEP_SUMMARY}"
fi

echo "clippy-check: advisory complete (checked=${checked} skipped=${skipped} warn_files=${warn_files})"
exit 0
