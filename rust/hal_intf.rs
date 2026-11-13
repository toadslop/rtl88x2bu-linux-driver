// SPDX-License-Identifier: GPL-2.0
//! HAL interface helpers — Rust port of `hal/hal_intf.c` (W4-07 TRX mode dumps, W4-08 RF/NSS dumps,
//! W4-36 chip ODM map, W4-59 chip configure + read_chip_info, W4-60 chip version + def_value_init,
//! W4-61 hal_data_init, W4-62 hal_data_deinit + hal_free_data, W4-64 chip_rftype_to_hal_rftype,
//! W4-67 hal_power_on/off).

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

#[cfg(not(any(
    host_hal_intf_trx_test,
    host_hal_intf_rf_nss_test,
    host_hal_intf_chip_odm_test,
    host_hal_intf_chip_rftype_test
)))]
use core::ffi::{c_char, c_void};

#[cfg(any(
    host_hal_intf_trx_test,
    host_hal_intf_rf_nss_test,
    host_hal_intf_chip_odm_test,
    host_hal_intf_chip_rftype_test
))]
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

#[cfg(any(
    host_hal_intf_trx_test,
    host_hal_intf_rf_nss_test,
    host_hal_intf_chip_odm_test,
    host_hal_intf_chip_rftype_test
))]
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

#[cfg(not(any(
    host_hal_intf_trx_test,
    host_hal_intf_rf_nss_test,
    host_hal_intf_chip_odm_test,
    host_hal_intf_chip_rftype_test
)))]
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

const MAX_CHIP_TYPE: usize = 18;

const CHIP_TYPE_TO_ODM_IC_TYPE: [U32; MAX_CHIP_TYPE + 1] = [
    0, 1,        // ODM_RTL8188E
    8,        // ODM_RTL8192E
    2,        // ODM_RTL8812
    4,        // ODM_RTL8821
    16,       // ODM_RTL8723B
    32,       // ODM_RTL8814A
    256,      // ODM_RTL8703B
    1024,     // ODM_RTL8188F
    1024,     // ODM_RTL8188GTV → ODM_RTL8188F
    128,      // ODM_RTL8822B
    2048,     // ODM_RTL8723D
    8192,     // ODM_RTL8821C
    65536,    // ODM_RTL8710B
    131072,   // ODM_RTL8192F
    262144,   // ODM_RTL8822C
    16384,    // ODM_RTL8814B
    16777216, // ODM_RTL8723F
    0,
];

fn chip_type_to_odm_ic_type_impl(chip_type: U8) -> U32 {
    let idx = if chip_type as usize >= MAX_CHIP_TYPE {
        MAX_CHIP_TYPE
    } else {
        chip_type as usize
    };
    CHIP_TYPE_TO_ODM_IC_TYPE[idx]
}

#[cfg(host_hal_intf_chip_odm_test)]
#[no_mangle]
pub extern "C" fn chip_type_to_odm_ic_type(chip_type: U8) -> U32 {
    chip_type_to_odm_ic_type_impl(chip_type)
}

const RF_TYPE_1T1R: U8 = 0;
const RF_TYPE_1T2R: U8 = 1;
const RF_TYPE_2T2R: U8 = 2;
const RF_TYPE_2T3R: U8 = 3;
const RF_TYPE_2T4R: U8 = 4;
const RF_TYPE_3T3R: U8 = 5;
const RF_TYPE_3T4R: U8 = 6;
const RF_TYPE_4T4R: U8 = 7;

fn rtw_min_u8(a: U8, b: U8) -> U8 {
    if a > b {
        b
    } else {
        a
    }
}

