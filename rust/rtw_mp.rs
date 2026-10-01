// SPDX-License-Identifier: GPL-2.0
//! MP PMAC signal generators — Rust port (W3-113 PR6c: CCK + L-SIG/HT-SIG).

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

use std::os::raw::{c_uchar, c_uint, c_void};

type u8 = c_uchar;
type u32 = c_uint;

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

fn read_u16_le(p: *const u8) -> u16 {
    unsafe { u16::from_le_bytes([*p, *p.add(1)]) }
}
fn write_u16_le(p: *mut u8, v: u16) {
    let b = v.to_le_bytes();
    unsafe {
        *p = b[0];
        *p.add(1) = b[1];
    }
}
fn read_u32_le(p: *const u8) -> u32 {
    unsafe { u32::from_le_bytes([*p, *p.add(1), *p.add(2), *p.add(3)]) }
}

struct TxView(*mut u8);
impl TxView {
    fn tx_rate(&self) -> u8 {
        unsafe { *self.0.add(1) }
    }
    fn b_spreamble(&self) -> bool {
        unsafe { (*self.0.add(4) & 2) != 0 }
    }
    fn b_stbc(&self) -> bool {
        unsafe { (*self.0.add(4) & 4) != 0 }
    }
    fn b_ldpc(&self) -> bool {
        unsafe { (*self.0.add(4) & 8) != 0 }
    }
    fn b_sgi(&self) -> bool {
        unsafe { (*self.0.add(4) & 1) != 0 }
    }
    fn ndp_sound(&self) -> bool {
        unsafe { (*self.0.add(4) & 16) != 0 }
    }
    fn bandwidth(&self) -> u8 {
        unsafe { (*self.0.add(4) >> 5) & 7 }
    }
    fn ntx(&self) -> u8 {
        unsafe { *self.0 >> 4 }
    }
    fn packet_length(&self) -> u32 {
        read_u32_le(unsafe { self.0.add(12) })
    }
    fn set_sfd(&self, v: u16) {
        write_u16_le(unsafe { self.0.add(18) }, v);
    }
    fn set_signal_field(&self, v: u8) {
        unsafe { *self.0.add(20) = v };
    }
    fn set_service_field(&self, v: u8) {
        unsafe { *self.0.add(21) = v };
    }
    fn set_length(&self, v: u32) {
        write_u16_le(unsafe { self.0.add(22) }, v as u16);
    }
    fn length_u32(&self) -> u32 {
        read_u16_le(unsafe { self.0.add(22) }) as u32
    }
    fn crc16(&self) -> *mut u8 {
        unsafe { self.0.add(24) }
    }
    fn lsig(&self) -> *mut u8 {
        unsafe { self.0.add(26) }
    }
    fn ht_sig(&self) -> *mut u8 {
        unsafe { self.0.add(29) }
    }
}

