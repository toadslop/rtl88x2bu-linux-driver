// SPDX-License-Identifier: GPL-2.0
//! W3-57 op-class dump formatters — Rust port of `core/rtw_rf_op_class_dump.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe,
    unused_variables,
    static_mut_refs
)]

#[cfg(host_rf_op_class_dump_test)]
use std::os::raw::{c_char, c_int, c_void};

#[cfg(not(host_rf_op_class_dump_test))]
use core::ffi::{c_char, c_int, c_void};

const UNSPECIFIED_MBM: i16 = 32767;

const CHANNEL_WIDTH_20: u8 = 0;
const CHANNEL_WIDTH_40: u8 = 1;
const CHANNEL_WIDTH_80: u8 = 2;
const CHANNEL_WIDTH_160: u8 = 3;

const HAL_PRIME_CHNL_OFFSET_DONT_CARE: u8 = 0;
const HAL_PRIME_CHNL_OFFSET_LOWER: u8 = 1;
const HAL_PRIME_CHNL_OFFSET_UPPER: u8 = 2;

const BAND_MAX: usize = 2;
const OPC_BW_NUM: usize = 6;
const MAX_CHANNEL_NUM_OF_BAND: usize = 28;

const OPC_BW20: i32 = 0;
const OPC_BW40PLUS: i32 = 1;
const OPC_BW40MINUS: i32 = 2;
const OPC_BW80: i32 = 3;
const OPC_BW160: i32 = 4;
const OPC_BW80P80: i32 = 5;

#[repr(C)]
pub struct OpClassT {
    pub class_id: u8,
    pub band: i32,
    pub bw: i32,
    pub len_ch_attr: *const u8,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct OpChT {
    pub ch: u8,
    pub bitfield: u8,
    pub max_txpwr: i16,
}

impl OpChT {
    fn static_non_op(self) -> bool {
        (self.bitfield & 0x1) != 0
    }
    fn no_ir(self) -> bool {
        (self.bitfield & 0x2) != 0
    }
}

#[repr(C)]
pub struct OpClassPrefT {
    pub class_id: u8,
    pub band: i32,
    pub bw: i32,
    pub ch_num: u8,
    pub op_ch_num: u8,
    pub ir_ch_num: u8,
    pub chs: [OpChT; MAX_CHANNEL_NUM_OF_BAND],
}

#[repr(C)]
pub struct RfCtlT {
    pub country_ent: *const c_void,
    pub channel_set: [RtChannelInfo; MAX_CHANNEL_NUM],
    pub spt_op_class_ch: *mut *mut OpClassPrefT,
    pub cap_spt_op_class_num: u8,
    pub reg_spt_op_class_num: u8,
    pub cur_spt_op_class_num: u8,
}

#[repr(C)]
pub struct RtChannelInfo {
    pub channel_num: u8,
    pub flags: u8,
}

const MAX_CHANNEL_NUM: usize = 59;

#[cfg(not(host_rf_op_class_dump_test))]
mod kernel {
    use super::*;

