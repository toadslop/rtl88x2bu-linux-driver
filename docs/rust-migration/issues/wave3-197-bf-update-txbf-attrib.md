---
title: "[W3-197] Translate rtw_beamforming.c — update_attrib_txbf_info leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-197
epic: E05
blocked_by: [W3-193]
estimate_loc: 80
---

## Goal

Port the transmit attribute beamforming ID helper from [`core/rtw_beamforming.c`](../../../core/rtw_beamforming.c) to
[`rust/rtw_beamforming_cmd.rs`](../../../rust/rtw_beamforming_cmd.rs) or
[`rust/rtw_beamforming.rs`](../../../rust/rtw_beamforming.rs) (pick module with no Makefile collision):

- `update_attrib_txbf_info`

## Notes

- Small leaf after sounding stack ([W3-193](wave3-193-bf-sounding-leave-p2.md)); copies `g_id` / `p_aid` from `sta_info::cmn.bf_info` into `pkt_attrib`.
- Parallel beamforming lane once W3-193 lands — no overlap with `rtw_beamforming_sound.rs` object swap timing (coordinate Makefile with sounding part 2).

## Acceptance

- L0 + L1 on `update_attrib_txbf_info`
- L2: host fixture — STA with non-zero bf_info → attrib fields set; NULL sta → no-op
