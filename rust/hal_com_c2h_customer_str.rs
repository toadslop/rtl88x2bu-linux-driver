// SPDX-License-Identifier: GPL-2.0
// C2H customer-string report handlers (W4-56 PR2).

use core::ffi::c_void;

type U8 = u8;
type S32 = i32;

const SUCCESS: S32 = 1;
const FAIL: S32 = 0;

const CUSTOMER_STR_RPT_LEN: usize = 8;
const CUSTOMER_STR_RPT_2_LEN: usize = 8;

const RTW_SCTX_SUBMITTED: i32 = -1;
const RTX_SCTX_CSTR_WAIT_RPT2: i32 = 13;

extern "C" {
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_warn_c2h_short_len(func: *const u8, len: U8, expect: i32);
    fn rtw_rust_hal_com_customer_str_mutex(dvobj: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_enter_critical_mutex(mutex: *mut c_void);
    fn rtw_rust_hal_com_exit_critical_mutex(mutex: *mut c_void);
    fn rtw_rust_hal_com_customer_str_sctx(dvobj: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_customer_str_buf(dvobj: *mut c_void) -> *mut U8;
    fn rtw_rust_hal_com_sctx_set_status(sctx: *mut c_void, status: i32);
    fn rtw_rust_hal_com_sctx_status(sctx: *mut c_void) -> i32;
    fn rtw_rust_hal_com_customer_str_sctx_done(dvobj: *mut c_void);
    fn rtw_rust_hal_com_warn_customer_str_invalid_sctx_status(status: i32);
    fn rtw_rust_hal_com_warn_customer_str_sctx_not_set();
    fn rtw_rust_hal_com_warn_customer_str_rpt_not_ready();
    fn rtw_rust_hal_com_h2c_customer_str_req(adapter: *mut c_void) -> S32;
    fn _rtw_memcpy(dst: *mut c_void, src: *const c_void, sz: u32);
}

static FN_C2H_CSTR_RPT: &[u8] = b"c2h_customer_str_rpt_hdl\0";
static FN_C2H_CSTR_RPT2: &[u8] = b"c2h_customer_str_rpt_2_hdl\0";

#[no_mangle]
pub extern "C" fn rtw_hal_h2c_customer_str_req(adapter: *mut c_void) -> S32 {
    unsafe { rtw_rust_hal_com_h2c_customer_str_req(adapter) }
}

#[no_mangle]
pub extern "C" fn c2h_customer_str_rpt_hdl(adapter: *mut c_void, data: *mut U8, len: U8) -> S32 {
    if (len as usize) < CUSTOMER_STR_RPT_LEN {
        unsafe {
            rtw_rust_hal_com_warn_c2h_short_len(
                FN_C2H_CSTR_RPT.as_ptr(),
                len,
                CUSTOMER_STR_RPT_LEN as i32,
            );
        }
        return FAIL;
    }

    let dvobj = unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) };
    let mutex = unsafe { rtw_rust_hal_com_customer_str_mutex(dvobj) };
    unsafe { rtw_rust_hal_com_enter_critical_mutex(mutex) };

    let sctx = unsafe { rtw_rust_hal_com_customer_str_sctx(dvobj) };
    if !sctx.is_null() {
        let status = unsafe { rtw_rust_hal_com_sctx_status(sctx) };
        if status != RTW_SCTX_SUBMITTED {
            unsafe {
                rtw_rust_hal_com_warn_customer_str_invalid_sctx_status(status);
            }
        }
        let dst = unsafe { rtw_rust_hal_com_customer_str_buf(dvobj) };
        unsafe {
            _rtw_memcpy(
                dst as *mut c_void,
                data as *const c_void,
                CUSTOMER_STR_RPT_LEN as u32,
            );
        }
        unsafe { rtw_rust_hal_com_sctx_set_status(sctx, RTX_SCTX_CSTR_WAIT_RPT2) };
    } else {
        unsafe { rtw_rust_hal_com_warn_customer_str_sctx_not_set() };
    }

    unsafe { rtw_rust_hal_com_exit_critical_mutex(mutex) };
    SUCCESS
}

#[no_mangle]
pub extern "C" fn c2h_customer_str_rpt_2_hdl(adapter: *mut c_void, data: *mut U8, len: U8) -> S32 {
    if (len as usize) < CUSTOMER_STR_RPT_2_LEN {
        unsafe {
            rtw_rust_hal_com_warn_c2h_short_len(
                FN_C2H_CSTR_RPT2.as_ptr(),
                len,
                CUSTOMER_STR_RPT_2_LEN as i32,
            );
        }
        return FAIL;
    }

    let dvobj = unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) };
    let mutex = unsafe { rtw_rust_hal_com_customer_str_mutex(dvobj) };
    unsafe { rtw_rust_hal_com_enter_critical_mutex(mutex) };

    let sctx = unsafe { rtw_rust_hal_com_customer_str_sctx(dvobj) };
    if !sctx.is_null() {
        if unsafe { rtw_rust_hal_com_sctx_status(sctx) } != RTX_SCTX_CSTR_WAIT_RPT2 {
            unsafe { rtw_rust_hal_com_warn_customer_str_rpt_not_ready() };
        }
        let dst = unsafe { rtw_rust_hal_com_customer_str_buf(dvobj).add(CUSTOMER_STR_RPT_LEN) };
        unsafe {
            _rtw_memcpy(
                dst as *mut c_void,
                data as *const c_void,
                CUSTOMER_STR_RPT_2_LEN as u32,
            );
        }
        unsafe { rtw_rust_hal_com_customer_str_sctx_done(dvobj) };
    } else {
        unsafe { rtw_rust_hal_com_warn_customer_str_sctx_not_set() };
    }

    unsafe { rtw_rust_hal_com_exit_critical_mutex(mutex) };
    SUCCESS
}
