// SPDX-License-Identifier: GPL-2.0
//! RF rest helpers — Rust port of `core/rtw_rf_rest.c` channel layout (W3-19),
//! frequency conversion (W3-20), lookup/format tables (W3-21), global
//! operating-class lookup (W3-22), and RF type / trx-path helpers (W3-23).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_assignments
)]

#[cfg(not(host_rf_rest_test))]
use core::ffi::c_int;

const CHANNEL_WIDTH_20: u8 = 0;
const CHANNEL_WIDTH_40: u8 = 1;
const CHANNEL_WIDTH_80: u8 = 2;
const CHANNEL_WIDTH_160: u8 = 3;
const CHANNEL_WIDTH_80_80: u8 = 4;
const CHANNEL_WIDTH_5: u8 = 5;
const CHANNEL_WIDTH_10: u8 = 6;
const CHANNEL_WIDTH_MAX: usize = 7;

const BW_CAP_5M: u8 = 1;
const BW_CAP_10M: u8 = 2;
const BW_CAP_20M: u8 = 4;
const BW_CAP_40M: u8 = 8;
const BW_CAP_80M: u8 = 16;
const BW_CAP_160M: u8 = 32;
const BW_CAP_80_80M: u8 = 64;

const BAND_CAP_2G: u8 = 1;
const BAND_CAP_5G: u8 = 2;

const OPC_BW_NUM: usize = 6;

/// ABI-compatible wrapper for `const char *const` in exported string-pointer tables.
#[repr(transparent)]
pub struct CStrPtr(*const u8);

unsafe impl Sync for CStrPtr {}

static CH_WIDTH_STR_20: &[u8; 6] = b"20MHz\0";
static CH_WIDTH_STR_40: &[u8; 6] = b"40MHz\0";
static CH_WIDTH_STR_80: &[u8; 6] = b"80MHz\0";
static CH_WIDTH_STR_160: &[u8; 7] = b"160MHz\0";
static CH_WIDTH_STR_80_80: &[u8; 9] = b"80_80MHz\0";
static CH_WIDTH_STR_5: &[u8; 5] = b"5MHz\0";
static CH_WIDTH_STR_10: &[u8; 6] = b"10MHz\0";

#[no_mangle]
pub static _ch_width_str: [CStrPtr; CHANNEL_WIDTH_MAX] = [
    CStrPtr(CH_WIDTH_STR_20.as_ptr()),
    CStrPtr(CH_WIDTH_STR_40.as_ptr()),
    CStrPtr(CH_WIDTH_STR_80.as_ptr()),
    CStrPtr(CH_WIDTH_STR_160.as_ptr()),
    CStrPtr(CH_WIDTH_STR_80_80.as_ptr()),
    CStrPtr(CH_WIDTH_STR_5.as_ptr()),
    CStrPtr(CH_WIDTH_STR_10.as_ptr()),
];

#[no_mangle]
pub static _ch_width_to_bw_cap: [u8; CHANNEL_WIDTH_MAX] = [
    BW_CAP_20M,
    BW_CAP_40M,
    BW_CAP_80M,
    BW_CAP_160M,
    BW_CAP_80_80M,
    BW_CAP_5M,
    BW_CAP_10M,
];

static BAND_STR_2G: &[u8; 5] = b"2.4G\0";
static BAND_STR_5G: &[u8; 3] = b"5G\0";
static BAND_STR_MAX: &[u8; 9] = b"BAND_MAX\0";

#[no_mangle]
pub static _band_str: [CStrPtr; 3] = [
    CStrPtr(BAND_STR_2G.as_ptr()),
    CStrPtr(BAND_STR_5G.as_ptr()),
    CStrPtr(BAND_STR_MAX.as_ptr()),
];

#[no_mangle]
pub static _band_to_band_cap: [u8; 3] = [BAND_CAP_2G, BAND_CAP_5G, 0];

static OPC_BW_STR_20: &[u8; 5] = b"20M \0";
static OPC_BW_STR_40PLUS: &[u8; 5] = b"40M+\0";
static OPC_BW_STR_40MINUS: &[u8; 5] = b"40M-\0";
static OPC_BW_STR_80: &[u8; 5] = b"80M \0";
static OPC_BW_STR_160: &[u8; 6] = b"160M \0";
static OPC_BW_STR_80P80: &[u8; 8] = b"80+80M \0";

#[no_mangle]
pub static _opc_bw_str: [CStrPtr; OPC_BW_NUM] = [
    CStrPtr(OPC_BW_STR_20.as_ptr()),
    CStrPtr(OPC_BW_STR_40PLUS.as_ptr()),
    CStrPtr(OPC_BW_STR_40MINUS.as_ptr()),
    CStrPtr(OPC_BW_STR_80.as_ptr()),
    CStrPtr(OPC_BW_STR_160.as_ptr()),
    CStrPtr(OPC_BW_STR_80P80.as_ptr()),
];

#[no_mangle]
pub static _opc_bw_to_ch_width: [u8; OPC_BW_NUM] = [
    CHANNEL_WIDTH_20,
    CHANNEL_WIDTH_40,
    CHANNEL_WIDTH_40,
    CHANNEL_WIDTH_80,
    CHANNEL_WIDTH_160,
    CHANNEL_WIDTH_80_80,
];

