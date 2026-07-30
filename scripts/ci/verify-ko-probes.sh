#!/bin/bash
# L0 gate: verify expected Rust/C symbols are present in 88x2bu.ko (T6).
# Single source of truth for the probe list — keep in sync with AGENTS.md.
set -euo pipefail

KO="${1:-88x2bu.ko}"

if [ ! -f "${KO}" ]; then
	echo "verify-ko-probes: ${KO} not found" >&2
	exit 1
fi

# One symbol per line.
PROBES="$(cat <<'EOF'
rtw_rust_kbuild_probe
rtw_rust_scaffold_init
rtw_rust_bindings_probe
rtw_rust_domain_types_probe
rtw_rust_aes_ctr_probe
rtw_rust_aes_omac1_probe
rtw_rust_gcmp_probe
rtw_rust_aes_siv_probe
rtw_rust_aes_ccm_probe
rtw_rust_aes_gcm_probe
rtw_rust_ccmp_probe
rtw_rust_aes_internal_probe
rtw_rust_aes_internal_enc_probe
rtw_rust_sha256_internal_probe
rtw_rust_sha256_probe
rtw_rust_sha256_prf_probe
rtw_rust_rtw_crypto_wrap_probe
rtw_rust_chplan_probe
rtw_rust_swcrypto_probe
rtw_rust_ieee80211_probe
rtw_rust_security_probe
rtw_rust_security_rest_probe
rtw_rust_wlan_util_probe
rtw_rust_rm_util_probe
aes_ctr_encrypt
aes_siv_encrypt
aes_ccm_ae
aes_ccm_ad
aes_gcm_ae
aes_gcm_ad
ccmp_get_pn
ccmp_decrypt
ccmp_encrypt
ccmp_encrypt_pv1
ccmp_256_decrypt
ccmp_256_encrypt
rijndaelKeySetupEnc
aes_encrypt_init
aes_encrypt
sha256_vector
hmac_sha256_vector
sha256_prf
sha256_prf_bits
os_memcmp_const
rtw_registrypriv_amsdu_mode
EOF
)"

DEFINED="$(mktemp)"
trap 'rm -f "${DEFINED}"' EXIT
nm -g --defined-only "${KO}" 2>/dev/null | awk '{print $NF}' | sort -u > "${DEFINED}"

missing=()
while IFS= read -r sym; do
	[ -n "${sym}" ] || continue
	if ! grep -qxF "${sym}" "${DEFINED}"; then
		missing+=("${sym}")
	fi
done <<< "${PROBES}"

if [ "${#missing[@]}" -gt 0 ]; then
	echo "verify-ko-probes: missing ${#missing[@]} symbol(s) in ${KO}:" >&2
	printf '  %s\n' "${missing[@]}" >&2
	exit 1
fi

echo "verify-ko-probes: OK (${KO})"
