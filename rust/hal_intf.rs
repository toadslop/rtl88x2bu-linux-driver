// SPDX-License-Identifier: GPL-2.0
//! HAL interface helpers — Rust port of `hal/hal_intf.c` (W4-07 TRX mode dumps, W4-08 RF/NSS dumps).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe
)]

#[cfg(not(any(host_hal_intf_trx_test, host_hal_intf_rf_nss_test)))]
use core::ffi::{c_char, c_void};

#[cfg(any(host_hal_intf_trx_test, host_hal_intf_rf_nss_test))]
use std::os::raw::c_char;

type U8 = u8;
type U32 = u32;

const RF_TYPE_MAX: usize = 16;

#[repr(C)]
pub struct HostHalIntfTrxFixture {
    pub trx_path_bmp: U8,
    pub rf_type: U8,
    pub NumTotalRFPath: U8,
    pub max_tx_cnt: U8,
    pub tx_nss: U8,
    pub rx_nss: U8,
    pub txpath_cap_num_nss: [U8; 4],
    pub _pad: [U8; 2],
    pub txpath: U32,
    pub rxpath: U32,
    pub txpath_nss: [U32; 4],
    pub txpath_num_nss: [U8; 4],
}

#[repr(C)]
struct RtwHalIntfTrxFields {
    trx_path_bmp: U8,
    rf_type: U8,
    NumTotalRFPath: U8,
    max_tx_cnt: U8,
    tx_nss: U8,
    rx_nss: U8,
    txpath_cap_num_nss: [U8; 4],
    txpath: U32,
    rxpath: U32,
    txpath_nss: [U32; 4],
    txpath_num_nss: [U8; 4],
}

#[cfg(any(host_hal_intf_trx_test, host_hal_intf_rf_nss_test))]
fn rf_type_to_rfpath_str_local(rf_type: U8) -> &'static [u8] {
    static RF_TBL: [&[u8]; RF_TYPE_MAX] = [
        b"RF_1T1R", b"RF_1T2R", b"RF_2T2R", b"RF_2T3R", b"RF_2T4R", b"RF_3T3R", b"RF_3T4R",
        b"RF_4T4R", b"RF_4T3R", b"RF_4T2R", b"RF_4T1R", b"RF_3T2R", b"RF_3T1R", b"RF_2T1R",
        b"RF_1T4R", b"RF_1T3R",
    ];
    if rf_type as usize >= RF_TYPE_MAX {
        return b"UNKNOWN";
    }
    RF_TBL[rf_type as usize]
}

#[cfg(not(any(host_hal_intf_trx_test, host_hal_intf_rf_nss_test)))]
fn rf_type_to_rfpath_str_local(rf_type: U8) -> &'static [u8] {
    extern "C" {
        fn rtw_rust_hal_intf_rf_type_str(rf_type: U8) -> *const c_char;
    }
    c_str_bytes(unsafe { rtw_rust_hal_intf_rf_type_str(rf_type) })
}

fn c_str_bytes(ptr: *const c_char) -> &'static [u8] {
    if ptr.is_null() {
        return b"";
    }
    unsafe {
        let mut len = 0usize;
        while *ptr.add(len) != 0 {
            len += 1;
        }
        core::slice::from_raw_parts(ptr as *const u8, len) // kernel + host
    }
}

fn push_byte(out: &mut [u8], pos: &mut usize, b: U8) {
    if *pos < out.len() {
        out[*pos] = b;
        *pos += 1;
    }
}

fn push_str(out: &mut [u8], pos: &mut usize, s: &[u8]) {
    for &b in s {
        push_byte(out, pos, b);
    }
}

fn push_u8_hex2(out: &mut [u8], pos: &mut usize, v: U8) {
    for shift in [4i32, 0] {
        let nib = ((v >> shift) & 0xf) as U8;
        push_byte(
            out,
            pos,
            if nib < 10 {
                b'0' + nib
            } else {
                b'a' + (nib - 10)
            },
        );
    }
}

fn push_u32_hex(out: &mut [u8], pos: &mut usize, v: U32) {
    let mut started = false;
    for shift in (0..32).step_by(4).rev() {
        let nib = ((v >> shift) & 0xf) as U8;
        if !started && nib == 0 && shift > 0 {
            continue;
        }
        started = true;
        push_byte(
            out,
            pos,
            if nib < 10 {
                b'0' + nib
            } else {
                b'a' + (nib - 10)
            },
        );
    }
    if !started {
        push_byte(out, pos, b'0');
    }
}

