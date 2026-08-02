---
title: "[W3-113] mp PMAC sig generators"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-113
epic: E05
blocked_by: [W3-112]
estimate_loc: 320
---

## Goal

Port helpers from [`core/rtw_mp.c`](../../../core/rtw_mp.c) to [`rust/rtw_mp.rs`](../../../rust/rtw_mp.rs):

- `ByteToBit`
- `CRC16_generator`
- `CCK_generator`
- `PMAC_Get_Pkt_Param`
- `L_SIG_generator`
- `CRC8_generator`
- `HT_SIG_generator`

## Notes

- Pure PMAC signal-generation helpers; low HAL coupling — first slice of `rtw_mp.c`.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for mp PMAC sig generator helpers
