// SPDX-License-Identifier: GPL-2.0
//! MP PMAC signal generators — Rust port (W3-113 PR6a: ByteToBit + CRC8/16).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unused_unsafe
)]
#![cfg(host_rtw_mp_test)]

use std::os::raw::c_uchar;

type u8 = c_uchar;

fn zero_bytes(p: *mut u8, n: usize) {
    unsafe {
        for i in 0..n {
            *p.add(i) = 0;
        }
    }
}

fn byte_to_bit_inner(out: *mut u8, in_bits: &[bool], in_size: u8) {
    zero_bytes(out, in_size as usize);
    for i in 0..in_size {
        for j in 0..8 {
            if in_bits[(8 * i + j) as usize] {
                unsafe {
                    *out.add(i as usize) |= 1 << j;
                }
            }
        }
    }
}

fn crc16_generator_inner(out: &mut [bool; 16], input: &[bool], in_size: u8) {
    let mut reg = [true; 16];
    for i in 0..in_size as usize {
        let temp = input[i] ^ reg[15];
        reg[15] = reg[14];
        reg[14] = reg[13];
        reg[13] = reg[12];
        reg[12] = reg[11];
        reg[11] = reg[10];
        reg[10] = reg[9];
        reg[9] = reg[8];
        reg[8] = reg[7];
        reg[7] = reg[6];
        reg[6] = reg[5];
        reg[5] = reg[4];
        reg[4] = reg[3];
        reg[3] = reg[2];
        reg[2] = reg[1];
        reg[1] = reg[0];
        reg[12] ^= temp;
        reg[5] ^= temp;
        reg[0] = temp;
    }
    for i in 0..16 {
        out[i] = !reg[15 - i];
    }
}

fn crc8_generator_inner(out: &mut [bool; 8], input: &[bool], in_size: u8) {
    let mut reg = [true; 8];
    for i in 0..in_size as usize {
        let temp = input[i] ^ reg[7];
        reg[7] = reg[6];
        reg[6] = reg[5];
        reg[5] = reg[4];
        reg[4] = reg[3];
        reg[3] = reg[2];
        reg[2] = reg[1] ^ temp;
        reg[1] = reg[0] ^ temp;
        reg[0] = temp;
    }
    for i in 0..8 {
        out[i] = reg[7 - i] ^ true;
    }
}

fn read_bool_slice(ptr: *const u8, len: usize) -> Vec<bool> {
    unsafe { std::slice::from_raw_parts(ptr as *const bool, len) }
        .iter()
        .map(|&b| b)
        .collect()
}

fn write_bool_slice(ptr: *mut u8, bits: &[bool]) {
    for (i, b) in bits.iter().enumerate() {
        unsafe {
            *((ptr as *mut bool).add(i)) = *b;
        }
    }
}

#[no_mangle]
pub extern "C" fn ByteToBit(out: *mut u8, in_bits: *mut u8, in_size: u8) {
    if out.is_null() || in_bits.is_null() {
        return;
    }
    let n = (in_size as usize) * 8;
    byte_to_bit_inner(out, &read_bool_slice(in_bits, n), in_size);
}

#[no_mangle]
pub extern "C" fn CRC16_generator(out: *mut u8, in_bits: *mut u8, in_size: u8) {
    if out.is_null() || in_bits.is_null() {
        return;
    }
    let in_vec = read_bool_slice(in_bits, in_size as usize);
    let mut bits = [false; 16];
    crc16_generator_inner(&mut bits, &in_vec, in_size);
    write_bool_slice(out, &bits);
}

#[no_mangle]
pub extern "C" fn CRC8_generator(out: *mut u8, in_bits: *mut u8, in_size: u8) {
    if out.is_null() || in_bits.is_null() {
        return;
    }
    let in_vec = read_bool_slice(in_bits, in_size as usize);
    let mut bits = [false; 8];
    crc8_generator_inner(&mut bits, &in_vec, in_size);
    write_bool_slice(out, &bits);
}
