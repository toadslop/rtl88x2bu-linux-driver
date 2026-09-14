// SPDX-License-Identifier: GPL-2.0
//! W3-82 `rtw_check_restore_rf18` — Rust port of `core/rtw_ap_rf18_restore.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;
type Adapter = c_void;

const _TRUE: U8 = 1;
const _FALSE: U8 = 0;

#[cfg(host_ap_rf18_restore_test)]
#[repr(C)]
struct HalDataHost {
    current_channel: U8,
}

#[cfg(host_ap_rf18_restore_test)]
#[repr(C)]
struct MlmeExtHost {
    cur_channel: U8,
    cur_ch_offset: U8,
    cur_bwmode: U8,
}

#[cfg(host_ap_rf18_restore_test)]
#[repr(C)]
struct AdapterHost {
    hal_data: HalDataHost,
    mlmeextpriv: MlmeExtHost,
}

#[cfg(host_ap_rf18_restore_test)]
extern "C" {
    fn rtw_hal_read_rfreg(padapter: *mut AdapterHost, path: U32, addr: U32, mask: U32) -> U32;
    fn rtw_mi_get_ch_setting_union(
        padapter: *mut AdapterHost,
        ch: *mut U8,
        bw: *mut U8,
        offset: *mut U8,
    ) -> U8;
    fn set_channel_bwmode(padapter: *mut AdapterHost, ch: U8, offset: U8, bw: U8);
}

#[cfg(not(host_ap_rf18_restore_test))]
extern "C" {
    fn rtw_hal_read_rfreg(padapter: *mut Adapter, path: U32, addr: U32, mask: U32) -> U32;
    fn rtw_mi_get_ch_setting_union(
        padapter: *mut Adapter,
        ch: *mut U8,
        bw: *mut U8,
        offset: *mut U8,
    ) -> U8;
    fn set_channel_bwmode(padapter: *mut Adapter, ch: U8, offset: U8, bw: U8);
    fn rtw_rust_rf18_mlme_cur_channel(padapter: *mut Adapter) -> U8;
    fn rtw_rust_rf18_mlme_cur_ch_offset(padapter: *mut Adapter) -> U8;
    fn rtw_rust_rf18_mlme_cur_bwmode(padapter: *mut Adapter) -> U8;
    fn rtw_rust_rf18_clear_current_channel(padapter: *mut Adapter);
}

#[cfg(host_ap_rf18_restore_test)]
fn restore_rf18_host(padapter: *mut AdapterHost) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        let padapter = &mut *padapter;
        let mut setchbw = _FALSE;
        let mut reg = rtw_hal_read_rfreg(padapter, 0, 0x18, 0x3FF);
        if (reg & 0xFF) == 0 {
            setchbw = _TRUE;
        }
        reg = rtw_hal_read_rfreg(padapter, 1, 0x18, 0x3FF);
        if (reg & 0xFF) == 0 {
            setchbw = _TRUE;
        }
        if setchbw == _FALSE {
            return;
        }
        let mut union_ch: U8 = 0;
        let mut union_bw: U8 = 0;
        let mut union_offset: U8 = 0;
        if rtw_mi_get_ch_setting_union(padapter, &mut union_ch, &mut union_bw, &mut union_offset)
            == _FALSE
        {
            union_ch = padapter.mlmeextpriv.cur_channel;
            union_offset = padapter.mlmeextpriv.cur_ch_offset;
            union_bw = padapter.mlmeextpriv.cur_bwmode;
        }
        padapter.hal_data.current_channel = 0;
        set_channel_bwmode(padapter, union_ch, union_offset, union_bw);
    }
}

#[cfg(not(host_ap_rf18_restore_test))]
fn restore_rf18_kernel(padapter: *mut Adapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        let mut setchbw = _FALSE;
        let mut reg = rtw_hal_read_rfreg(padapter, 0, 0x18, 0x3FF);
        if (reg & 0xFF) == 0 {
            setchbw = _TRUE;
        }
        reg = rtw_hal_read_rfreg(padapter, 1, 0x18, 0x3FF);
        if (reg & 0xFF) == 0 {
            setchbw = _TRUE;
        }
        if setchbw == _FALSE {
            return;
        }
        let mut union_ch: U8 = 0;
        let mut union_bw: U8 = 0;
        let mut union_offset: U8 = 0;
        if rtw_mi_get_ch_setting_union(padapter, &mut union_ch, &mut union_bw, &mut union_offset)
            == _FALSE
        {
            union_ch = rtw_rust_rf18_mlme_cur_channel(padapter);
            union_offset = rtw_rust_rf18_mlme_cur_ch_offset(padapter);
            union_bw = rtw_rust_rf18_mlme_cur_bwmode(padapter);
        }
        rtw_rust_rf18_clear_current_channel(padapter);
        set_channel_bwmode(padapter, union_ch, union_offset, union_bw);
    }
}

#[no_mangle]
pub extern "C" fn rtw_check_restore_rf18(padapter: *mut Adapter) {
    #[cfg(host_ap_rf18_restore_test)]
    restore_rf18_host(padapter as *mut AdapterHost);
    #[cfg(not(host_ap_rf18_restore_test))]
    restore_rf18_kernel(padapter);
}