fn push_decimal(mut n: u32, out: &mut [u8], pos: &mut usize) {
    if n == 0 {
        push_byte(out, pos, b'0');
        return;
    }
    let mut digits = [0u8; 10];
    let mut nd = 0usize;
    while n > 0 {
        digits[nd] = b'0' + (n % 10) as u8;
        n /= 10;
        nd += 1;
    }
    while nd > 0 {
        nd -= 1;
        push_byte(out, pos, digits[nd]);
    }
}

fn push_u8_decimal(n: U8, out: &mut [u8], pos: &mut usize) {
    push_decimal(n as u32, out, pos);
}

fn push_i8_decimal(n: i8, out: &mut [u8], pos: &mut usize) {
    if n < 0 {
        push_byte(out, pos, b'-');
        push_decimal((-(n as i32)) as u32, out, pos);
    } else {
        push_decimal(n as u32, out, pos);
    }
}

#[repr(C)]
pub struct HostHalIntfRfPathFixture {
    pub ver_id_rf_type: U8,
    pub rf_reg_trx_path_bmp: U8,
    pub rf_reg_path_avail_num: U8,
    pub hal_spec_max_tx_cnt: U8,
    pub eeprom_trx_path_bmp: U8,
    pub eeprom_max_tx_cnt: U8,
    pub reg_trx_path_bmp: U8,
    pub tx_path_lmt: U8,
    pub rx_path_lmt: U8,
    pub hal_trx_path_bmp: U8,
    pub hal_max_tx_cnt: U8,
    pub hal_rf_type: U8,
    pub NumTotalRFPath: i8,
}

#[repr(C)]
pub struct HostHalIntfTrxNssFixture {
    pub hal_spec_tx_nss: i8,
    pub hal_spec_rx_nss: i8,
    pub reg_tx_nss: i8,
    pub reg_rx_nss: i8,
    pub hal_tx_nss: i8,
    pub hal_rx_nss: i8,
}

#[repr(C)]
struct RtwHalIntfRfPathFields {
    ver_id_rf_type: U8,
    rf_reg_trx_path_bmp: U8,
    rf_reg_path_avail_num: U8,
    hal_spec_max_tx_cnt: U8,
    eeprom_trx_path_bmp: U8,
    eeprom_max_tx_cnt: U8,
    reg_trx_path_bmp: U8,
    tx_path_lmt: U8,
    rx_path_lmt: U8,
    hal_trx_path_bmp: U8,
    hal_max_tx_cnt: U8,
    hal_rf_type: U8,
    NumTotalRFPath: i8,
}

#[repr(C)]
struct RtwHalIntfTrxNssFields {
    hal_spec_tx_nss: i8,
    hal_spec_rx_nss: i8,
    reg_tx_nss: i8,
    reg_rx_nss: i8,
    hal_tx_nss: i8,
    hal_rx_nss: i8,
}

fn format_rf_path(fx: &HostHalIntfRfPathFixture, out: &mut [u8]) -> i32 {
    let mut pos = 0usize;
    let ver_str = rf_type_to_rfpath_str_local(fx.ver_id_rf_type);
    let hal_str = rf_type_to_rfpath_str_local(fx.hal_rf_type);

    push_str(out, &mut pos, b"[RF_PATH] ver_id.RF_TYPE:");
    push_str(out, &mut pos, ver_str);
    push_byte(out, &mut pos, b'\n');

    push_str(out, &mut pos, b"[RF_PATH] HALSPEC's rf_reg_trx_path_bmp:0x");
    push_u8_hex2(out, &mut pos, fx.rf_reg_trx_path_bmp);
    push_str(out, &mut pos, b", rf_reg_path_avail_num:");
    push_u8_decimal(fx.rf_reg_path_avail_num, out, &mut pos);
    push_str(out, &mut pos, b", max_tx_cnt:");
    push_u8_decimal(fx.hal_spec_max_tx_cnt, out, &mut pos);
    push_byte(out, &mut pos, b'\n');

    push_str(out, &mut pos, b"[RF_PATH] PG's trx_path_bmp:0x");
    push_u8_hex2(out, &mut pos, fx.eeprom_trx_path_bmp);
    push_str(out, &mut pos, b", max_tx_cnt:");
    push_u8_decimal(fx.eeprom_max_tx_cnt, out, &mut pos);
    push_byte(out, &mut pos, b'\n');

    push_str(out, &mut pos, b"[RF_PATH] Registry's trx_path_bmp:0x");
    push_u8_hex2(out, &mut pos, fx.reg_trx_path_bmp);
    push_str(out, &mut pos, b", tx_path_lmt:");
    push_u8_decimal(fx.tx_path_lmt, out, &mut pos);
    push_str(out, &mut pos, b", rx_path_lmt:");
    push_u8_decimal(fx.rx_path_lmt, out, &mut pos);
    push_byte(out, &mut pos, b'\n');

    push_str(out, &mut pos, b"[RF_PATH] HALDATA's trx_path_bmp:0x");
    push_u8_hex2(out, &mut pos, fx.hal_trx_path_bmp);
    push_str(out, &mut pos, b", max_tx_cnt:");
    push_u8_decimal(fx.hal_max_tx_cnt, out, &mut pos);
    push_byte(out, &mut pos, b'\n');

    push_str(out, &mut pos, b"[RF_PATH] HALDATA's rf_type:");
    push_str(out, &mut pos, hal_str);
    push_str(out, &mut pos, b", NumTotalRFPath:");
    push_i8_decimal(fx.NumTotalRFPath, out, &mut pos);
    push_byte(out, &mut pos, b'\n');

    if pos >= out.len() {
        return -1;
    }
    pos as i32
}