const HAL_PRIME_CHNL_OFFSET_DONT_CARE: u8 = 0;
const HAL_PRIME_CHNL_OFFSET_LOWER: u8 = 1;
const HAL_PRIME_CHNL_OFFSET_UPPER: u8 = 2;

const BAND_ON_2_4G: u8 = 0;
const BAND_ON_5G: u8 = 1;
const BAND_MAX: u8 = 2;

const CENTER_CH_2G_40M_NUM: usize = 9;
const CENTER_CH_2G_NUM: usize = 14;
const CENTER_CH_5G_20M_NUM: usize = 28;
const CENTER_CH_5G_40M_NUM: usize = 14;
const CENTER_CH_5G_80M_NUM: usize = 7;
const CENTER_CH_5G_160M_NUM: usize = 3;

#[no_mangle]
pub static center_ch_2g: [u8; CENTER_CH_2G_NUM] = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14];

#[no_mangle]
pub static center_ch_2g_40m: [u8; CENTER_CH_2G_40M_NUM] = [3, 4, 5, 6, 7, 8, 9, 10, 11];

static OP_CHS_OF_CCH_2G_40M: [[u8; 2]; CENTER_CH_2G_40M_NUM] = [
    [1, 5],
    [2, 6],
    [3, 7],
    [4, 8],
    [5, 9],
    [6, 10],
    [7, 11],
    [8, 12],
    [9, 13],
];

#[no_mangle]
pub static center_ch_5g_20m: [u8; CENTER_CH_5G_20M_NUM] = [
    36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144,
    149, 153, 157, 161, 165, 169, 173, 177,
];

#[no_mangle]
pub static center_ch_5g_40m: [u8; CENTER_CH_5G_40M_NUM] = [
    38, 46, 54, 62, 102, 110, 118, 126, 134, 142, 151, 159, 167, 175,
];

static OP_CHS_OF_CCH_5G_40M: [[u8; 2]; CENTER_CH_5G_40M_NUM] = [
    [36, 40],
    [44, 48],
    [52, 56],
    [60, 64],
    [100, 104],
    [108, 112],
    [116, 120],
    [124, 128],
    [132, 136],
    [140, 144],
    [149, 153],
    [157, 161],
    [165, 169],
    [173, 177],
];

#[no_mangle]
pub static center_ch_5g_80m: [u8; CENTER_CH_5G_80M_NUM] = [42, 58, 106, 122, 138, 155, 171];

static OP_CHS_OF_CCH_5G_80M: [[u8; 4]; CENTER_CH_5G_80M_NUM] = [
    [36, 40, 44, 48],
    [52, 56, 60, 64],
    [100, 104, 108, 112],
    [116, 120, 124, 128],
    [132, 136, 140, 144],
    [149, 153, 157, 161],
    [165, 169, 173, 177],
];

static CENTER_CH_5G_160M: [u8; CENTER_CH_5G_160M_NUM] = [50, 114, 163];

static OP_CHS_OF_CCH_5G_160M: [[u8; 8]; CENTER_CH_5G_160M_NUM] = [
    [36, 40, 44, 48, 52, 56, 60, 64],
    [100, 104, 108, 112, 116, 120, 124, 128],
    [149, 153, 157, 161, 165, 169, 173, 177],
];

fn center_chs_2g_table(bw: u8) -> Option<&'static [u8]> {
    match bw {
        0 => Some(&center_ch_2g),
        1 => Some(&center_ch_2g_40m),
        _ => None,
    }
}

fn center_chs_5g_table(bw: u8) -> Option<&'static [u8]> {
    match bw {
        0 => Some(&center_ch_5g_20m),
        1 => Some(&center_ch_5g_40m),
        2 => Some(&center_ch_5g_80m),
        3 => Some(&CENTER_CH_5G_160M),
        _ => None,
    }
}

fn op_chs_2g_num(bw: u8) -> u8 {
    match bw {
        0 => 1,
        1 => 2,
        _ => 0,
    }
}

fn op_chs_5g_num(bw: u8) -> u8 {
    match bw {
        0 => 1,
        1 => 2,
        2 => 4,
        3 => 8,
        _ => 0,
    }
}

fn op_chs_2g_row(bw: u8, idx: usize) -> Option<&'static [u8]> {
    match bw {
        0 if idx < center_ch_2g.len() => Some(&center_ch_2g[idx..idx + 1]),
        1 if idx < OP_CHS_OF_CCH_2G_40M.len() => Some(&OP_CHS_OF_CCH_2G_40M[idx]),
        _ => None,
    }
}

fn op_chs_5g_row(bw: u8, idx: usize) -> Option<&'static [u8]> {
    match bw {
        0 if idx < center_ch_5g_20m.len() => Some(&center_ch_5g_20m[idx..idx + 1]),
        1 if idx < OP_CHS_OF_CCH_5G_40M.len() => Some(&OP_CHS_OF_CCH_5G_40M[idx]),
        2 if idx < OP_CHS_OF_CCH_5G_80M.len() => Some(&OP_CHS_OF_CCH_5G_80M[idx]),
        3 if idx < OP_CHS_OF_CCH_5G_160M.len() => Some(&OP_CHS_OF_CCH_5G_160M[idx]),
        _ => None,
    }
}

#[cfg(not(host_rf_rest_test))]
mod kernel {
    use super::*;

    extern "C" {
        fn rtw_rust_rf_warn_on(condition: c_int);
        fn rtw_rust_rf_warn_invalid_ch(func: *const u8, ch: u8);
    }

