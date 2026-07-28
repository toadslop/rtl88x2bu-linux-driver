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

const WLAN_HDR_A3_LEN: U32 = 24;
const WLAN_HDR_A3_QOS_LEN: U32 = 26;
const WLAN_HDR_A4_QOS_LEN: U32 = 32;

const WIFI_DATA_CFACK: U32 = (1 << 4) | WIFI_DATA_TYPE;
const WIFI_DATA_CFPOLL: U32 = (1 << 5) | WIFI_DATA_TYPE;
const WIFI_DATA_CFACKPOLL: U32 = (1 << 5) | (1 << 4) | WIFI_DATA_TYPE;

const _SUCCESS: Sint = 1;
const _FAIL: Sint = 0;
const MAX_MSG_SIZE: usize = 2048;

#[cfg(not(host_security_rest_test))]
extern "C" {
    fn rtw_aes_decipher_log_mic_mismatch(i: c_int, pframe_byte: U8, message_byte: U8);
}

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

fn get_frame_type_le(pframe: &[U8]) -> U32 {
    let v = u16::from_le_bytes([pframe[0], pframe[1]]);
    (v as U32) & ((1 << 3) | (1 << 2))
}

fn get_frame_sub_type_le(pframe: &[U8]) -> U32 {
    let v = u16::from_le_bytes([pframe[0], pframe[1]]);
    (v as U32) & 0xfc
}

// ----- AES-CCMP software encrypt (W3-12) -----

fn aes_cipher(key: &[U8; 16], hdrlen: u32, pframe: &mut [U8], plen: u32) -> Sint {
    let mut hdrlen = hdrlen;
    let frtype = get_frame_type_le(pframe);
    let frsubtype = get_frame_sub_type_le(pframe) >> 4;

    let mut mic_iv = [0u8; 16];
    let mut mic_header1 = [0u8; 16];
    let mut mic_header2 = [0u8; 16];
    let mut ctr_preload = [0u8; 16];
    let mut chain_buffer = [0u8; 16];
    let mut aes_out = [0u8; 16];
    let mut padded_buffer = [0u8; 16];
    let mut mic = [0u8; 8];

    let a4_exists = if hdrlen == WLAN_HDR_A3_LEN || hdrlen == WLAN_HDR_A3_QOS_LEN {
        0
    } else {
        1
    };

    let qc_exists = if (frtype | frsubtype) == WIFI_DATA_CFACK
        || (frtype | frsubtype) == WIFI_DATA_CFPOLL
        || (frtype | frsubtype) == WIFI_DATA_CFACKPOLL
    {
        if hdrlen != WLAN_HDR_A3_QOS_LEN && hdrlen != WLAN_HDR_A4_QOS_LEN {
            hdrlen += 2;
        }
        1
    } else if frtype == WIFI_DATA_TYPE
        && (frsubtype == 0x08
            || frsubtype == 0x09
            || frsubtype == 0x0a
            || frsubtype == 0x0b)
    {
        if hdrlen != WLAN_HDR_A3_QOS_LEN && hdrlen != WLAN_HDR_A4_QOS_LEN {
            hdrlen += 2;
        }
        1
    } else {
        0
    };

    let pn_vector = [
        pframe[hdrlen as usize],
        pframe[hdrlen as usize + 1],
        pframe[hdrlen as usize + 4],
        pframe[hdrlen as usize + 5],
        pframe[hdrlen as usize + 6],
        pframe[hdrlen as usize + 7],
    ];

    construct_mic_iv(
        &mut mic_iv,
        qc_exists,
        a4_exists,
        pframe,
        plen,
        &pn_vector,
        frtype,
    );
    construct_mic_header1(&mut mic_header1, hdrlen as Sint, pframe, frtype);
    construct_mic_header2(&mut mic_header2, pframe, a4_exists, qc_exists);

    let payload_remainder = plen % 16;
    let num_blocks = plen / 16;
    let payload_base = hdrlen as usize + 8;
    let mut payload_index = payload_base;

    aes128k128d(key, &mic_iv, &mut aes_out);
    bitwise_xor(&aes_out, &mic_header1, &mut chain_buffer);
    aes128k128d(key, &chain_buffer, &mut aes_out);
    bitwise_xor(&aes_out, &mic_header2, &mut chain_buffer);
    aes128k128d(key, &chain_buffer, &mut aes_out);

    for _ in 0..num_blocks {
        let block = {
            let src = &pframe[payload_index..payload_index + 16];
            let mut arr = [0u8; 16];
            arr.copy_from_slice(src);
            arr
        };
        bitwise_xor(&aes_out, &block, &mut chain_buffer);
        payload_index += 16;
        aes128k128d(key, &chain_buffer, &mut aes_out);
    }

    if payload_remainder > 0 {
        padded_buffer.fill(0);
        for j in 0..payload_remainder as usize {
            padded_buffer[j] = pframe[payload_index + j];
        }
        payload_index += payload_remainder as usize;
        bitwise_xor(&aes_out, &padded_buffer, &mut chain_buffer);
        aes128k128d(key, &chain_buffer, &mut aes_out);
    }

    for j in 0..8 {
        mic[j] = aes_out[j];
    }
    for j in 0..8 {
        pframe[payload_index + j] = mic[j];
    }

    payload_index = payload_base;
    for i in 0..num_blocks {
        construct_ctr_preload(
            &mut ctr_preload,
            a4_exists,
            qc_exists,
            pframe,
            &pn_vector,
            (i + 1) as Sint,
            frtype,
        );
        aes128k128d(key, &ctr_preload, &mut aes_out);
        let block = {
            let src = &pframe[payload_index..payload_index + 16];
            let mut arr = [0u8; 16];
            arr.copy_from_slice(src);
            arr
        };
        bitwise_xor(&aes_out, &block, &mut chain_buffer);
        for j in 0..16 {
            pframe[payload_index + j] = chain_buffer[j];
        }
        payload_index += 16;
    }

    if payload_remainder > 0 {
        construct_ctr_preload(
            &mut ctr_preload,
            a4_exists,
            qc_exists,
            pframe,
            &pn_vector,
            (num_blocks + 1) as Sint,
            frtype,
        );
        padded_buffer.fill(0);
        for j in 0..payload_remainder as usize {
            padded_buffer[j] = pframe[payload_index + j];
        }
        aes128k128d(key, &ctr_preload, &mut aes_out);
        bitwise_xor(&aes_out, &padded_buffer, &mut chain_buffer);
        for j in 0..payload_remainder as usize {
            pframe[payload_index + j] = chain_buffer[j];
        }
        payload_index += payload_remainder as usize;
    }

    construct_ctr_preload(
        &mut ctr_preload,
        a4_exists,
        qc_exists,
        pframe,
        &pn_vector,
        0,
        frtype,
    );
    padded_buffer.fill(0);
    for j in 0..8 {
        padded_buffer[j] = pframe[hdrlen as usize + 8 + plen as usize + j];
    }
    aes128k128d(key, &ctr_preload, &mut aes_out);
    bitwise_xor(&aes_out, &padded_buffer, &mut chain_buffer);
    for j in 0..8 {
        pframe[payload_index + j] = chain_buffer[j];
    }

    _SUCCESS
}