fn format_trx_nss(fx: &HostHalIntfTrxNssFixture, out: &mut [u8]) -> i32 {
    let mut pos = 0usize;

    push_str(out, &mut pos, b"[TRX_Nss] HALSPEC - tx_nss:");
    push_i8_decimal(fx.hal_spec_tx_nss, out, &mut pos);
    push_str(out, &mut pos, b", rx_nss:");
    push_i8_decimal(fx.hal_spec_rx_nss, out, &mut pos);
    push_byte(out, &mut pos, b'\n');

    push_str(out, &mut pos, b"[TRX_Nss] Registry - tx_nss:");
    push_i8_decimal(fx.reg_tx_nss, out, &mut pos);
    push_str(out, &mut pos, b", rx_nss:");
    push_i8_decimal(fx.reg_rx_nss, out, &mut pos);
    push_byte(out, &mut pos, b'\n');

    push_str(out, &mut pos, b"[TRX_Nss] HALDATA - tx_nss:");
    push_i8_decimal(fx.hal_tx_nss, out, &mut pos);
    push_str(out, &mut pos, b", rx_nss:");
    push_i8_decimal(fx.hal_rx_nss, out, &mut pos);
    push_byte(out, &mut pos, b'\n');

    if pos >= out.len() {
        return -1;
    }
    pos as i32
}

fn rf_path_fixture_from_fields(fields: &RtwHalIntfRfPathFields) -> HostHalIntfRfPathFixture {
    HostHalIntfRfPathFixture {
        ver_id_rf_type: fields.ver_id_rf_type,
        rf_reg_trx_path_bmp: fields.rf_reg_trx_path_bmp,
        rf_reg_path_avail_num: fields.rf_reg_path_avail_num,
        hal_spec_max_tx_cnt: fields.hal_spec_max_tx_cnt,
        eeprom_trx_path_bmp: fields.eeprom_trx_path_bmp,
        eeprom_max_tx_cnt: fields.eeprom_max_tx_cnt,
        reg_trx_path_bmp: fields.reg_trx_path_bmp,
        tx_path_lmt: fields.tx_path_lmt,
        rx_path_lmt: fields.rx_path_lmt,
        hal_trx_path_bmp: fields.hal_trx_path_bmp,
        hal_max_tx_cnt: fields.hal_max_tx_cnt,
        hal_rf_type: fields.hal_rf_type,
        NumTotalRFPath: fields.NumTotalRFPath,
    }
}

fn trx_nss_fixture_from_fields(fields: &RtwHalIntfTrxNssFields) -> HostHalIntfTrxNssFixture {
    HostHalIntfTrxNssFixture {
        hal_spec_tx_nss: fields.hal_spec_tx_nss,
        hal_spec_rx_nss: fields.hal_spec_rx_nss,
        reg_tx_nss: fields.reg_tx_nss,
        reg_rx_nss: fields.reg_rx_nss,
        hal_tx_nss: fields.hal_tx_nss,
        hal_rx_nss: fields.hal_rx_nss,
    }
}

