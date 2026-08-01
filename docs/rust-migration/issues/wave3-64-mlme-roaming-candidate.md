---
title: "[W3-64] Translate rtw_mlme.c — roaming candidate check and select"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-64
epic: E05
blocked_by: [W3-63]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme.c`](../../../core/rtw_mlme.c) to [`rust/rtw_mlme_rest.rs`](../../../rust/rtw_mlme_rest.rs):

- `rtw_select_roaming_candidate` (exported)
- static helper: `rtw_check_roaming_candidate` (called only from select)

## Notes

- **`CONFIG_LAYER2_ROAMING` only** — both helpers live inside the same `#ifdef`
  block; `rtw_check_roaming_candidate` is `static`.
- Scanned-queue walk + compare; uses W3-53 same-network helpers.
- L2: new `tests/host/mlme/` harness with JSON differential vectors; include
  `CONFIG_LAYER2_ROAMING` fixture for scanned-queue state.

## Acceptance

- L0 build + L2 host unit tests for roaming candidate check/select
