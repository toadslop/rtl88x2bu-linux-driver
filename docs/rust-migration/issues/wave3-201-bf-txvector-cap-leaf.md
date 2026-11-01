---
title: "[W3-201] Translate rtw_beamforming.c — txvector and STA BF cap leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-201
epic: E05
blocked_by: [W3-197]
estimate_loc: 200
---

## Goal

Port sounding prep helpers from [`core/rtw_beamforming.c`](../../../core/rtw_beamforming.c) to
[`rust/rtw_beamforming.rs`](../../../rust/rtw_beamforming.rs):

- `_get_txvector_parameter` (export as `rtw_bf_get_txvector_parameter` or keep static + test hook per Makefile swap plan)
- `_get_sta_beamform_cap`

## Notes

- Leaf math / HT-VHT cap intersection only — NDPA transmit and `_sounding_*` state machine stay in C ([W3-192](wave3-192-bf-sounding-enter-p1.md) / [W3-193](wave3-193-bf-sounding-leave-p2.md)).
- When `CONFIG_RUST && !HOST_BF_SOUND_ENTER_C_REF`, C already defines partial `_rust_get_sta_beamform_cap` (HT-only) for `rtw_rust_beamforming_enter_impl`. This slice ports the full static `_get_sta_beamform_cap` (HT + VHT, AP vs infra). **Consolidate or replace** the partial `_rust_*` helper at Makefile swap so sounding-enter and the leaf cap path cannot drift.
- Chains on [W3-197](wave3-197-bf-update-txbf-attrib.md) (same TU, in flight).
- L2 uses AP vs infra STA fixtures with stub `sta_info` / `mlme_priv` (no HAL MMIO).

## Acceptance

- L0 with `CONFIG_BEAMFORMING`
- L1 on ported symbols for enabled Kconfig matrix
- L2: golden `g_id` / `p_aid` for AP, adhoc, and infra-STA paths; BF cap flags for HT explicit beamformer/bformee combinations