struct PktView(*mut u8);
impl PktView {
    fn mcs(&self) -> u8 {
        unsafe { *self.0 }
    }
    fn nss(&self) -> u8 {
        unsafe { *self.0.add(1) }
    }
    fn nsts(&self) -> u8 {
        unsafe { *self.0.add(2) }
    }
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

#[no_mangle]
pub extern "C" fn CCK_generator(p_tx: *mut c_void, p_pkt: *mut c_void) {
    if p_tx.is_null() || p_pkt.is_null() {
        return;
    }
    let tx = TxView(p_tx as *mut u8);
    let pkt = PktView(p_pkt as *mut u8);
    if tx.b_spreamble() {
        tx.set_sfd(0x05cf);
    } else {
        tx.set_sfd(0xf3a0);
    }
    let mut ratio = 0.0f64;
    let mut crc16_in = [false; 32];
    match pkt.mcs() {
        0 => {
            tx.set_signal_field(0x0a);
            ratio = 8.0;
            crc16_in[1] = true;
            crc16_in[3] = true;
        }
        1 => {
            tx.set_signal_field(0x14);
            ratio = 4.0;
            crc16_in[2] = true;
            crc16_in[4] = true;
        }
        2 => {
            tx.set_signal_field(0x37);
            ratio = 8.0 / 5.5;
            crc16_in[0] = true;
            crc16_in[1] = true;
            crc16_in[2] = true;
            crc16_in[4] = true;
            crc16_in[5] = true;
        }
        3 => {
            tx.set_signal_field(0x6e);
            ratio = 8.0 / 11.0;
            crc16_in[1] = true;
            crc16_in[2] = true;
            crc16_in[3] = true;
            crc16_in[5] = true;
            crc16_in[6] = true;
        }
        _ => {}
    }
    let length_exact = (tx.packet_length() as f64) * ratio;
    let length_psdu = length_exact.ceil();
    let length_ext = pkt.mcs() == 3
        && ((length_psdu - length_exact) >= 0.727 || (length_psdu - length_exact) <= -0.727);
    tx.set_length(length_psdu as u32);
    for i in 0..16 {
        crc16_in[i + 16] = ((tx.length_u32() >> i) & 1) != 0;
    }
    if !length_ext {
        tx.set_service_field(0);
    } else {
        tx.set_service_field(0x80);
        crc16_in[15] = true;
    }
    let mut crc16_out = [false; 16];
    crc16_generator_inner(&mut crc16_out, &crc16_in, 32);
    zero_bytes(tx.crc16(), 2);
    byte_to_bit_inner(tx.crc16(), &crc16_out, 2);
}

#[no_mangle]
pub extern "C" fn L_SIG_generator(n_sym: u32, p_tx: *mut c_void, p_pkt: *mut c_void) {
    if p_tx.is_null() || p_pkt.is_null() {
        return;
    }
    let tx = TxView(p_tx as *mut u8);
    let pkt = PktView(p_pkt as *mut u8);
    let mut sig_bi = [false; 24];
    let (mode, length) = if mpt_is_ofdm_rate(tx.tx_rate()) {
        (pkt.mcs(), tx.packet_length())
    } else {
        let n_ltf = if pkt.nsts() <= 2 { pkt.nsts() } else { 4 };
        let t_data = if tx.b_sgi() { 3.6 } else { 4.0 };
        let ofdm_symbol = if mpt_is_vht_rate(tx.tx_rate()) {
            ((8.0 + 4.0 + (n_ltf as f64) * 4.0 + (n_sym as f64) * t_data + 4.0) / 4.0).ceil()
                as u32
        } else {
            ((8.0 + 4.0 + (n_ltf as f64) * 4.0 + (n_sym as f64) * t_data) / 4.0).ceil() as u32
        };
        (0u8, ofdm_symbol * 3 - 3)
    };
    const RATE_BITS: [[bool; 4]; 8] = [
        [true, true, false, true],
        [true, true, true, true],
        [false, true, false, true],
        [false, true, true, true],
        [true, false, false, true],
        [true, false, true, true],
        [false, false, false, true],
        [false, false, true, true],
    ];
    if mode < 8 {
        sig_bi[..4].copy_from_slice(&RATE_BITS[mode as usize]);
    }
    for i in 0..12 {
        sig_bi[i + 5] = ((length >> i) & 1) != 0;
    }
    let mut parity = 0u8;
    for i in 0..17 {
        if sig_bi[i] {
            parity += 1;
        }
    }
    sig_bi[17] = (parity % 2) != 0;
    zero_bytes(tx.lsig(), 3);
    byte_to_bit_inner(tx.lsig(), &sig_bi, 3);
}

#[no_mangle]
pub extern "C" fn HT_SIG_generator(p_tx: *mut c_void, p_pkt: *mut c_void) {
    if p_tx.is_null() || p_pkt.is_null() {
        return;
    }
    let tx = TxView(p_tx as *mut u8);
    let pkt = PktView(p_pkt as *mut u8);
    let mut sig_bi = [false; 48];
    for i in 0..7 {
        sig_bi[i] = ((pkt.mcs() >> i) & 1) != 0;
    }
    sig_bi[7] = tx.bandwidth() != 0;
    let plen = tx.packet_length();
    for i in 0..16 {
        sig_bi[i + 8] = ((plen >> i) & 1) != 0;
    }
    sig_bi[24] = true;
    sig_bi[25] = !tx.ndp_sound();
    sig_bi[26] = true;
    sig_bi[27] = false;
    if tx.b_stbc() {
        sig_bi[28] = true;
    }
    sig_bi[30] = tx.b_ldpc();
    sig_bi[31] = tx.b_sgi();
    if !tx.ndp_sound() {
        sig_bi[32] = false;
        sig_bi[33] = false;
    } else {
        let n_eltf = tx.ntx() - pkt.nss();
        sig_bi[32] = (n_eltf & 1) != 0;
        sig_bi[33] = ((n_eltf >> 1) & 1) != 0;
    }
    let mut crc8 = [false; 8];
    crc8_generator_inner(&mut crc8, &sig_bi[..34], 34);
    for i in 0..8 {
        sig_bi[34 + i] = crc8[i];
    }
    zero_bytes(tx.ht_sig(), 6);
    byte_to_bit_inner(tx.ht_sig(), &sig_bi, 6);
}
