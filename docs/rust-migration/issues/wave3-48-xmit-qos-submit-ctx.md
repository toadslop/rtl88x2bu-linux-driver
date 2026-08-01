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

- **Pure helpers** (table/byte logic): `qos_acm`, `rtw_put_snap`,
  `rtw_calculate_wlan_pkt_size_by_attribue`. `tos_to_up` may need export or test via
  `qos_acm` if static in C.
- **Submit-context group** (`rtw_sctx_*`) coordinates async xmit/cmd paths — uses kernel
  completion/wait primitives (`init_completion`, `wait_for_completion_timeout`, `complete`)
  guarded by `PLATFORM_LINUX`. Not table lookups; L2 should use host completion stubs for
  init/wait/done paths and test status/state-machine helpers (`rtw_sctx_chk_waring_status`,
  `rtw_sctx_done_err`, `rtw_sctx_done`) without a live wait queue where possible.
- Frame queue/coalesce/HAL xmit paths stay in C (W3-40 covered rate bmp by bw).
- L2: extend `tests/host/xmit/` — ToS→UP table, SNAP bytes; separate submit_ctx vectors
  (pure status transitions vs. stubbed completion init/wait).

## Acceptance

- L0 build + L2 host unit tests for QoS, SNAP, and submit context helpers
