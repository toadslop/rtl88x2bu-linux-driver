// SPDX-License-Identifier: GPL-2.0
//! Security helpers — Rust port of `core/rtw_security.c` slices (W3-04+).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[path = "domain/types.rs"]
mod domain_types;

use domain_types::{BipGmcs, SecurityType};

#[cfg(host_security_test)]
use std::os::raw::{c_char, c_int};

#[cfg(not(host_security_test))]
use core::ffi::{c_char, c_int};

type U8 = u8;
type U32 = u32;

const _SEC_TYPE_BIT_: U8 = 0x20;
const _BIP_CMAC_128_: U8 = 0x20;
const _BIP_MAX_: U8 = 0x24;

static SECURITY_TYPE_STR: [&[u8]; 8] = [
    b"N/A\0",
    b"WEP40\0",
    b"TKIP\0",
    b"TKIP_WM\0",
    b"AES\0",
    b"WEP104\0",
    b"SMS4\0",
    b"GCMP\0",
];

static SECURITY_TYPE_BIP_STR: [&[u8]; 4] = [
    b"BIP_CMAC_128\0",
    b"BIP_GMAC_128\0",
    b"BIP_GMAC_256\0",
    b"BIP_CMAC_256\0",
];

static CCMP_256_STR: &[u8] = b"CCMP_256\0";
static GCMP_256_STR: &[u8] = b"GCMP_256\0";

#[no_mangle]
pub extern "C" fn rtw_rust_security_probe() -> c_int {
    0x1e04
}

fn security_type_str_inner(value: U8) -> Option<&'static [u8]> {
    if (_BIP_MAX_ > value) && (value >= _BIP_CMAC_128_) {
        let idx = (value & !_SEC_TYPE_BIT_) as usize;
        return SECURITY_TYPE_BIP_STR.get(idx).copied();
    }

    if value == SecurityType::Ccmp256.to_raw() {
        return Some(CCMP_256_STR);
    }
    if value == SecurityType::Gcmp256.to_raw() {
        return Some(GCMP_256_STR);
    }

    if value < SecurityType::SEC_TYPE_MAX {
        return SECURITY_TYPE_STR.get(value as usize).copied();
    }

    None
}

#[no_mangle]
pub extern "C" fn security_type_str(value: U8) -> *const c_char {
    match security_type_str_inner(value) {
        Some(s) => s.as_ptr() as *const c_char,
        None => core::ptr::null(),
    }
}

#[no_mangle]
pub extern "C" fn security_type_bip_to_gmcs(type_: U8) -> U32 {
    match SecurityType::try_from(type_) {
        Ok(sec) => match BipGmcs::try_from_security_type(sec) {
            Ok(gmcs) => gmcs.to_raw(),
            Err(_) => 0,
        },
        Err(_) => 0,
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn probe_constant() {
        assert_eq!(rtw_rust_security_probe(), 0x1e04);
    }
}