    pub(super) fn warn_on(condition: bool) {
        unsafe { rtw_rust_rf_warn_on(condition as c_int) };
    }

    pub(super) fn warn_invalid_ch(func: &[u8], ch: u8) {
        unsafe { rtw_rust_rf_warn_invalid_ch(func.as_ptr(), ch) };
    }
}

#[cfg(host_rf_rest_test)]
mod kernel {
    pub(super) fn warn_on(_condition: bool) {}

    pub(super) fn warn_invalid_ch(_func: &[u8], _ch: u8) {}
}

fn rtw_get_scch_by_cch_offset_inner(cch: u8, bw: u8, offset: u8) -> u8 {
    let mut t_cch = 0u8;

    if bw == CHANNEL_WIDTH_20 {
        t_cch = cch;
        return t_cch;
    }

    if offset == HAL_PRIME_CHNL_OFFSET_DONT_CARE {
        kernel::warn_on(true);
        return t_cch;
    }

    if cch >= 3 && cch <= 11 && bw == CHANNEL_WIDTH_40 {
        t_cch = if offset == HAL_PRIME_CHNL_OFFSET_UPPER {
            cch + 2
        } else {
            cch - 2
        };
        return t_cch;
    }

    if cch >= 50 && cch <= 163 && bw == CHANNEL_WIDTH_160 {
        t_cch = if offset == HAL_PRIME_CHNL_OFFSET_UPPER {
            cch + 8
        } else {
            cch - 8
        };
        return t_cch;
    }

    if cch >= 42 && cch <= 171 && bw == CHANNEL_WIDTH_80 {
        t_cch = if offset == HAL_PRIME_CHNL_OFFSET_UPPER {
            cch + 4
        } else {
            cch - 4
        };
        return t_cch;
    }

    if cch >= 38 && cch <= 175 && bw == CHANNEL_WIDTH_40 {
        t_cch = if offset == HAL_PRIME_CHNL_OFFSET_UPPER {
            cch + 2
        } else {
            cch - 2
        };
        return t_cch;
    }

    kernel::warn_on(true);
    t_cch
}

#[no_mangle]
pub extern "C" fn rtw_get_scch_by_cch_offset(cch: u8, bw: u8, offset: u8) -> u8 {
    rtw_get_scch_by_cch_offset_inner(cch, bw, offset)
}

#[no_mangle]
pub extern "C" fn rtw_get_scch_by_cch_opch(cch: u8, bw: u8, opch: u8) -> u8 {
    let mut offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;

    if opch > cch {
        offset = HAL_PRIME_CHNL_OFFSET_UPPER;
    } else if opch < cch {
        offset = HAL_PRIME_CHNL_OFFSET_LOWER;
    }

    rtw_get_scch_by_cch_offset_inner(cch, bw, offset)
}

#[no_mangle]
pub extern "C" fn center_chs_2g_num(bw: u8) -> u8 {
    center_chs_2g_table(bw).map(|t| t.len() as u8).unwrap_or(0)
}

#[no_mangle]
pub extern "C" fn center_chs_2g(bw: u8, id: u8) -> u8 {
    let Some(table) = center_chs_2g_table(bw) else {
        return 0;
    };
    if id as usize >= table.len() {
        return 0;
    }
    table[id as usize]
}

#[no_mangle]
pub extern "C" fn center_chs_5g_num(bw: u8) -> u8 {
    center_chs_5g_table(bw).map(|t| t.len() as u8).unwrap_or(0)
}

#[no_mangle]
pub extern "C" fn center_chs_5g(bw: u8, id: u8) -> u8 {
    let Some(table) = center_chs_5g_table(bw) else {
        return 0;
    };
    if id as usize >= table.len() {
        return 0;
    }
    table[id as usize]
}

#[no_mangle]
pub extern "C" fn rtw_get_op_chs_by_cch_bw(
    cch: u8,
    bw: u8,
    op_chs: *mut *mut u8,
    op_ch_num: *mut u8,
) -> u8 {
    let is_2g = cch <= 14 && bw <= CHANNEL_WIDTH_40;
    let is_5g = cch >= 36 && cch <= 177 && bw <= CHANNEL_WIDTH_160;
    if !is_2g && !is_5g {
        return 0;
    }

    let c_table = if is_2g {
        center_chs_2g_table(bw)
    } else {
        center_chs_5g_table(bw)
    };
    let Some(c_table) = c_table else {
        return 0;
    };

    let mut i = 0usize;
    while i < c_table.len() {
        if cch == c_table[i] {
            break;
        }
        i += 1;
    }

    if i == c_table.len() {
        return 0;
    }

    let row = if is_2g {
        op_chs_2g_row(bw, i)
    } else {
        op_chs_5g_row(bw, i)
    };
    let Some(row) = row else {
        return 0;
    };

    if !op_chs.is_null() {
        unsafe {
            *op_chs = row.as_ptr() as *mut u8;
        }
    }
    if !op_ch_num.is_null() {
        unsafe {
            *op_ch_num = if is_2g {
                op_chs_2g_num(bw)
            } else {
                op_chs_5g_num(bw)
            };
        }
    }

    1
}

