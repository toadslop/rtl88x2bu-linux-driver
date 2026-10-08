---
title: "[W3-132] Translate rtw_debug.c — manufacturing/test hook globals"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-132
epic: E05
blocked_by: [W3-130]
estimate_loc: 200
---

## Goal

Port manufacturing / self-test hook functions from [`core/rtw_debug.c`](../../../core/rtw_debug.c)
to [`rust/rtw_debug.rs`](../../../rust/rtw_debug.rs):

- `rtw_fwdl_test_trigger_chksum_fail`, `rtw_fwdl_test_trigger_wintint_rdy_fail`
- `rtw_del_rx_ampdu_test_trigger_no_tx_fail`
- `rtw_get_wait_hiq_empty_ms`
- `rtw_sta_linking_test_set_start`, `rtw_sta_linking_test_wait_done`, `rtw_sta_linking_test_force_fail`
- `rtw_ap_linking_test_force_auth_fail`, `rtw_ap_linking_test_force_asoc_fail`

## Notes

- Mostly atomic/global test flags — good host L2 candidate with mocked state.
- `rtw_ap_linking_test_force_auth_fail` / `rtw_ap_linking_test_force_asoc_fail` are
  `#ifdef CONFIG_AP_MODE` only (~C L924–936). The pinned 8822BU USB STA config may
  omit AP mode — port under matching `cfg` and document L1 allowlist/vacuous for absent
  AP symbols on STA-only builds (same spirit as W3-45 AP-gated helpers).
- Keep behavior identical for MP / proc test entry points still in C.

## Acceptance

- L0 + L1 on swapped object; AP-only exports excluded from strict L1 parity when
  `CONFIG_AP_MODE` is disabled on the pinned kernel config
- L2 host tests for flag transitions where feasible without full adapter (document gaps)
