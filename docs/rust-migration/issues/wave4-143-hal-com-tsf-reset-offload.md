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
- H2C payload: `reset_port == HW_PORT0` → `{0x1, 0}`; **every other port** (not only port 1)
  → `{0x0, 0x1}`. Counter register: `REG_FW_RESET_TSF_CNT_0` vs `REG_FW_RESET_TSF_CNT_1`
  with the same port-0 vs non-port-0 split. Callers pass `iface->hw_port` (port 2+ on 8822B).
- Calls `rtw_mi_buddy_scan_abort(adapter, _FALSE)` before reset (same as C).
- If `rtw_hal_fill_h2c_cmd` returns other than `_SUCCESS`, return immediately — **no** sleep/poll.
- Poll runs only after successful H2C: `rtw_msleep_os(100)` up to 10 iterations;
  `_FAIL` when `loop_cnt >= 10` and counter unchanged.

## Acceptance

- L0 + L1 on profile with `CONFIG_TSF_RESET_OFFLOAD=y`
- L2: H2C bytes for port 0 vs port ≥2; stub counter poll; vector where H2C fill fails (no poll)

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)