fn aes_decipher_inner(key: &[U8; 16], hdrlen: u32, pframe: &mut [U8], plen: u32) -> Sint {
    let mut hdrlen = hdrlen;
    let frtype = get_frame_type_le(pframe);
    let frsubtype = get_frame_sub_type_le(pframe) >> 4;

    let mut message = [0u8; MAX_MSG_SIZE];
    let mut mic_iv = [0u8; 16];
    let mut mic_header1 = [0u8; 16];
    let mut mic_header2 = [0u8; 16];
    let mut ctr_preload = [0u8; 16];
    let mut chain_buffer = [0u8; 16];
    let mut aes_out = [0u8; 16];
    let mut padded_buffer = [0u8; 16];
    let mut mic = [0u8; 8];
    let mut res = _SUCCESS;

    let mut num_blocks = (plen - 8) / 16;
    let mut payload_remainder = (plen - 8) % 16;

    let mut pn_vector = [
        pframe[hdrlen as usize],
        pframe[hdrlen as usize + 1],
        pframe[hdrlen as usize + 4],
        pframe[hdrlen as usize + 5],
        pframe[hdrlen as usize + 6],
        pframe[hdrlen as usize + 7],
    ];

    let a4_exists = if hdrlen == WLAN_HDR_A3_LEN || hdrlen == WLAN_HDR_A3_QOS_LEN {
        0
    } else {
        1
    };

    let qc_exists = if (frtype | frsubtype) == WIFI_DATA_CFACK
        || (frtype | frsubtype) == WIFI_DATA_CFPOLL
        || (frtype | frsubtype) == WIFI_DATA_CFACKPOLL
    {
        if hdrlen != WLAN_HDR_A3_QOS_LEN && hdrlen != WLAN_HDR_A4_QOS_LEN {
            hdrlen += 2;
        }
        1
    } else if frtype == WIFI_DATA_TYPE
        && (frsubtype == 0x08
            || frsubtype == 0x09
            || frsubtype == 0x0a
            || frsubtype == 0x0b)
    {
        if hdrlen != WLAN_HDR_A3_QOS_LEN && hdrlen != WLAN_HDR_A4_QOS_LEN {
            hdrlen += 2;
        }
        1
    } else {
        0
    };

    let payload_base = hdrlen as usize + 8;
    let mut payload_index = payload_base;

    for i in 0..num_blocks {
        construct_ctr_preload(
            &mut ctr_preload,
            a4_exists,
            qc_exists,
            pframe,
            &pn_vector,
            (i + 1) as Sint,
            frtype,
        );
        aes128k128d(key, &ctr_preload, &mut aes_out);
        let block = {
            let src = &pframe[payload_index..payload_index + 16];
            let mut arr = [0u8; 16];
            arr.copy_from_slice(src);
            arr
        };
        bitwise_xor(&aes_out, &block, &mut chain_buffer);
        for j in 0..16 {
            pframe[payload_index + j] = chain_buffer[j];
        }
        payload_index += 16;
    }

    if payload_remainder > 0 {
        construct_ctr_preload(
            &mut ctr_preload,
            a4_exists,
            qc_exists,
            pframe,
            &pn_vector,
            (num_blocks + 1) as Sint,
            frtype,
        );
        padded_buffer.fill(0);
        for j in 0..payload_remainder as usize {
            padded_buffer[j] = pframe[payload_index + j];
        }
        aes128k128d(key, &ctr_preload, &mut aes_out);
        bitwise_xor(&aes_out, &padded_buffer, &mut chain_buffer);
        for j in 0..payload_remainder as usize {
            pframe[payload_index + j] = chain_buffer[j];
        }
    }

    let copy_len = hdrlen as usize + plen as usize + 8;
    if copy_len <= MAX_MSG_SIZE {
        message[..copy_len].copy_from_slice(&pframe[..copy_len]);
    }

    pn_vector = [
        pframe[hdrlen as usize],
        pframe[hdrlen as usize + 1],
        pframe[hdrlen as usize + 4],
        pframe[hdrlen as usize + 5],
        pframe[hdrlen as usize + 6],
        pframe[hdrlen as usize + 7],
    ];

    construct_mic_iv(
        &mut mic_iv,
        qc_exists,
        a4_exists,
        &message,
        plen - 8,
        &pn_vector,
        frtype,
    );
    construct_mic_header1(&mut mic_header1, hdrlen as Sint, &message, frtype);
    construct_mic_header2(&mut mic_header2, &message, a4_exists, qc_exists);

    payload_remainder = (plen - 8) % 16;
    num_blocks = (plen - 8) / 16;
    payload_index = payload_base;

    aes128k128d(key, &mic_iv, &mut aes_out);
    bitwise_xor(&aes_out, &mic_header1, &mut chain_buffer);
    aes128k128d(key, &chain_buffer, &mut aes_out);
    bitwise_xor(&aes_out, &mic_header2, &mut chain_buffer);
    aes128k128d(key, &chain_buffer, &mut aes_out);

    for _ in 0..num_blocks {
        let block = {
            let src = &message[payload_index..payload_index + 16];
            let mut arr = [0u8; 16];
            arr.copy_from_slice(src);
            arr
        };
        bitwise_xor(&aes_out, &block, &mut chain_buffer);
        payload_index += 16;
        aes128k128d(key, &chain_buffer, &mut aes_out);
    }

    if payload_remainder > 0 {
        padded_buffer.fill(0);
        for j in 0..payload_remainder as usize {
            padded_buffer[j] = message[payload_index + j];
        }
        payload_index += payload_remainder as usize;
        bitwise_xor(&aes_out, &padded_buffer, &mut chain_buffer);
        aes128k128d(key, &chain_buffer, &mut aes_out);
    }

    for j in 0..8 {
        mic[j] = aes_out[j];
    }
    for j in 0..8 {
        message[payload_index + j] = mic[j];
    }

    payload_index = payload_base;
    for i in 0..num_blocks {
        construct_ctr_preload(
            &mut ctr_preload,
            a4_exists,
            qc_exists,
            &message,
            &pn_vector,
            (i + 1) as Sint,
            frtype,
        );
        aes128k128d(key, &ctr_preload, &mut aes_out);
        let block = {
            let src = &message[payload_index..payload_index + 16];
            let mut arr = [0u8; 16];
            arr.copy_from_slice(src);
            arr
        };
        bitwise_xor(&aes_out, &block, &mut chain_buffer);
        for j in 0..16 {
            message[payload_index + j] = chain_buffer[j];
        }
        payload_index += 16;
    }

    if payload_remainder > 0 {
        construct_ctr_preload(
            &mut ctr_preload,
            a4_exists,
            qc_exists,
            &message,
            &pn_vector,
            (num_blocks + 1) as Sint,
            frtype,
        );
        padded_buffer.fill(0);
        for j in 0..payload_remainder as usize {
            padded_buffer[j] = message[payload_index + j];
        }
        aes128k128d(key, &ctr_preload, &mut aes_out);
        bitwise_xor(&aes_out, &padded_buffer, &mut chain_buffer);
        for j in 0..payload_remainder as usize {
            message[payload_index + j] = chain_buffer[j];
        }
        payload_index += payload_remainder as usize;
    }

    construct_ctr_preload(
        &mut ctr_preload,
        a4_exists,
        qc_exists,
        &message,
        &pn_vector,
        0,
        frtype,
    );
    padded_buffer.fill(0);
    for j in 0..8 {
        padded_buffer[j] = message[hdrlen as usize + 8 + plen as usize - 8 + j];
    }
    aes128k128d(key, &ctr_preload, &mut aes_out);
    bitwise_xor(&aes_out, &padded_buffer, &mut chain_buffer);
    for j in 0..8 {
        message[payload_index + j] = chain_buffer[j];
    }

    let mic_off = hdrlen as usize + 8 + plen as usize - 8;
    for i in 0..8 {
        if pframe[mic_off + i] != message[mic_off + i] {
            #[cfg(not(host_security_rest_test))]
            unsafe {
                rtw_aes_decipher_log_mic_mismatch(
                    i as c_int,
                    pframe[mic_off + i],
                    message[mic_off + i],
                );
            }
            res = _FAIL;
        }
    }

    res
}

