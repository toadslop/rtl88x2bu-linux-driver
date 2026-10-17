---
title: "[W4-38] Translate hal_dm.c — Init_ODM big-endian hook block"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-38
epic: E06
blocked_by: [W4-30]
estimate_loc: 180
---

## Goal

Port the four `ODM_CMNINFO_*` hook registrations for band, security mode, channel width, and `net_closed` inside `Init_ODM_ComInfo` from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- **`CONFIG_LITTLE_ENDIAN`:** `odm_cmn_info_hook` with direct pointers — `&(pHalData->current_band_type)`, `&(adapter->securitypriv.dot11PrivacyAlgrthm)`, `&(pHalData->current_channel_bw)`, `&(adapter->net_closed)`
- **`CONFIG_BIG_ENDIAN` (else branch):** static `convert_to_big_endian` scratch buffers and hooks on those buffers for the same four `ODM_CMNINFO_*` IDs (`BAND`, `SEC_MODE`, `BW`, `NET_CLOSED`)
- Export `rtw_rust_hal_dm_init_odm_endian_hooks(...)` (or split LE/BE `#[no_mangle]` helpers) invoked from remaining C `Init_ODM_ComInfo` under the same `#ifdef CONFIG_LITTLE_ENDIAN` / `#else` structure as today

## Notes

- Same-file follow-up to [W4-30](wave4-30-hal-dm-board-type.md); does not port full `Init_ODM_ComInfo`.
- L2: run oracles for **both** compile-time paths (LE direct pointers vs BE `convert_to_big_endian` layout) with synthetic HalData/security/band/net_closed values.

## Acceptance

- L0 + L1 for the Rust endian-hook helper
- L2: fixture HalData/security/band/net_closed → expected hook addresses/values
