// SPDX-License-Identifier: GPL-2.0
//! HAL common helpers — Rust port of `hal/hal_com.c` rate-map slice (W4-01).

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

#[cfg(all(not(host_hal_com_hw_rate_test), config_rtw_debug))]
use core::ffi::{c_char, c_void};

type U8 = u8;

const DESC_RATE_NUM: usize = 0x54;
const DESC_RATE1M: U8 = 0x00;
const MGN_1M: U8 = 0x02;
const MGN_MCS32: U8 = 0x7f;
const MGN_UNKNOWN: U8 = 200;
const HT_4SS: u32 = 5;
const RATE_SECTION_NUM: u32 = 10;

static HW_RATE_TO_M_RATE: [U8; DESC_RATE_NUM] = [
    0x02, 0x04, 0x0b, 0x16, 0x0c, 0x12, 0x18, 0x24, 0x30, 0x48, 0x60, 0x6c, 0x80, 0x81, 0x82,
    0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91,
    0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1,
    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1,
    0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1,
    0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
];

#[repr(C)]
struct RateSectionEnt {
    tx_num: U8,
    rate_num: U8,
    rates: *mut U8,
}

#[cfg(not(host_hal_com_hw_rate_test))]
extern "C" {
    fn MRateToHwRate(rate: U8) -> U8;
    fn MGN_RATE_STR(rate: U8) -> *const c_char;
    static mut rates_by_sections: [RateSectionEnt; RATE_SECTION_NUM as usize];
    fn rtw_rust_hal_com_print_line(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_hal_com_hdata_rate(hw_rate: U8) -> *const c_char;
}

#[no_mangle]
pub extern "C" fn hw_rate_to_m_rate(hw_rate: U8) -> U8 {
    if (hw_rate as usize) < DESC_RATE_NUM {
        HW_RATE_TO_M_RATE[hw_rate as usize]
    } else {
        MGN_1M
    }
}

#[cfg(config_rtw_debug)]
fn c_str_bytes(ptr: *const c_char) -> &'static [u8] {
    if ptr.is_null() {
        return b"";
    }
    unsafe {
        let mut len = 0usize;
        while *ptr.add(len) != 0 {
            len += 1;
        }
        core::slice::from_raw_parts(ptr as *const u8, len)
    }
}

#[cfg(config_rtw_debug)]
fn print_line(sel: *mut c_void, line: &[u8]) {
    let mut buf = [0i8; 256];
    let n = core::cmp::min(line.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = line[i] as i8;
    }
    unsafe {
        rtw_rust_hal_com_print_line(sel, buf.as_ptr());
    }
}

#[cfg(config_rtw_debug)]
fn push_decimal(mut n: u32, out: &mut [u8], pos: &mut usize) {
    if n == 0 {
        if *pos < out.len() {
            out[*pos] = b'0';
            *pos += 1;
        }
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
        if *pos < out.len() {
            out[*pos] = digits[nd];
            *pos += 1;
        }
    }
}

#[cfg(config_rtw_debug)]
fn format_m_to_hw_line(m_rate: U8, hw_rate: U8) -> (usize, [u8; 256]) {
    let mut line = [0u8; 256];
    let mut pos = 0usize;
    let m_str = c_str_bytes(unsafe { MGN_RATE_STR(m_rate) });
    let h_str = c_str_bytes(unsafe { rtw_rust_hal_com_hdata_rate(hw_rate) });
    for &b in b"m_rate:" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in m_str {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in b"(" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    push_decimal(m_rate as u32, &mut line, &mut pos);
    for &b in b") to hw_rate:" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in h_str {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in b"(" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    push_decimal(hw_rate as u32, &mut line, &mut pos);
    if pos < line.len() {
        line[pos] = b')';
        pos += 1;
    }
    if pos < line.len() {
        line[pos] = b'\n';
        pos += 1;
    }
    (pos, line)
}

#[cfg(config_rtw_debug)]
fn format_hw_to_m_line(hw_rate: U8, m_rate: U8) -> (usize, [u8; 256]) {
    let mut line = [0u8; 256];
    let mut pos = 0usize;
    let h_str = c_str_bytes(unsafe { rtw_rust_hal_com_hdata_rate(hw_rate) });
    let m_str = c_str_bytes(unsafe { MGN_RATE_STR(m_rate) });
    for &b in b"hw_rate:" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in h_str {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in b"(" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    push_decimal(hw_rate as u32, &mut line, &mut pos);
    for &b in b") to m_rate:" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in m_str {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in b"(" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    push_decimal(m_rate as u32, &mut line, &mut pos);
    if pos < line.len() {
        line[pos] = b')';
        pos += 1;
    }
    if pos < line.len() {
        line[pos] = b'\n';
        pos += 1;
    }
    (pos, line)
}

#[cfg(config_rtw_debug)]
#[no_mangle]
pub extern "C" fn dump_hw_rate_map_test(sel: *mut c_void) {
    unsafe {
        for rs in 0..RATE_SECTION_NUM {
            let ent = &rates_by_sections[rs as usize];
            for i in 0..ent.rate_num {
                let m_rate = *ent.rates.add(i as usize);
                let hw_rate = MRateToHwRate(m_rate);
                let (len, line) = format_m_to_hw_line(m_rate, hw_rate);
                print_line(sel, &line[..len]);
            }
            if rs == HT_4SS {
                let hw_rate = MRateToHwRate(MGN_MCS32);
                let (len, line) = format_m_to_hw_line(MGN_MCS32, hw_rate);
                print_line(sel, &line[..len]);
            }
        }
        let hw_rate = MRateToHwRate(MGN_UNKNOWN);
        let (len, line) = format_m_to_hw_line(MGN_UNKNOWN, hw_rate);
        print_line(sel, &line[..len]);

        let mut i = DESC_RATE1M as i32;
        while i <= DESC_RATE_NUM as i32 {
            let m_rate = hw_rate_to_m_rate(i as U8);
            let (len, line) = format_hw_to_m_line(i as U8, m_rate);
            print_line(sel, &line[..len]);
            i += 1;
        }
    }
}
