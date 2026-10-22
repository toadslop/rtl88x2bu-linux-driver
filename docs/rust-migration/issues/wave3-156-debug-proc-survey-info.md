---
title: "[W3-156] Translate rtw_debug_rest.c — proc survey_info get/set"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-156
epic: E05
blocked_by: [W3-155]
estimate_loc: 220
---

## Goal

Port sitesurvey proc helpers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc.rs`](../../../rust/rtw_debug_proc.rs):

- `proc_get_survey_info` — scanned queue walk + IE flag columns (`CONFIG_RTW_MESH` / ACS / noise hooks)
- `proc_set_survey_info` — user buffer parse path

## Notes

- Large formatter; split only if `git diff --shortstat` exceeds 250 lines during implementation — keep this issue scoped to survey proc only.
- `rtw_rson_show_survey_info` branch stays in C via FFI when `CONFIG_RTW_REPEATER_SON`.
- Chains after [W3-155](wave3-155-debug-proc-rson-data.md).

## Acceptance

- L0 with proc debug + scan enabled on pinned profile
- L1 on getter/setter symbols
- Survey table columns match C for a fixed fixture network (manual or host shim)
