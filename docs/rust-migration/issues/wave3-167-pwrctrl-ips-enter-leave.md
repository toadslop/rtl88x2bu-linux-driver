---
title: "[W3-167] Translate rtw_pwrctrl.c — IPS enter/leave leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-167
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port IPS entry/exit helpers from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) into
[`rust/rtw_pwrctrl_ips.rs`](../../../rust/rtw_pwrctrl_ips.rs):

- `_ips_enter`
- `ips_enter`
- `_ips_leave`
- `ips_leave`

## Notes

- Parallel lane to debug proc work — extends `rtw_pwrctrl` after [W3-94](wave3-94-pwrctrl-ps-gate-lps.md) (LPS/deny slice).
- `rtw_fw_ps_state` and `rtw_ps_processor` deferred to a follow-up issue.
- L2: host harness with minimal `pwrctrl_priv` / adapter fixtures (pattern from W3-94).

## Acceptance

- L0 build + L1 symbol check on swapped object
- L2 differential tests for IPS enter/leave state transitions where harness infra allows
