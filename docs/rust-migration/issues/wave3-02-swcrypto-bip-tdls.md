---
title: "[W3-02] Translate rtw_swcrypto BIP/SIV/TDLS wrappers"
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
- `_rtw_aes_siv_encrypt` / `_rtw_aes_siv_decrypt`
- Any remaining thin wrappers not covered by W3-01

## Notes

- Completes `rtw_swcrypto.c` port; swap Makefile object when both W3-01 and W3-02 land.
- In-flight: `cursor/w3-02-swcrypto-bip-tdls-3dd4`.

## Acceptance

- L0 build + L1 symbols + L2 host vectors
- No `core/rtw_swcrypto.o` in CONFIG_RUST build after this slice
