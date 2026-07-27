// SPDX-License-Identifier: GPL-2.0
//! Security-rest helpers — Rust port of `core/rtw_security_rest.c` slices (W3-11+).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_security_rest_test)]
use std::os::raw::c_int;

#[cfg(not(host_security_rest_test))]
use core::ffi::c_int;

type U8 = u8;
type Sint = i32;
type U32 = u32;

const WIFI_MGT_TYPE: U32 = 0;
#[allow(dead_code)]
const WIFI_DATA_TYPE: U32 = 1 << 3;
const BIT4: U8 = 1 << 4;

const SBOX_TABLE: [U8; 256] = [
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16,
];

#[no_mangle]
pub extern "C" fn rtw_rust_security_rest_probe() -> c_int {
    0x1e11
}

fn xor_128(a: &[U8; 16], b: &[U8; 16], out: &mut [U8; 16]) {
    for i in 0..16 {
        out[i] = a[i] ^ b[i];
    }
}

fn xor_32(a: &[U8; 4], b: &[U8; 4], out: &mut [U8; 4]) {
    for i in 0..4 {
        out[i] = a[i] ^ b[i];
    }
}

fn sbox(a: U8) -> U8 {
    SBOX_TABLE[a as usize]
}

fn next_key(key: &mut [U8; 16], round: Sint) {
    let rcon_table: [U8; 12] = [
        0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x36, 0x36,
    ];
    let sbox_key = [
        sbox(key[13]),
        sbox(key[14]),
        sbox(key[15]),
        sbox(key[12]),
    ];
    let rcon = rcon_table[round as usize];

    let mut tmp = [0u8; 4];
    xor_32(&[key[0], key[1], key[2], key[3]], &sbox_key, &mut tmp);
    key[0] = tmp[0] ^ rcon;
    key[1] = tmp[1];
    key[2] = tmp[2];
    key[3] = tmp[3];

    xor_32(&[key[4], key[5], key[6], key[7]], &[key[0], key[1], key[2], key[3]], &mut tmp);
    key[4..8].copy_from_slice(&tmp);
    xor_32(&[key[8], key[9], key[10], key[11]], &[key[4], key[5], key[6], key[7]], &mut tmp);
    key[8..12].copy_from_slice(&tmp);
    xor_32(
        &[key[12], key[13], key[14], key[15]],
        &[key[8], key[9], key[10], key[11]],
        &mut tmp,
    );
    key[12..16].copy_from_slice(&tmp);
}

fn byte_sub(in_: &[U8; 16], out: &mut [U8; 16]) {
    for i in 0..16 {
        out[i] = sbox(in_[i]);
    }
}

fn shift_row(in_: &[U8; 16], out: &mut [U8; 16]) {
    out[0] = in_[0];
    out[1] = in_[5];
    out[2] = in_[10];
    out[3] = in_[15];
    out[4] = in_[4];
    out[5] = in_[9];
    out[6] = in_[14];
    out[7] = in_[3];
    out[8] = in_[8];
    out[9] = in_[13];
    out[10] = in_[2];
    out[11] = in_[7];
    out[12] = in_[12];
    out[13] = in_[1];
    out[14] = in_[6];
    out[15] = in_[11];
}