#[cfg(not(host_security_rest_test))]
#[no_mangle]
pub extern "C" fn aes_decipher(
    key: *mut U8,
    hdrlen: u32,
    pframe: *mut U8,
    plen: u32,
) -> Sint {
    if key.is_null() || pframe.is_null() || plen < 8 {
        return _FAIL;
    }
    let key_arr: [U8; 16] = unsafe { core::ptr::read_unaligned(key as *const [U8; 16]) };
    let need = hdrlen as usize + 8 + plen as usize;
    let frame_slice = unsafe { core::slice::from_raw_parts_mut(pframe, need) };
    aes_decipher_inner(&key_arr, hdrlen, frame_slice, plen)
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

#[cfg(host_security_rest_test)]
#[no_mangle]
pub extern "C" fn host_ccmp_aes_cipher(
    key: *mut U8,
    hdrlen: u32,
    pframe: *mut U8,
    plen: u32,
) -> Sint {
    if key.is_null() || pframe.is_null() {
        return 0;
    }
    let key_arr: [U8; 16] = unsafe { core::ptr::read_unaligned(key as *const [U8; 16]) };
    let need = hdrlen as usize + 8 + plen as usize + 8;
    let frame_slice = unsafe { core::slice::from_raw_parts_mut(pframe, need) };
    aes_cipher(&key_arr, hdrlen, frame_slice, plen)
}

#[cfg(host_security_rest_test)]
#[no_mangle]
pub extern "C" fn host_ccmp_aes_decipher(
    key: *mut U8,
    hdrlen: u32,
    pframe: *mut U8,
    plen: u32,
) -> Sint {
    if key.is_null() || pframe.is_null() {
        return _FAIL;
    }
    let key_arr: [U8; 16] = unsafe { core::ptr::read_unaligned(key as *const [U8; 16]) };
    let need = hdrlen as usize + 8 + plen as usize;
    let frame_slice = unsafe { core::slice::from_raw_parts_mut(pframe, need) };
    aes_decipher_inner(&key_arr, hdrlen, frame_slice, plen)
}

// ----- rtw_aes_encrypt frame path (W3-12c) -----

const _AES_: U8 = 0x04;
const _SEC_TYPE_256_: U8 = 0x10;
const _CCMP_256_: U8 = _AES_ | _SEC_TYPE_256_;
const AES_RTW_SUCCESS: U32 = 1;
const AES_RTW_FAIL: U32 = 0;

#[cfg(host_security_rest_test)]
const TXDESC_OFFSET: usize = 48 + 8;

#[cfg(not(host_security_rest_test))]
const TXDESC_SIZE: usize = 48;
#[cfg(not(host_security_rest_test))]
const PACKET_OFFSET_SZ: usize = 8;

#[repr(C)]
pub struct KeyType {
    pub skey: [U8; 32],
}

#[cfg(host_security_rest_test)]
pub type AesAdapter = core::ffi::c_void;

#[cfg(not(host_security_rest_test))]
pub type AesAdapter = core::ffi::c_void;

extern "C" {
    fn _rtw_ccmp_encrypt(
        padapter: *mut core::ffi::c_void,
        key: *mut U8,
        key_len: U32,
        hdrlen: U32,
        frame: *mut U8,
        plen: U32,
    ) -> i32;
    fn _rtw_ccmp_decrypt(
        padapter: *mut core::ffi::c_void,
        key: *mut U8,
        key_len: U32,
        hdrlen: U32,
        frame: *mut U8,
        plen: U32,
    ) -> i32;
    fn rtw_get_stainfo(stapriv: *mut core::ffi::c_void, hwaddr: *mut U8) -> *mut core::ffi::c_void;
    fn rtw_tkip_decrypt_mcast_gkey_check(
        padapter: *mut AesAdapter,
        ra: *const U8,
        grpkey_installed: U8,
    ) -> U8;
    fn rtw_gcmp_decrypt_mcast_gkey_check(
        padapter: *mut AesAdapter,
        ra: *const U8,
        grpkey_installed: U8,
    ) -> U8;
}

fn rnd4(ptr: usize) -> usize {
    ((ptr >> 2) + if ptr & 3 == 0 { 0 } else { 1 }) << 2
}

fn is_mcast_ra(ra: &[U8; 6]) -> bool {
    (ra[0] & 0x01) != 0
}

fn is_broadcast_mac_addr(addr: &[U8; 6]) -> bool {
    addr[0] == 0xff && addr[1] == 0xff && addr[2] == 0xff && addr[3] == 0xff
        && addr[4] == 0xff && addr[5] == 0xff
}

fn is_multicast_mac_addr(addr: &[U8; 6]) -> bool {
    (addr[0] & 0x01) != 0 && !is_broadcast_mac_addr(addr)
}

#[cfg(host_security_rest_test)]
fn hw_hdr_offset(pkt_offset: i8) -> usize {
    TXDESC_OFFSET + (pkt_offset as usize) * 8
}

#[cfg(not(host_security_rest_test))]
fn hw_hdr_offset(pkt_offset: i8) -> usize {
    TXDESC_SIZE + (pkt_offset as usize) * PACKET_OFFSET_SZ
}

#[cfg(not(host_security_rest_test))]
mod kernel_layout {
    use super::*;

    extern "C" {
        static rtw_rust_wep_off_adapter_securitypriv: usize;
        static rtw_rust_wep_off_adapter_xmitpriv: usize;
        static rtw_rust_wep_off_xmitpriv_frag_len: usize;
        static rtw_rust_wep_off_xmit_frame_attrib: usize;
        static rtw_rust_wep_off_xmit_frame_buf_addr: usize;
        static rtw_rust_wep_off_xmit_frame_pkt_offset: usize;
        static rtw_rust_tkip_off_securitypriv_dot118021XGrpKeyid: usize;
        static rtw_rust_tkip_off_securitypriv_dot118021XGrpKey: usize;
        static rtw_rust_tkip_off_pkt_attrib_dot118021x_UncstKey: usize;
        static rtw_rust_aes_off_securitypriv_aes_sw_enc_cnt_bc: usize;
        static rtw_rust_aes_off_securitypriv_aes_sw_enc_cnt_mc: usize;
        static rtw_rust_aes_off_securitypriv_aes_sw_enc_cnt_uc: usize;
        static rtw_rust_aes_off_securitypriv_aes_sw_dec_cnt_bc: usize;
        static rtw_rust_aes_off_securitypriv_aes_sw_dec_cnt_mc: usize;
        static rtw_rust_aes_off_securitypriv_aes_sw_dec_cnt_uc: usize;
        static rtw_rust_wep_off_recv_frame_hdr: usize;
        static rtw_rust_wep_off_recv_frame_hdr_attrib: usize;
        static rtw_rust_wep_off_recv_frame_hdr_len: usize;
        static rtw_rust_wep_off_recv_frame_hdr_rx_data: usize;
        static rtw_rust_tkip_off_securitypriv_binstallGrpkey: usize;
        static rtw_rust_tkip_off_adapter_stapriv: usize;
        static rtw_rust_tkip_off_sta_info_dot118021x_UncstKey: usize;
    }

    #[repr(C)]
    pub struct RxPktAttrib {
        pub pkt_len: u16,
        pub physt: U8,
        pub drvinfo_sz: U8,
        pub shift_sz: U8,
        pub hdrlen: U8,
        pub to_fr_ds: U8,
        pub amsdu: U8,
        pub qos: U8,
        pub priority: U8,
        pub pw_save: U8,
        pub mdata: U8,
        pub seq_num: u16,
        pub frag_num: U8,
        pub mfrag: U8,
        pub order: U8,
        pub privacy: U8,
        pub bdecrypted: U8,
        pub encrypt: U8,
        pub iv_len: U8,
        pub icv_len: U8,
        pub crc_err: U8,
        pub icv_err: U8,
        pub dst: [U8; 6],
        pub src: [U8; 6],
        pub ta: [U8; 6],
        pub ra: [U8; 6],
        pub bssid: [U8; 6],
        pub ack_policy: U8,
        pub key_index: U8,
    }

    #[repr(C)]
    pub struct PktAttrib {
        pub encrypt: U8,
        pub nr_frags: U8,
        pub hdrlen: u16,
        pub last_txcmdsz: U32,
        pub iv_len: U8,
        pub icv_len: U8,
        pub ra: [U8; 6],
    }

    pub unsafe fn adapter_securitypriv(padapter: *mut AesAdapter) -> *mut u8 {
        unsafe {
            (padapter as *mut u8).add(rtw_rust_wep_off_adapter_securitypriv)
        }
    }

    pub unsafe fn adapter_xmitpriv(padapter: *mut AesAdapter) -> *mut u8 {
        unsafe { (padapter as *mut u8).add(rtw_rust_wep_off_adapter_xmitpriv) }
    }

    pub unsafe fn xmitpriv_frag_len(pxmitpriv: *mut u8) -> U32 {
        unsafe {
            *((pxmitpriv.add(rtw_rust_wep_off_xmitpriv_frag_len)) as *const U32)
        }
    }

    pub unsafe fn xmit_frame_attrib(pxmitframe: *mut u8) -> *mut PktAttrib {
        unsafe { (pxmitframe.add(rtw_rust_wep_off_xmit_frame_attrib)) as *mut PktAttrib }
    }

    pub unsafe fn xmit_frame_buf_addr(pxmitframe: *mut u8) -> *mut U8 {
        unsafe {
            *((pxmitframe.add(rtw_rust_wep_off_xmit_frame_buf_addr)) as *const *mut U8)
        }
    }

    pub unsafe fn xmit_frame_pkt_offset(pxmitframe: *mut u8) -> i8 {
        unsafe {
            *((pxmitframe.add(rtw_rust_wep_off_xmit_frame_pkt_offset)) as *const i8)
        }
    }

    pub unsafe fn securitypriv_grp_keyid(psecuritypriv: *mut u8) -> U32 {
        unsafe {
            *((psecuritypriv.add(rtw_rust_tkip_off_securitypriv_dot118021XGrpKeyid))
                as *const U32)
        }
    }

    pub unsafe fn securitypriv_grp_key_skey(psecuritypriv: *mut u8, index: usize) -> *mut U8 {
        unsafe {
            let base = psecuritypriv.add(rtw_rust_tkip_off_securitypriv_dot118021XGrpKey);
            let stride = core::mem::size_of::<KeyType>();
            base.add(index * stride) as *mut U8
        }
    }

    pub unsafe fn pkt_attrib_unicast_key_skey(pattrib: *mut PktAttrib) -> *mut U8 {
        unsafe {
            (pattrib as *mut u8)
                .add(rtw_rust_tkip_off_pkt_attrib_dot118021x_UncstKey) as *mut U8
        }
    }

    pub unsafe fn aes_sw_enc_cnt_inc(psecuritypriv: *mut u8, ra: &[U8; 6]) {
        unsafe {
            let off = if is_broadcast_mac_addr(ra) {
                rtw_rust_aes_off_securitypriv_aes_sw_enc_cnt_bc
            } else if is_multicast_mac_addr(ra) {
                rtw_rust_aes_off_securitypriv_aes_sw_enc_cnt_mc
            } else {
                rtw_rust_aes_off_securitypriv_aes_sw_enc_cnt_uc
            };
            let cnt = (psecuritypriv.add(off)) as *mut u64;
            *cnt = cnt.read().wrapping_add(1);
        }
    }

    pub unsafe fn recv_frame_base(precvframe: *mut u8) -> *mut u8 {
        unsafe { precvframe.add(rtw_rust_wep_off_recv_frame_hdr) }
    }

    pub unsafe fn recv_frame_attrib(precvframe: *mut u8) -> *mut RxPktAttrib {
        unsafe {
            (recv_frame_base(precvframe).add(rtw_rust_wep_off_recv_frame_hdr_attrib))
                as *mut RxPktAttrib
        }
    }

    pub unsafe fn recv_frame_len(precvframe: *mut u8) -> U32 {
        unsafe {
            *((recv_frame_base(precvframe).add(rtw_rust_wep_off_recv_frame_hdr_len)) as *const U32)
        }
    }

    pub unsafe fn recv_frame_rx_data(precvframe: *mut u8) -> *mut U8 {
        unsafe {
            *((recv_frame_base(precvframe).add(rtw_rust_wep_off_recv_frame_hdr_rx_data))
                as *const *mut U8)
        }
    }

    pub unsafe fn adapter_stapriv(padapter: *mut AesAdapter) -> *mut u8 {
        unsafe { (padapter as *mut u8).add(rtw_rust_tkip_off_adapter_stapriv) }
    }

    pub unsafe fn securitypriv_binstall_grpkey(psecuritypriv: *mut u8) -> U8 {
        unsafe {
            *((psecuritypriv.add(rtw_rust_tkip_off_securitypriv_binstallGrpkey)) as *const U8)
        }
    }

    pub unsafe fn sta_info_unicast_key_skey(stainfo: *mut u8) -> *mut U8 {
        unsafe {
            (stainfo.add(rtw_rust_tkip_off_sta_info_dot118021x_UncstKey)) as *mut U8
        }
    }

    pub unsafe fn aes_sw_dec_cnt_inc(psecuritypriv: *mut u8, ra: &[U8; 6]) {
        unsafe {
            let off = if is_broadcast_mac_addr(ra) {
                rtw_rust_aes_off_securitypriv_aes_sw_dec_cnt_bc
            } else if is_multicast_mac_addr(ra) {
                rtw_rust_aes_off_securitypriv_aes_sw_dec_cnt_mc
            } else {
                rtw_rust_aes_off_securitypriv_aes_sw_dec_cnt_uc
            };
            let cnt = (psecuritypriv.add(off)) as *mut u64;
            *cnt = cnt.read().wrapping_add(1);
        }
    }
}

unsafe fn aes_encrypt_frag_new_crypto(
    padapter: *mut AesAdapter,
    prwskey: *mut U8,
    prwskeylen: U32,
    pxmitpriv_frag_len: U32,
    nr_frags: U8,
    hdrlen: u16,
    last_txcmdsz: U32,
    iv_len: U8,
    icv_len: U8,
    buf_addr: *mut U8,
    hw: usize,
) {
    unsafe {
    let mut pframe = buf_addr.add(hw);
    let mut curfragnum: i32 = 0;

    while curfragnum < nr_frags as i32 {
        let plen = if (curfragnum + 1) == nr_frags as i32 {
            last_txcmdsz as i32 - hdrlen as i32 - iv_len as i32 - icv_len as i32
        } else {
            pxmitpriv_frag_len as i32 - hdrlen as i32 - iv_len as i32 - icv_len as i32
        };

        _rtw_ccmp_encrypt(
            padapter as *mut core::ffi::c_void,
            prwskey,
            prwskeylen,
            hdrlen as U32,
            pframe,
            plen as U32,
        );

        if (curfragnum + 1) != nr_frags as i32 {
            pframe = pframe.add(pxmitpriv_frag_len as usize);
            pframe = rnd4(pframe as usize) as *mut U8;
        }
        curfragnum += 1;
    }
    }
}

#[cfg(not(host_security_rest_test))]
#[no_mangle]
pub extern "C" fn rtw_aes_encrypt(padapter: *mut AesAdapter, pxmitframe: *mut U8) -> U32 {
    if padapter.is_null() || pxmitframe.is_null() {
        return AES_RTW_FAIL;
    }
    unsafe {
        let px = pxmitframe;
        let buf_addr = kernel_layout::xmit_frame_buf_addr(px);
        if buf_addr.is_null() {
            return AES_RTW_FAIL;
        }
        let hw = hw_hdr_offset(kernel_layout::xmit_frame_pkt_offset(px));
        let pattrib = kernel_layout::xmit_frame_attrib(px);
        let encrypt = (*pattrib).encrypt;
        if encrypt != _AES_ && encrypt != _CCMP_256_ {
            return AES_RTW_SUCCESS;
        }

        let psecuritypriv = kernel_layout::adapter_securitypriv(padapter);
        let pxmitpriv = kernel_layout::adapter_xmitpriv(padapter);
        let frag_len = kernel_layout::xmitpriv_frag_len(pxmitpriv);
        let ra = (*pattrib).ra;

        let prwskey = if is_mcast_ra(&ra) {
            let kid = kernel_layout::securitypriv_grp_keyid(psecuritypriv) as usize;
            kernel_layout::securitypriv_grp_key_skey(psecuritypriv, kid)
        } else {
            kernel_layout::pkt_attrib_unicast_key_skey(pattrib)
        };
        let prwskeylen = if encrypt == _CCMP_256_ { 32 } else { 16 };

        aes_encrypt_frag_new_crypto(
            padapter,
            prwskey,
            prwskeylen,
            frag_len,
            (*pattrib).nr_frags,
            (*pattrib).hdrlen,
            (*pattrib).last_txcmdsz,
            (*pattrib).iv_len,
            (*pattrib).icv_len,
            buf_addr,
            hw,
        );

        kernel_layout::aes_sw_enc_cnt_inc(psecuritypriv, &ra);
        AES_RTW_SUCCESS
    }
}

const _FALSE: U8 = 0;

#[cfg(not(host_security_rest_test))]
#[no_mangle]
pub extern "C" fn rtw_aes_decrypt(padapter: *mut AesAdapter, precvframe: *mut U8) -> U32 {
    if padapter.is_null() || precvframe.is_null() {
        return AES_RTW_FAIL;
    }
    unsafe {
        let psecuritypriv = kernel_layout::adapter_securitypriv(padapter);
        let attrib = kernel_layout::recv_frame_attrib(precvframe);
        let encrypt = (*attrib).encrypt;
        if encrypt != _AES_ && encrypt != _CCMP_256_ {
            return AES_RTW_SUCCESS;
        }

        let stapriv = kernel_layout::adapter_stapriv(padapter);
        let stainfo =
            rtw_get_stainfo(stapriv as *mut core::ffi::c_void, (*attrib).ta.as_mut_ptr());
        if stainfo.is_null() {
            return AES_RTW_FAIL;
        }

        let ra = (*attrib).ra;
        let prwskey = if is_mcast_ra(&ra) {
            let grpkey_installed = kernel_layout::securitypriv_binstall_grpkey(psecuritypriv);
            if rtw_tkip_decrypt_mcast_gkey_check(padapter, ra.as_ptr(), grpkey_installed) != _FALSE
            {
                return AES_RTW_FAIL;
            }
            let key_index = (*attrib).key_index as usize;
            if kernel_layout::securitypriv_grp_keyid(psecuritypriv) as usize != key_index {
                return AES_RTW_FAIL;
            }
            kernel_layout::securitypriv_grp_key_skey(psecuritypriv, key_index)
        } else {
            kernel_layout::sta_info_unicast_key_skey(stainfo as *mut u8)
        };

        let prwskeylen = if encrypt == _CCMP_256_ { 32 } else { 16 };
        let res = _rtw_ccmp_decrypt(
            padapter as *mut core::ffi::c_void,
            prwskey,
            prwskeylen,
            (*attrib).hdrlen as U32,
            kernel_layout::recv_frame_rx_data(precvframe),
            kernel_layout::recv_frame_len(precvframe),
        ) as U32;

        kernel_layout::aes_sw_dec_cnt_inc(psecuritypriv, &ra);
        res
    }
}

// ----- GCMP frame encrypt/decrypt (W3-14) -----

const _GCMP_: U8 = 0x07;
const _GCMP_256_: U8 = _GCMP_ | _SEC_TYPE_256_;
const GCMP_RTW_SUCCESS: U32 = 1;
const GCMP_RTW_FAIL: U32 = 0;

#[cfg(host_gcmp_frame_test)]
const HOST_GCMP_SUCCESS: U32 = 0;
#[cfg(host_gcmp_frame_test)]
const HOST_GCMP_FAIL: U32 = 1;

#[cfg(any(not(host_security_rest_test), host_gcmp_frame_test))]
extern "C" {
    fn _rtw_gcmp_encrypt(
        padapter: *mut core::ffi::c_void,
        key: *mut U8,
        key_len: U32,
        hdrlen: U32,
        frame: *mut U8,
        plen: U32,
    ) -> i32;
    #[cfg(any(not(host_security_rest_test), host_gcmp_frame_test))]
    fn _rtw_gcmp_decrypt(
        padapter: *mut core::ffi::c_void,
        key: *mut U8,
        key_len: U32,
        hdrlen: U32,
        frame: *mut U8,
        plen: U32,
    ) -> i32;
}

#[cfg(any(not(host_security_rest_test), host_gcmp_frame_test))]
unsafe fn gcmp_encrypt_frags(
    padapter: *mut core::ffi::c_void,
    prwskey: *mut U8,
    prwskeylen: U32,
    pxmitpriv_frag_len: U32,
    nr_frags: U8,
    hdrlen: u16,
    last_txcmdsz: U32,
    iv_len: U8,
    icv_len: U8,
    buf_addr: *mut U8,
    hw: usize,
) {
    unsafe {
        let mut pframe = buf_addr.add(hw);
        let mut curfragnum: i32 = 0;

        while curfragnum < nr_frags as i32 {
            let plen = if (curfragnum + 1) == nr_frags as i32 {
                last_txcmdsz as i32 - hdrlen as i32 - iv_len as i32 - icv_len as i32
            } else {
                pxmitpriv_frag_len as i32 - hdrlen as i32 - iv_len as i32 - icv_len as i32
            };

            _rtw_gcmp_encrypt(
                padapter,
                prwskey,
                prwskeylen,
                hdrlen as U32,
                pframe,
                plen as U32,
            );

            if (curfragnum + 1) != nr_frags as i32 {
                pframe = pframe.add(pxmitpriv_frag_len as usize);
                pframe = rnd4(pframe as usize) as *mut U8;
            }
            curfragnum += 1;
        }
    }
}

#[cfg(not(host_security_rest_test))]
mod gcmp_kernel_layout {
    use super::*;

    extern "C" {
        static rtw_rust_gcmp_off_securitypriv_gcmp_sw_enc_cnt_bc: usize;
        static rtw_rust_gcmp_off_securitypriv_gcmp_sw_enc_cnt_mc: usize;
        static rtw_rust_gcmp_off_securitypriv_gcmp_sw_enc_cnt_uc: usize;
        static rtw_rust_gcmp_off_securitypriv_gcmp_sw_dec_cnt_bc: usize;
        static rtw_rust_gcmp_off_securitypriv_gcmp_sw_dec_cnt_mc: usize;
        static rtw_rust_gcmp_off_securitypriv_gcmp_sw_dec_cnt_uc: usize;
    }

    pub unsafe fn gcmp_sw_enc_cnt_inc(psecuritypriv: *mut u8, ra: &[U8; 6]) {
        unsafe {
            let off = if is_broadcast_mac_addr(ra) {
                rtw_rust_gcmp_off_securitypriv_gcmp_sw_enc_cnt_bc
            } else if is_multicast_mac_addr(ra) {
                rtw_rust_gcmp_off_securitypriv_gcmp_sw_enc_cnt_mc
            } else {
                rtw_rust_gcmp_off_securitypriv_gcmp_sw_enc_cnt_uc
            };
            let cnt = (psecuritypriv.add(off)) as *mut u64;
            *cnt = cnt.read().wrapping_add(1);
        }
    }

    pub unsafe fn gcmp_sw_dec_cnt_inc(psecuritypriv: *mut u8, ra: &[U8; 6]) {
        unsafe {
            let off = if is_broadcast_mac_addr(ra) {
                rtw_rust_gcmp_off_securitypriv_gcmp_sw_dec_cnt_bc
            } else if is_multicast_mac_addr(ra) {
                rtw_rust_gcmp_off_securitypriv_gcmp_sw_dec_cnt_mc
            } else {
                rtw_rust_gcmp_off_securitypriv_gcmp_sw_dec_cnt_uc
            };
            let cnt = (psecuritypriv.add(off)) as *mut u64;
            *cnt = cnt.read().wrapping_add(1);
        }
    }
}

#[cfg(not(host_security_rest_test))]
#[no_mangle]
pub extern "C" fn rtw_gcmp_encrypt(padapter: *mut AesAdapter, pxmitframe: *mut U8) -> U32 {
    if padapter.is_null() || pxmitframe.is_null() {
        return GCMP_RTW_FAIL;
    }
    unsafe {
        let px = pxmitframe;
        let buf_addr = kernel_layout::xmit_frame_buf_addr(px);
        if buf_addr.is_null() {
            return GCMP_RTW_FAIL;
        }
        let hw = hw_hdr_offset(kernel_layout::xmit_frame_pkt_offset(px));
        let pattrib = kernel_layout::xmit_frame_attrib(px);
        let encrypt = (*pattrib).encrypt;
        if encrypt != _GCMP_ && encrypt != _GCMP_256_ {
            return GCMP_RTW_SUCCESS;
        }

        let psecuritypriv = kernel_layout::adapter_securitypriv(padapter);
        let pxmitpriv = kernel_layout::adapter_xmitpriv(padapter);
        let frag_len = kernel_layout::xmitpriv_frag_len(pxmitpriv);
        let ra = (*pattrib).ra;

        let prwskey = if is_mcast_ra(&ra) {
            let kid = kernel_layout::securitypriv_grp_keyid(psecuritypriv) as usize;
            kernel_layout::securitypriv_grp_key_skey(psecuritypriv, kid)
        } else {
            kernel_layout::pkt_attrib_unicast_key_skey(pattrib)
        };
        let prwskeylen = if encrypt == _GCMP_256_ { 32 } else { 16 };

        gcmp_encrypt_frags(
            padapter as *mut core::ffi::c_void,
            prwskey,
            prwskeylen,
            frag_len,
            (*pattrib).nr_frags,
            (*pattrib).hdrlen,
            (*pattrib).last_txcmdsz,
            (*pattrib).iv_len,
            (*pattrib).icv_len,
            buf_addr,
            hw,
        );

        gcmp_kernel_layout::gcmp_sw_enc_cnt_inc(psecuritypriv, &ra);
        GCMP_RTW_SUCCESS
    }
}

#[cfg(host_gcmp_frame_test)]
#[repr(C)]
pub struct HostGcmpPktAttrib {
    pub encrypt: U8,
    pub nr_frags: U8,
    pub _pad0: U8,
    pub hdrlen: u16,
    pub last_txcmdsz: U32,
    pub iv_len: U8,
    pub icv_len: U8,
    pub _pad1: [U8; 2],
    pub ra: [U8; 6],
    pub ta: [U8; 6],
    pub dot118021x_UncstKey: KeyType,
}

#[cfg(host_gcmp_frame_test)]
#[repr(C)]
pub struct HostGcmpXmitFrame {
    pub attrib: HostGcmpPktAttrib,
    pub buf_addr: *mut U8,
    pub pkt_offset: i8,
}

#[cfg(host_gcmp_frame_test)]
#[repr(C)]
pub struct HostGcmpSecurityPriv {
    pub dot11_privacy_key_index: U32,
    pub dot11_def_key: [KeyType; 6],
    pub dot11_def_keylen: [U32; 6],
    pub dot118021XGrpKeyid: U32,
    pub dot118021XGrpKey: [KeyType; 6],
    pub binstallGrpkey: U8,
}

#[cfg(host_gcmp_frame_test)]
#[repr(C)]
pub struct HostGcmpXmitPriv {
    pub frag_len: U32,
}

#[cfg(host_gcmp_frame_test)]
#[repr(C)]
pub struct HostGcmpStaInfo {
    pub used: U8,
    pub ta: [U8; 6],
    pub dot118021x_UncstKey: KeyType,
}

#[cfg(host_gcmp_frame_test)]
#[repr(C)]
pub struct HostGcmpStapriv {
    pub stas: [HostGcmpStaInfo; 4],
}

#[cfg(host_gcmp_frame_test)]
#[repr(C)]
pub struct HostGcmpRxPktAttrib {
    pub pkt_len: u16,
    pub _pad0: [U8; 3],
    pub hdrlen: U8,
    pub _pad1: [U8; 12],
    pub encrypt: U8,
    pub iv_len: U8,
    pub _pad2: [U8; 32],
    pub key_index: U8,
    pub _pad3: U8,
    pub ra: [U8; 6],
    pub ta: [U8; 6],
}

#[cfg(host_gcmp_frame_test)]
#[repr(C)]
pub struct HostGcmpRecvFrameHdr {
    pub attrib: HostGcmpRxPktAttrib,
    pub len: U32,
    pub rx_data: *mut U8,
}

#[cfg(host_gcmp_frame_test)]
#[repr(C)]
pub struct HostGcmpRecvFrame {
    pub hdr: HostGcmpRecvFrameHdr,
}

#[cfg(host_gcmp_frame_test)]
#[repr(C)]
pub struct HostGcmpAdapter {
    pub securitypriv: HostGcmpSecurityPriv,
    pub xmitpriv: HostGcmpXmitPriv,
    pub stapriv: HostGcmpStapriv,
}

#[cfg(host_gcmp_frame_test)]
const HOST_GCMP_TXDESC_OFFSET: usize = 56;

#[cfg(host_gcmp_frame_test)]
#[no_mangle]
pub extern "C" fn rtw_gcmp_encrypt(padapter: *mut HostGcmpAdapter, pxmitframe: *mut U8) -> U32 {
    if padapter.is_null() || pxmitframe.is_null() {
        return HOST_GCMP_FAIL;
    }
    unsafe {
        let xmit = &*(pxmitframe as *const HostGcmpXmitFrame);
        if xmit.buf_addr.is_null() {
            return HOST_GCMP_FAIL;
        }
        let attrib = &xmit.attrib;
        if attrib.encrypt != _GCMP_ && attrib.encrypt != _GCMP_256_ {
            return HOST_GCMP_SUCCESS;
        }

        let sec = &(*padapter).securitypriv;
        let hw = HOST_GCMP_TXDESC_OFFSET + (xmit.pkt_offset as usize) * 8;
        let prwskey = if is_mcast_ra(&attrib.ra) {
            sec.dot118021XGrpKey[sec.dot118021XGrpKeyid as usize]
                .skey
                .as_ptr() as *mut U8
        } else {
            attrib.dot118021x_UncstKey.skey.as_ptr() as *mut U8
        };
        let prwskeylen = if attrib.encrypt == _GCMP_256_ { 32 } else { 16 };

        gcmp_encrypt_frags(
            padapter as *mut core::ffi::c_void,
            prwskey,
            prwskeylen,
            (*padapter).xmitpriv.frag_len,
            attrib.nr_frags,
            attrib.hdrlen,
            attrib.last_txcmdsz,
            attrib.iv_len,
            attrib.icv_len,
            xmit.buf_addr,
            hw,
        );
        HOST_GCMP_SUCCESS
    }
}

#[cfg(host_gcmp_frame_test)]
fn host_gcmp_get_stainfo<'a>(
    stapriv: &'a HostGcmpStapriv,
    ta: &[U8; 6],
) -> Option<&'a HostGcmpStaInfo> {
    for sta in &stapriv.stas {
        if sta.used != 0 && sta.ta == *ta {
            return Some(sta);
        }
    }
    None
}

