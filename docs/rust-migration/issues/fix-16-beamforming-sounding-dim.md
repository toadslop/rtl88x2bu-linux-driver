---
title: "[FX-16] beamforming: enter fast path sets sounding_dim in the beamformer branch"
labels: [rust-migration, phase-1, regression, size/~100]
type: child
id: FX-16
epic: E14
blocked_by: []
estimate_loc: 10
---

## Priority

**P4 (low)** — divergence from C with no observable effect today.

## Problem

`_rust_get_sta_beamform_cap` in [`core/rtw_beamforming.c`](../../../core/rtw_beamforming.c)
(~1384–1400, 9b18d587 W3-192 #1601) sets both `*sounding_dim` and
`*comp_steering` in the `BEAMFORMER_ENABLE` branch. The original non-AP HT path of
`_get_sta_beamform_cap` set `sounding_dim = (cap & CHNL_EST_CAP) >> 6` only in the
`BEAMFORMEE_ENABLE` branch. Latent: in STA mode `CHNL_EST` bits are only set with
`BEAMFORMEE_ENABLE`, and 8822B reads only `bfer->NumofSoundingDim`.

## Fix

Drop the `*sounding_dim =` assignment from the beamformer branch.

## Acceptance

- L0; L1 unchanged; diff of the branch vs `9b18d587^:core/rtw_beamforming.c` is identical.
