// SPDX-License-Identifier: GPL-2.0
// CONFIG_RTW_CUSTOMER_STR sync read/write API (W4-86).

use core::ffi::c_void;

#[path = "ffi_osdep_memcpy.rs"]
mod ffi_osdep_memcpy;
use ffi_osdep_memcpy::_rtw_memcpy;

type U8 = u8;
type U32 = u32;
type S32 = i32;

const SUCCESS: S32 = 1;
const FAIL: S32 = 0;
const TRUE: i32 = 1;

const RTW_CUSTOMER_STR_LEN: u32 = 16;
const RTW_SCTX_DONE_SUCCESS: i32 = 0;
const SCTX_WAIT_MS: i32 = 2 * 1000;

const H2C_CUSTOMER_STR_W1: U8 = 0xC6;
const H2C_CUSTOMER_STR_W2: U8 = 0xC7;
const H2C_CUSTOMER_STR_W3: U8 = 0xC8;
const H2C_CUSTOMER_STR_W1_LEN: U32 = 7;
const H2C_CUSTOMER_STR_W2_LEN: U32 = 7;
const H2C_CUSTOMER_STR_W3_LEN: U32 = 5;

static FN_READ: &[u8] = b"rtw_hal_customer_str_read\0";
static FN_WRITE: &[u8] = b"rtw_hal_customer_str_write\0";
static FN_H2C_WRITE: &[u8] = b"rtw_hal_h2c_customer_str_write\0";

extern "C" {
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_customer_str_mutex(dvobj: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_customer_str_sctx(dvobj: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_customer_str_sctx_set(dvobj: *mut c_void, sctx: *mut c_void);
    fn rtw_rust_hal_com_customer_str_buf(dvobj: *mut c_void) -> *mut U8;
    fn rtw_rust_hal_com_enter_critical_mutex(mutex: *mut c_void);
    fn rtw_rust_hal_com_exit_critical_mutex(mutex: *mut c_void);
    fn rtw_rust_hal_com_sctx_status(sctx: *mut c_void) -> i32;
    fn rtw_rust_hal_com_warn_customer_str_sync_ongoing(func: *const U8);
    fn rtw_rust_hal_com_warn_customer_str_cmd_fail(func: *const U8, msg: *const U8);
    fn rtw_rust_hal_com_warn_customer_str_h2c_w_fail(func: *const U8, part: *const U8);
    fn rtw_rust_hal_com_warn_customer_str_write_check_fail(func: *const U8);
    fn rtw_rust_hal_com_info_dump_customer_str(title: *const U8, buf: *const U8);
    fn rtw_sctx_init(sctx: *mut c_void, timeout_ms: i32);
    fn rtw_sctx_wait(sctx: *mut c_void, msg: *const U8) -> S32;
    fn rtw_customer_str_req_cmd(adapter: *mut c_void) -> U8;
    fn rtw_customer_str_write_cmd(adapter: *mut c_void, cs: *const U8) -> U8;
    fn rtw_hal_fill_h2c_cmd(
        adapter: *mut c_void,
        element_id: U8,
        cmd_len: U32,
        cmd_buf: *mut U8,
    ) -> S32;
    fn _rtw_memcmp(a: *const c_void, b: *const c_void, sz: U32) -> i32;
}

/// Stack slot large enough for `struct submit_ctx` (opaque to Rust).
type SctxSlot = [u8; 64];

#[inline]
fn h2c_en_byte() -> U8 {
    1
}

#[no_mangle]
pub extern "C" fn rtw_hal_customer_str_read(adapter: *mut c_void, cs: *mut U8) -> S32 {
    let dvobj = unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) };
    let mutex = unsafe { rtw_rust_hal_com_customer_str_mutex(dvobj) };
    let mut sctx: SctxSlot = [0; 64];
    let mut ret = SUCCESS;

    unsafe { rtw_rust_hal_com_enter_critical_mutex(mutex) };
    if !unsafe { rtw_rust_hal_com_customer_str_sctx(dvobj) }.is_null() {
        ret = FAIL;
    } else {
        unsafe { rtw_sctx_init(sctx.as_mut_ptr() as *mut c_void, SCTX_WAIT_MS) };
        unsafe {
            rtw_rust_hal_com_customer_str_sctx_set(dvobj, sctx.as_mut_ptr() as *mut c_void);
        }
    }
    unsafe { rtw_rust_hal_com_exit_critical_mutex(mutex) };

    if ret == FAIL {
        unsafe { rtw_rust_hal_com_warn_customer_str_sync_ongoing(FN_READ.as_ptr()) };
        return ret;
    }

    ret = unsafe { rtw_customer_str_req_cmd(adapter) } as S32;
    if ret != SUCCESS {
        unsafe {
            rtw_rust_hal_com_warn_customer_str_cmd_fail(
                FN_READ.as_ptr(),
                b"read cmd fail\0".as_ptr(),
            );
        }
        unsafe { rtw_rust_hal_com_enter_critical_mutex(mutex) };
        unsafe { rtw_rust_hal_com_customer_str_sctx_set(dvobj, core::ptr::null_mut()) };
        unsafe { rtw_rust_hal_com_exit_critical_mutex(mutex) };
        return ret;
    }

    unsafe { rtw_sctx_wait(sctx.as_mut_ptr() as *mut c_void, FN_READ.as_ptr()) };

    unsafe { rtw_rust_hal_com_enter_critical_mutex(mutex) };
    unsafe { rtw_rust_hal_com_customer_str_sctx_set(dvobj, core::ptr::null_mut()) };
    if unsafe { rtw_rust_hal_com_sctx_status(sctx.as_mut_ptr() as *mut c_void) }
        == RTW_SCTX_DONE_SUCCESS
    {
        unsafe {
            _rtw_memcpy(
                cs as *mut c_void,
                rtw_rust_hal_com_customer_str_buf(dvobj) as *const c_void,
                RTW_CUSTOMER_STR_LEN,
            );
        }
    } else {
        ret = FAIL;
    }
    unsafe { rtw_rust_hal_com_exit_critical_mutex(mutex) };
    ret
}