fn chip_rftype_to_hal_rftype_impl(cv_rf_type: U8, limit: U8) -> i32 {
    let mut tx_num = 0u8;
    let mut rx_num = 0u8;

    if cv_rf_type == RF_TYPE_1T1R {
        tx_num = 1;
        rx_num = 1;
    } else if cv_rf_type == RF_TYPE_1T2R {
        tx_num = 1;
        rx_num = 2;
    } else if cv_rf_type == RF_TYPE_2T2R {
        tx_num = 2;
        rx_num = 2;
    } else if cv_rf_type == RF_TYPE_2T3R {
        tx_num = 2;
        rx_num = 3;
    } else if cv_rf_type == RF_TYPE_2T4R {
        tx_num = 2;
        rx_num = 4;
    } else if cv_rf_type == RF_TYPE_3T3R {
        tx_num = 3;
        rx_num = 3;
    } else if cv_rf_type == RF_TYPE_3T4R {
        tx_num = 3;
        rx_num = 4;
    } else if cv_rf_type == RF_TYPE_4T4R {
        tx_num = 4;
        rx_num = 4;
    }

    let (tx_num, rx_num) = if limit != 0 {
        (rtw_min_u8(tx_num, limit), rtw_min_u8(rx_num, limit))
    } else {
        (tx_num, rx_num)
    };

    extern "C" {
        fn trx_num_to_rf_type(tx_num: U8, rx_num: U8) -> i32;
    }
    unsafe { trx_num_to_rf_type(tx_num, rx_num) }
}

#[cfg(host_hal_intf_chip_rftype_test)]
#[no_mangle]
pub extern "C" fn host_chip_rftype_to_hal_rftype(cv_rf_type: U8, limit: U8) -> i32 {
    chip_rftype_to_hal_rftype_impl(cv_rf_type, limit)
}

#[cfg(not(any(
    host_hal_intf_trx_test,
    host_hal_intf_rf_nss_test,
    host_hal_intf_chip_odm_test,
    host_hal_intf_chip_rftype_test
)))]
#[no_mangle]
pub static _chip_type_to_odm_ic_type: [U32; MAX_CHIP_TYPE + 1] = CHIP_TYPE_TO_ODM_IC_TYPE;

#[cfg(not(any(
    host_hal_intf_trx_test,
    host_hal_intf_rf_nss_test,
    host_hal_intf_chip_odm_test,
    host_hal_intf_chip_rftype_test
)))]
mod kernel {
    use super::*;

    type Systime = core::ffi::c_ulong;

    const RTW_SDIO: U8 = 4;
    const RTW_GSPI: U8 = 8;

    const PROTO_CAP_11B: U8 = 1 << 0;
    const PROTO_CAP_11G: U8 = 1 << 1;
    const PROTO_CAP_11N: U8 = 1 << 2;
    const PROTO_CAP_11AC: U8 = 1 << 3;
    const BAND_CAP_2G: U8 = 1 << 0;
    const BAND_CAP_5G: U8 = 1 << 1;

    const WIRELESS_11B: U8 = 1 << 0;
    const WIRELESS_11G: U8 = 1 << 1;
    const WIRELESS_11A: U8 = 1 << 2;
    const WIRELESS_11_24N: U8 = 1 << 3;
    const WIRELESS_11_5N: U8 = 1 << 4;
    const WIRELESS_11AC: U8 = 1 << 6;