fn format_runtime_trx_mode(fx: &HostHalIntfTrxFixture, out: &mut [u8], pos: &mut usize) -> bool {
    push_str(out, pos, b"txpath=0x");
    push_u32_hex(out, pos, fx.txpath);
    push_str(out, pos, b", rxpath=0x");
    push_u32_hex(out, pos, fx.rxpath);
    push_byte(out, pos, b'\n');
    for i in 0..fx.tx_nss as usize {
        push_str(out, pos, b"txpath_");
        push_u8_decimal((i + 1) as U8, out, pos);
        push_str(out, pos, b"ss:0x");
        push_u32_hex(out, pos, fx.txpath_nss[i]);
        push_str(out, pos, b", num:");
        push_u8_decimal(fx.txpath_num_nss[i], out, pos);
        push_byte(out, pos, b'\n');
    }
    *pos < out.len()
}

fn fixture_from_fields(fields: &RtwHalIntfTrxFields) -> HostHalIntfTrxFixture {
    HostHalIntfTrxFixture {
        trx_path_bmp: fields.trx_path_bmp,
        rf_type: fields.rf_type,
        NumTotalRFPath: fields.NumTotalRFPath,
        max_tx_cnt: fields.max_tx_cnt,
        tx_nss: fields.tx_nss,
        rx_nss: fields.rx_nss,
        txpath_cap_num_nss: fields.txpath_cap_num_nss,
        _pad: [0, 0],
        txpath: fields.txpath,
        rxpath: fields.rxpath,
        txpath_nss: fields.txpath_nss,
        txpath_num_nss: fields.txpath_num_nss,
    }
}

fn format_trx_mode(fx: &HostHalIntfTrxFixture, out: &mut [u8]) -> i32 {
    let mut pos = 0usize;
    let rf_str = rf_type_to_rfpath_str_local(fx.rf_type);

    push_str(out, &mut pos, b"trx_path_bmp:0x");
    push_u8_hex2(out, &mut pos, fx.trx_path_bmp);
    push_byte(out, &mut pos, b'(');
    push_str(out, &mut pos, rf_str);
    push_str(out, &mut pos, b"), NumTotalRFPath:");
    push_u8_decimal(fx.NumTotalRFPath, out, &mut pos);
    push_str(out, &mut pos, b", max_tx_cnt:");
    push_u8_decimal(fx.max_tx_cnt, out, &mut pos);
    push_byte(out, &mut pos, b'\n');

    push_str(out, &mut pos, b"tx_nss:");
    push_u8_decimal(fx.tx_nss, out, &mut pos);
    push_str(out, &mut pos, b", rx_nss:");
    push_u8_decimal(fx.rx_nss, out, &mut pos);
    push_byte(out, &mut pos, b'\n');

    for i in 0..fx.tx_nss as usize {
        push_str(out, &mut pos, b"txpath_cap_num_");
        push_u8_decimal((i + 1) as U8, out, &mut pos);
        push_str(out, &mut pos, b"ss:");
        push_u8_decimal(fx.txpath_cap_num_nss[i], out, &mut pos);
        push_byte(out, &mut pos, b'\n');
    }
    push_byte(out, &mut pos, b'\n');

    if !format_runtime_trx_mode(fx, out, &mut pos) {
        return -1;
    }
    if pos >= out.len() {
        return -1;
    }
    pos as i32
}

#[no_mangle]
pub extern "C" fn dump_hal_trx_mode_format(
    fx: *const HostHalIntfTrxFixture,
    out: *mut u8,
    buflen: usize,
) -> i32 {
    if fx.is_null() || out.is_null() || buflen == 0 {
        return -1;
    }
    let fx = unsafe { &*fx };
    let buf = unsafe {
        #[cfg(host_hal_intf_trx_test)]
        {
            std::slice::from_raw_parts_mut(out, buflen)
        }
        #[cfg(not(host_hal_intf_trx_test))]
        {
            core::slice::from_raw_parts_mut(out, buflen)
        }
    };
    let n = format_trx_mode(fx, buf);
    if n > 0 && (n as usize) < buflen {
        buf[n as usize] = 0;
    }
    n
}