fn mix_column(in_: &[U8; 4], out: &mut [U8; 4]) {
    let mut add1b = [0u8; 4];
    let mut add1bf7 = [0u8; 4];
    let mut rotl = [0u8; 4];
    let mut swap_halfs = [0u8; 4];
    let mut andf7 = [0u8; 4];
    let mut rotr = [0u8; 4];
    let mut temp = [0u8; 4];
    let mut tempb = [0u8; 4];

    for i in 0..4 {
        add1b[i] = if (in_[i] & 0x80) == 0x80 { 0x1b } else { 0x00 };
    }

    swap_halfs[0] = in_[2];
    swap_halfs[1] = in_[3];
    swap_halfs[2] = in_[0];
    swap_halfs[3] = in_[1];

    rotl[0] = in_[3];
    rotl[1] = in_[0];
    rotl[2] = in_[1];
    rotl[3] = in_[2];

    andf7[0] = in_[0] & 0x7f;
    andf7[1] = in_[1] & 0x7f;
    andf7[2] = in_[2] & 0x7f;
    andf7[3] = in_[3] & 0x7f;

    for i in (1..4).rev() {
        andf7[i] <<= 1;
        if (andf7[i - 1] & 0x80) == 0x80 {
            andf7[i] |= 0x01;
        }
    }
    andf7[0] <<= 1;
    andf7[0] &= 0xfe;

    xor_32(&add1b, &andf7, &mut add1bf7);
    xor_32(in_, &add1bf7, &mut rotr);

    temp[0] = rotr[0];
    rotr[0] = rotr[1];
    rotr[1] = rotr[2];
    rotr[2] = rotr[3];
    rotr[3] = temp[0];

    xor_32(&add1bf7, &rotr, &mut temp);
    xor_32(&swap_halfs, &rotl, &mut tempb);
    xor_32(&temp, &tempb, out);
}

fn aes128k128d(key: &[U8; 16], data: &[U8; 16], ciphertext: &mut [U8; 16]) {
    let mut round_key = *key;
    let mut intermediatea = [0u8; 16];
    let mut intermediateb = [0u8; 16];

    for round in 0..11 {
        if round == 0 {
            xor_128(&round_key, data, ciphertext);
            next_key(&mut round_key, round);
        } else if round == 10 {
            byte_sub(ciphertext, &mut intermediatea);
            shift_row(&intermediatea, &mut intermediateb);
            xor_128(&intermediateb, &round_key, ciphertext);
        } else {
            byte_sub(ciphertext, &mut intermediatea);
            shift_row(&intermediatea, &mut intermediateb);
            for col in 0..4 {
                let mut out_col = [0u8; 4];
                let base = col * 4;
                let in_col = [
                    intermediateb[base],
                    intermediateb[base + 1],
                    intermediateb[base + 2],
                    intermediateb[base + 3],
                ];
                mix_column(&in_col, &mut out_col);
                intermediatea[base..base + 4].copy_from_slice(&out_col);
            }
            xor_128(&intermediatea, &round_key, ciphertext);
            next_key(&mut round_key, round);
        }
    }
}

fn bitwise_xor(ina: &[U8; 16], inb: &[U8; 16], out: &mut [U8; 16]) {
    xor_128(ina, inb, out);
}

// ----- CCMP MIC/CTR construct helpers (W3-11) -----

fn construct_mic_iv(
    mic_iv: &mut [U8; 16],
    qc_exists: Sint,
    a4_exists: Sint,
    mpdu: &[U8],
    payload_length: U32,
    pn_vector: &[U8; 6],
    frtype: U32,
) {
    mic_iv[0] = 0x59;
    if qc_exists != 0 && a4_exists != 0 {
        mic_iv[1] = mpdu[30] & 0x0f;
    }
    if qc_exists != 0 && a4_exists == 0 {
        mic_iv[1] = mpdu[24] & 0x0f;
    }
    if qc_exists == 0 {
        mic_iv[1] = 0x00;
    }
    if frtype == WIFI_MGT_TYPE {
        mic_iv[1] |= BIT4;
    }
    for i in 2..8 {
        mic_iv[i] = mpdu[i + 8];
    }
    for i in 8..14 {
        mic_iv[i] = pn_vector[(13 - i) as usize];
    }
    mic_iv[14] = (payload_length / 256) as U8;
    mic_iv[15] = (payload_length % 256) as U8;
}

fn construct_mic_header1(
    mic_header1: &mut [U8; 16],
    header_length: Sint,
    mpdu: &[U8],
    frtype: U32,
) {
    mic_header1[0] = ((header_length - 2) / 256) as U8;
    mic_header1[1] = ((header_length - 2) % 256) as U8;
    if frtype == WIFI_MGT_TYPE {
        mic_header1[2] = mpdu[0];
    } else {
        mic_header1[2] = mpdu[0] & 0xcf;
    }
    mic_header1[3] = mpdu[1] & 0xc7;
    for i in 4..16 {
        mic_header1[i] = mpdu[i];
    }
}