    extern "C" {
        fn rtw_rust_hal_intf_intf_chip_configure(adapter: *mut c_void);
        fn rtw_rust_hal_intf_read_adapter_info(adapter: *mut c_void) -> U8;
        fn rtw_rust_hal_intf_log_read_chip_info_ms(ms: i32);
        fn rtw_rust_hal_intf_call_read_chip_version(adapter: *mut c_void);
        fn rtw_rust_hal_intf_alloc_hal_data(adapter: *mut c_void) -> U8;
        fn rtw_rust_hal_intf_get_hal_data_ptr(adapter: *mut c_void) -> *mut c_void;
        fn rtw_rust_hal_intf_get_hal_data_sz(adapter: *mut c_void) -> U32;
        fn rtw_rust_hal_intf_clear_hal_data_fields(adapter: *mut c_void);
        fn rtw_rust_hal_intf_phy_free_filebuf(adapter: *mut c_void);
        fn rtw_rust_hal_intf_vmfree_hal_data(ptr: *mut c_void, sz: U32);
        fn rtw_rust_hal_intf_call_phydm_priv_init(adapter: *mut c_void);
        fn rtw_rust_hal_intf_call_dm_init(adapter: *mut c_void);
        fn rtw_rust_hal_intf_call_dm_deinit(adapter: *mut c_void);
        fn rtw_rust_hal_intf_dm_init_tail(adapter: *mut c_void);
        fn rtw_rust_hal_intf_dm_deinit_tail(adapter: *mut c_void);
        fn rtw_rust_hal_intf_hal_data_version_rf_type(adapter: *mut c_void) -> U8;
        fn rtw_rust_hal_intf_call_hal_power_on(adapter: *mut c_void) -> U32;
        fn rtw_rust_hal_intf_call_hal_power_off(adapter: *mut c_void);
        #[cfg(config_rtw_sw_led)]
        fn rtw_rust_hal_intf_led_b_reg_use_led(adapter: *mut c_void) -> U8;
        #[cfg(config_rtw_sw_led)]
        fn rtw_rust_hal_intf_call_init_sw_leds(adapter: *mut c_void);
        #[cfg(config_rtw_sw_led)]
        fn rtw_rust_hal_intf_call_deinit_sw_leds(adapter: *mut c_void);
        #[cfg(config_rtw_sw_led)]
        fn rtw_rust_hal_intf_led_set_ctl_en_mask_primary(adapter: *mut c_void);
        #[cfg(config_rtw_sw_led)]
        fn rtw_rust_hal_intf_led_set_iface_en(adapter: *mut c_void, en: U8);
        fn rtw_rust_hal_intf_eeprom_bt_coexist(adapter: *mut c_void) -> U8;
        #[cfg(config_bt_coexist)]
        fn rtw_rust_hal_intf_btcoex_power_on(adapter: *mut c_void);
        #[cfg(config_bt_coexist)]
        fn rtw_rust_hal_intf_btcoex_power_off(adapter: *mut c_void);
        fn rtw_rust_hal_intf_power_off_clear_macid(adapter: *mut c_void);
        #[cfg(config_lps_1t1r)]
        fn rtw_rust_hal_intf_clear_lps_1t1r(adapter: *mut c_void);
        fn rtw_rust_hal_intf_hal_spec_proto_cap(adapter: *mut c_void) -> U8;
        fn rtw_rust_hal_intf_hal_spec_band_cap(adapter: *mut c_void) -> U8;
        fn rtw_rust_hal_intf_get_wireless_mode(adapter: *mut c_void) -> U8;
        fn rtw_rust_hal_intf_set_wireless_mode(adapter: *mut c_void, mode: U8);
        fn rtw_rust_hal_intf_set_fw_psmode_iface_id(adapter: *mut c_void, id: U8);
        fn rtw_rust_hal_intf_call_init_default_value(adapter: *mut c_void);
        fn rtw_rust_hal_intf_set_rx_tsf_addr_filter_config(adapter: *mut c_void, v: U8);
        fn rtw_rust_init_is_primary(adapter: *mut c_void) -> U8;
        fn rtw_odm_init_ic_type(adapter: *mut c_void);
        fn rtw_init_hal_com_default_value(adapter: *mut c_void);
        fn rtw_rust_hal_com_get_intf_type(adapter: *mut c_void) -> U8;
        fn rtw_rust_hal_com_is_hw_init_completed(adapter: *mut c_void) -> U8;
        fn rtw_rust_hal_com_hal_power_on(adapter: *mut c_void);
        fn rtw_rust_hal_com_hal_power_off(adapter: *mut c_void);
        fn rtw_rust_hal_com_get_current_time() -> Systime;
        fn rtw_rust_hal_com_get_passing_time_ms(start: Systime) -> u32;
        fn rtw_rust_hal_intf_print_sel(sel: *mut c_void, line: *const c_char);
        fn rtw_rust_hal_intf_trx_fields(adapter: *mut c_void, out: *mut RtwHalIntfTrxFields);
        fn rtw_rust_hal_intf_rf_path_fields(adapter: *mut c_void, out: *mut RtwHalIntfRfPathFields);
        fn rtw_rust_hal_intf_trx_nss_fields(adapter: *mut c_void, out: *mut RtwHalIntfTrxNssFields);
        #[cfg(config_fw_multi_port_support)]
        fn rtw_rust_hal_intf_dft_defaults(adapter: *mut c_void);
        #[cfg(config_hw_p0_tsf_sync)]
        fn rtw_rust_hal_intf_p0_tsf_defaults(adapter: *mut c_void);
    }

