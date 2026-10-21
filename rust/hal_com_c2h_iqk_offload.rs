// SPDX-License-Identifier: GPL-2.0
// C2H IQK / txpwr idx offload submit_ctx helpers (W4-50).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;
type S32 = i32;

extern "C" {
    fn rtw_rust_hal_com_iqk_sctx(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_sctx_passing_time_ms(sctx: *mut c_void) -> U32;
    fn rtw_rust_hal_com_log_iqk_offload_finish_ms(ms: U32);
    fn rtw_rust_hal_com_iqk_sctx_begin_wait(adapter: *mut c_void, timeout_ms: U32);
    fn rtw_sctx_done(sctx: *mut *mut c_void);
    fn rtw_sctx_wait(sctx: *mut c_void, msg: *const U8) -> S32;
}

#[cfg(config_fw_offload_set_txpwr_idx)]
extern "C" {
    fn rtw_rust_hal_com_txpwr_idx_offload_sctx(adapter: *mut c_void) -> *mut c_void;
}

static C2H_IQK_OFFLOAD_WAIT: &[U8] = b"c2h_iqk_offload_wait\0";

#[cfg(config_fw_offload_set_txpwr_idx)]
static C2H_TXPWR_IDX_OFFLOAD_WAIT: &[U8] = b"c2h_txpwr_idx_offload_wait\0";

#[no_mangle]
pub extern "C" fn c2h_iqk_offload(adapter: *mut c_void, _data: *mut U8, _len: U8) {
    let sctx = unsafe { rtw_rust_hal_com_iqk_sctx(adapter) };
    let ms = unsafe { rtw_rust_hal_com_sctx_passing_time_ms(sctx) };
    unsafe { rtw_rust_hal_com_log_iqk_offload_finish_ms(ms) };
    let mut handle = sctx;
    unsafe { rtw_sctx_done(&mut handle) };
}

#[no_mangle]
pub extern "C" fn c2h_iqk_offload_wait(adapter: *mut c_void, timeout_ms: U32) -> S32 {
    unsafe { rtw_rust_hal_com_iqk_sctx_begin_wait(adapter, timeout_ms) };
    let sctx = unsafe { rtw_rust_hal_com_iqk_sctx(adapter) };
    unsafe { rtw_sctx_wait(sctx, C2H_IQK_OFFLOAD_WAIT.as_ptr()) }
}

#[cfg(config_fw_offload_set_txpwr_idx)]
#[no_mangle]
pub extern "C" fn c2h_txpwr_idx_offload_done(adapter: *mut c_void, _data: *mut U8, _len: U8) {
    let sctx = unsafe { rtw_rust_hal_com_txpwr_idx_offload_sctx(adapter) };
    let mut handle = sctx;
    unsafe { rtw_sctx_done(&mut handle) };
}

#[cfg(config_fw_offload_set_txpwr_idx)]
#[no_mangle]
pub extern "C" fn c2h_txpwr_idx_offload_wait(adapter: *mut c_void) -> S32 {
    let sctx = unsafe { rtw_rust_hal_com_txpwr_idx_offload_sctx(adapter) };
    unsafe { rtw_sctx_wait(sctx, C2H_TXPWR_IDX_OFFLOAD_WAIT.as_ptr()) }
}
