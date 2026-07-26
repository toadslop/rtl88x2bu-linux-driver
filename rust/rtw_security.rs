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

// ----- WEP primitives (W3-05) -----

type Sint = i32;

const CRC32_POLY: U32 = 0x04c11db7;

#[repr(C)]
pub struct arc4context {
    pub x: U32,
    pub y: U32,
    pub state: [U8; 256],
}

static mut BCRC32INITIALIZED: Sint = 0;
static mut CRC32_TABLE: [U32; 256] = [0; 256];

fn crc32_reverse_bit(data: U8) -> U8 {
    ((data << 7) & 0x80)
        | ((data << 5) & 0x40)
        | ((data << 3) & 0x20)
        | ((data << 1) & 0x10)
        | ((data >> 1) & 0x08)
        | ((data >> 3) & 0x04)
        | ((data >> 5) & 0x02)
        | ((data >> 7) & 0x01)
}

fn crc32_init_inner() {
    unsafe {
        if BCRC32INITIALIZED == 1 {
            return;
        }
        for i in 0..256usize {
            let k = crc32_reverse_bit(i as U8);
            let mut c = (k as U32) << 24;
            let mut j = 8;
            while j > 0 {
                c = if c & 0x8000_0000 != 0 {
                    (c << 1) ^ CRC32_POLY
                } else {
                    c << 1
                };
                j -= 1;
            }
            let p = c.to_ne_bytes();
            CRC32_TABLE[i] = u32::from_ne_bytes([
                crc32_reverse_bit(p[3]),
                crc32_reverse_bit(p[2]),
                crc32_reverse_bit(p[1]),
                crc32_reverse_bit(p[0]),
            ]);
        }
        BCRC32INITIALIZED = 1;
    }
}

#[no_mangle]
pub extern "C" fn arcfour_init(parc4ctx: *mut arc4context, key: *mut U8, key_len: U32) {
    if parc4ctx.is_null() || key.is_null() {
        return;
    }
    unsafe {
        let ctx = &mut *parc4ctx;
        // Mirror C wrap semantics: key_len == 0 still reads key[0] each round.
        let key_len = core::cmp::max(key_len, 1);
        let key = core::slice::from_raw_parts(key, key_len as usize);
        let state = &mut ctx.state;
        ctx.x = 0;
        ctx.y = 0;
        for (counter, slot) in state.iter_mut().enumerate() {
            *slot = counter as U8;
        }
        let mut keyindex = 0usize;
        let mut stateindex = 0u32;
        for counter in 0..256usize {
            let t = state[counter] as u32;
            stateindex = (stateindex + key[keyindex] as u32 + t) & 0xff;
            let u = state[stateindex as usize];
            state[stateindex as usize] = t as U8;
            state[counter] = u;
            keyindex += 1;
            if keyindex >= key_len as usize {
                keyindex = 0;
            }
        }
    }
}

fn arcfour_byte(ctx: &mut arc4context) -> U32 {
    let state = &mut ctx.state;
    let x = (ctx.x + 1) & 0xff;
    let sx = state[x as usize] as u32;
    let y = (sx + ctx.y) & 0xff;
    let sy = state[y as usize] as u32;
    ctx.x = x;
    ctx.y = y;
    state[y as usize] = sx as U8;
    state[x as usize] = sy as U8;
    state[((sx + sy) & 0xff) as usize] as U32
}

#[no_mangle]
pub extern "C" fn arcfour_encrypt(
    parc4ctx: *mut arc4context,
    dest: *mut U8,
    src: *mut U8,
    len: U32,
) {
    if parc4ctx.is_null() || dest.is_null() || src.is_null() || len == 0 {
        return;
    }
    unsafe {
        let ctx = &mut *parc4ctx;
        let dest = core::slice::from_raw_parts_mut(dest, len as usize);
        let src = core::slice::from_raw_parts(src, len as usize);
        for i in 0..len as usize {
            dest[i] = src[i] ^ arcfour_byte(ctx) as U8;
        }
    }
}

#[no_mangle]
pub extern "C" fn getcrc32(buf: *mut U8, len: Sint) -> U32 {
    if buf.is_null() || len <= 0 {
        return 0;
    }
    crc32_init_inner();
    unsafe {
        let buf = core::slice::from_raw_parts(buf, len as usize);
        let mut crc = 0xffff_ffffu32;
        for &byte in buf {
            crc = CRC32_TABLE[((crc ^ u32::from(byte)) & 0xff) as usize] ^ (crc >> 8);
        }
        !crc
    }
}

#[cfg(host_security_test)]
#[no_mangle]
pub extern "C" fn host_wep_arcfour_crypt(
    key: *const U8,
    key_len: U32,
    src: *const U8,
    dest: *mut U8,
    len: U32,
) {
    if key.is_null() || src.is_null() || dest.is_null() {
        return;
    }
    let mut ctx = arc4context {
        x: 0,
        y: 0,
        state: [0; 256],
    };
    arcfour_init(
        &mut ctx,
        key as *mut U8,
        key_len,
    );
    arcfour_encrypt(&mut ctx, dest, src as *mut U8, len);
}

#[cfg(host_security_test)]
#[no_mangle]
pub extern "C" fn host_wep_getcrc32(buf: *mut U8, len: Sint) -> U32 {
    getcrc32(buf, len)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn probe_constant() {
        assert_eq!(rtw_rust_security_probe(), 0x1e04);
    }
}
