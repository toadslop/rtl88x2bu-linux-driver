// SPDX-License-Identifier: GPL-2.0
// CONFIG_FW_C2H_REG helpers c2h_evt_clear / c2h_evt_read_88xx (W4-47).

use core::ffi::c_void;

type U8 = u8;
type S32 = i32;

const C2H_REG_LEN: usize = 16;
const REG_C2HEVT_MSG_NORMAL: u32 = 0x01a0;
const REG_C2HEVT_CMD_SEQ_88XX: u32 = 0x01a1;
const REG_C2HEVT_CMD_LEN_88XX: u32 = 0x01ae;
const REG_C2HEVT_CLEAR: u32 = 0x01af;
const C2H_EVT_HOST_CLOSE: U8 = 0x00;
const C2H_EVT_FW_CLOSE: U8 = 0xff;
const SUCCESS: S32 = 1;
const FAIL: S32 = 0;

extern "C" {
    fn rtw_rust_hal_com_read8(adapter: *mut c_void, addr: u32) -> U8;
    fn rtw_rust_hal_com_write8(adapter: *mut c_void, addr: u32, val: U8);
}

#[inline]
fn set_c2h_id_88xx(buf: *mut U8, val: U8) {
    unsafe {
        *buf = val;
    }
}

#[inline]
fn set_c2h_seq_88xx(buf: *mut U8, val: U8) {
    unsafe {
        *buf.add(1) = val;
    }
}

#[inline]
fn set_c2h_plen_88xx(buf: *mut U8, val: U8) {
    unsafe {
        *buf.add(14) = val;
    }
}

#[inline]
fn c2h_plen_88xx(buf: *mut U8) -> U8 {
    unsafe { *buf.add(14) }
}

#[inline]
fn c2h_payload_88xx(buf: *mut U8) -> *mut U8 {
    unsafe { buf.add(2) }
}

#[no_mangle]
pub extern "C" fn c2h_evt_clear(adapter: *mut c_void) {
    unsafe {
        rtw_rust_hal_com_write8(adapter, REG_C2HEVT_CLEAR, C2H_EVT_HOST_CLOSE);
    }
}

#[no_mangle]
pub extern "C" fn c2h_evt_read_88xx(adapter: *mut c_void, buf: *mut U8) -> S32 {
    let mut ret = FAIL;

    if buf.is_null() {
        return ret;
    }

    let trigger = unsafe { rtw_rust_hal_com_read8(adapter, REG_C2HEVT_CLEAR) };

    if trigger == C2H_EVT_HOST_CLOSE {
        return ret;
    }
    if trigger != C2H_EVT_FW_CLOSE {
        c2h_evt_clear(adapter);
        return ret;
    }

    unsafe {
        core::ptr::write_bytes(buf, 0, C2H_REG_LEN);
    }

    let id = unsafe { rtw_rust_hal_com_read8(adapter, REG_C2HEVT_MSG_NORMAL) };
    let seq = unsafe { rtw_rust_hal_com_read8(adapter, REG_C2HEVT_CMD_SEQ_88XX) };
    let plen = unsafe { rtw_rust_hal_com_read8(adapter, REG_C2HEVT_CMD_LEN_88XX) };

    set_c2h_id_88xx(buf, id);
    set_c2h_seq_88xx(buf, seq);
    set_c2h_plen_88xx(buf, plen);

    let plen_val = c2h_plen_88xx(buf);
    let payload = c2h_payload_88xx(buf);
    for i in 0..plen_val {
        let byte = unsafe { rtw_rust_hal_com_read8(adapter, REG_C2HEVT_MSG_NORMAL + 2 + i as u32) };
        unsafe {
            *payload.add(i as usize) = byte;
        }
    }

    ret = SUCCESS;
    c2h_evt_clear(adapter);
    ret
}
