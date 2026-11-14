// SPDX-License-Identifier: GPL-2.0
//! HAL interface helpers — Rust port of `hal/hal_intf.c` (W4-07 TRX mode dumps, W4-08 RF/NSS dumps,
//! W4-36 chip ODM map, W4-59 chip configure + read_chip_info, W4-60 chip version + def_value_init,
//! W4-61 hal_data_init, W4-62 hal_data_deinit + hal_free_data, W4-64 chip_rftype_to_hal_rftype,
//! W4-65 hal_rfpath_init, W4-66 hal_trxnss_init, W4-67 hal_power_on/off, W4-68 sw_led init/deinit,
//! W4-69 hal_init_opmode, W4-70 interrupt enable/disable + IPS check).

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
    host_hal_intf_chip_rftype_test, host_hal_intf_rfpath_test, host_hal_intf_trxnss_test
)))]
use core::ffi::{c_char, c_void};

#[cfg(any(
    host_hal_intf_trx_test,
    host_hal_intf_rf_nss_test,
    host_hal_intf_chip_odm_test,
    host_hal_intf_chip_rftype_test, host_hal_intf_rfpath_test, host_hal_intf_trxnss_test
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
    host_hal_intf_chip_rftype_test, host_hal_intf_rfpath_test, host_hal_intf_trxnss_test
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
    host_hal_intf_chip_rftype_test, host_hal_intf_rfpath_test, host_hal_intf_trxnss_test
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

fn rtw_max_u8(a: U8, b: U8) -> U8 {
    if a > b {
        a
    } else {
        b
    }
}

const RTW_FAIL: U8 = 0;
const RTW_SUCCESS: U8 = 1;
const RFPATH_FAIL_NONE: U8 = 0;
const RFPATH_FAIL_REG_INTERSECT: U8 = 1;
const RFPATH_FAIL_EEPROM_INTERSECT: U8 = 2;
const RFPATH_FAIL_RESTRICT: U8 = 3;

#[repr(C)]
pub struct HostHalRfpathInitIn {
    pub rf_reg_trx_path_bmp: U8,
    pub reg_trx_path_bmp: U8,
    pub eeprom_trx_path_bmp: U8,
    pub eeprom_max_tx_cnt: U8,
    pub hal_spec_max_tx_cnt: U8,
    pub tx_path_lmt: U8,
    pub rx_path_lmt: U8,
}

#[repr(C)]
pub struct HostHalRfpathInitOut {
    pub status: U8,
    pub hal_trx_path_bmp: U8,
    pub hal_rf_type: U8,
    pub NumTotalRFPath: U8,
    pub hal_max_tx_cnt: U8,
    pub fail_reason: U8,
}

extern "C" {
    fn rtw_restrict_trx_path_bmp_by_trx_num_lmt(
        trx_path_bmp: U8,
        tx_num_lmt: U8,
        rx_num_lmt: U8,
        tx_num: *mut U8,
        rx_num: *mut U8,
    ) -> U8;
    fn trx_bmp_to_rf_type(tx_bmp: U8, rx_bmp: U8) -> i32;
}

