---
title: "[W3-62] Translate rtw_mlme.c — unassociated STA queue helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-62
epic: E05
blocked_by: [W3-61]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme.c`](../../../core/rtw_mlme.c) to [`rust/rtw_mlme_rest.rs`](../../../rust/rtw_mlme_rest.rs):

- `rtw_rx_add_unassoc_sta`
- `rtw_add_interested_unassoc_sta`
- `rtw_undo_interested_unassoc_sta`
- `rtw_search_unassoc_sta`
- static queue helpers: `alloc_unassoc_sta`, `del_unassoc_sta`, `del_unassoc_sta_chk`

## Notes

- **`CONFIG_RTW_MULTI_AP` only.** Queue/buffer setup and teardown live inside
  `rtw_init_mlme_priv` / `rtw_free_mlme_priv` (not separate exported
  `rtw_unassoc_sta_init` / `rtw_unassoc_sta_deinit` symbols). Do not add init/deinit
  exports — port the static helpers and public CRUD/search functions above.
- Queue CRUD under mlme lock. Builds on W3-53 network compare helpers.
- L2: new `tests/host/mlme/` harness with JSON differential vectors (pattern from
  W3-53); include `CONFIG_RTW_MULTI_AP` fixture for queue state.

## Acceptance

- L0 build + L2 host unit tests for unassoc STA queue helpers