#[cfg(not(host_security_rest_test))]
#[no_mangle]
pub extern "C" fn rtw_gcmp_decrypt(padapter: *mut AesAdapter, precvframe: *mut U8) -> U32 {
    if padapter.is_null() || precvframe.is_null() {
        return GCMP_RTW_FAIL;
    }
    unsafe {
        let psecuritypriv = kernel_layout::adapter_securitypriv(padapter);
        let attrib = kernel_layout::recv_frame_attrib(precvframe);
        let encrypt = (*attrib).encrypt;
        if encrypt != _GCMP_ && encrypt != _GCMP_256_ {
            return GCMP_RTW_SUCCESS;
        }

        let stapriv = kernel_layout::adapter_stapriv(padapter);
        let stainfo =
            rtw_get_stainfo(stapriv as *mut core::ffi::c_void, (*attrib).ta.as_mut_ptr());
        if stainfo.is_null() {
            return GCMP_RTW_FAIL;
        }

        let ra = (*attrib).ra;
        let prwskey = if is_mcast_ra(&ra) {
            let grpkey_installed = kernel_layout::securitypriv_binstall_grpkey(psecuritypriv);
            if rtw_gcmp_decrypt_mcast_gkey_check(padapter, ra.as_ptr(), grpkey_installed) != _FALSE
            {
                return GCMP_RTW_FAIL;
            }
            let key_index = (*attrib).key_index as usize;
            if kernel_layout::securitypriv_grp_keyid(psecuritypriv) as usize != key_index {
                return GCMP_RTW_FAIL;
            }
            kernel_layout::securitypriv_grp_key_skey(psecuritypriv, key_index)
        } else {
            kernel_layout::sta_info_unicast_key_skey(stainfo as *mut u8)
        };

        let prwskeylen = if encrypt == _GCMP_256_ { 32 } else { 16 };
        let res = _rtw_gcmp_decrypt(
            padapter as *mut core::ffi::c_void,
            prwskey,
            prwskeylen,
            (*attrib).hdrlen as U32,
            kernel_layout::recv_frame_rx_data(precvframe),
            kernel_layout::recv_frame_len(precvframe),
        ) as U32;

        gcmp_kernel_layout::gcmp_sw_dec_cnt_inc(psecuritypriv, &ra);
        res
    }
}

