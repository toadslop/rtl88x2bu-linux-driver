---
title: "[W3-92] joinbss and drvextra dispatch"
labels: [rust-migration, phase-1, wave-3]
type: child
id: W3-92
epic: E05
blocked_by: [W3-91]
estimate_loc: 395
---

## Goal

Port helpers from [`core/rtw_cmd.c`](../../../core/rtw_cmd.c) to [`rust/rtw_cmd_rest.rs`](../../../rust/rtw_cmd_rest.rs):

- `rtw_joinbss_cmd` (~201 LOC)
- `rtw_drvextra_cmd_hdl` (~194 LOC)

## Notes

- **Multi-PR slice (~395 LOC total)** — `plan-stacked-prs` must split into stacked PRs (≤250 changed lines each).
- Cmd dispatch slice bridging mlme join to firmware path.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for joinbss and drvextra dispatch