#[no_mangle]
pub extern "C" fn dump_rf_path_format(
    fx: *const HostHalIntfRfPathFixture,
    out: *mut u8,
    buflen: usize,
) -> i32 {
    if fx.is_null() || out.is_null() || buflen == 0 {
        return -1;
    }
    let fx = unsafe { &*fx };
    let buf = unsafe {
        #[cfg(any(host_hal_intf_trx_test, host_hal_intf_rf_nss_test))]
        {
            std::slice::from_raw_parts_mut(out, buflen)
        }
        #[cfg(not(any(host_hal_intf_trx_test, host_hal_intf_rf_nss_test)))]
        {
            core::slice::from_raw_parts_mut(out, buflen)
        }
    };
    let n = format_rf_path(fx, buf);
    if n > 0 && (n as usize) < buflen {
        buf[n as usize] = 0;
    }
    n
}

#[no_mangle]
pub extern "C" fn dump_trx_nss_format(
    fx: *const HostHalIntfTrxNssFixture,
    out: *mut u8,
    buflen: usize,
) -> i32 {
    if fx.is_null() || out.is_null() || buflen == 0 {
        return -1;
    }
    let fx = unsafe { &*fx };
    let buf = unsafe {
        #[cfg(any(host_hal_intf_trx_test, host_hal_intf_rf_nss_test))]
        {
            std::slice::from_raw_parts_mut(out, buflen)
        }
        #[cfg(not(any(host_hal_intf_trx_test, host_hal_intf_rf_nss_test)))]
        {
            core::slice::from_raw_parts_mut(out, buflen)
        }
    };
    let n = format_trx_nss(fx, buf);
    if n > 0 && (n as usize) < buflen {
        buf[n as usize] = 0;
    }
    n
}

#[cfg(not(any(host_hal_intf_trx_test, host_hal_intf_rf_nss_test)))]
mod kernel {
    use super::*;

    extern "C" {
        fn rtw_rust_hal_intf_print_sel(sel: *mut c_void, line: *const c_char);
        fn rtw_rust_hal_intf_trx_fields(adapter: *mut c_void, out: *mut RtwHalIntfTrxFields);
    }

    fn print_line(sel: *mut c_void, line: &[u8]) {
        let mut buf = [0i8; 512];
        let n = core::cmp::min(line.len(), buf.len().saturating_sub(1));
        for i in 0..n {
            buf[i] = line[i] as i8;
        }
        unsafe {
            rtw_rust_hal_intf_print_sel(sel, buf.as_ptr());
        }
    }

    fn dump_lines(sel: *mut c_void, text: &[u8], len: usize) {
        let mut start = 0usize;
        for i in 0..len {
            if text[i] == b'\n' {
                print_line(sel, &text[start..=i]);
                start = i + 1;
            }
        }
        if start < len {
            print_line(sel, &text[start..len]);
        }
    }

    #[no_mangle]
    pub extern "C" fn dump_hal_runtime_trx_mode(sel: *mut c_void, adapter: *mut c_void) {
        let mut fields = RtwHalIntfTrxFields {
            trx_path_bmp: 0,
            rf_type: 0,
            NumTotalRFPath: 0,
            max_tx_cnt: 0,
            tx_nss: 0,
            rx_nss: 0,
            txpath_cap_num_nss: [0; 4],
            txpath: 0,
            rxpath: 0,
            txpath_nss: [0; 4],
            txpath_num_nss: [0; 4],
        };
        unsafe {
            rtw_rust_hal_intf_trx_fields(adapter, &mut fields);
        }
        let fx = fixture_from_fields(&fields);
        let mut buf = [0u8; 256];
        let mut pos = 0usize;
        if !format_runtime_trx_mode(&fx, &mut buf, &mut pos) {
            return;
        }
        dump_lines(sel, &buf[..pos], pos);
    }

    #[no_mangle]
    pub extern "C" fn dump_hal_trx_mode(sel: *mut c_void, adapter: *mut c_void) {
        let mut fields = RtwHalIntfTrxFields {
            trx_path_bmp: 0,
            rf_type: 0,
            NumTotalRFPath: 0,
            max_tx_cnt: 0,
            tx_nss: 0,
            rx_nss: 0,
            txpath_cap_num_nss: [0; 4],
            txpath: 0,
            rxpath: 0,
            txpath_nss: [0; 4],
            txpath_num_nss: [0; 4],
        };
        unsafe {
            rtw_rust_hal_intf_trx_fields(adapter, &mut fields);
        }
        let fx = fixture_from_fields(&fields);
        let mut buf = [0u8; 512];
        let n = format_trx_mode(&fx, &mut buf);
        if n > 0 {
            dump_lines(sel, &buf[..n as usize], n as usize);
        }
    }
}