fn rfpath_init_common_from_in(in_: &HostHalRfpathInitIn, out: &mut HostHalRfpathInitOut) -> U8 {
    out.fail_reason = RFPATH_FAIL_NONE;
    let mut trx_path_bmp = in_.rf_reg_trx_path_bmp;
    let mut tx_path_num: U8 = 0;
    let mut rx_path_num: U8 = 0;

    if in_.reg_trx_path_bmp != 0 {
        trx_path_bmp &= in_.reg_trx_path_bmp;
        if trx_path_bmp == 0 {
            out.fail_reason = RFPATH_FAIL_REG_INTERSECT;
            out.status = RTW_FAIL;
            return RTW_FAIL;
        }
    } else if in_.eeprom_trx_path_bmp != 0 {
        trx_path_bmp &= in_.eeprom_trx_path_bmp;
        if trx_path_bmp == 0 {
            out.fail_reason = RFPATH_FAIL_EEPROM_INTERSECT;
            out.status = RTW_FAIL;
            return RTW_FAIL;
        }
    }

    let restricted = unsafe {
        rtw_restrict_trx_path_bmp_by_trx_num_lmt(
            trx_path_bmp,
            in_.tx_path_lmt,
            in_.rx_path_lmt,
            &mut tx_path_num,
            &mut rx_path_num,
        )
    };
    if restricted == 0 {
        out.fail_reason = RFPATH_FAIL_RESTRICT;
        out.status = RTW_FAIL;
        return RTW_FAIL;
    }
    trx_path_bmp = restricted;

    let rf_type = unsafe {
        trx_bmp_to_rf_type((trx_path_bmp & 0xF0) >> 4, trx_path_bmp & 0x0F) as U8
    };
    let num_paths = rtw_max_u8(tx_path_num, rx_path_num);
    let mut max_tx_cnt = in_.hal_spec_max_tx_cnt;
    max_tx_cnt = rtw_min_u8(max_tx_cnt, tx_path_num);
    if in_.eeprom_max_tx_cnt != 0 {
        max_tx_cnt = rtw_min_u8(max_tx_cnt, in_.eeprom_max_tx_cnt);
    }

    out.hal_trx_path_bmp = trx_path_bmp;
    out.hal_rf_type = rf_type;
    out.NumTotalRFPath = num_paths;
    out.hal_max_tx_cnt = max_tx_cnt;
    out.status = RTW_SUCCESS;
    RTW_SUCCESS
}

#[cfg(host_hal_intf_rfpath_test)]
#[no_mangle]
pub extern "C" fn host_hal_rfpath_init_common(
    in_: *const HostHalRfpathInitIn,
    out: *mut HostHalRfpathInitOut,
) -> U8 {
    if in_.is_null() || out.is_null() {
        return RTW_FAIL;
    }
    unsafe { rfpath_init_common_from_in(&*in_, &mut *out) }
}

// Keep in sync with `core/rtw_rf_rest.c` `_rf_type_to_rf_rx_cnt`.
const _RF_TYPE_TO_RF_RX_CNT: [U8; RF_TYPE_MAX] = [
    1, 2, 2, 3, 4, 3, 4, 4, 3, 2, 1, 2, 1, 1, 4, 3,
];

fn rf_type_to_rf_rx_cnt_local(rf_type: U8) -> U8 {
    if (rf_type as usize) < RF_TYPE_MAX {
        _RF_TYPE_TO_RF_RX_CNT[rf_type as usize]
    } else {
        0
    }
}

#[repr(C)]
pub struct HostHalTrxnssInitIn {
    pub hal_spec_tx_nss: i8,
    pub hal_spec_rx_nss: i8,
    pub reg_tx_nss: i8,
    pub reg_rx_nss: i8,
    pub max_tx_cnt: U8,
    pub rf_path: U8,
    pub hw_full_tx: U8,
}

#[repr(C)]
pub struct HostHalTrxnssInitOut {
    pub tx_nss: U8,
    pub rx_nss: U8,
    pub txpath_cap_num_nss: [U8; 4],
}

fn trxnss_init_common_from_in(in_: &HostHalTrxnssInitIn, out: &mut HostHalTrxnssInitOut) {
    let mut tx_nss = in_.hal_spec_tx_nss as U8;
    let mut rx_nss = in_.hal_spec_rx_nss as U8;

    if in_.reg_tx_nss > 0 {
        tx_nss = rtw_min_u8(tx_nss, in_.reg_tx_nss as U8);
    }
    tx_nss = rtw_min_u8(tx_nss, in_.max_tx_cnt);
    if in_.reg_rx_nss > 0 {
        rx_nss = rtw_min_u8(rx_nss, in_.reg_rx_nss as U8);
    }
    rx_nss = rtw_min_u8(rx_nss, rf_type_to_rf_rx_cnt_local(in_.rf_path));

    out.txpath_cap_num_nss = [0; 4];
    for i in 0..4usize {
        let idx = i as U8;
        if tx_nss < idx.wrapping_add(1) {
            break;
        }
        out.txpath_cap_num_nss[i] = if in_.hw_full_tx != 0 {
            in_.max_tx_cnt
        } else {
            idx.wrapping_add(1)
        };
    }
    out.tx_nss = tx_nss;
    out.rx_nss = rx_nss;
}