#[no_mangle]
pub extern "C" fn rtw_get_offset_by_chbw(ch: u8, bw: u8, r_offset: *mut u8) -> u8 {
    let mut valid = 1u8;
    let mut offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;

    if bw == CHANNEL_WIDTH_20 {
        if valid != 0 && !r_offset.is_null() {
            unsafe {
                *r_offset = offset;
            }
        }
        return valid;
    }

    if bw >= CHANNEL_WIDTH_80 && ch <= 14 {
        return 0;
    }

    if (1..=4).contains(&ch) {
        offset = HAL_PRIME_CHNL_OFFSET_LOWER;
    } else if (5..=9).contains(&ch) {
        if !r_offset.is_null() {
            let in_offset = unsafe { *r_offset };
            if in_offset == HAL_PRIME_CHNL_OFFSET_LOWER || in_offset == HAL_PRIME_CHNL_OFFSET_UPPER
            {
                offset = in_offset;
            } else {
                offset = HAL_PRIME_CHNL_OFFSET_UPPER;
            }
        } else {
            offset = HAL_PRIME_CHNL_OFFSET_UPPER;
        }
    } else if (10..=13).contains(&ch) {
        offset = HAL_PRIME_CHNL_OFFSET_UPPER;
    } else if ch == 14 {
        return 0;
    } else if (36..=177).contains(&ch) {
        offset = match ch {
            36 | 44 | 52 | 60 | 100 | 108 | 116 | 124 | 132 | 140 | 149 | 157 | 165 | 173 => {
                HAL_PRIME_CHNL_OFFSET_LOWER
            }
            40 | 48 | 56 | 64 | 104 | 112 | 120 | 128 | 136 | 144 | 153 | 161 | 169 | 177 => {
                HAL_PRIME_CHNL_OFFSET_UPPER
            }
            _ => {
                valid = 0;
                HAL_PRIME_CHNL_OFFSET_DONT_CARE
            }
        };
    } else {
        valid = 0;
    }

    if valid != 0 && !r_offset.is_null() {
        unsafe {
            *r_offset = offset;
        }
    }

    valid
}

#[no_mangle]
pub extern "C" fn rtw_get_center_ch(ch: u8, bw: u8, offset: u8) -> u8 {
    let mut cch = ch;

    if bw == CHANNEL_WIDTH_160 {
        if ch % 4 == 0 {
            if (36..=64).contains(&ch) {
                cch = 50;
            } else if (100..=128).contains(&ch) {
                cch = 114;
            }
        } else if ch % 4 == 1 {
            if (149..=177).contains(&ch) {
                cch = 163;
            }
        }
    } else if bw == CHANNEL_WIDTH_80 {
        if ch <= 14 {
            cch = 7;
        } else if ch % 4 == 0 {
            if (36..=48).contains(&ch) {
                cch = 42;
            } else if (52..=64).contains(&ch) {
                cch = 58;
            } else if (100..=112).contains(&ch) {
                cch = 106;
            } else if (116..=128).contains(&ch) {
                cch = 122;
            } else if (132..=144).contains(&ch) {
                cch = 138;
            }
        } else if ch % 4 == 1 {
            if (149..=161).contains(&ch) {
                cch = 155;
            } else if (165..=177).contains(&ch) {
                cch = 171;
            }
        }
    } else if bw == CHANNEL_WIDTH_40 {
        if offset == HAL_PRIME_CHNL_OFFSET_LOWER {
            cch = ch + 2;
        } else if offset == HAL_PRIME_CHNL_OFFSET_UPPER {
            cch = ch - 2;
        }
    } else if bw == CHANNEL_WIDTH_20 || bw == CHANNEL_WIDTH_10 || bw == CHANNEL_WIDTH_5 {
        // same as ch
    } else {
        kernel::warn_on(true);
    }

    cch
}

#[no_mangle]
pub extern "C" fn rtw_get_ch_group(ch: u8, group: *mut u8, cck_group: *mut u8) -> u8 {
    let mut band = BAND_MAX;
    let mut gp: i8 = -1;
    let mut cck_gp: i8 = -1;

    if ch <= 14 {
        band = BAND_ON_2_4G;

        gp = if (1..=2).contains(&ch) {
            0
        } else if (3..=5).contains(&ch) {
            1
        } else if (6..=8).contains(&ch) {
            2
        } else if (9..=11).contains(&ch) {
            3
        } else if (12..=14).contains(&ch) {
            4
        } else {
            band = BAND_MAX;
            -1
        };

        if ch == 14 {
            cck_gp = 5;
        } else {
            cck_gp = gp;
        }
    } else {
        band = BAND_ON_5G;

        gp = if (36..=42).contains(&ch) {
            0
        } else if (44..=48).contains(&ch) {
            1
        } else if (50..=58).contains(&ch) {
            2
        } else if (60..=64).contains(&ch) {
            3
        } else if (100..=106).contains(&ch) {
            4
        } else if (108..=114).contains(&ch) {
            5
        } else if (116..=122).contains(&ch) {
            6
        } else if (124..=130).contains(&ch) {
            7
        } else if (132..=138).contains(&ch) {
            8
        } else if (140..=144).contains(&ch) {
            9
        } else if (149..=155).contains(&ch) {
            10
        } else if (157..=161).contains(&ch) {
            11
        } else if (165..=171).contains(&ch) {
            12
        } else if (173..=177).contains(&ch) {
            13
        } else {
            band = BAND_MAX;
            -1
        };
    }

    if band == BAND_MAX || (band == BAND_ON_2_4G && cck_gp == -1) || gp == -1 {
        kernel::warn_invalid_ch(b"rtw_get_ch_group\0", ch);
        kernel::warn_on(true);
        return band;
    }

    if !group.is_null() {
        unsafe {
            *group = gp as u8;
        }
    }
    if !cck_group.is_null() && band == BAND_ON_2_4G {
        unsafe {
            *cck_group = cck_gp as u8;
        }
    }

    band
}