    #[no_mangle]
    pub extern "C" fn rtw_hal_chip_configure(adapter: *mut c_void) {
        unsafe {
            rtw_rust_hal_intf_intf_chip_configure(adapter);
        }
    }

    fn init_wireless_mode(adapter: *mut c_void) {
        let mut proto_wireless_mode: U8 = 0;
        let proto_cap = unsafe { rtw_rust_hal_intf_hal_spec_proto_cap(adapter) };
        let band_cap = unsafe { rtw_rust_hal_intf_hal_spec_band_cap(adapter) };

        if proto_cap & PROTO_CAP_11B != 0 {
            proto_wireless_mode |= WIRELESS_11B;
        }
        if proto_cap & PROTO_CAP_11G != 0 {
            proto_wireless_mode |= WIRELESS_11G;
        }
        if band_cap & BAND_CAP_5G != 0 {
            proto_wireless_mode |= WIRELESS_11A;
        }
        #[cfg(config_80211n_ht)]
        {
            if proto_cap & PROTO_CAP_11N != 0 {
                if band_cap & BAND_CAP_2G != 0 {
                    proto_wireless_mode |= WIRELESS_11_24N;
                }
                if band_cap & BAND_CAP_5G != 0 {
                    proto_wireless_mode |= WIRELESS_11_5N;
                }
            }
        }
        #[cfg(config_80211ac_vht)]
        {
            if proto_cap & PROTO_CAP_11AC != 0 {
                proto_wireless_mode |= WIRELESS_11AC;
            }
        }

        let mode = unsafe { rtw_rust_hal_intf_get_wireless_mode(adapter) };
        unsafe {
            rtw_rust_hal_intf_set_wireless_mode(adapter, mode & proto_wireless_mode);
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_hal_read_chip_version(adapter: *mut c_void) {
        unsafe {
            rtw_rust_hal_intf_call_read_chip_version(adapter);
            rtw_odm_init_ic_type(adapter);
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_hal_def_value_init(adapter: *mut c_void) {
        if adapter.is_null() {
            return;
        }
        if unsafe { rtw_rust_init_is_primary(adapter) } == 0 {
            return;
        }
        unsafe {
            rtw_rust_hal_intf_set_fw_psmode_iface_id(adapter, 0xff);
        }
        init_wireless_mode(adapter);
        unsafe {
            rtw_rust_hal_intf_call_init_default_value(adapter);
            rtw_init_hal_com_default_value(adapter);
        }
        #[cfg(config_fw_multi_port_support)]
        unsafe {
            rtw_rust_hal_intf_dft_defaults(adapter);
        }
        #[cfg(config_hw_p0_tsf_sync)]
        unsafe {
            rtw_rust_hal_intf_p0_tsf_defaults(adapter);
        }
        unsafe {
            rtw_rust_hal_intf_set_rx_tsf_addr_filter_config(adapter, 0);
        }
    }

    const SUCCESS: U8 = 1;

    #[no_mangle]
    pub extern "C" fn rtw_hal_data_init(adapter: *mut c_void) -> U8 {
        if adapter.is_null() {
            return SUCCESS;
        }
        if unsafe { rtw_rust_init_is_primary(adapter) } == 0 {
            return SUCCESS;
        }
        let rtn = unsafe { rtw_rust_hal_intf_alloc_hal_data(adapter) };
        if rtn != SUCCESS {
            return rtn;
        }
        unsafe {
            rtw_rust_hal_intf_call_phydm_priv_init(adapter);
        }
        SUCCESS
    }

    fn hal_data_deinit_inner(adapter: *mut c_void) {
        if adapter.is_null() {
            return;
        }
        if unsafe { rtw_rust_init_is_primary(adapter) } == 0 {
            return;
        }
        let hal = unsafe { rtw_rust_hal_intf_get_hal_data_ptr(adapter) };
        if hal.is_null() {
            return;
        }
        unsafe {
            rtw_rust_hal_intf_phy_free_filebuf(adapter);
            let sz = rtw_rust_hal_intf_get_hal_data_sz(adapter);
            rtw_rust_hal_intf_vmfree_hal_data(hal, sz);
            rtw_rust_hal_intf_clear_hal_data_fields(adapter);
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_hal_data_deinit(adapter: *mut c_void) {
        hal_data_deinit_inner(adapter);
    }

    #[no_mangle]
    pub extern "C" fn rtw_hal_free_data(adapter: *mut c_void) {
        hal_data_deinit_inner(adapter);
    }

    #[no_mangle]
    pub extern "C" fn rtw_hal_dm_init(adapter: *mut c_void) {
        if adapter.is_null() {
            return;
        }
        if unsafe { rtw_rust_init_is_primary(adapter) } == 0 {
            return;
        }
        unsafe {
            rtw_rust_hal_intf_call_dm_init(adapter);
            rtw_rust_hal_intf_dm_init_tail(adapter);
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_hal_dm_deinit(adapter: *mut c_void) {
        if adapter.is_null() {
            return;
        }
        if unsafe { rtw_rust_init_is_primary(adapter) } == 0 {
            return;
        }
        unsafe {
            rtw_rust_hal_intf_call_dm_deinit(adapter);
            rtw_rust_hal_intf_dm_deinit_tail(adapter);
        }
    }

    const TRUE: U8 = 1;

    #[no_mangle]
    pub extern "C" fn rtw_hal_power_on(adapter: *mut c_void) -> U32 {
        let ret = unsafe { rtw_rust_hal_intf_call_hal_power_on(adapter) };
        #[cfg(config_bt_coexist)]
        {
            if ret == SUCCESS as U32
                && unsafe { rtw_rust_hal_intf_eeprom_bt_coexist(adapter) } == TRUE
            {
                unsafe { rtw_rust_hal_intf_btcoex_power_on(adapter) };
            }
        }
        ret
    }

    #[cfg(config_rtw_sw_led)]
    const LED_FALSE: U8 = 0;

    #[cfg(config_rtw_sw_led)]
    fn sw_led_guard(adapter: *mut c_void) -> bool {
        if adapter.is_null() {
            return false;
        }
        if unsafe { rtw_rust_hal_intf_led_b_reg_use_led(adapter) } == LED_FALSE {
            return false;
        }
        if unsafe { rtw_rust_init_is_primary(adapter) } == 0 {
            return false;
        }
        true
    }

    #[cfg(config_rtw_sw_led)]
    #[no_mangle]
    pub extern "C" fn rtw_hal_sw_led_init(adapter: *mut c_void) {
        if !sw_led_guard(adapter) {
            return;
        }
        unsafe {
            rtw_rust_hal_intf_call_init_sw_leds(adapter);
            rtw_rust_hal_intf_led_set_ctl_en_mask_primary(adapter);
            rtw_rust_hal_intf_led_set_iface_en(adapter, 1);
        }
    }

    #[cfg(config_rtw_sw_led)]
    #[no_mangle]
    pub extern "C" fn rtw_hal_sw_led_deinit(adapter: *mut c_void) {
        if !sw_led_guard(adapter) {
            return;
        }
        unsafe {
            rtw_rust_hal_intf_call_deinit_sw_leds(adapter);
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_hal_power_off(adapter: *mut c_void) {
        unsafe {
            rtw_rust_hal_intf_power_off_clear_macid(adapter);
        }
        #[cfg(config_lps_1t1r)]
        unsafe {
            rtw_rust_hal_intf_clear_lps_1t1r(adapter);
        }
        #[cfg(config_bt_coexist)]
        unsafe {
            rtw_rust_hal_intf_btcoex_power_off(adapter);
        }
        unsafe {
            rtw_rust_hal_intf_call_hal_power_off(adapter);
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_chip_rftype_to_hal_rftype(adapter: *mut c_void, limit: U8) -> i32 {
        let cv_rf = unsafe { rtw_rust_hal_intf_hal_data_version_rf_type(adapter) };
        chip_rftype_to_hal_rftype_impl(cv_rf, limit)
    }

    #[no_mangle]
    pub extern "C" fn rtw_hal_read_chip_info(adapter: *mut c_void) -> U8 {
        let hci_type = unsafe { rtw_rust_hal_com_get_intf_type(adapter) };
        let start = unsafe { rtw_rust_hal_com_get_current_time() };

        /*  before access eFuse, make sure card enable has been called */
        if (hci_type == RTW_SDIO || hci_type == RTW_GSPI)
            && unsafe { rtw_rust_hal_com_is_hw_init_completed(adapter) } == 0
        {
            unsafe { rtw_rust_hal_com_hal_power_on(adapter) };
        }

        let rtn = unsafe { rtw_rust_hal_intf_read_adapter_info(adapter) };

        if (hci_type == RTW_SDIO || hci_type == RTW_GSPI)
            && unsafe { rtw_rust_hal_com_is_hw_init_completed(adapter) } == 0
        {
            unsafe { rtw_rust_hal_com_hal_power_off(adapter) };
        }

        let ms = unsafe { rtw_rust_hal_com_get_passing_time_ms(start) } as i32;
        unsafe {
            rtw_rust_hal_intf_log_read_chip_info_ms(ms);
        }

        rtn
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

    #[no_mangle]
    pub extern "C" fn _dump_rf_path(sel: *mut c_void, adapter: *mut c_void) {
        let mut fields = RtwHalIntfRfPathFields {
            ver_id_rf_type: 0,
            rf_reg_trx_path_bmp: 0,
            rf_reg_path_avail_num: 0,
            hal_spec_max_tx_cnt: 0,
            eeprom_trx_path_bmp: 0,
            eeprom_max_tx_cnt: 0,
            reg_trx_path_bmp: 0,
            tx_path_lmt: 0,
            rx_path_lmt: 0,
            hal_trx_path_bmp: 0,
            hal_max_tx_cnt: 0,
            hal_rf_type: 0,
            NumTotalRFPath: 0,
        };
        unsafe {
            rtw_rust_hal_intf_rf_path_fields(adapter, &mut fields);
        }
        let fx = rf_path_fixture_from_fields(&fields);
        let mut buf = [0u8; 512];
        let n = format_rf_path(&fx, &mut buf);
        if n > 0 {
            dump_lines(sel, &buf[..n as usize], n as usize);
        }
    }

    #[no_mangle]
    pub extern "C" fn _dump_trx_nss(sel: *mut c_void, adapter: *mut c_void) {
        let mut fields = RtwHalIntfTrxNssFields {
            hal_spec_tx_nss: 0,
            hal_spec_rx_nss: 0,
            reg_tx_nss: 0,
            reg_rx_nss: 0,
            hal_tx_nss: 0,
            hal_rx_nss: 0,
        };
        unsafe {
            rtw_rust_hal_intf_trx_nss_fields(adapter, &mut fields);
        }
        let fx = trx_nss_fixture_from_fields(&fields);
        let mut buf = [0u8; 256];
        let n = format_trx_nss(&fx, &mut buf);
        if n > 0 {
            dump_lines(sel, &buf[..n as usize], n as usize);
        }
    }
}
