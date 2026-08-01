---
title: "[W3-79] stainfo free and sta priv lifecycle"
labels: [rust-migration, phase-1, wave-3]
type: child
id: W3-79
epic: E05
blocked_by: [W3-78]
estimate_loc: 470
---

## Goal

Port helpers from [`core/rtw_sta_mgt.c`](../../../core/rtw_sta_mgt.c) to [`rust/rtw_sta_mgt.rs`](../../../rust/rtw_sta_mgt.rs):

- `rtw_free_stainfo` (~250 LOC)
- `_rtw_init_sta_priv` (~118 LOC)
- `_rtw_free_sta_priv` (~56 LOC)
- `rtw_init_bcmc_stainfo` (~37 LOC)
- `rtw_mfree_stainfo` (~11 LOC)

## Notes

- **Multi-PR slice (~470 LOC total)** — `plan-stacked-prs` must split into stacked PRs (≤250 changed lines each).
- Free path calls C-only teardown hooks; document shim boundaries in harness.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for stainfo free and sta priv lifecycle
