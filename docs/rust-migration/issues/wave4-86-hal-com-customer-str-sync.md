---
title: "[W4-86] Translate hal_com_rest.c — customer string read/write API"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-86
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port `CONFIG_RTW_CUSTOMER_STR` synchronous API from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
[`rust/hal_com_customer_str.rs`](../../../rust/hal_com_customer_str.rs) (new module + Makefile / `hal_com.rs` include):

- `rtw_hal_customer_str_read` (mutex + `submit_ctx` wait)
- `rtw_hal_h2c_customer_str_write` (W1/W2/W3 H2C packing)
- `rtw_hal_customer_str_write` (write + validate via report)

## Notes

- **Parallel lane:** disjoint from hal_intf and hal_dm tranche 17.
- Reuse existing C2H report handlers in `hal_com_c2h_customer_str.rs` and `rtw_rust_hal_com_*` mutex/sctx FFI from `hal_com_rest.c`.
- Comment in C today marks read/write still unported — this slice closes that gap.

## Acceptance

- L0 with `CONFIG_RTW_CUSTOMER_STR=y` on host profile
- L1 symbols + L2 host test with mocked `submit_ctx` / H2C fill recording

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)