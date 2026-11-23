---
title: "[W4-137] Translate hal_com_rest.c — RCR address-match HW_VAR delegate"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-137
epic: E06
blocked_by: [W4-127]
estimate_loc: 60
---

## Goal

Port `hw_var_set_rcr_am` from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
[`rust/hal_com_rcr.rs`](../../../rust/hal_com_rcr.rs) and wire `HW_VAR_AC_PARAM` / `HW_VAR_AMPDU` caller path only as needed for the RCR_AM enable bit (match C `hw_var_set_rcr_am` export).

## Notes

- **RCR lane (part 2):** toggles `RCR_AM` via `rtw_hal_rcr_add` / `rtw_hal_rcr_clear` from W4-127.
- Does not port `hw_var_rcr_config` / `hw_var_rcr_get` static helpers (defer to a later slice if still C-only).

## Acceptance

- L0 + L1 with W4-127 landed
- L2: host oracle — enable/disable sequences invoke add/clear with `RCR_AM` mask

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** [W4-127](wave4-127-hal-com-rcr-leaf.md)