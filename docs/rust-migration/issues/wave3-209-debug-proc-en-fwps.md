---
title: "[W3-209] Translate rtw_debug_rest.c — proc check_fw_ps leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-209
epic: E05
blocked_by: [W3-208]
estimate_loc: 80
---

## Goal

Port FW power-state debug registry procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_fwps.rs`](../../../rust/rtw_debug_proc_fwps.rs):

- `proc_get_en_fwps` / `proc_set_en_fwps`

## Notes

- Registry-only (`pregpriv->check_fw_ps`); setter clamps `mode < 2`.
- Chains after [W3-208](wave3-208-debug-proc-vht-24g.md) on the `rtw_debug_rest.c` guard lane.
- `proc_get_dyn_rrsr` / `proc_set_dyn_rrsr` stay in C (PHYDM `rtw_phydm_*` — defer).

## Acceptance

- L0 with `CONFIG_RTW_DEBUG`
- L1 on both handlers
- L2: proc_set then proc_get golden lines for fixture adapter registry
