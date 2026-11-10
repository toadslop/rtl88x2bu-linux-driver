---
title: "[FX-09] hal: LPS_ACK shim guard, rtw_malloc macro call, usb recv free NULL"
labels: [rust-migration, phase-1, regression, size/~100]
type: child
id: FX-09
epic: E14
blocked_by: []
estimate_loc: 50
---

## Priority

**P3 (low)** — latent build breaks / pre-existing NULL path; no default-build impact.

## Problem

1. **`CONFIG_LPS_ACK` guard dropped.** c828dc29 (W4-58 PR2, #1469) moved the LPS
   status handling into shims `rtw_rust_hal_com_lps_status_apply` /
   `rtw_rust_hal_com_lps_sctx_done` in [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
   (~15204–15225) that touch `pwrpriv->lps_ack_status` / `lps_ack_sctx`
   unconditionally; the original body was under `#ifdef CONFIG_LPS_ACK`.
2. **`rtw_malloc` is a macro.** [`rust/hal_usb.rs`](../../../rust/hal_usb.rs) (~545, ~592,
   c616929c W4-44 #1386) declares `extern fn rtw_malloc(sz: u32)` for the `c2h_wk`
   path. `rtw_malloc` is a macro in `include/osdep_service.h`; only `_rtw_malloc` is a
   symbol. With `CONFIG_FW_C2H_REG`, modpost fails.
3. **Pre-existing, kept:** `usb_free_recv_priv` after a failed `usb_init_recv_priv`
   (`precv_buf == NULL`, `recvbuf_nr > 0`) passes `NULL + offset` to
   `rtw_os_recvbuf_resource_free`; `.add()` on null is UB in Rust.

## Fix

1. Put the two shim bodies under `#ifdef CONFIG_LPS_ACK` (empty otherwise).
2. Call `_rtw_malloc` (as `rust/rtw_crypto_wrap.rs` does).
3. Return early from the free loop when `precv_buf` is null.

## Acceptance

- Default L0; L1 unchanged for `rust/hal_com.o`, `rust/hal_usb.o`.
- `nm -u rust/hal_usb.o` built with `--cfg config_fw_c2h_reg` shows `_rtw_malloc`, not `rtw_malloc`.
- `hal/hal_com_rest.o` compiles with `CONFIG_LPS_ACK` undefined.
