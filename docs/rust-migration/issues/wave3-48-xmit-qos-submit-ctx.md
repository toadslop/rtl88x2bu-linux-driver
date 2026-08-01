---
title: "[W3-48] Translate rtw_xmit.c — QoS, SNAP, and submit context"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-48
epic: E05
blocked_by: [W3-47]
estimate_loc: 200
---

## Goal

Port xmit QoS/SNAP/submit-context helpers from [`core/rtw_xmit.c`](../../../core/rtw_xmit.c) to [`rust/rtw_xmit.rs`](../../../rust/rtw_xmit.rs):

- `qos_acm`, `tos_to_up`, `rtw_put_snap`, `rtw_calculate_wlan_pkt_size_by_attribue`
- `rtw_sctx_init`, `rtw_sctx_wait`, `rtw_sctx_chk_waring_status`, `rtw_sctx_done_err`, `rtw_sctx_done`

## Notes

- Mostly pure logic; `tos_to_up` may need export or test via `qos_acm` if static in C.
- Frame queue/coalesce/HAL xmit paths stay in C (W3-40 covered rate bmp by bw).
- L2: extend `tests/host/xmit/` with ToS→UP table, SNAP bytes, submit_ctx status transitions.

## Acceptance

- L0 build + L2 host unit tests for QoS, SNAP, and submit context helpers