    extern "C" {
        pub fn rtw_rust_opc_dump_print_sel(sel: *mut c_void, line: *const c_char);
        pub fn rtw_rust_opc_dump_spt_entry(rfctl: *mut c_void, idx: u8) -> *mut OpClassPrefT;
        pub fn rtw_rust_opc_dump_cap_num(rfctl: *mut c_void) -> u8;
        pub fn rtw_rust_opc_dump_reg_num(rfctl: *mut c_void) -> u8;
        pub fn rtw_rust_opc_dump_cur_num(rfctl: *mut c_void) -> u8;
    }
}

fn dump_spt_entries(sel: *mut c_void, rfctl: *mut c_void, show_snon_ocp: bool, show_no_ir: bool, detail: bool) {
    let n = unsafe { global_op_class_num as usize };
    for i in 0..n {
        #[cfg(host_rf_op_class_dump_test)]
        let pref = {
            let rf = unsafe { &*(rfctl as *mut RfCtlT) };
            unsafe { *rf.spt_op_class_ch.add(i) }
        };
        #[cfg(not(host_rf_op_class_dump_test))]
        let pref = unsafe { kernel::rtw_rust_opc_dump_spt_entry(rfctl, i as u8) };
        if pref.is_null() {
            continue;
        }
        dump_opc_pref_single(sel, unsafe { &*pref }, show_snon_ocp, show_no_ir, detail);
    }
}

fn dump_op_count(sel: *mut c_void, rfctl: *mut c_void, which: u8) {
    #[cfg(host_rf_op_class_dump_test)]
    let num = {
        let rf = unsafe { &*(rfctl as *mut RfCtlT) };
        match which {
            0 => rf.cap_spt_op_class_num,
            1 => rf.reg_spt_op_class_num,
            _ => rf.cur_spt_op_class_num,
        }
    };
    #[cfg(not(host_rf_op_class_dump_test))]
    let num = unsafe {
        match which {
            0 => kernel::rtw_rust_opc_dump_cap_num(rfctl),
            1 => kernel::rtw_rust_opc_dump_reg_num(rfctl),
            _ => kernel::rtw_rust_opc_dump_cur_num(rfctl),
        }
    };
    let mut line = [0i8; 64];
    unsafe {
        snprintf(
            line.as_mut_ptr(),
            line.len(),
            b"op_class number:%d\n\0".as_ptr() as *const c_char,
            num as c_int,
        );
        print_sel(sel, line.as_ptr());
    }
}

extern "C" {
    static global_op_class: [OpClassT; 19];
    static global_op_class_num: c_int;
    static _band_str: [*const c_char; 3];
    static _opc_bw_str: [*const c_char; OPC_BW_NUM];

    fn rtw_get_center_ch(ch: u8, bw: u8, offset: u8) -> u8;
    fn rtw_get_op_chs_by_cch_bw(cch: u8, bw: u8, op_chs: *mut *mut u8, op_ch_num: *mut u8) -> u8;
    fn snprintf(s: *mut c_char, n: usize, fmt: *const c_char, ...) -> c_int;
}

#[cfg(host_rf_op_class_dump_test)]
#[repr(C)]
pub struct HostSelCapture {
    pub buf: [u8; 8192],
    pub len: usize,
}

#[cfg(host_rf_op_class_dump_test)]
extern "C" {
    static mut host_sel_out: HostSelCapture;
}

fn opc_ch_list_len(opc: &OpClassT) -> u8 {
    unsafe { *opc.len_ch_attr }
}

fn opc_ch_list_ch(opc: &OpClassT, i: u8) -> u8 {
    unsafe { *opc.len_ch_attr.add(i as usize + 1) }
}

unsafe fn band_str(band: i32) -> *const c_char {
    unsafe {
        let idx = if band >= 0 && (band as usize) < BAND_MAX {
            band as usize
        } else {
            BAND_MAX
        };
        let s = _band_str[idx];
        if s.is_null() {
            b"?\0".as_ptr() as *const c_char
        } else {
            s
        }
    }
}

unsafe fn opc_bw_str(bw: i32) -> *const c_char {
    unsafe {
        if bw >= 0 && (bw as usize) < OPC_BW_NUM {
            let s = _opc_bw_str[bw as usize];
            if s.is_null() {
                b"N/A\0".as_ptr() as *const c_char
            } else {
                s
            }
        } else {
            b"N/A\0".as_ptr() as *const c_char
        }
    }
}

fn print_sel(sel: *mut c_void, line: *const c_char) {
    #[cfg(host_rf_op_class_dump_test)]
    unsafe {
        let mut len = 0usize;
        while *line.add(len) != 0 {
            len += 1;
        }
        let remain = host_sel_out.buf.len().saturating_sub(host_sel_out.len);
        let n = core::cmp::min(len, remain.saturating_sub(1));
        core::ptr::copy_nonoverlapping(line as *const u8, host_sel_out.buf.as_mut_ptr().add(host_sel_out.len), n);
        host_sel_out.len += n;
    }
    #[cfg(not(host_rf_op_class_dump_test))]
    unsafe {
        kernel::rtw_rust_opc_dump_print_sel(sel, line);
    }
}

#[cfg(any(host_rf_op_class_dump_test, config_rtw_debug))]
#[no_mangle]
pub extern "C" fn dbg_global_op_class_validate(gid: u8) -> bool {
    let idx = gid as usize;
    if idx >= unsafe { global_op_class_num as usize } {
        return false;
    }
    let ent = unsafe { &global_op_class[idx] };
    let (bw, offset) = match ent.bw {
        OPC_BW20 => (CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE),
        OPC_BW40PLUS => (CHANNEL_WIDTH_40, HAL_PRIME_CHNL_OFFSET_LOWER),
        OPC_BW40MINUS => (CHANNEL_WIDTH_40, HAL_PRIME_CHNL_OFFSET_UPPER),
        OPC_BW80 => (CHANNEL_WIDTH_80, HAL_PRIME_CHNL_OFFSET_DONT_CARE),
        OPC_BW160 => (CHANNEL_WIDTH_160, HAL_PRIME_CHNL_OFFSET_DONT_CARE),
        OPC_BW80P80 | _ => return false,
    };

    let mut ret = true;
    for i in 0..opc_ch_list_len(ent) {
        let ch = opc_ch_list_ch(ent, i);
        let cch = unsafe { rtw_get_center_ch(ch, bw, offset) };
        if cch == 0 {
            ret = false;
            continue;
        }
        let mut op_chs: *mut u8 = core::ptr::null_mut();
        let mut op_ch_num: u8 = 0;
        if unsafe { rtw_get_op_chs_by_cch_bw(cch, bw, &mut op_chs, &mut op_ch_num) } == 0 {
            ret = false;
            continue;
        }
        let mut found = false;
        for k in 0..op_ch_num {
            if unsafe { *op_chs.add(k as usize) } == ch {
                found = true;
                break;
            }
        }
        if !found {
            ret = false;
        }
    }
    ret
}

fn dump_op_class_ch_title(sel: *mut c_void) {
    unsafe {
        print_sel(
            sel,
            b"class band bw      ch_list\n\0".as_ptr() as *const c_char,
        );
    }
}

fn dump_global_op_class_ch_single(sel: *mut c_void, gid: u8) {
    let ent = unsafe { &global_op_class[gid as usize] };
    let mut buf = [0i8; 100];
    let mut pos = 0usize;
    for i in 0..opc_ch_list_len(ent) {
        let ch = opc_ch_list_ch(ent, i);
        let n = unsafe {
            snprintf(
                buf.as_mut_ptr().add(pos),
                buf.len() - pos,
                b" %u\0".as_ptr() as *const c_char,
                ch as c_int,
            )
        };
        if n > 0 {
            pos += n as usize;
        }
    }
    buf[pos] = 0;
    let mut line = [0i8; 256];
    unsafe {
        snprintf(
            line.as_mut_ptr(),
            line.len(),
            b"%5u %4s %7s%s\n\0".as_ptr() as *const c_char,
            ent.class_id as c_int,
            band_str(ent.band),
            opc_bw_str(ent.bw),
            buf.as_ptr(),
        );
        print_sel(sel, line.as_ptr());
    }
}

#[no_mangle]
pub extern "C" fn dump_global_op_class(sel: *mut c_void) {
    dump_op_class_ch_title(sel);
    let n = unsafe { global_op_class_num as usize };
    for i in 0..n {
        dump_global_op_class_ch_single(sel, i as u8);
    }
}

fn dump_opc_pref_single(
    sel: *mut c_void,
    opc_pref: &OpClassPrefT,
    show_snon_ocp: bool,
    show_no_ir: bool,
    detail: bool,
) {
    if !show_snon_ocp && opc_pref.op_ch_num == 0 {
        return;
    }
    if !show_no_ir && opc_pref.ir_ch_num == 0 {
        return;
    }

    let mut buf = [0i8; 256];
    let mut pos = 0usize;
    for ch in opc_pref.chs.iter() {
        if ch.ch == 0 {
            break;
        }
        if (show_snon_ocp || !ch.static_non_op()) && (show_no_ir || !ch.no_ir()) {
            let fmt = if detail {
                b" %4u\0".as_ptr() as *const c_char
            } else {
                b" %u\0".as_ptr() as *const c_char
            };
            let n = unsafe {
                snprintf(buf.as_mut_ptr().add(pos), buf.len() - pos, fmt, ch.ch as c_int)
            };
            if n > 0 {
                pos += n as usize;
            }
        }
    }
    buf[pos] = 0;

    let mut line = [0i8; 512];
    unsafe {
        snprintf(
            line.as_mut_ptr(),
            line.len(),
            b"%5u %4s %7s%s\n\0".as_ptr() as *const c_char,
            opc_pref.class_id as c_int,
            band_str(opc_pref.band),
            opc_bw_str(opc_pref.bw),
            buf.as_ptr(),
        );
        print_sel(sel, line.as_ptr());
    }

    if !detail {
        return;
    }

    pos = 0;
    for ch in opc_pref.chs.iter() {
        if ch.ch == 0 {
            break;
        }
        if (show_snon_ocp || !ch.static_non_op()) && (show_no_ir || !ch.no_ir()) {
            let n = unsafe {
                snprintf(
                    buf.as_mut_ptr().add(pos),
                    buf.len() - pos,
                    b"   %c%c\0".as_ptr() as *const c_char,
                    if ch.no_ir() { b' ' } else { b'I' } as c_int,
                    if ch.static_non_op() { b' ' } else { b'E' } as c_int,
                )
            };
            if n > 0 {
                pos += n as usize;
            }
        }
    }
    buf[pos] = 0;
    unsafe {
        snprintf(
            line.as_mut_ptr(),
            line.len(),
            b"                  %s\n\0".as_ptr() as *const c_char,
            buf.as_ptr(),
        );
        print_sel(sel, line.as_ptr());
    }

    pos = 0;
    for ch in opc_pref.chs.iter() {
        if ch.ch == 0 {
            break;
        }
        if (show_snon_ocp || !ch.static_non_op()) && (show_no_ir || !ch.no_ir()) {
            let n = if ch.max_txpwr == UNSPECIFIED_MBM {
                unsafe {
                    snprintf(
                        buf.as_mut_ptr().add(pos),
                        buf.len() - pos,
                        b"     \0".as_ptr() as *const c_char,
                    )
                }
            } else {
                unsafe {
                    snprintf(
                        buf.as_mut_ptr().add(pos),
                        buf.len() - pos,
                        b" %4d\0".as_ptr() as *const c_char,
                        ch.max_txpwr as c_int,
                    )
                }
            };
            if n > 0 {
                pos += n as usize;
            }
        }
    }
    buf[pos] = 0;
    unsafe {
        snprintf(
            line.as_mut_ptr(),
            line.len(),
            b"                  %s\n\0".as_ptr() as *const c_char,
            buf.as_ptr(),
        );
        print_sel(sel, line.as_ptr());
    }
}

#[no_mangle]
pub extern "C" fn dump_cap_spt_op_class_ch(sel: *mut c_void, rfctl: *mut RfCtlT, detail: bool) {
    if rfctl.is_null() {
        return;
    }
    dump_op_class_ch_title(sel);
    dump_spt_entries(sel, rfctl as *mut c_void, true, true, detail);
    dump_op_count(sel, rfctl as *mut c_void, 0);
}

#[no_mangle]
pub extern "C" fn dump_reg_spt_op_class_ch(sel: *mut c_void, rfctl: *mut RfCtlT, detail: bool) {
    if rfctl.is_null() {
        return;
    }
    dump_op_class_ch_title(sel);
    dump_spt_entries(sel, rfctl as *mut c_void, false, true, detail);
    dump_op_count(sel, rfctl as *mut c_void, 1);
}

#[no_mangle]
pub extern "C" fn dump_cur_spt_op_class_ch(sel: *mut c_void, rfctl: *mut RfCtlT, detail: bool) {
    if rfctl.is_null() {
        return;
    }
    dump_op_class_ch_title(sel);
    dump_spt_entries(sel, rfctl as *mut c_void, false, false, detail);
    dump_op_count(sel, rfctl as *mut c_void, 2);
}
