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

- **Parallel lane:** separate submodule from RCR/MBSSID.
- **Enable:** clears `tsf_update_pause_stime`, sets `tsf_update_required=1` — does **not** touch `en_hw_update_tsf`.
- **Disable:** clears `tsf_update_required`, `tsf_update_pause_stime`, and `en_hw_update_tsf`.
- Trailing `rtw_hal_set_tsf_update` is cfg-gated like C (`#else` of `CONFIG_MI_WITH_MBSSID_CAM` only).
- Defer `rtw_hal_periodic_tsf_update_*` and timer handler to [W4-130](wave4-130-hal-com-tsf-periodic-chk.md).

## Acceptance

- L0 + L1
- L2: host shim records field deltas per enable/disable (enable leaves `en_hw_update_tsf` unchanged; disable clears it) plus optional `rtw_hal_set_tsf_update` when cfg allows

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)