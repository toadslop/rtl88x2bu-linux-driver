// Generated for W4-17 — keep in sync with hal_hdata_rate_c_oracle.c
use super::{U8, DESC_RATE_NUM};
use core::ffi::c_char;

static HDATA_STR_00: [u8; 7] = [67, 67, 75, 95, 49, 77, 0];
static HDATA_STR_01: [u8; 7] = [67, 67, 75, 95, 50, 77, 0];
static HDATA_STR_02: [u8; 8] = [67, 67, 75, 53, 95, 53, 77, 0];
static HDATA_STR_03: [u8; 8] = [67, 67, 75, 95, 49, 49, 77, 0];
static HDATA_STR_04: [u8; 8] = [79, 70, 68, 77, 95, 54, 77, 0];
static HDATA_STR_05: [u8; 8] = [79, 70, 68, 77, 95, 57, 77, 0];
static HDATA_STR_06: [u8; 9] = [79, 70, 68, 77, 95, 49, 50, 77, 0];
static HDATA_STR_07: [u8; 9] = [79, 70, 68, 77, 95, 49, 56, 77, 0];
static HDATA_STR_08: [u8; 9] = [79, 70, 68, 77, 95, 50, 52, 77, 0];
static HDATA_STR_09: [u8; 9] = [79, 70, 68, 77, 95, 51, 54, 77, 0];
static HDATA_STR_0a: [u8; 9] = [79, 70, 68, 77, 95, 52, 56, 77, 0];
static HDATA_STR_0b: [u8; 9] = [79, 70, 68, 77, 95, 53, 52, 77, 0];
static HDATA_STR_0c: [u8; 5] = [77, 67, 83, 48, 0];
static HDATA_STR_0d: [u8; 5] = [77, 67, 83, 49, 0];
static HDATA_STR_0e: [u8; 5] = [77, 67, 83, 50, 0];
static HDATA_STR_0f: [u8; 5] = [77, 67, 83, 51, 0];
static HDATA_STR_10: [u8; 5] = [77, 67, 83, 52, 0];
static HDATA_STR_11: [u8; 5] = [77, 67, 83, 53, 0];
static HDATA_STR_12: [u8; 5] = [77, 67, 83, 54, 0];
static HDATA_STR_13: [u8; 5] = [77, 67, 83, 55, 0];
static HDATA_STR_14: [u8; 5] = [77, 67, 83, 56, 0];
static HDATA_STR_15: [u8; 5] = [77, 67, 83, 57, 0];
static HDATA_STR_16: [u8; 6] = [77, 67, 83, 49, 48, 0];
static HDATA_STR_17: [u8; 6] = [77, 67, 83, 49, 49, 0];
static HDATA_STR_18: [u8; 6] = [77, 67, 83, 49, 50, 0];
static HDATA_STR_19: [u8; 6] = [77, 67, 83, 49, 51, 0];
static HDATA_STR_1a: [u8; 6] = [77, 67, 83, 49, 52, 0];
static HDATA_STR_1b: [u8; 6] = [77, 67, 83, 49, 53, 0];
static HDATA_STR_1c: [u8; 6] = [77, 67, 83, 49, 54, 0];
static HDATA_STR_1d: [u8; 6] = [77, 67, 83, 49, 55, 0];
static HDATA_STR_1e: [u8; 6] = [77, 67, 83, 49, 56, 0];
static HDATA_STR_1f: [u8; 6] = [77, 67, 83, 49, 57, 0];
static HDATA_STR_20: [u8; 6] = [77, 67, 83, 50, 48, 0];
static HDATA_STR_21: [u8; 6] = [77, 67, 83, 50, 49, 0];
static HDATA_STR_22: [u8; 6] = [77, 67, 83, 50, 50, 0];
static HDATA_STR_23: [u8; 6] = [77, 67, 83, 50, 51, 0];
static HDATA_STR_24: [u8; 6] = [77, 67, 83, 50, 52, 0];
static HDATA_STR_25: [u8; 6] = [77, 67, 83, 50, 53, 0];
static HDATA_STR_26: [u8; 6] = [77, 67, 83, 50, 54, 0];
static HDATA_STR_27: [u8; 6] = [77, 67, 83, 50, 55, 0];
static HDATA_STR_28: [u8; 6] = [77, 67, 83, 50, 56, 0];
static HDATA_STR_29: [u8; 6] = [77, 67, 83, 50, 57, 0];
static HDATA_STR_2a: [u8; 6] = [77, 67, 83, 51, 48, 0];
static HDATA_STR_2b: [u8; 6] = [77, 67, 83, 51, 49, 0];
static HDATA_STR_2c: [u8; 10] = [86, 72, 84, 49, 83, 77, 67, 83, 48, 0];
static HDATA_STR_2d: [u8; 10] = [86, 72, 84, 49, 83, 77, 67, 83, 49, 0];
static HDATA_STR_2e: [u8; 10] = [86, 72, 84, 49, 83, 77, 67, 83, 50, 0];
static HDATA_STR_2f: [u8; 10] = [86, 72, 84, 49, 83, 77, 67, 83, 51, 0];
static HDATA_STR_30: [u8; 10] = [86, 72, 84, 49, 83, 77, 67, 83, 52, 0];
static HDATA_STR_31: [u8; 10] = [86, 72, 84, 49, 83, 77, 67, 83, 53, 0];
static HDATA_STR_32: [u8; 10] = [86, 72, 84, 49, 83, 77, 67, 83, 54, 0];
static HDATA_STR_33: [u8; 10] = [86, 72, 84, 49, 83, 77, 67, 83, 55, 0];
static HDATA_STR_34: [u8; 10] = [86, 72, 84, 49, 83, 77, 67, 83, 56, 0];
static HDATA_STR_35: [u8; 10] = [86, 72, 84, 49, 83, 77, 67, 83, 57, 0];
static HDATA_STR_36: [u8; 10] = [86, 72, 84, 50, 83, 77, 67, 83, 48, 0];
static HDATA_STR_37: [u8; 10] = [86, 72, 84, 50, 83, 77, 67, 83, 49, 0];
static HDATA_STR_38: [u8; 10] = [86, 72, 84, 50, 83, 77, 67, 83, 50, 0];
static HDATA_STR_39: [u8; 10] = [86, 72, 84, 50, 83, 77, 67, 83, 51, 0];
static HDATA_STR_3a: [u8; 10] = [86, 72, 84, 50, 83, 77, 67, 83, 52, 0];
static HDATA_STR_3b: [u8; 10] = [86, 72, 84, 50, 83, 77, 67, 83, 53, 0];
static HDATA_STR_3c: [u8; 10] = [86, 72, 84, 50, 83, 77, 67, 83, 54, 0];
static HDATA_STR_3d: [u8; 10] = [86, 72, 84, 50, 83, 77, 67, 83, 55, 0];
static HDATA_STR_3e: [u8; 10] = [86, 72, 84, 50, 83, 77, 67, 83, 56, 0];
static HDATA_STR_3f: [u8; 10] = [86, 72, 84, 50, 83, 77, 67, 83, 57, 0];
static HDATA_STR_40: [u8; 10] = [86, 72, 84, 51, 83, 77, 67, 83, 48, 0];
static HDATA_STR_41: [u8; 10] = [86, 72, 84, 51, 83, 77, 67, 83, 49, 0];
static HDATA_STR_42: [u8; 10] = [86, 72, 84, 51, 83, 77, 67, 83, 50, 0];
static HDATA_STR_43: [u8; 10] = [86, 72, 84, 51, 83, 77, 67, 83, 51, 0];
static HDATA_STR_44: [u8; 10] = [86, 72, 84, 51, 83, 77, 67, 83, 52, 0];
static HDATA_STR_45: [u8; 10] = [86, 72, 84, 51, 83, 77, 67, 83, 53, 0];
static HDATA_STR_46: [u8; 10] = [86, 72, 84, 51, 83, 77, 67, 83, 54, 0];
static HDATA_STR_47: [u8; 10] = [86, 72, 84, 51, 83, 77, 67, 83, 55, 0];
static HDATA_STR_48: [u8; 10] = [86, 72, 84, 51, 83, 77, 67, 83, 56, 0];
static HDATA_STR_49: [u8; 10] = [86, 72, 84, 51, 83, 77, 67, 83, 57, 0];
static HDATA_STR_4a: [u8; 10] = [86, 72, 84, 52, 83, 77, 67, 83, 48, 0];
static HDATA_STR_4b: [u8; 10] = [86, 72, 84, 52, 83, 77, 67, 83, 49, 0];
static HDATA_STR_4c: [u8; 10] = [86, 72, 84, 52, 83, 77, 67, 83, 50, 0];
static HDATA_STR_4d: [u8; 10] = [86, 72, 84, 52, 83, 77, 67, 83, 51, 0];
static HDATA_STR_4e: [u8; 10] = [86, 72, 84, 52, 83, 77, 67, 83, 52, 0];
static HDATA_STR_4f: [u8; 10] = [86, 72, 84, 52, 83, 77, 67, 83, 53, 0];
static HDATA_STR_50: [u8; 10] = [86, 72, 84, 52, 83, 77, 67, 83, 54, 0];
static HDATA_STR_51: [u8; 10] = [86, 72, 84, 52, 83, 77, 67, 83, 55, 0];
static HDATA_STR_52: [u8; 10] = [86, 72, 84, 52, 83, 77, 67, 83, 56, 0];
static HDATA_STR_53: [u8; 10] = [86, 72, 84, 52, 83, 77, 67, 83, 57, 0];
static HDATA_STR_54: [u8; 8] = [85, 78, 75, 78, 79, 87, 78, 0];

