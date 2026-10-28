---
title: "[W3-193] Translate rtw_beamforming.c — sounding leave (part 2/2)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-193
epic: E05
blocked_by: [W3-192]
estimate_loc: 220
---

## Goal

Complete sounding state machine port in [`rust/rtw_beamforming_sound.rs`](../../../rust/rtw_beamforming_sound.rs) (continued from [W3-192](wave3-192-bf-sounding-enter-p1.md)):

- `_beamforming_leave` and sounding timer/callback teardown helpers through NDPA/report completion

## Notes

- Same Makefile object as part 1; drop remaining static sounding helpers from C when done.
- Keep MU-MIMO GID mgmt in existing Rust from W3-129 unless shared helpers force a small C stub.

## Acceptance

- L0 + L1 on leave + sounding completion symbols
- L2: host differential enter→leave sequence on stub `beamforming_info`
