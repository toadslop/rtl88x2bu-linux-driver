---
title: "[FX-17] Fix hand-written extern C signature drift (narrow args, systime, duplicates)"
labels: [rust-migration, phase-1, regression, size/~200]
type: child
id: FX-17
epic: E14
blocked_by: [FX-06, FX-12]
estimate_loc: 120
---

## Priority

**P2 (medium)** for the `systime` rows (HZ=300 kernels), **P3** for the rest.

## Problem

The review ran bindgen over the C files that define every imported symbol (real
module flags) and diffed against the hand-written `extern "C"` blocks: 20
mismatching declarations, plus 6 symbols declared inconsistently across Rust files.

**Width / ABI bugs**

| Symbol | Rust | C | Effect |
|---|---|---|---|
| `rtw_rust_recv_sta_get_current_time` / `_get_passing_time_ms` ([`rust/rtw_recv_sta_validate.rs`](../../../rust/rtw_recv_sta_validate.rs) ~233, dde6134a #1055) | `-> U32`, `start: U32` | `systime` | 10 s deauth rate-limit for unassociated unicast senders defeated on HZ=300 after 5 min uptime (same arithmetic as FX-01) → deauth per frame |
| `rtw_rust_hal_com_get_current_time` / `_get_passing_time_ms` ([`rust/hal_com_c2h_mac_hidden.rs`](../../../rust/hal_com_c2h_mac_hidden.rs) ~44, e0422b94 #1464) | `u64` | `systime` | wrong on 32-bit; `hal_com_sec_cam_rw.rs` already uses `c_ulong` |
| `rtw_rust_hal_com_warn_c2h_short_len` (`hal_com_c2h_{debug_leaf,customer_str,mac_hidden,status}.rs`) | `len: u8` | `u32` | AArch64 does not extend narrow args → garbage length in log |
| `MGN_RATE_STR` ([`rust/hal_com.rs`](../../../rust/hal_com.rs) ~90) | `U8` | `enum MGN_RATE` (u32) | same AArch64 issue |
| `rtw_hal_{read,write}_rfreg` ([`rust/hal_phy.rs`](../../../rust/hal_phy.rs)), `rtw_rust_hal_com_set_hwreg` | `i32` | `u32` / `enum rf_path` | sign only; benign |

**Benign but misleading:** `_rtw_memcpy` / `_rtw_memset` declared `(…, usize) -> *mut c_void`
(C: `(…, u32) -> void`) in `hal_usb.rs`, `rtw_crypto_wrap.rs`; `bool` vs `u8` returns
(`rtw_cbuf_push`, `rtw_rx_ampdu_*`); ignored-return mismatches (`rtw_set_rpwm`,
`rtw_reset_drv_sw`, `rtw_os_recvbuf_resource_free`, `rtw_rx_ampdu_apply`).

**Inconsistent duplicates:** `_rtw_memcpy`, `rtw_malloc` (`u32` vs `usize`),
`rtw_mfree`, `rtw_set_rpwm` (`-> u8` vs `-> ()`), `rtw_rust_hal_com_get_current_time`,
`rtw_rust_hal_com_get_passing_time_ms`.

## Fix

Declare every import exactly as C (use `core::ffi::{c_ulong, c_int, …}` and `bool`
for C `bool`); for symbols used in several crates, move the declaration into one
shared `#[path]` include (e.g. `rust/osdep_extern.rs`) so it cannot diverge.

## Acceptance

- L0; L1 unchanged.
- T23's drift check (or `/tmp/drift`-style script attached to the PR) reports 0
  mismatches and 0 inconsistent duplicates.

## Notes

- **Dependency rationale:** touches the same `rust/hal_com_c2h_*.rs` files as FX-06 and `rust/hal_com_sec_cam_ent.rs` (`_rtw_memcpy` decl) as FX-12.
