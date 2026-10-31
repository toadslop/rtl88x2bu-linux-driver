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

Port the transmit attribute beamforming ID helper for **legacy beamforming builds** (`!RTW_BEAMFORMING_VERSION_2`) from
[`core/rtw_beamforming.c`](../../../core/rtw_beamforming.c) to
[`rust/rtw_beamforming.rs`](../../../rust/rtw_beamforming.rs) (avoid colliding with [W3-191](wave3-191-bf-cmd-traffic-leaf.md) `rtw_beamforming_cmd.rs`):

- `update_attrib_txbf_info` (C implementation under `#else /* !RTW_BEAMFORMING_VERSION_2 */` in `rtw_beamforming.c`)

## Notes

- **88x2bu default (`CONFIG_RTL8822B` + `CONFIG_BEAMFORMING`):** `RTW_BEAMFORMING_VERSION_2` is on; `update_attrib_txbf_info` is a header macro to `rtw_bf_update_attrib` ([W3-191](wave3-191-bf-cmd-traffic-leaf.md)). This issue is a **no-op port** for that config — L1 already covers `rtw_bf_update_attrib`; close with evidence from `check-symbols` / build matrix note only.
- **`!RTW_BEAMFORMING_VERSION_2`:** port the legacy function body; L2 vectors apply to that Kconfig path only.
- Small leaf after sounding stack ([W3-193](wave3-193-bf-sounding-leave-p2.md)); copies `g_id` / `p_aid` from `sta_info::cmn.bf_info` into `pkt_attrib`.
- Parallel beamforming lane once W3-193 lands — no overlap with `rtw_beamforming_sound.rs` object swap timing (coordinate Makefile with sounding part 2).

## Acceptance

- L0 on all supported beamforming Kconfig combinations
- L1: `rtw_bf_update_attrib` when `RTW_BEAMFORMING_VERSION_2`; `update_attrib_txbf_info` when legacy v1
- L2 (legacy v1 only): host fixture — STA with non-zero bf_info → attrib fields set; NULL sta → no-op
