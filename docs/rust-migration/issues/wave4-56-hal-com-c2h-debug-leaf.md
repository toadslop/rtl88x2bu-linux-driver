---
title: "[W4-56] Translate hal_com_rest.c — C2H defeature + customer string rpt"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-56
epic: E06
blocked_by: [W4-55]
estimate_loc: 160
---

## Goal

Port C2H debug/customer-string handlers from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
[`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `c2h_defeature_dbg_hdl`
- `rtw_hal_h2c_customer_str_req` (`CONFIG_RTW_CUSTOMER_STR`)
- `c2h_customer_str_rpt_hdl` / `c2h_customer_str_rpt_2_hdl`

## Notes

- Preserve `customer_str_mutex`, `customer_str_sctx` status transitions, and length checks.
- Mac-hidden report path stays in C for a later slice.

## Acceptance

- L0 with customer-string Kconfig enabled on pinned profile
- L1 on all exported symbols in this slice
- `RTW_SCTX` status updates match C for mocked submit contexts
