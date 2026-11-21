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

- **Parallel lane:** separate submodule from RCR/MBSSID; always updates `mlmeextpriv` pause/required/en_hw fields (outside any cfg).
- Only the trailing `rtw_hal_set_tsf_update` call sits in `#ifdef CONFIG_MI_WITH_MBSSID_CAM` / `#else` — omitted on pinned concurrent profile, present when that macro is unset; wire via FFI or port with W4-128 pause/restore helpers.
- Defer `rtw_hal_periodic_tsf_update_*` and timer handler to [W4-130](wave4-130-hal-com-tsf-periodic-chk.md).

## Acceptance

- L0 + L1
- L2: host shim records `tsf_update_required`, `en_hw_update_tsf`, and `rtw_hal_set_tsf_update` calls for enable/disable pairs

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)