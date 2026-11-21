---
title: "[W4-129] Translate hal_com_rest.c — TSF iface enable/disable"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-129
epic: E06
blocked_by: []
estimate_loc: 100
---

## Goal

Port TSF pause bookkeeping from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
`rust/hal_com_tsf.rs`:

- `rtw_iface_enable_tsf_update`
- `rtw_iface_disable_tsf_update`

## Notes

- **Parallel lane:** separate submodule from RCR/MBSSID; toggles `mlmeextpriv` fields.
- When `CONFIG_MI_WITH_MBSSID_CAM` is unset, enable/disable call `rtw_hal_set_tsf_update` (static in C today) — wire via FFI or port in the same PR as W4-128 pause/restore.
- Under `CONFIG_MI_WITH_MBSSID_CAM` (pinned `CONFIG_IFACE_NUMBER=3` + `CONFIG_HWMPCAP_GEN2`), bodies are empty no-ops like C.
- Defer `rtw_hal_periodic_tsf_update_*` and timer handler to [W4-130](wave4-130-hal-com-tsf-periodic-chk.md).

## Acceptance

- L0 + L1
- L2: host shim records `tsf_update_required`, `en_hw_update_tsf`, and `rtw_hal_set_tsf_update` calls for enable/disable pairs

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)