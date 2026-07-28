// SPDX-License-Identifier: GPL-2.0
//! RF rest helpers — Rust port of `core/rtw_rf_rest.c` channel layout (W3-19).

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
const CHANNEL_WIDTH_5: u8 = 5;
const CHANNEL_WIDTH_10: u8 = 6;

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