const fn hdata_cstr_ptr(s: &[u8]) -> *const c_char {
    s.as_ptr() as *const c_char
}

#[repr(transparent)]
pub struct CStrPtr(pub *const c_char);
unsafe impl Sync for CStrPtr {}

#[link_section = ".data"]
#[no_mangle]
pub static _HDATA_RATE: [CStrPtr; 85] = [
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_00)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_01)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_02)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_03)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_04)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_05)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_06)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_07)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_08)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_09)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_0a)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_0b)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_0c)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_0d)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_0e)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_0f)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_10)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_11)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_12)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_13)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_14)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_15)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_16)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_17)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_18)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_19)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_1a)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_1b)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_1c)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_1d)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_1e)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_1f)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_20)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_21)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_22)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_23)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_24)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_25)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_26)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_27)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_28)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_29)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_2a)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_2b)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_2c)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_2d)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_2e)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_2f)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_30)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_31)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_32)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_33)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_34)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_35)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_36)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_37)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_38)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_39)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_3a)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_3b)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_3c)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_3d)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_3e)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_3f)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_40)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_41)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_42)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_43)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_44)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_45)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_46)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_47)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_48)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_49)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_4a)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_4b)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_4c)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_4d)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_4e)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_4f)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_50)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_51)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_52)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_53)),
    CStrPtr(hdata_cstr_ptr(&HDATA_STR_54)),
];

#[no_mangle]
pub extern "C" fn rtw_rust_hal_com_hdata_rate(hw_rate: U8) -> *const c_char {
    let idx = if (hw_rate as usize) >= DESC_RATE_NUM {
        DESC_RATE_NUM
    } else {
        hw_rate as usize
    };
    _HDATA_RATE[idx].0
}