#[cfg(host_gcmp_frame_test)]
#[no_mangle]
pub extern "C" fn rtw_gcmp_decrypt(padapter: *mut HostGcmpAdapter, precvframe: *mut U8) -> U32 {
    if padapter.is_null() || precvframe.is_null() {
        return HOST_GCMP_FAIL;
    }
    unsafe {
        let recv = &*(precvframe as *const HostGcmpRecvFrame);
        let attrib = &recv.hdr.attrib;
        if attrib.encrypt != _GCMP_ && attrib.encrypt != _GCMP_256_ {
            return HOST_GCMP_SUCCESS;
        }

        let adapter = &*padapter;
        let stainfo = match host_gcmp_get_stainfo(&adapter.stapriv, &attrib.ta) {
            Some(sta) => sta,
            None => return HOST_GCMP_FAIL,
        };

        let prwskey = if is_mcast_ra(&attrib.ra) {
            if adapter.securitypriv.binstallGrpkey == _FALSE {
                return HOST_GCMP_FAIL;
            }
            let key_index = attrib.key_index as usize;
            if adapter.securitypriv.dot118021XGrpKeyid as usize != key_index {
                return HOST_GCMP_FAIL;
            }
            adapter.securitypriv.dot118021XGrpKey[key_index]
                .skey
                .as_ptr() as *mut U8
        } else {
            stainfo.dot118021x_UncstKey.skey.as_ptr() as *mut U8
        };

        let prwskeylen = if attrib.encrypt == _GCMP_256_ { 32 } else { 16 };
        if _rtw_gcmp_decrypt(
            padapter as *mut core::ffi::c_void,
            prwskey,
            prwskeylen,
            attrib.hdrlen as U32,
            recv.hdr.rx_data,
            recv.hdr.len,
        ) == 0
        {
            return HOST_GCMP_FAIL;
        }
        HOST_GCMP_SUCCESS
    }
}
