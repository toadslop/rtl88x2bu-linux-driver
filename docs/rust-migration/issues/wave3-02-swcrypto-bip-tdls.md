---
title: "[W3-02] Translate rtw_swcrypto BIP/SIV wrappers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-02
epic: E05
blocked_by: [W3-01]
estimate_loc: 110
---

## Goal

Port remaining software-crypto wrappers from [`core/rtw_swcrypto.c`](../../../core/rtw_swcrypto.c) to [`rust/rtw_swcrypto.rs`](../../../rust/rtw_swcrypto.rs):

- `_bip_ccmp_protect`, `_bip_gcmp_protect` (`CONFIG_IEEE80211W`)
- `_aes_siv_encrypt` / `_aes_siv_decrypt` (`CONFIG_RTW_MESH_AEK`)
- Any remaining thin wrappers not covered by W3-01

## Notes

- Completes the thin-wrapper port of `rtw_swcrypto.c`; swap Makefile object when both W3-01 and W3-02 land.
- **`_tdls_generate_tpk`** (`CONFIG_TDLS`) stays in C: extract to `rtw_swcrypto_rest.c` (same `*_rest.c` pattern as `rtw_security_rest.c`). It reads `struct sta_info` layout and is deferred to a later adapter-coupled slice — see W3-01 deferral note.
- In-flight branch `cursor/w3-02-swcrypto-bip-tdls-3dd4` covers BIP/SIV; TDLS extraction is part of this slice's C-side cleanup, not a Rust port.

## Acceptance

- L0 build + L1 symbols + L2 host vectors
- No `core/rtw_swcrypto.o` in CONFIG_RUST build after this slice; `_tdls_generate_tpk` remains in `rtw_swcrypto_rest.c` until a later issue