#[no_mangle]
pub extern "C" fn rtw_ch2freq(chan: i32) -> i32 {
    if (1..=14).contains(&chan) {
        if chan == 14 {
            2484
        } else {
            2407 + chan * 5
        }
    } else if (36..=177).contains(&chan) {
        5000 + chan * 5
    } else {
        0
    }
}

#[no_mangle]
pub extern "C" fn rtw_freq2ch(freq: i32) -> i32 {
    if freq == 2484 {
        14
    } else if freq < 2484 {
        (freq - 2407) / 5
    } else if (4910..=4980).contains(&freq) {
        (freq - 4000) / 5
    } else if freq <= 45000 {
        (freq - 5000) / 5
    } else if (58320..=64800).contains(&freq) {
        (freq - 56160) / 2160
    } else {
        0
    }
}

#[no_mangle]
pub extern "C" fn rtw_chbw_to_freq_range(
    ch: u8,
    bw: u8,
    offset: u8,
    hi: *mut u32,
    lo: *mut u32,
) -> bool {
    let mut hi_ret = 0u32;
    let mut lo_ret = 0u32;
    let mut valid = false;

    if !hi.is_null() {
        unsafe {
            *hi = 0;
        }
    }
    if !lo.is_null() {
        unsafe {
            *lo = 0;
        }
    }

    let c_ch = rtw_get_center_ch(ch, bw, offset);
    let freq = rtw_ch2freq(c_ch as i32);

    if freq == 0 {
        kernel::warn_on(true);
        return valid;
    }

    if bw == CHANNEL_WIDTH_160 {
        hi_ret = (freq + 80) as u32;
        lo_ret = (freq - 80) as u32;
    } else if bw == CHANNEL_WIDTH_80 {
        hi_ret = (freq + 40) as u32;
        lo_ret = (freq - 40) as u32;
    } else if bw == CHANNEL_WIDTH_40 {
        hi_ret = (freq + 20) as u32;
        lo_ret = (freq - 20) as u32;
    } else if bw == CHANNEL_WIDTH_20 {
        hi_ret = (freq + 10) as u32;
        lo_ret = (freq - 10) as u32;
    } else {
        kernel::warn_on(true);
    }

    if !hi.is_null() {
        unsafe {
            *hi = hi_ret;
        }
    }
    if !lo.is_null() {
        unsafe {
            *lo = lo_ret;
        }
    }

    valid = true;
    valid
}

// W3-22: global operating-class lookup (802.11-2016 Table E-4, partial).

const OPC_BW20: u8 = 0;
const OPC_BW40PLUS: u8 = 1;
const OPC_BW40MINUS: u8 = 2;
const OPC_BW80: u8 = 3;
const OPC_BW160: u8 = 4;
const OPC_BW_ENUM_NUM: u8 = 6;

#[repr(C)]
pub struct OpClassT {
    pub class_id: u8,
    pub band: i32,
    pub bw: i32,
    pub len_ch_attr: *const u8,
}

unsafe impl Sync for OpClassT {}

static OP_CLASS_ATTR_81: [u8; 14] = [13, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13];
static OP_CLASS_ATTR_82: [u8; 2] = [1, 14];
static OP_CLASS_ATTR_83: [u8; 10] = [9, 1, 2, 3, 4, 5, 6, 7, 8, 9];
static OP_CLASS_ATTR_84: [u8; 10] = [9, 5, 6, 7, 8, 9, 10, 11, 12, 13];
static OP_CLASS_ATTR_115: [u8; 5] = [4, 36, 40, 44, 48];
static OP_CLASS_ATTR_116: [u8; 3] = [2, 36, 44];
static OP_CLASS_ATTR_117: [u8; 3] = [2, 40, 48];
static OP_CLASS_ATTR_118: [u8; 5] = [4, 52, 56, 60, 64];
static OP_CLASS_ATTR_119: [u8; 3] = [2, 52, 60];
static OP_CLASS_ATTR_120: [u8; 3] = [2, 56, 64];
static OP_CLASS_ATTR_121: [u8; 13] = [12, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144];
static OP_CLASS_ATTR_122: [u8; 7] = [6, 100, 108, 116, 124, 132, 140];
static OP_CLASS_ATTR_123: [u8; 7] = [6, 104, 112, 120, 128, 136, 144];
static OP_CLASS_ATTR_124: [u8; 5] = [4, 149, 153, 157, 161];
static OP_CLASS_ATTR_125: [u8; 7] = [6, 149, 153, 157, 161, 165, 169];
static OP_CLASS_ATTR_126: [u8; 3] = [2, 149, 157];
static OP_CLASS_ATTR_127: [u8; 3] = [2, 153, 161];
static OP_CLASS_ATTR_128: [u8; 25] = [
    24, 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140,
    144, 149, 153, 157, 161,
];
static OP_CLASS_ATTR_129: [u8; 17] = [
    16, 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128,
];

