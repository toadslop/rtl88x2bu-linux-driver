---
title: "[W4-57] Translate hal_com_rest.c — C2H mac hidden report"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-57
epic: E06
blocked_by: [W4-56]
estimate_loc: 180
---

## Goal

Port mac-hidden C2H handlers from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
[`rust/hal_com_c2h_mac_hidden.rs`](../../../rust/hal_com_c2h_mac_hidden.rs):

- `c2h_mac_hidden_rpt_hdl`
- `c2h_mac_hidden_rpt_2_hdl`
- `hal_read_mac_hidden_rpt`

## Notes

- Deferred from [W4-56](wave4-56-hal-com-c2h-debug-leaf.md) notes; preserve buffer length checks and adapter state updates.
- Parallel lane vs Wave 3 debug proc issues (disjoint `hal/` touch set).

## Acceptance

- L0 on pinned profile
- L1 on all three symbols
- Report parsing matches C for fixed C2H fixture bytes
