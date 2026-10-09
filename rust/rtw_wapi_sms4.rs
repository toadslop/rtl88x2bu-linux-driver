// SPDX-License-Identifier: GPL-2.0
//! W3-135 SMS4 block cipher core — Rust port of `core/rtw_wapi_sms4.c` (CONFIG_WAPI_SW_SMS4).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_wapi_sms4_test)]
use std::os::raw::{c_uchar, c_uint, c_void};

#[cfg(not(host_wapi_sms4_test))]
use core::ffi::{c_uchar, c_uint, c_void};

type U8 = c_uchar;
type U32 = c_uint;

const ENCRYPT: U32 = 0;
const DECRYPT: U32 = 1;

const SBOX: [U8; 256] = [
    0xd6, 0x90, 0xe9, 0xfe, 0xcc, 0xe1, 0x3d, 0xb7, 0x16, 0xb6, 0x14, 0xc2, 0x28, 0xfb, 0x2c, 0x05,
    0x2b, 0x67, 0x9a, 0x76, 0x2a, 0xbe, 0x04, 0xc3, 0xaa, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99,
    0x9c, 0x42, 0x50, 0xf4, 0x91, 0xef, 0x98, 0x7a, 0x33, 0x54, 0x0b, 0x43, 0xed, 0xcf, 0xac, 0x62,
    0xe4, 0xb3, 0x1c, 0xa9, 0xc9, 0x08, 0xe8, 0x95, 0x80, 0xdf, 0x94, 0xfa, 0x75, 0x8f, 0x3f, 0xa6,
    0x47, 0x07, 0xa7, 0xfc, 0xf3, 0x73, 0x17, 0xba, 0x83, 0x59, 0x3c, 0x19, 0xe6, 0x85, 0x4f, 0xa8,
    0x68, 0x6b, 0x81, 0xb2, 0x71, 0x64, 0xda, 0x8b, 0xf8, 0xeb, 0x0f, 0x4b, 0x70, 0x56, 0x9d, 0x35,
    0x1e, 0x24, 0x0e, 0x5e, 0x63, 0x58, 0xd1, 0xa2, 0x25, 0x22, 0x7c, 0x3b, 0x01, 0x21, 0x78, 0x87,
    0xd4, 0x00, 0x46, 0x57, 0x9f, 0xd3, 0x27, 0x52, 0x4c, 0x36, 0x02, 0xe7, 0xa0, 0xc4, 0xc8, 0x9e,
    0xea, 0xbf, 0x8a, 0xd2, 0x40, 0xc7, 0x38, 0xb5, 0xa3, 0xf7, 0xf2, 0xce, 0xf9, 0x61, 0x15, 0xa1,
    0xe0, 0xae, 0x5d, 0xa4, 0x9b, 0x34, 0x1a, 0x55, 0xad, 0x93, 0x32, 0x30, 0xf5, 0x8c, 0xb1, 0xe3,
    0x1d, 0xf6, 0xe2, 0x2e, 0x82, 0x66, 0xca, 0x60, 0xc0, 0x29, 0x23, 0xab, 0x0d, 0x53, 0x4e, 0x6f,
    0xd5, 0xdb, 0x37, 0x45, 0xde, 0xfd, 0x8e, 0x2f, 0x03, 0xff, 0x6a, 0x72, 0x6d, 0x6c, 0x5b, 0x51,
    0x8d, 0x1b, 0xaf, 0x92, 0xbb, 0xdd, 0xbc, 0x7f, 0x11, 0xd9, 0x5c, 0x41, 0x1f, 0x10, 0x5a, 0xd8,
    0x0a, 0xc1, 0x31, 0x88, 0xa5, 0xcd, 0x7b, 0xbd, 0x2d, 0x74, 0xd0, 0x12, 0xb8, 0xe5, 0xb4, 0xb0,
    0x89, 0x69, 0x97, 0x4a, 0x0c, 0x96, 0x77, 0x7e, 0x65, 0xb9, 0xf1, 0x09, 0xc5, 0x6e, 0xc6, 0x84,
    0x18, 0xf0, 0x7d, 0xec, 0x3a, 0xdc, 0x4d, 0x20, 0x79, 0xee, 0x5f, 0x3e, 0xd7, 0xcb, 0x39, 0x48,
];

const CK: [U32; 32] = [
    0x00070e15, 0x1c232a31, 0x383f464d, 0x545b6269, 0x70777e85, 0x8c939aa1, 0xa8afb6bd, 0xc4cbd2d9,
    0xe0e7eef5, 0xfc030a11, 0x181f262d, 0x343b4249, 0x50575e65, 0x6c737a81, 0x888f969d, 0xa4abb2b9,
    0xc0c7ced5, 0xdce3eaf1, 0xf8ff060d, 0x141b2229, 0x30373e45, 0x4c535a61, 0x686f767d, 0x848b9299,
    0xa0a7aeb5, 0xbcc3cad1, 0xd8dfe6ed, 0xf4fb0209, 0x10171e25, 0x2c333a41, 0x484f565d, 0x646b7279,
];

#[inline]
fn rotl(x: U32, y: u32) -> U32 {
    (x << y) | (x >> (32 - y))
}

#[inline]
fn byte_sub(a: U32) -> U32 {
    (u32::from(SBOX[(a >> 24) as usize & 0xff]) << 24)
        | (u32::from(SBOX[(a >> 16) as usize & 0xff]) << 16)
        | (u32::from(SBOX[(a >> 8) as usize & 0xff]) << 8)
        | u32::from(SBOX[(a & 0xff) as usize])
}

#[inline]
fn l1(b: U32) -> U32 {
    b ^ rotl(b, 2) ^ rotl(b, 10) ^ rotl(b, 18) ^ rotl(b, 24)
}