#[no_mangle]
pub static global_op_class: [OpClassT; 19] = [
    OpClassT {
        class_id: 81,
        band: BAND_ON_2_4G as i32,
        bw: OPC_BW20 as i32,
        len_ch_attr: OP_CLASS_ATTR_81.as_ptr(),
    },
    OpClassT {
        class_id: 82,
        band: BAND_ON_2_4G as i32,
        bw: OPC_BW20 as i32,
        len_ch_attr: OP_CLASS_ATTR_82.as_ptr(),
    },
    OpClassT {
        class_id: 83,
        band: BAND_ON_2_4G as i32,
        bw: OPC_BW40PLUS as i32,
        len_ch_attr: OP_CLASS_ATTR_83.as_ptr(),
    },
    OpClassT {
        class_id: 84,
        band: BAND_ON_2_4G as i32,
        bw: OPC_BW40MINUS as i32,
        len_ch_attr: OP_CLASS_ATTR_84.as_ptr(),
    },
    OpClassT {
        class_id: 115,
        band: BAND_ON_5G as i32,
        bw: OPC_BW20 as i32,
        len_ch_attr: OP_CLASS_ATTR_115.as_ptr(),
    },
    OpClassT {
        class_id: 116,
        band: BAND_ON_5G as i32,
        bw: OPC_BW40PLUS as i32,
        len_ch_attr: OP_CLASS_ATTR_116.as_ptr(),
    },
    OpClassT {
        class_id: 117,
        band: BAND_ON_5G as i32,
        bw: OPC_BW40MINUS as i32,
        len_ch_attr: OP_CLASS_ATTR_117.as_ptr(),
    },
    OpClassT {
        class_id: 118,
        band: BAND_ON_5G as i32,
        bw: OPC_BW20 as i32,
        len_ch_attr: OP_CLASS_ATTR_118.as_ptr(),
    },
    OpClassT {
        class_id: 119,
        band: BAND_ON_5G as i32,
        bw: OPC_BW40PLUS as i32,
        len_ch_attr: OP_CLASS_ATTR_119.as_ptr(),
    },
    OpClassT {
        class_id: 120,
        band: BAND_ON_5G as i32,
        bw: OPC_BW40MINUS as i32,
        len_ch_attr: OP_CLASS_ATTR_120.as_ptr(),
    },
    OpClassT {
        class_id: 121,
        band: BAND_ON_5G as i32,
        bw: OPC_BW20 as i32,
        len_ch_attr: OP_CLASS_ATTR_121.as_ptr(),
    },
    OpClassT {
        class_id: 122,
        band: BAND_ON_5G as i32,
        bw: OPC_BW40PLUS as i32,
        len_ch_attr: OP_CLASS_ATTR_122.as_ptr(),
    },
    OpClassT {
        class_id: 123,
        band: BAND_ON_5G as i32,
        bw: OPC_BW40MINUS as i32,
        len_ch_attr: OP_CLASS_ATTR_123.as_ptr(),
    },
    OpClassT {
        class_id: 124,
        band: BAND_ON_5G as i32,
        bw: OPC_BW20 as i32,
        len_ch_attr: OP_CLASS_ATTR_124.as_ptr(),
    },
    OpClassT {
        class_id: 125,
        band: BAND_ON_5G as i32,
        bw: OPC_BW20 as i32,
        len_ch_attr: OP_CLASS_ATTR_125.as_ptr(),
    },
    OpClassT {
        class_id: 126,
        band: BAND_ON_5G as i32,
        bw: OPC_BW40PLUS as i32,
        len_ch_attr: OP_CLASS_ATTR_126.as_ptr(),
    },
    OpClassT {
        class_id: 127,
        band: BAND_ON_5G as i32,
        bw: OPC_BW40MINUS as i32,
        len_ch_attr: OP_CLASS_ATTR_127.as_ptr(),
    },
    OpClassT {
        class_id: 128,
        band: BAND_ON_5G as i32,
        bw: OPC_BW80 as i32,
        len_ch_attr: OP_CLASS_ATTR_128.as_ptr(),
    },
    OpClassT {
        class_id: 129,
        band: BAND_ON_5G as i32,
        bw: OPC_BW160 as i32,
        len_ch_attr: OP_CLASS_ATTR_129.as_ptr(),
    },
];

#[no_mangle]
pub static global_op_class_num: i32 = 19;

fn opc_bw_to_ch_width_bw(bw: i32) -> u8 {
    if bw >= 0 && (bw as usize) < OPC_BW_NUM {
        _opc_bw_to_ch_width[bw as usize]
    } else {
        CHANNEL_WIDTH_MAX as u8
    }
}

fn rtw_is_2g_ch(ch: u8) -> bool {
    (1..=14).contains(&ch)
}

fn rtw_is_5g_ch(ch: u8) -> bool {
    (36..=177).contains(&ch)
}

fn get_global_op_class_by_id(gid: u8) -> Option<usize> {
    (0..global_op_class.len()).find(|&i| global_op_class[i].class_id == gid)
}

fn is_valid_global_op_class_ch_idx(idx: usize, ch: u8) -> bool {
    let ent = &global_op_class[idx];
    if ent.len_ch_attr.is_null() {
        return false;
    }
    unsafe {
        let len = *ent.len_ch_attr as usize;
        let attrs = core::slice::from_raw_parts(ent.len_ch_attr, len + 1);
        attrs[1..=len].iter().any(|&c| c == ch)
    }
}

fn get_global_opc_bw_by_id(gid: u8) -> i32 {
    get_global_op_class_by_id(gid)
        .map(|i| global_op_class[i].bw)
        .unwrap_or(OPC_BW_ENUM_NUM as i32)
}

