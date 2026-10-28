---
title: "[W3-192] Translate rtw_beamforming.c — sounding enter (part 1/2)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-192
epic: E05
blocked_by: [W3-191]
estimate_loc: 220
---

## Goal

Start the internal sounding state machine port from [`core/rtw_beamforming.c`](../../../core/rtw_beamforming.c) to
[`rust/rtw_beamforming_sound.rs`](../../../rust/rtw_beamforming_sound.rs):

- `_beamforming_enter` and closely coupled HT/VHT NDPA prep helpers through first HAL `beamforming_*` ODM calls (stop before MU/GID side paths)

## Notes

- Epic follow-up to [W3-128](wave3-128-bf-entry-packet-leaf.md) / [W3-129](wave3-129-bf-init-cmd-leaf.md); HAL coupling via existing ODM FFI only.
- Part 2 will cover leave/sounding finish and `_bfee_*` state updates.

## Acceptance

- L0 + L1 on exported sounding entry hooks wired from C
- L2 optional: stub ODM + beamforming_info fixtures for enter path
