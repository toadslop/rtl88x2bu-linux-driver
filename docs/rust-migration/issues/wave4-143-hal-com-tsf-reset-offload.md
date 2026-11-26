---
title: "[W4-143] Translate hal_com_rest.c — TSF reset H2C offload"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-143
epic: E06
blocked_by: []
estimate_loc: 70
---

## Goal

Port TSF reset offload from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_tsf_reset.rs` under `CONFIG_TSF_RESET_OFFLOAD`:

- `rtw_hal_h2c_reset_tsf` (static)
- `rtw_hal_reset_tsf`

## Notes

- **Parallel lane:** H2C + poll loop on `REG_FW_RESET_TSF_CNT_*` — disjoint modules.
- Calls `rtw_mi_buddy_scan_abort(adapter, _FALSE)` before reset (same as C).
- Poll uses `rtw_msleep_os(100)` up to 10 iterations.

## Acceptance

- L0 + L1 on profile with `CONFIG_TSF_RESET_OFFLOAD=y`
- L2: H2C payload bytes for port0 vs port1 + stub counter reads

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)