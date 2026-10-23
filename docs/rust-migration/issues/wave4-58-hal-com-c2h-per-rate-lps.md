---
title: "[W4-58] Translate hal_com_rest.c — C2H per-rate + LPS status"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-58
epic: E06
blocked_by: []
estimate_loc: 160
---

## Goal

Port C2H status handlers from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
[`rust/hal_com_c2h_status.rs`](../../../rust/hal_com_c2h_status.rs):

- `c2h_per_rate_rpt_hdl` (+ `per_rate_rpt_update` helper if kept module-local)
- `c2h_lps_status_rpt`

## Notes

- Independent of [W4-57](wave4-57-hal-com-c2h-mac-hidden.md) — different Rust module; ready immediately on `master`.
- Preserve mac_id / rate table updates and LPS state transitions.

## Acceptance

- L0 on pinned profile
- L1 on exported handler symbols
- Handler return codes and side effects match C for mocked C2H payloads
