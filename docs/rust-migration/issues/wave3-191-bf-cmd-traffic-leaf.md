---
title: "[W3-191] Translate rtw_beamforming.c — cmd and traffic leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-191
epic: E05
blocked_by: [W3-129]
estimate_loc: 200
---

## Goal

Port remaining cmd/dispatch leaf exports from [`core/rtw_beamforming.c`](../../../core/rtw_beamforming.c) to
[`rust/rtw_beamforming_cmd.rs`](../../../rust/rtw_beamforming_cmd.rs):

- `rtw_bf_cmd`
- `rtw_bf_update_attrib`
- `rtw_bf_c2h_handler`
- `rtw_bf_update_traffic`

## Notes

- Completes the deferral list in [W3-129](wave3-129-bf-init-cmd-leaf.md).
- Internal `_beamforming_*` / NDPA sounding paths stay in C until [W3-192](wave3-192-bf-sounding-enter-p1.md).
- Parallel beamforming lane (disjoint Rust file from `rtw_beamforming.rs`).

## Acceptance

- L0 + L1 on all four symbols
- L2: host JSON vectors for cmd enqueue + traffic update side effects (pattern from W3-128)
