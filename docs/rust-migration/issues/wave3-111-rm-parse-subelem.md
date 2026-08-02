---
title: "[W3-111] rm meas subelem parse"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-111
epic: E05
blocked_by: [W3-110]
estimate_loc: 260
---

## Goal

Port helpers from [`core/rtw_rm.c`](../../../core/rtw_rm.c) to [`rust/rtw_rm.rs`](../../../rust/rtw_rm.rs):

- `rm_parse_ch_load_s_elem`
- `rm_parse_noise_histo_s_elem`
- `rm_parse_bcn_req_s_elem`
- `rm_parse_meas_req`
- `rm_bcn_req_cond_mach`

## Notes

- Radio measurement subelement parse helpers; complements W3-33/W3-34 `rtw_rm_util.c` pure helpers.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for rm meas subelem parse helpers