#[no_mangle]
pub extern "C" fn rtw_hal_h2c_customer_str_write(adapter: *mut c_void, cs: *const U8) -> S32 {
    let mut h2c_w1 = [0u8; 7];
    let mut h2c_w2 = [0u8; 7];
    let mut h2c_w3 = [0u8; 5];

    h2c_w1[0] = h2c_en_byte();
    unsafe {
        _rtw_memcpy(
            h2c_w1[1..].as_mut_ptr() as *mut c_void,
            cs as *const c_void,
            6,
        );
    }

    h2c_w2[0] = h2c_en_byte();
    unsafe {
        _rtw_memcpy(
            h2c_w2[1..].as_mut_ptr() as *mut c_void,
            cs.add(6) as *const c_void,
            6,
        );
    }

    h2c_w3[0] = h2c_en_byte();
    unsafe {
        _rtw_memcpy(
            h2c_w3[1..].as_mut_ptr() as *mut c_void,
            cs.add(12) as *const c_void,
            4,
        );
    }

    let mut ret = unsafe {
        rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_CUSTOMER_STR_W1,
            H2C_CUSTOMER_STR_W1_LEN,
            h2c_w1.as_mut_ptr(),
        )
    };
    if ret != SUCCESS {
        unsafe {
            rtw_rust_hal_com_warn_customer_str_h2c_w_fail(FN_H2C_WRITE.as_ptr(), b"w1\0".as_ptr());
        }
        return ret;
    }

    ret = unsafe {
        rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_CUSTOMER_STR_W2,
            H2C_CUSTOMER_STR_W2_LEN,
            h2c_w2.as_mut_ptr(),
        )
    };
    if ret != SUCCESS {
        unsafe {
            rtw_rust_hal_com_warn_customer_str_h2c_w_fail(FN_H2C_WRITE.as_ptr(), b"w2\0".as_ptr());
        }
        return ret;
    }

    ret = unsafe {
        rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_CUSTOMER_STR_W3,
            H2C_CUSTOMER_STR_W3_LEN,
            h2c_w3.as_mut_ptr(),
        )
    };
    if ret != SUCCESS {
        unsafe {
            rtw_rust_hal_com_warn_customer_str_h2c_w_fail(FN_H2C_WRITE.as_ptr(), b"w3\0".as_ptr());
        }
    }
    ret
}

