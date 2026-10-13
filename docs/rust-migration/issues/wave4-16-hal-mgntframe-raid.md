---
title: "[W4-16] Translate hal_com_rest.c — rtw_get_mgntframe_raid"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-16
epic: E06
blocked_by: [W4-15]
estimate_loc: 200
---

## Goal

Port management-frame rate ID selection from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into [`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `rtw_get_mgntframe_raid`

## Notes

- Maps `network_type` + chip generation (`IS_NEW_GENERATION_IC`) to `RATEID_IDX_*` / `RATR_INX_*`.
- Unblocks replacing the `extern "C"` call sites in `rust/rtw_mlme_ext_mgnt_attrib.rs` with direct Rust.
- Same `hal_com` lane as W4-14/W4-15.

## Acceptance

- L0 + L1 for `rtw_get_mgntframe_raid`
- L2: host oracle — legacy vs new-gen IC flag, 11B vs 11G network_type combinations
