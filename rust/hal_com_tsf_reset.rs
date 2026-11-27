// SPDX-License-Identifier: GPL-2.0
// TSF reset H2C offload under CONFIG_TSF_RESET_OFFLOAD (W4-143).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;
type S32 = i32;

const H2C_RESET_TSF: U8 = 0xc0;
const REG_FW_RESET_TSF_CNT_0: U32 = 0x05fd;
const REG_FW_RESET_TSF_CNT_1: U32 = 0x05fc;
const HW_PORT0: U8 = 0;
const _SUCCESS: S32 = 1;
const _FAIL: S32 = 0;
const _FALSE: U8 = 0;

extern "C" {
    fn rtw_rust_hal_com_read8(adapter: *mut c_void, addr: U32) -> U8;
    fn rtw_hal_fill_h2c_cmd(
        adapter: *mut c_void,
        element_id: U8,
        cmd_len: U32,
        cmd_buf: *mut U8,
    ) -> S32;
    fn rtw_msleep_os(ms: U32);
    fn rtw_mi_buddy_scan_abort(adapter: *mut c_void, bwait: U8);
}

fn h2c_reset_tsf(adapter: *mut c_void, reset_port: U8) -> S32 {
    let buf = if reset_port == HW_PORT0 {
        [0x1u8, 0]
    } else {
        [0x0u8, 0x1]
    };
    let mut payload = buf;
    unsafe {
        rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_RESET_TSF,
            2,
            payload.as_mut_ptr(),
        )
    }
}

#[cfg(any(config_tsf_reset_offload, host_hal_com_tsf_reset_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_reset_tsf(adapter: *mut c_void, reset_port: U8) -> S32 {
    let reg_reset_tsf_cnt = if reset_port == HW_PORT0 {
        REG_FW_RESET_TSF_CNT_0
    } else {
        REG_FW_RESET_TSF_CNT_1
    };

    unsafe { rtw_mi_buddy_scan_abort(adapter, _FALSE) };
    let reset_cnt_before = unsafe { rtw_rust_hal_com_read8(adapter, reg_reset_tsf_cnt) };
    let mut reset_cnt_after = reset_cnt_before;
    let ret = h2c_reset_tsf(adapter, reset_port);
    if ret != _SUCCESS {
        return ret;
    }

    let mut loop_cnt: U8 = 0;
    while reset_cnt_after == reset_cnt_before && loop_cnt < 10 {
        unsafe { rtw_msleep_os(100) };
        loop_cnt = loop_cnt.wrapping_add(1);
        reset_cnt_after = unsafe { rtw_rust_hal_com_read8(adapter, reg_reset_tsf_cnt) };
    }

    if loop_cnt >= 10 {
        _FAIL
    } else {
        _SUCCESS
    }
}