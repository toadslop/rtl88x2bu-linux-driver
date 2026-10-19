// SPDX-License-Identifier: GPL-2.0
//! W3-142: kernel SMS4 frame encrypt/decrypt entry points (core/rtw_wapi_sms4_rest.c).

#![allow(non_snake_case, unreachable_code)]

use core::ffi::c_void;

use super::{U8, U32};

const _SUCCESS: U32 = 1;
const _FAIL: U32 = 0;

extern "C" {
    static rtw_rust_wep_off_xmit_frame_buf_addr: usize;
    static rtw_rust_wapi_frame_off_WapiSupport: usize;
    static rtw_rust_wapi_frame_off_wapiInfo: usize;
    static rtw_rust_wapi_frame_off_wapi_bWapiEnable: usize;
}

#[inline]
unsafe fn adapter_wapi_info(padapter: *mut u8) -> *mut u8 {
    unsafe { padapter.add(rtw_rust_wapi_frame_off_wapiInfo) }
}

#[no_mangle]
pub extern "C" fn SecSWSMS4Encryption(_padapter: *mut c_void, _pxmitframe: *mut U8) {
    if _padapter.is_null() || _pxmitframe.is_null() {
        return;
    }
    /* Matches core/rtw_wapi_sms4_rest.c early return (frame path disabled). */
}

#[no_mangle]
pub extern "C" fn SecSWSMS4Decryption(
    _padapter: *mut c_void,
    _precv_frame: *mut U8,
    _precv_priv: *mut c_void,
) -> U8 {
    /* Matches core/rtw_wapi_sms4_rest.c early return 0. */
    0
}

#[no_mangle]
pub extern "C" fn rtw_sms4_encrypt(padapter: *mut c_void, pxmitframe: *mut U8) -> U32 {
    if padapter.is_null() || pxmitframe.is_null() {
        return _FAIL;
    }
    let padapter = padapter as *mut u8;
    let wapi = unsafe { adapter_wapi_info(padapter) };
    if unsafe { *padapter.add(rtw_rust_wapi_frame_off_WapiSupport) } == 0
        || unsafe { *wapi.add(rtw_rust_wapi_frame_off_wapi_bWapiEnable) } == 0
    {
        return _FAIL;
    }
    let buf_addr = unsafe {
        *((pxmitframe.add(rtw_rust_wep_off_xmit_frame_buf_addr)) as *const *mut U8)
    };
    if buf_addr.is_null() {
        return _FAIL;
    }
    SecSWSMS4Encryption(padapter as *mut c_void, pxmitframe);
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn rtw_sms4_decrypt(padapter: *mut c_void, _precvframe: *mut U8) -> U32 {
    if padapter.is_null() {
        return _FAIL;
    }
    let padapter = padapter as *mut u8;
    let wapi = unsafe { adapter_wapi_info(padapter) };
    if unsafe { *padapter.add(rtw_rust_wapi_frame_off_WapiSupport) } == 0
        || unsafe { *wapi.add(rtw_rust_wapi_frame_off_wapi_bWapiEnable) } == 0
    {
        return _FAIL;
    }
    /* Matches core/rtw_wapi_sms4_rest.c temporary HW-decrypt drop. */
    _FAIL
}