fn construct_mic_header2(
    mic_header2: &mut [U8; 16],
    mpdu: &[U8],
    a4_exists: Sint,
    qc_exists: Sint,
) {
    mic_header2.fill(0);
    mic_header2[0] = mpdu[16];
    mic_header2[1] = mpdu[17];
    mic_header2[2] = mpdu[18];
    mic_header2[3] = mpdu[19];
    mic_header2[4] = mpdu[20];
    mic_header2[5] = mpdu[21];

    if qc_exists == 0 && a4_exists != 0 {
        for i in 0..6 {
            mic_header2[8 + i] = mpdu[24 + i];
        }
    }
    if qc_exists != 0 && a4_exists == 0 {
        mic_header2[8] = mpdu[24] & 0x0f;
        mic_header2[9] = 0x00;
    }
    if qc_exists != 0 && a4_exists != 0 {
        for i in 0..6 {
            mic_header2[8 + i] = mpdu[24 + i];
        }
        mic_header2[14] = mpdu[30] & 0x0f;
        mic_header2[15] = 0x00;
    }
}

fn construct_ctr_preload(
    ctr_preload: &mut [U8; 16],
    a4_exists: Sint,
    qc_exists: Sint,
    mpdu: &[U8],
    pn_vector: &[U8; 6],
    c: Sint,
    frtype: U32,
) {
    ctr_preload.fill(0);
    ctr_preload[0] = 0x01;
    if qc_exists != 0 && a4_exists != 0 {
        ctr_preload[1] = mpdu[30] & 0x0f;
    }
    if qc_exists != 0 && a4_exists == 0 {
        ctr_preload[1] = mpdu[24] & 0x0f;
    }
    if frtype == WIFI_MGT_TYPE {
        ctr_preload[1] |= BIT4;
    }
    for i in 2..8 {
        ctr_preload[i] = mpdu[i + 8];
    }
    for i in 8..14 {
        ctr_preload[i] = pn_vector[(13 - i) as usize];
    }
    ctr_preload[14] = ((c / 256) & 0xff) as U8;
    ctr_preload[15] = ((c % 256) & 0xff) as U8;
}

// ----- Host L2 exports (W3-11) -----

#[cfg(host_security_rest_test)]
#[no_mangle]
pub extern "C" fn host_ccmp_aes128k128d(key: *mut U8, data: *mut U8, ciphertext: *mut U8) {
    let key_arr: [U8; 16] = unsafe { core::ptr::read_unaligned(key as *const [U8; 16]) };
    let data_arr: [U8; 16] = unsafe { core::ptr::read_unaligned(data as *const [U8; 16]) };
    let mut out = [0u8; 16];
    aes128k128d(&key_arr, &data_arr, &mut out);
    unsafe {
        core::ptr::write_unaligned(ciphertext as *mut [U8; 16], out);
    }
}

#[cfg(host_security_rest_test)]
#[no_mangle]
pub extern "C" fn host_ccmp_xor_128(a: *mut U8, b: *mut U8, out: *mut U8) {
    let a_arr: [U8; 16] = unsafe { core::ptr::read_unaligned(a as *const [U8; 16]) };
    let b_arr: [U8; 16] = unsafe { core::ptr::read_unaligned(b as *const [U8; 16]) };
    let mut result = [0u8; 16];
    xor_128(&a_arr, &b_arr, &mut result);
    unsafe {
        core::ptr::write_unaligned(out as *mut [U8; 16], result);
    }
}

#[cfg(host_security_rest_test)]
#[no_mangle]
pub extern "C" fn host_ccmp_xor_32(a: *mut U8, b: *mut U8, out: *mut U8) {
    let a_arr: [U8; 4] = unsafe { core::ptr::read_unaligned(a as *const [U8; 4]) };
    let b_arr: [U8; 4] = unsafe { core::ptr::read_unaligned(b as *const [U8; 4]) };
    let mut result = [0u8; 4];
    xor_32(&a_arr, &b_arr, &mut result);
    unsafe {
        core::ptr::write_unaligned(out as *mut [U8; 4], result);
    }
}