#[cfg(host_hal_intf_trxnss_test)]
#[no_mangle]
pub extern "C" fn host_hal_trxnss_init_common(
    in_: *const HostHalTrxnssInitIn,
    out: *mut HostHalTrxnssInitOut,
) {
    if in_.is_null() || out.is_null() {
        return;
    }
    unsafe {
        trxnss_init_common_from_in(&*in_, &mut *out);
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
    host_hal_intf_chip_rftype_test, host_hal_intf_rfpath_test, host_hal_intf_trxnss_test
)))]
#[no_mangle]
pub static _chip_type_to_odm_ic_type: [U32; MAX_CHIP_TYPE + 1] = CHIP_TYPE_TO_ODM_IC_TYPE;

#[cfg(not(any(
    host_hal_intf_trx_test,
    host_hal_intf_rf_nss_test,
    host_hal_intf_chip_odm_test,
    host_hal_intf_chip_rftype_test, host_hal_intf_rfpath_test, host_hal_intf_trxnss_test
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
        fn rtw_rust_hal_intf_get_fwstate(adapter: *mut c_void) -> i32;
        fn rtw_rust_hal_intf_setopmode_cmd(
            adapter: *mut c_void,
            networktype: i32,
            flags: U8,
        ) -> U8;
        fn rtw_rust_hal_intf_call_enable_interrupt(adapter: *mut c_void);
        fn rtw_rust_hal_intf_call_disable_interrupt(adapter: *mut c_void);
        fn rtw_rust_hal_intf_call_check_ips_status(
            adapter: *mut c_void,
            handler_missing: *mut U8,
        ) -> U8;
        fn rtw_rust_hal_intf_log_check_ips_status_null();
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
        fn rtw_rust_hal_intf_rfpath_store(
            adapter: *mut c_void,
            trx_path_bmp: U8,
            rf_type: U8,
            num_total_rf_path: U8,
            max_tx_cnt: U8,
        );
        fn rtw_rust_hal_intf_is_hw_8814a(adapter: *mut c_void) -> U8;
        fn rtw_rust_hal_intf_rfpath_8814a(
            adapter: *mut c_void,
            trx_path_bmp: *mut U8,
            rf_type: *mut U8,
            num_total_rf_path: *mut U8,
            max_tx_cnt: *mut U8,
        ) -> U8;
        fn rtw_rust_hal_intf_rfpath_dump(adapter: *mut c_void);
        fn rtw_rust_hal_intf_rfpath_info(
            adapter: *mut c_void,
            trx_path_bmp: U8,
            rf_type: U8,
            num_total_rf_path: U8,
            max_tx_cnt: U8,
        );
        fn rtw_rust_hal_intf_rfpath_err_reg(adapter: *mut c_void, spec_bmp: U8, reg_bmp: U8);
        fn rtw_rust_hal_intf_rfpath_err_eeprom(
            adapter: *mut c_void,
            spec_bmp: U8,
            eeprom_bmp: U8,
        );
        fn rtw_rust_hal_intf_rfpath_err_restrict(
            adapter: *mut c_void,
            trx_path_bmp: U8,
            tx_lmt: U8,
            rx_lmt: U8,
        );
        fn rtw_rust_hal_intf_trxnss_init_read(adapter: *mut c_void, out: *mut HostHalTrxnssInitIn);
        fn rtw_rust_hal_intf_trxnss_init_store(
            adapter: *mut c_void,
            tx_nss: U8,
            rx_nss: U8,
            txpath_cap: *const U8,
        );
        fn rtw_rust_hal_intf_trxnss_dump(adapter: *mut c_void);
        fn rtw_rust_hal_intf_trxnss_info(adapter: *mut c_void, tx_nss: U8, rx_nss: U8);
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

    const WIFI_ADHOC_STATE: i32 = 0x00000020;
    const WIFI_STATION_STATE: i32 = 0x00000008;
    #[cfg(config_ap_mode)]
    const WIFI_AP_STATE: i32 = 0x00000010;
    #[cfg(config_rtw_mesh)]
    const WIFI_MESH_STATE: i32 = 0x00000200;

    const NDIS802_11_IBSS: i32 = 0;
    const NDIS802_11_INFRASTRUCTURE: i32 = 1;
    const NDIS802_11_AP_MODE: i32 = 4;
    const NDIS802_11_MESH: i32 = 6;

    const RTW_CMDF_DIRECTLY: U8 = 1;

    fn hal_init_opmode_impl(adapter: *mut c_void) {
        if adapter.is_null() {
            return;
        }
        let fw_state = unsafe { rtw_rust_hal_intf_get_fwstate(adapter) };
        let network_type = if (fw_state & WIFI_ADHOC_STATE) != 0 {
            NDIS802_11_IBSS
        } else if (fw_state & WIFI_STATION_STATE) != 0 {
            NDIS802_11_INFRASTRUCTURE
        } else if {
            #[cfg(config_ap_mode)]
            {
                (fw_state & WIFI_AP_STATE) != 0
            }
            #[cfg(not(config_ap_mode))]
            {
                false
            }
        } {
            NDIS802_11_AP_MODE
        } else if {
            #[cfg(config_rtw_mesh)]
            {
                (fw_state & WIFI_MESH_STATE) != 0
            }
            #[cfg(not(config_rtw_mesh))]
            {
                false
            }
        } {
            NDIS802_11_MESH
        } else {
            return;
        };
        unsafe {
            rtw_rust_hal_intf_setopmode_cmd(adapter, network_type, RTW_CMDF_DIRECTLY);
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_hal_init_opmode(adapter: *mut c_void) {
        hal_init_opmode_impl(adapter);
    }

    const RTW_TRUE: U8 = 1;

    #[no_mangle]
    pub extern "C" fn rtw_hal_enable_interrupt(adapter: *mut c_void) {
        unsafe {
            rtw_rust_hal_intf_call_enable_interrupt(adapter);
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_hal_disable_interrupt(adapter: *mut c_void) {
        unsafe {
            rtw_rust_hal_intf_call_disable_interrupt(adapter);
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_hal_check_ips_status(adapter: *mut c_void) -> U8 {
        let mut handler_missing: U8 = 0;
        let val = unsafe { rtw_rust_hal_intf_call_check_ips_status(adapter, &mut handler_missing) };
        if handler_missing == RTW_TRUE {
            unsafe {
                rtw_rust_hal_intf_log_check_ips_status_null();
            }
        }
        val
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

    fn rfpath_init_from_adapter(adapter: *mut c_void) -> U8 {
        if adapter.is_null() {
            return RTW_FAIL;
        }
        unsafe {
            if rtw_rust_hal_intf_is_hw_8814a(adapter) != 0 {
                let mut trx_path_bmp = 0u8;
                let mut rf_type = 0u8;
                let mut num_paths = 0u8;
                let mut max_tx_cnt = 0u8;
                if rtw_rust_hal_intf_rfpath_8814a(
                    adapter,
                    &mut trx_path_bmp,
                    &mut rf_type,
                    &mut num_paths,
                    &mut max_tx_cnt,
                ) != 0
                {
                    rtw_rust_hal_intf_rfpath_dump(adapter);
                    rtw_rust_hal_intf_rfpath_info(
                        adapter,
                        trx_path_bmp,
                        rf_type,
                        num_paths,
                        max_tx_cnt,
                    );
                    return RTW_SUCCESS;
                }
            }

            let mut fields = core::mem::MaybeUninit::<RtwHalIntfRfPathFields>::uninit();
            rtw_rust_hal_intf_rf_path_fields(adapter, fields.as_mut_ptr());
            let fields = fields.assume_init();
            let in_ = HostHalRfpathInitIn {
                rf_reg_trx_path_bmp: fields.rf_reg_trx_path_bmp,
                reg_trx_path_bmp: fields.reg_trx_path_bmp,
                eeprom_trx_path_bmp: fields.eeprom_trx_path_bmp,
                eeprom_max_tx_cnt: fields.eeprom_max_tx_cnt,
                hal_spec_max_tx_cnt: fields.hal_spec_max_tx_cnt,
                tx_path_lmt: fields.tx_path_lmt,
                rx_path_lmt: fields.rx_path_lmt,
            };
            let mut out = HostHalRfpathInitOut {
                status: RTW_FAIL,
                hal_trx_path_bmp: 0,
                hal_rf_type: 0,
                NumTotalRFPath: 0,
                hal_max_tx_cnt: 0,
                fail_reason: RFPATH_FAIL_NONE,
            };
            let rtn = rfpath_init_common_from_in(&in_, &mut out);
            if rtn == RTW_FAIL {
                match out.fail_reason {
                    RFPATH_FAIL_REG_INTERSECT => rtw_rust_hal_intf_rfpath_err_reg(
                        adapter,
                        in_.rf_reg_trx_path_bmp,
                        in_.reg_trx_path_bmp,
                    ),
                    RFPATH_FAIL_EEPROM_INTERSECT => rtw_rust_hal_intf_rfpath_err_eeprom(
                        adapter,
                        in_.rf_reg_trx_path_bmp,
                        in_.eeprom_trx_path_bmp,
                    ),
                    _ => rtw_rust_hal_intf_rfpath_err_restrict(
                        adapter,
                        0,
                        in_.tx_path_lmt,
                        in_.rx_path_lmt,
                    ),
                }
                return RTW_FAIL;
            }

            rtw_rust_hal_intf_rfpath_store(
                adapter,
                out.hal_trx_path_bmp,
                out.hal_rf_type,
                out.NumTotalRFPath,
                out.hal_max_tx_cnt,
            );
            rtw_rust_hal_intf_rfpath_dump(adapter);
            rtw_rust_hal_intf_rfpath_info(
                adapter,
                out.hal_trx_path_bmp,
                out.hal_rf_type,
                out.NumTotalRFPath,
                out.hal_max_tx_cnt,
            );
            RTW_SUCCESS
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_hal_rfpath_init(adapter: *mut c_void) -> U8 {
        rfpath_init_from_adapter(adapter)
    }

    fn trxnss_init_from_adapter(adapter: *mut c_void) -> U8 {
        if adapter.is_null() {
            return RTW_FAIL;
        }
        unsafe {
            let mut in_ = core::mem::MaybeUninit::<HostHalTrxnssInitIn>::uninit();
            rtw_rust_hal_intf_trxnss_init_read(adapter, in_.as_mut_ptr());
            let in_ = in_.assume_init();
            let mut out = HostHalTrxnssInitOut {
                tx_nss: 0,
                rx_nss: 0,
                txpath_cap_num_nss: [0; 4],
            };
            trxnss_init_common_from_in(&in_, &mut out);
            rtw_rust_hal_intf_trxnss_init_store(
                adapter,
                out.tx_nss,
                out.rx_nss,
                out.txpath_cap_num_nss.as_ptr(),
            );
            rtw_rust_hal_intf_trxnss_dump(adapter);
            rtw_rust_hal_intf_trxnss_info(adapter, out.tx_nss, out.rx_nss);
        }
        RTW_SUCCESS
    }

    #[no_mangle]
    pub extern "C" fn rtw_hal_trxnss_init(adapter: *mut c_void) -> U8 {
        trxnss_init_from_adapter(adapter)
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
