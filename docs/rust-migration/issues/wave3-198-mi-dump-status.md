---
title: "[W3-198] Translate rtw_mi.c — dump_mi_status leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-198
epic: E05
blocked_by: []
estimate_loc: 120
---

## Goal

Port multi-interface status dump formatters still in [`core/rtw_mi.c`](../../../core/rtw_mi.c) to
[`rust/rtw_mi.rs`](../../../rust/rtw_mi.rs):

- `dump_mi_status`
- `dump_dvobj_mi_status`

## Notes

- Completes the dump surface called out in [W3-122](wave3-122-mi-status-leaf.md) / [W3-144](wave3-144-debug-adapters-status.md) (`dump_mi_status` remained C-only after status aggregation moved to `core/rtw_mi_status*.c`).
- Keep calling into existing `rtw_mi_status_by_ifbmp` / `dump_*` helpers via thin C FFI where needed.
- Parallel **mi** lane root — disjoint from in-flight pwrctrl ([W3-196](wave3-196-pwrctrl-wow-pm-setters.md)) and beamforming ([W3-197](wave3-197-bf-update-txbf-attrib.md)).

## Acceptance

- L0 build with `CONFIG_CONCURRENT_MODE` / MI paths enabled
- L1: exported dump symbols match prior C ABI
- L2: host fixture — stub `dvobj_priv` + iface bitmap → stable dump lines (golden substrings)