#[no_mangle]
pub extern "C" fn is_valid_global_op_class_id(gid: u8) -> bool {
    get_global_op_class_by_id(gid).is_some()
}

#[no_mangle]
pub extern "C" fn get_sub_op_class(gid: u8, ch: u8) -> i16 {
    let Some(idx) = get_global_op_class_by_id(gid) else {
        return -1;
    };

    if !is_valid_global_op_class_ch_idx(idx, ch) {
        return -1;
    }

    let opc = &global_op_class[idx];
    if opc.bw == OPC_BW20 as i32 {
        return 0;
    }

    let bw = opc_bw_to_ch_width_bw(opc.bw);
    for i in 0..global_op_class.len() {
        if bw != opc_bw_to_ch_width_bw(global_op_class[i].bw).wrapping_add(1) {
            continue;
        }
        if is_valid_global_op_class_ch_idx(i, ch) {
            return global_op_class[i].class_id as i16;
        }
    }

    -2
}

#[no_mangle]
pub extern "C" fn rtw_get_op_class_by_chbw(ch: u8, bw: u8, offset: u8) -> u8 {
    let band = if rtw_is_2g_ch(ch) {
        BAND_ON_2_4G
    } else if rtw_is_5g_ch(ch) {
        BAND_ON_5G
    } else {
        return 0;
    };

    if !matches!(
        bw,
        CHANNEL_WIDTH_20 | CHANNEL_WIDTH_40 | CHANNEL_WIDTH_80 | CHANNEL_WIDTH_160
    ) {
        return 0;
    }

    for i in 0..global_op_class.len() {
        let ent = &global_op_class[i];
        if ent.band != band as i32 {
            continue;
        }
        if opc_bw_to_ch_width_bw(ent.bw) != bw {
            continue;
        }
        if (ent.bw == OPC_BW40PLUS as i32 && offset != HAL_PRIME_CHNL_OFFSET_LOWER)
            || (ent.bw == OPC_BW40MINUS as i32 && offset != HAL_PRIME_CHNL_OFFSET_UPPER)
        {
            continue;
        }
        if is_valid_global_op_class_ch_idx(i, ch) {
            return ent.class_id;
        }
    }

    0
}

#[no_mangle]
pub extern "C" fn rtw_get_bw_offset_by_op_class_ch(
    gid: u8,
    ch: u8,
    bw: *mut u8,
    offset: *mut u8,
) -> u8 {
    if bw.is_null() || offset.is_null() {
        return 0;
    }

    let opc_bw = get_global_opc_bw_by_id(gid);
    if opc_bw == OPC_BW_ENUM_NUM as i32 {
        return 0;
    }

    unsafe {
        *bw = opc_bw_to_ch_width_bw(opc_bw);
        if opc_bw == OPC_BW40PLUS as i32 {
            *offset = HAL_PRIME_CHNL_OFFSET_LOWER;
        } else if opc_bw == OPC_BW40MINUS as i32 {
            *offset = HAL_PRIME_CHNL_OFFSET_UPPER;
        }
        if rtw_get_offset_by_chbw(ch, *bw, offset) != 0 {
            1
        } else {
            0
        }
    }
}

const RF_PATH_MAX: usize = 4;
const RF_TYPE_MAX: u8 = 16;

const RF_1T1R: u8 = 0;
const RF_1T2R: u8 = 1;
const RF_2T2R: u8 = 2;
const RF_2T3R: u8 = 3;
const RF_2T4R: u8 = 4;
const RF_3T3R: u8 = 5;
const RF_3T4R: u8 = 6;
const RF_4T4R: u8 = 7;
const RF_4T3R: u8 = 8;
const RF_4T2R: u8 = 9;
const RF_4T1R: u8 = 10;
const RF_3T2R: u8 = 11;
const RF_3T1R: u8 = 12;
const RF_2T1R: u8 = 13;
const RF_1T4R: u8 = 14;
const RF_1T3R: u8 = 15;

static _RF_TYPE_TO_RF_TX_CNT: [u8; RF_TYPE_MAX as usize] = [
    1, 1, 2, 2, 2, 3, 3, 4, 4, 4, 4, 3, 3, 2, 1, 1,
];

static _RF_TYPE_TO_RF_RX_CNT: [u8; RF_TYPE_MAX as usize] = [
    1, 2, 2, 3, 4, 3, 4, 4, 3, 2, 1, 2, 1, 1, 4, 3,
];

static _TRX_NUM_TO_RF_TYPE: [[u8; RF_PATH_MAX]; RF_PATH_MAX] = [
    [RF_1T1R, RF_1T2R, RF_1T3R, RF_1T4R],
    [RF_2T1R, RF_2T2R, RF_2T3R, RF_2T4R],
    [RF_3T1R, RF_3T2R, RF_3T3R, RF_3T4R],
    [RF_4T1R, RF_4T2R, RF_4T3R, RF_4T4R],
];

fn rf_type_valid(rf_type: u8) -> bool {
    rf_type < RF_TYPE_MAX
}

fn rf_type_to_rf_tx_cnt(rf_type: u8) -> u8 {
    if rf_type_valid(rf_type) {
        _RF_TYPE_TO_RF_TX_CNT[rf_type as usize]
    } else {
        0
    }
}

