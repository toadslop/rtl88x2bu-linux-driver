---
title: "[W4-40] Translate hal_com.c — dump_tx_aclt_confs formatter"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-40
epic: E06
blocked_by: [W4-39]
estimate_loc: 200
---

## Goal

Port `dump_tx_aclt_confs` from [`hal/hal_com.c`](../../../hal/hal_com.c) into
[`rust/hal_com.rs`](../../../rust/hal_com.rs):

- Iterate `TX_ACLT_CONF_NUM` entries on `dvobj->tx_aclt_confs`
- Format per-row vo_vi/be_bk us and register columns using `hal_spec->tx_aclt_unit_factor`
- Match C `TX_ACLT_CONF_MSG_LEN` snprintf truncation behavior

## Notes

- Chains hal_com ACLT debug lane after [W4-39](wave4-39-hal-com-tx-aclt-force-dump.md) (shared FFI for hal_spec + dvobj).

## Acceptance

- L0 + L1 for `dump_tx_aclt_confs`
- L2: golden dump for 2–3 synthetic conf rows (enabled/disabled mix)