#[no_mangle]
pub extern "C" fn rtw_hal_customer_str_write(adapter: *mut c_void, cs: *const U8) -> S32 {
    let dvobj = unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) };
    let mutex = unsafe { rtw_rust_hal_com_customer_str_mutex(dvobj) };
    let mut sctx: SctxSlot = [0; 64];
    let mut ret = SUCCESS;

    unsafe { rtw_rust_hal_com_enter_critical_mutex(mutex) };
    if !unsafe { rtw_rust_hal_com_customer_str_sctx(dvobj) }.is_null() {
        ret = FAIL;
    } else {
        unsafe { rtw_sctx_init(sctx.as_mut_ptr() as *mut c_void, SCTX_WAIT_MS) };
        unsafe {
            rtw_rust_hal_com_customer_str_sctx_set(dvobj, sctx.as_mut_ptr() as *mut c_void);
        }
    }
    unsafe { rtw_rust_hal_com_exit_critical_mutex(mutex) };

    if ret == FAIL {
        unsafe { rtw_rust_hal_com_warn_customer_str_sync_ongoing(FN_WRITE.as_ptr()) };
        return ret;
    }

    ret = unsafe { rtw_customer_str_write_cmd(adapter, cs) } as S32;
    if ret != SUCCESS {
        unsafe {
            rtw_rust_hal_com_warn_customer_str_cmd_fail(
                FN_WRITE.as_ptr(),
                b"write cmd fail\0".as_ptr(),
            );
        }
        unsafe { rtw_rust_hal_com_enter_critical_mutex(mutex) };
        unsafe { rtw_rust_hal_com_customer_str_sctx_set(dvobj, core::ptr::null_mut()) };
        unsafe { rtw_rust_hal_com_exit_critical_mutex(mutex) };
        return ret;
    }

    ret = unsafe { rtw_customer_str_req_cmd(adapter) } as S32;
    if ret != SUCCESS {
        unsafe {
            rtw_rust_hal_com_warn_customer_str_cmd_fail(
                FN_WRITE.as_ptr(),
                b"read cmd fail\0".as_ptr(),
            );
        }
        unsafe { rtw_rust_hal_com_enter_critical_mutex(mutex) };
        unsafe { rtw_rust_hal_com_customer_str_sctx_set(dvobj, core::ptr::null_mut()) };
        unsafe { rtw_rust_hal_com_exit_critical_mutex(mutex) };
        return ret;
    }

    unsafe { rtw_sctx_wait(sctx.as_mut_ptr() as *mut c_void, FN_WRITE.as_ptr()) };

    unsafe { rtw_rust_hal_com_enter_critical_mutex(mutex) };
    unsafe { rtw_rust_hal_com_customer_str_sctx_set(dvobj, core::ptr::null_mut()) };
    if unsafe { rtw_rust_hal_com_sctx_status(sctx.as_mut_ptr() as *mut c_void) }
        == RTW_SCTX_DONE_SUCCESS
    {
        if unsafe {
            _rtw_memcmp(
                cs as *const c_void,
                rtw_rust_hal_com_customer_str_buf(dvobj) as *const c_void,
                RTW_CUSTOMER_STR_LEN,
            )
        } != TRUE
        {
            unsafe { rtw_rust_hal_com_warn_customer_str_write_check_fail(FN_WRITE.as_ptr()) };
            unsafe {
                rtw_rust_hal_com_info_dump_customer_str(b"write req: \0".as_ptr(), cs);
            }
            unsafe {
                rtw_rust_hal_com_info_dump_customer_str(
                    b"read back: \0".as_ptr(),
                    rtw_rust_hal_com_customer_str_buf(dvobj),
                );
            }
            ret = FAIL;
        }
    } else {
        ret = FAIL;
    }
    unsafe { rtw_rust_hal_com_exit_critical_mutex(mutex) };
    ret
}