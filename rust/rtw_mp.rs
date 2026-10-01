// SPDX-License-Identifier: GPL-2.0
//! MP PMAC signal generators — Rust port (W3-113 PR6b adds PMAC_Get_Pkt_Param).

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

use std::os::raw::{c_uchar, c_void};

type u8 = c_uchar;

const MPT_RATE_1M: u8 = 1;
const MPT_RATE_6M: u8 = 5;
const MPT_RATE_MCS0: u8 = 13;
const MPT_RATE_MCS8: u8 = 21;
const MPT_RATE_MCS16: u8 = 29;
const MPT_RATE_MCS24: u8 = 37;
const MPT_RATE_MCS31: u8 = 44;
const MPT_RATE_VHT1SS_MCS0: u8 = 100;
const MPT_RATE_VHT2SS_MCS0: u8 = 110;
const MPT_RATE_VHT3SS_MCS0: u8 = 120;
const MPT_RATE_VHT4SS_MCS0: u8 = 130;
const MPT_RATE_VHT4SS_MCS9: u8 = 139;

fn mpt_is_cck_rate(v: u8) -> bool {
    (MPT_RATE_1M..=4).contains(&v)
}
fn mpt_is_ofdm_rate(v: u8) -> bool {
    (MPT_RATE_6M..=12).contains(&v)
}
fn mpt_is_ht_rate(v: u8) -> bool {
    (MPT_RATE_MCS0..=MPT_RATE_MCS31).contains(&v)
}
fn mpt_is_vht_rate(v: u8) -> bool {
    (MPT_RATE_VHT1SS_MCS0..=MPT_RATE_VHT4SS_MCS9).contains(&v)
}
fn mpt_is_vht_2s_rate(v: u8) -> bool {
    (MPT_RATE_VHT2SS_MCS0..=119).contains(&v)
}
fn mpt_is_vht_3s_rate(v: u8) -> bool {
    (MPT_RATE_VHT3SS_MCS0..=129).contains(&v)
}
fn mpt_is_vht_4s_rate(v: u8) -> bool {
    (MPT_RATE_VHT4SS_MCS0..=MPT_RATE_VHT4SS_MCS9).contains(&v)
}
fn mpt_is_2ss_rate(r: u8) -> bool {
    (MPT_RATE_MCS8..=28).contains(&r) || mpt_is_vht_2s_rate(r)
}
fn mpt_is_3ss_rate(r: u8) -> bool {
    (MPT_RATE_MCS16..=36).contains(&r) || mpt_is_vht_3s_rate(r)
}
fn mpt_is_4ss_rate(r: u8) -> bool {
    (MPT_RATE_MCS24..=MPT_RATE_MCS31).contains(&r) || mpt_is_vht_4s_rate(r)
}

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

#[no_mangle]
pub extern "C" fn PMAC_Get_Pkt_Param(p_tx: *mut c_void, p_pkt: *mut c_void) {
    if p_tx.is_null() || p_pkt.is_null() {
        return;
    }
    let tx = p_tx as *mut u8;
    let pkt = p_pkt as *mut u8;
    let tx_rate = unsafe { *tx.add(1) };
    let mut tx_rate_hex = 0u8;
    let mut mcs = 0u8;
    let nss = if mpt_is_2ss_rate(tx_rate) {
        2
    } else if mpt_is_3ss_rate(tx_rate) {
        3
    } else if mpt_is_4ss_rate(tx_rate) {
        4
    } else {
        1
    };
    unsafe {
        *pkt.add(1) = nss;
    }

    if mpt_is_cck_rate(tx_rate) {
        match tx_rate {
            MPT_RATE_1M => {
                tx_rate_hex = 0;
                mcs = 0;
            }
            2 => {
                tx_rate_hex = 1;
                mcs = 1;
            }
            3 => {
                tx_rate_hex = 2;
                mcs = 2;
            }
            4 => {
                tx_rate_hex = 3;
                mcs = 3;
            }
            _ => {}
        }
    } else if mpt_is_ofdm_rate(tx_rate) {
        mcs = tx_rate - MPT_RATE_6M;
        tx_rate_hex = mcs + 4;
    } else if mpt_is_ht_rate(tx_rate) {
        mcs = tx_rate - MPT_RATE_MCS0;
        tx_rate_hex = mcs + 12;
    } else if mpt_is_vht_rate(tx_rate) {
        tx_rate_hex = tx_rate - MPT_RATE_VHT1SS_MCS0 + 44;
        mcs = if mpt_is_vht_2s_rate(tx_rate) {
            tx_rate - MPT_RATE_VHT2SS_MCS0
        } else if mpt_is_vht_3s_rate(tx_rate) {
            tx_rate - MPT_RATE_VHT3SS_MCS0
        } else if mpt_is_vht_4s_rate(tx_rate) {
            tx_rate - MPT_RATE_VHT4SS_MCS0
        } else {
            tx_rate - MPT_RATE_VHT1SS_MCS0
        };
    }

    unsafe {
        *pkt = mcs;
        *tx.add(2) = tx_rate_hex;
        *pkt.add(2) = nss;
    }
    let b_stbc = unsafe { (*tx.add(4) & 4) != 0 };
    if b_stbc {
        if nss == 1 {
            unsafe {
                *tx.add(5) = 2;
                *pkt.add(2) = nss * 2;
            }
        } else {
            unsafe { *tx.add(5) = 1 };
        }
    } else {
        unsafe { *tx.add(5) = 1 };
    }
}