#[inline]
fn l2(b: U32) -> U32 {
    b ^ rotl(b, 13) ^ rotl(b, 23)
}

#[inline]
fn le_swap32(x: U32) -> U32 {
    let x = rotl(x, 16);
    ((x & 0x00ff_00ff) << 8) | ((x & 0xff00_ff00) >> 8)
}

#[inline]
fn le_swap32_key(x: U32) -> U32 {
    let x = rotl(x, 16);
    ((x & 0x00ff_00ff) << 8) | ((x & 0xff00_ff00) >> 8)
}

/// 128-bit xor: *dst = *src1 xor *src2 (WAPI_LITTLE_ENDIAN driver layout).
#[no_mangle]
pub extern "C" fn xor_block(dst: *mut c_void, src1: *const c_void, src2: *const c_void) {
    if dst.is_null() || src1.is_null() || src2.is_null() {
        return;
    }
    let dst = dst as *mut U32;
    let src1 = src1 as *const U32;
    let src2 = src2 as *const U32;
    unsafe {
        dst.write(src1.read() ^ src2.read());
        dst.add(1).write(src1.add(1).read() ^ src2.add(1).read());
        dst.add(2).write(src1.add(2).read() ^ src2.add(2).read());
        dst.add(3).write(src1.add(3).read() ^ src2.add(3).read());
    }
}

#[no_mangle]
pub extern "C" fn SMS4Crypt(input: *mut U8, output: *mut U8, rk: *mut U32) {
    if input.is_null() || output.is_null() || rk.is_null() {
        return;
    }
    let p = input as *const U32;
    let mut x0 = unsafe { p.read() };
    let mut x1 = unsafe { p.add(1).read() };
    let mut x2 = unsafe { p.add(2).read() };
    let mut x3 = unsafe { p.add(3).read() };

    x0 = le_swap32(x0);
    x1 = le_swap32(x1);
    x2 = le_swap32(x2);
    x3 = le_swap32(x3);

    for r in (0..32).step_by(4) {
        let mut mid = x1 ^ x2 ^ x3 ^ unsafe { *rk.add(r) };
        mid = byte_sub(mid);
        x0 ^= l1(mid);
        mid = x2 ^ x3 ^ x0 ^ unsafe { *rk.add(r + 1) };
        mid = byte_sub(mid);
        x1 ^= l1(mid);
        mid = x3 ^ x0 ^ x1 ^ unsafe { *rk.add(r + 2) };
        mid = byte_sub(mid);
        x2 ^= l1(mid);
        mid = x0 ^ x1 ^ x2 ^ unsafe { *rk.add(r + 3) };
        mid = byte_sub(mid);
        x3 ^= l1(mid);
    }

    x0 = le_swap32(x0);
    x1 = le_swap32(x1);
    x2 = le_swap32(x2);
    x3 = le_swap32(x3);

    let out = output as *mut U32;
    unsafe {
        out.write(x3);
        out.add(1).write(x2);
        out.add(2).write(x1);
        out.add(3).write(x0);
    }
}

#[no_mangle]
pub extern "C" fn SMS4KeyExt(key: *mut U8, rk: *mut U32, crypt_flag: U32) {
    if key.is_null() || rk.is_null() {
        return;
    }
    let p = key as *const U32;
    let mut x0 = unsafe { p.read() };
    let mut x1 = unsafe { p.add(1).read() };
    let mut x2 = unsafe { p.add(2).read() };
    let mut x3 = unsafe { p.add(3).read() };

    x0 = le_swap32_key(x0);
    x1 = le_swap32_key(x1);
    x2 = le_swap32_key(x2);
    x3 = le_swap32_key(x3);

    x0 ^= 0xa3b1_bac6;
    x1 ^= 0x56aa_3350;
    x2 ^= 0x677d_9197;
    x3 ^= 0xb270_22dc;

    for r in (0..32).step_by(4) {
        let mut mid = x1 ^ x2 ^ x3 ^ CK[r];
        mid = byte_sub(mid);
        x0 ^= l2(mid);
        unsafe { *rk.add(r) = x0 };
        mid = x2 ^ x3 ^ x0 ^ CK[r + 1];
        mid = byte_sub(mid);
        x1 ^= l2(mid);
        unsafe { *rk.add(r + 1) = x1 };
        mid = x3 ^ x0 ^ x1 ^ CK[r + 2];
        mid = byte_sub(mid);
        x2 ^= l2(mid);
        unsafe { *rk.add(r + 2) = x2 };
        mid = x0 ^ x1 ^ x2 ^ CK[r + 3];
        mid = byte_sub(mid);
        x3 ^= l2(mid);
        unsafe { *rk.add(r + 3) = x3 };
    }

    if crypt_flag == DECRYPT {
        for r in 0..16 {
            let mid = unsafe { *rk.add(r) };
            let other = unsafe { *rk.add(31 - r) };
            unsafe {
                *rk.add(r) = other;
                *rk.add(31 - r) = mid;
            }
        }
    }
}

#[cfg(host_wapi_sms4_test)]
#[no_mangle]
pub extern "C" fn host_sms4_xor_block(dst: *mut c_void, src1: *const c_void, src2: *const c_void) {
    xor_block(dst, src1, src2);
}

#[cfg(host_wapi_sms4_test)]
#[no_mangle]
pub extern "C" fn host_sms4_crypt(input: *const U8, output: *mut U8, rk: *mut U32) {
    SMS4Crypt(input as *mut U8, output, rk);
}

#[cfg(host_wapi_sms4_test)]
#[no_mangle]
pub extern "C" fn host_sms4_key_ext(key: *const U8, rk: *mut U32, crypt_flag: U32) {
    SMS4KeyExt(key as *mut U8, rk, crypt_flag);
}