#[cfg(host_security_rest_test)]
#[no_mangle]
pub extern "C" fn host_ccmp_bitwise_xor(ina: *mut U8, inb: *mut U8, out: *mut U8) {
    let ina_arr: [U8; 16] = unsafe { core::ptr::read_unaligned(ina as *const [U8; 16]) };
    let inb_arr: [U8; 16] = unsafe { core::ptr::read_unaligned(inb as *const [U8; 16]) };
    let mut result = [0u8; 16];
    bitwise_xor(&ina_arr, &inb_arr, &mut result);
    unsafe {
        core::ptr::write_unaligned(out as *mut [U8; 16], result);
    }
}

#[cfg(host_security_rest_test)]
#[no_mangle]
pub extern "C" fn host_ccmp_construct_mic_iv(
    mic_iv: *mut U8,
    qc_exists: c_int,
    a4_exists: c_int,
    mpdu: *mut U8,
    payload_length: U32,
    pn_vector: *mut U8,
    frtype: U32,
) {
    let mpdu_slice = unsafe { core::slice::from_raw_parts(mpdu, 32) };
    let pn: [U8; 6] = unsafe { core::ptr::read_unaligned(pn_vector as *const [U8; 6]) };
    let mut out = [0u8; 16];
    construct_mic_iv(
        &mut out,
        qc_exists as Sint,
        a4_exists as Sint,
        mpdu_slice,
        payload_length,
        &pn,
        frtype,
    );
    unsafe {
        core::ptr::write_unaligned(mic_iv as *mut [U8; 16], out);
    }
}

#[cfg(host_security_rest_test)]
#[no_mangle]
pub extern "C" fn host_ccmp_construct_mic_header1(
    mic_header1: *mut U8,
    header_length: c_int,
    mpdu: *mut U8,
    frtype: U32,
) {
    let mpdu_slice = unsafe { core::slice::from_raw_parts(mpdu, 32) };
    let mut out = [0u8; 16];
    construct_mic_header1(&mut out, header_length as Sint, mpdu_slice, frtype);
    unsafe {
        core::ptr::write_unaligned(mic_header1 as *mut [U8; 16], out);
    }
}

#[cfg(host_security_rest_test)]
#[no_mangle]
pub extern "C" fn host_ccmp_construct_mic_header2(
    mic_header2: *mut U8,
    mpdu: *mut U8,
    a4_exists: c_int,
    qc_exists: c_int,
) {
    let mpdu_slice = unsafe { core::slice::from_raw_parts(mpdu, 32) };
    let mut out = [0u8; 16];
    construct_mic_header2(
        &mut out,
        mpdu_slice,
        a4_exists as Sint,
        qc_exists as Sint,
    );
    unsafe {
        core::ptr::write_unaligned(mic_header2 as *mut [U8; 16], out);
    }
}

#[cfg(host_security_rest_test)]
#[no_mangle]
pub extern "C" fn host_ccmp_construct_ctr_preload(
    ctr_preload: *mut U8,
    a4_exists: c_int,
    qc_exists: c_int,
    mpdu: *mut U8,
    pn_vector: *mut U8,
    c: c_int,
    frtype: U32,
) {
    let mpdu_slice = unsafe { core::slice::from_raw_parts(mpdu, 32) };
    let pn: [U8; 6] = unsafe { core::ptr::read_unaligned(pn_vector as *const [U8; 6]) };
    let mut out = [0u8; 16];
    construct_ctr_preload(
        &mut out,
        a4_exists as Sint,
        qc_exists as Sint,
        mpdu_slice,
        &pn,
        c as Sint,
        frtype,
    );
    unsafe {
        core::ptr::write_unaligned(ctr_preload as *mut [U8; 16], out);
    }
}
