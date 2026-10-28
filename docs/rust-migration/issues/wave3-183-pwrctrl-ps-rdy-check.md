---
title: "[W3-183] Translate rtw_pwrctrl.c — PS ready gate"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-183
epic: E05
blocked_by: []
estimate_loc: 120
---

## Goal

Port the LPS “may enter power save?” gate from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to
[`rust/rtw_pwrctrl_ps_rdy.rs`](../../../rust/rtw_pwrctrl_ps_rdy.rs):

- `PS_RDY_CHECK`

## Notes

- Pure mlme/pwrctrl state checks (WOWLAN suspend, survey/linking, AP/mesh/monitor, TDLS, DFS, group key, cfg80211 pwr mgmt).
- No HAL MMIO; optional cfg80211/roch hooks via existing FFI.
- Parallel pwrctrl lane root (disjoint Rust module).

## Acceptance

- L0 + L1 on `PS_RDY_CHECK`
- L2: host fixture table — suspend/WOWLAN, deny timer, and fwstate combinations vs C oracle