fn rf_type_to_rf_rx_cnt(rf_type: u8) -> u8 {
    if rf_type_valid(rf_type) {
        _RF_TYPE_TO_RF_RX_CNT[rf_type as usize]
    } else {
        0
    }
}

#[no_mangle]
pub extern "C" fn rf_type_to_default_trx_bmp(rf: u8, tx: *mut u32, rx: *mut u32) {
    if tx.is_null() || rx.is_null() {
        return;
    }

    let tx_num = rf_type_to_rf_tx_cnt(rf);
    let rx_num = rf_type_to_rf_rx_cnt(rf);
    let mut tx_bmp = 0u32;
    let mut rx_bmp = 0u32;

    for i in 0..tx_num {
        tx_bmp |= 1u32 << i;
    }
    for i in 0..rx_num {
        rx_bmp |= 1u32 << i;
    }

    unsafe {
        *tx = tx_bmp;
        *rx = rx_bmp;
    }
}

#[no_mangle]
pub extern "C" fn trx_num_to_rf_type(tx_num: u8, rx_num: u8) -> i32 {
    if tx_num > 0 && tx_num <= RF_PATH_MAX as u8 && rx_num > 0 && rx_num <= RF_PATH_MAX as u8 {
        _TRX_NUM_TO_RF_TYPE[(tx_num - 1) as usize][(rx_num - 1) as usize] as i32
    } else {
        RF_TYPE_MAX as i32
    }
}

#[no_mangle]
pub extern "C" fn trx_bmp_to_rf_type(tx_bmp: u8, rx_bmp: u8) -> i32 {
    let mut tx_num = 0u8;
    let mut rx_num = 0u8;

    for i in 0..RF_PATH_MAX {
        if (tx_bmp >> i) & 1 != 0 {
            tx_num += 1;
        }
        if (rx_bmp >> i) & 1 != 0 {
            rx_num += 1;
        }
    }

    trx_num_to_rf_type(tx_num, rx_num)
}

#[no_mangle]
pub extern "C" fn rf_type_is_a_in_b(a: u8, b: u8) -> bool {
    rf_type_to_rf_tx_cnt(a) <= rf_type_to_rf_tx_cnt(b)
        && rf_type_to_rf_rx_cnt(a) <= rf_type_to_rf_rx_cnt(b)
}

fn rtw_path_bmp_limit_from_higher(bmp: &mut u8, bmp_bit_cnt: &mut u8, bit_cnt_lmt: u8) {
    let mut i = RF_PATH_MAX as i32 - 1;
    while *bmp_bit_cnt > bit_cnt_lmt && i >= 0 {
        if *bmp & (1 << i) != 0 {
            *bmp &= !(1 << i);
            *bmp_bit_cnt -= 1;
        }
        i -= 1;
    }
}

#[no_mangle]
pub extern "C" fn rtw_restrict_trx_path_bmp_by_trx_num_lmt(
    trx_path_bmp: u8,
    tx_num_lmt: u8,
    rx_num_lmt: u8,
    tx_num: *mut u8,
    rx_num: *mut u8,
) -> u8 {
    let mut bmp_tx = (trx_path_bmp & 0xF0) >> 4;
    let mut bmp_rx = trx_path_bmp & 0x0F;
    let mut bmp_tx_num = 0u8;
    let mut bmp_rx_num = 0u8;
    let mut ret_type = RF_TYPE_MAX as i32;

    for i in 0..RF_PATH_MAX {
        if bmp_tx & (1 << i) != 0 {
            bmp_tx_num += 1;
        }
        if bmp_rx & (1 << i) != 0 {
            bmp_rx_num += 1;
        }
    }

    if tx_num_lmt != 0 {
        rtw_path_bmp_limit_from_higher(&mut bmp_tx, &mut bmp_tx_num, tx_num_lmt);
    }
    if rx_num_lmt != 0 {
        rtw_path_bmp_limit_from_higher(&mut bmp_rx, &mut bmp_rx_num, rx_num_lmt);
    }

    let mut j = bmp_rx_num;
    while j > 0 {
        let mut i = bmp_tx_num;
        while i > 0 {
            ret_type = trx_num_to_rf_type(i, j);
            if rf_type_valid(ret_type as u8) {
                rtw_path_bmp_limit_from_higher(&mut bmp_tx, &mut bmp_tx_num, i);
                rtw_path_bmp_limit_from_higher(&mut bmp_rx, &mut bmp_rx_num, j);
                if !tx_num.is_null() {
                    unsafe {
                        *tx_num = bmp_tx_num;
                    }
                }
                if !rx_num.is_null() {
                    unsafe {
                        *rx_num = bmp_rx_num;
                    }
                }
                return if rf_type_valid(ret_type as u8) {
                    (bmp_tx << 4) | bmp_rx
                } else {
                    0
                };
            }
            i -= 1;
        }
        j -= 1;
    }

    if rf_type_valid(ret_type as u8) {
        (bmp_tx << 4) | bmp_rx
    } else {
        0
    }
}

#[no_mangle]
pub extern "C" fn rtw_restrict_trx_path_bmp_by_rftype(
    trx_path_bmp: u8,
    rf_type: u8,
    tx_num: *mut u8,
    rx_num: *mut u8,
) -> u8 {
    rtw_restrict_trx_path_bmp_by_trx_num_lmt(
        trx_path_bmp,
        rf_type_to_rf_tx_cnt(rf_type),
        rf_type_to_rf_rx_cnt(rf_type),
        tx_num,
        rx_num,
    )
}
