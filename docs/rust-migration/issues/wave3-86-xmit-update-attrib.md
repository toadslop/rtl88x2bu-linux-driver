---
title: "[W3-86] update_attrib builders"
labels: [rust-migration, phase-1, wave-3]
type: child
id: W3-86
epic: E05
blocked_by: [W3-85]
estimate_loc: 690
---

## Goal

Port the `update_attrib` helper cluster from [`core/rtw_xmit.c`](../../../core/rtw_xmit.c) to [`rust/rtw_xmit.rs`](../../../rust/rtw_xmit.rs):

- `update_attrib_vcs_info` (~132 LOC)
- `update_attrib_phy_info` (~72 LOC)
- `update_attrib_sec_info` (~186 LOC)
- `update_attrib` (~270 LOC)
- `update_attrib_trigger_frame_info` (static, `CONFIG_WMMPS_STA` — port or document C shim boundary)

## Notes

- **Multi-PR slice (~690 LOC total)** — `plan-stacked-prs` must split into stacked PRs (≤250 changed lines each); do not implement as a single PR.
- W3-40/48/49 covered rate/QoS slices.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for update_attrib builders
