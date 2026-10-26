---
title: "[W3-173] Translate rtw_pwrctrl.c — IPS ps_processor loop"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-173
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port IPS-side power-save processing from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to
[`rust/rtw_pwrctrl_ps.rs`](../../../rust/rtw_pwrctrl_ps.rs):

- `rtw_ps_processor`
- `pwr_state_check_handler` (thin wrapper — may stay C calling into Rust)

## Notes

- Deferred from [W3-167](wave3-167-pwrctrl-ips-enter-leave.md); does **not** handle LPS (see [W3-94](wave3-94-pwrctrl-ps-gate-lps.md)).
- `SUPPORT_HW_RFOFF_DETECTED` paths call `RfOnOffDetect` / `rtw_hw_suspend` via FFI — keep HAL hooks in C shims as needed.
- Reuses `ips_enter` from `rust/rtw_pwrctrl_ips.rs` and `rtw_ps_deny_get` from `rust/rtw_pwrctrl.rs`.

## Acceptance

- L0 + L1 on `rtw_ps_processor`
- L2: host harness with mocked `pwrctrl_priv` + deny/idle flags (pattern from W3-94 / W3-167)
