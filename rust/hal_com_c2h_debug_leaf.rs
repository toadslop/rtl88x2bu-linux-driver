// SPDX-License-Identifier: GPL-2.0
// C2H defeature debug handler (W4-56 PR1).

use core::ffi::c_void;

type U8 = u8;
type S32 = i32;

const SUCCESS: S32 = 1;
const FAIL: S32 = 0;

const DEFEATURE_DBG_LEN: usize = 1;

extern "C" {
    fn rtw_rust_hal_com_log_defeature_bytes(data: *mut U8, len: U8);
    fn rtw_rust_hal_com_warn_c2h_short_len(func: *const u8, len: U8, expect: i32);
}

static FN_C2H_DEFEATURE: &[u8] = b"c2h_defeature_dbg_hdl\0";

#[no_mangle]
pub extern "C" fn c2h_defeature_dbg_hdl(adapter: *mut c_void, data: *mut U8, len: U8) -> S32 {
    let _ = adapter;
    if (len as usize) < DEFEATURE_DBG_LEN {
        unsafe {
            rtw_rust_hal_com_warn_c2h_short_len(
                FN_C2H_DEFEATURE.as_ptr(),
                len,
                DEFEATURE_DBG_LEN as i32,
            );
        }
        return FAIL;
    }
    unsafe { rtw_rust_hal_com_log_defeature_bytes(data, len) };
    SUCCESS
}
