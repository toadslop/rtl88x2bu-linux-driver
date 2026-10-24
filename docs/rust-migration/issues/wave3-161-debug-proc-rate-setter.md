---
title: "[W3-161] Translate rtw_debug_rest.c — proc_set_rate_ctl"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-161
epic: E05
blocked_by: [W3-160]
estimate_loc: 200
---

## Goal

Port the rate-control **setter** proc hook from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_rate.rs`](../../../rust/rtw_debug_proc_rate.rs):

- `proc_set_rate_ctl`

## Notes

- Deferred from [W3-160](wave3-160-debug-proc-rate-leaf.md) — touches PHYDM fix-rate / `rtw_update_tx_rate_bmp` thread cmd path.
- Keep `proc_get_rate_ctl` and related getters in the existing Rust module; this issue adds only the setter.
- `HDATA_RATE` / bandwidth helpers via existing `rtw_debug` FFI.

## Acceptance

- L0 with `CONFIG_PROC_DEBUG` on pinned profile
- L1 on `proc_set_rate_ctl`
- Fixed-rate and bandwidth side effects match C for fixture adapter state (document PHYDM stubs if needed)
