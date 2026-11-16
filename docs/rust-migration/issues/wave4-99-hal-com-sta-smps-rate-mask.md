---
title: "[W4-99] Translate hal_com_rest.c — STA SMPS cap and rate mask"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-99
epic: E06
blocked_by: [W4-97]
estimate_loc: 170
---

## Goal

Port STA rate-selection helpers from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into [`rust/hal_com.rs`](../../../rust/hal_com.rs) (same module as [W4-97](wave4-97-hal-com-update-sta-wset-mimo.md)):

- `rtw_hal_update_sta_smps_cap`
- `rtw_hal_update_sta_rate_mask` — HT/VHT RA bitmap from `bssrateset` / MCS maps

## Notes

- **Same-file follow-up** to W4-97 — chain after the STA wset/MIMO stack merges or stack on its PR branch.
- Does not port `rtw_hal_update_sta_ra_info` (thin orchestrator; add in a later slice if needed).

## Acceptance

- L0 + L1 for both exports under default 8822B USB kconfig
- L2: host vectors for legacy CCK/OFDM rates plus HT/VHT MCS bitmask parity vs C oracle

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-97 (#2085)