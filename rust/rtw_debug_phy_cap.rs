// SPDX-License-Identifier: GPL-2.0
//! W3-210 PHY cap debug dump helpers from `core/rtw_debug_rest.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe
)]

use core::ffi::{c_char, c_void};

type U8 = u8;
type U16 = u16;

const BIT0: U8 = 0x01;
const BIT1: U8 = 0x02;
const BIT2: U8 = 0x04;
const BIT3: U8 = 0x08;
const BIT4: U8 = 0x10;
const BIT5: U8 = 0x20;

#[repr(C)]
struct PhyCapView {
    ht_stbc: U8,
    ht_ldpc: U8,
    ht_bf: U8,
    vht_stbc: U8,
    vht_ldpc: U8,
    vht_bf: U16,
    reg_stbc: U8,
    reg_ldpc: U8,
    reg_bf: U8,
    reg_bfer_rf: U8,
    reg_bfee_rf: U8,
}

extern "C" {
    fn rtw_rust_debug_print_sel(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_debug_phy_cap_fill(adapter: *mut c_void, out: *mut PhyCapView);
    fn rtw_rust_debug_ht_use_default_setting(adapter: *mut c_void);
    fn rtw_rust_debug_vht_use_default_setting(adapter: *mut c_void);
}

fn caps(adapter: *mut c_void) -> PhyCapView {
    let mut view = PhyCapView {
        ht_stbc: 0,
        ht_ldpc: 0,
        ht_bf: 0,
        vht_stbc: 0,
        vht_ldpc: 0,
        vht_bf: 0,
        reg_stbc: 0,
        reg_ldpc: 0,
        reg_bf: 0,
        reg_bfer_rf: 0,
        reg_bfee_rf: 0,
    };
    unsafe { rtw_rust_debug_phy_cap_fill(adapter, &mut view) };
    view
}

fn emit_line(sel: *mut c_void, line: &[u8]) {
    let mut buf = [0i8; 128];
    let n = core::cmp::min(line.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = line[i] as i8;
    }
    unsafe {
        rtw_rust_debug_print_sel(sel, buf.as_ptr());
    }
}

fn push(out: &mut [u8], pos: &mut usize, b: u8) {
    if *pos < out.len() {
        out[*pos] = b;
        *pos += 1;
    }
}

fn push_s(out: &mut [u8], pos: &mut usize, s: &[u8]) {
    for &b in s {
        push(out, pos, b);
    }
}

fn emit_vx(sel: *mut c_void, prefix: &[u8], on: bool, nl2: bool) {
    let mut line = [0u8; 96];
    let mut pos = 0usize;
    push_s(&mut line, &mut pos, prefix);
    push_s(&mut line, &mut pos, if on { b"V" } else { b"X" });
    push_s(&mut line, &mut pos, b"\n");
    if nl2 {
        push(&mut line, &mut pos, b'\n');
    }
    emit_line(sel, &line[..pos]);
}

fn emit_hex_cap(sel: *mut c_void, label: &[u8], val: U8) {
    let mut line = [0u8; 64];
    let mut pos = 0usize;
    push_s(&mut line, &mut pos, label);
    push(&mut line, &mut pos, b"0123456789abcdef"[(val >> 4) as usize]);
    push(&mut line, &mut pos, b"0123456789abcdef"[(val & 0xf) as usize]);
    push_s(&mut line, &mut pos, b"\n");
    emit_line(sel, &line[..pos]);
}

fn emit_dec_line(sel: *mut c_void, prefix: &[u8], val: U8) {
    let mut line = [0u8; 64];
    let mut pos = 0usize;
    push_s(&mut line, &mut pos, prefix);
    if val >= 10 {
        push(&mut line, &mut pos, b'0' + val / 10);
    }
    push(&mut line, &mut pos, b'0' + val % 10);
    push_s(&mut line, &mut pos, b"\n");
    emit_line(sel, &line[..pos]);
}

#[no_mangle]
pub extern "C" fn rtw_dump_dft_phy_cap(sel: *mut c_void, adapter: *mut c_void) {
    let c = caps(adapter);
    #[cfg(any(config_80211ac_vht, host_rtw_debug_phy_cap_test))]
    {
        emit_vx(sel, b"[DFT CAP] VHT STBC Tx : ", c.vht_stbc & BIT1 != 0, false);
        emit_vx(sel, b"[DFT CAP] VHT STBC Rx : ", c.vht_stbc & BIT0 != 0, false);
    }
    emit_vx(sel, b"[DFT CAP] HT STBC Tx : ", c.ht_stbc & BIT1 != 0, false);
    emit_vx(sel, b"[DFT CAP] HT STBC Rx : ", c.ht_stbc & BIT0 != 0, true);
    #[cfg(any(config_80211ac_vht, host_rtw_debug_phy_cap_test))]
    {
        emit_vx(sel, b"[DFT CAP] VHT LDPC Tx : ", c.vht_ldpc & BIT1 != 0, false);
        emit_vx(sel, b"[DFT CAP] VHT LDPC Rx : ", c.vht_ldpc & BIT0 != 0, false);
    }
    emit_vx(sel, b"[DFT CAP] HT LDPC Tx : ", c.ht_ldpc & BIT1 != 0, false);
    emit_vx(sel, b"[DFT CAP] HT LDPC Rx : ", c.ht_ldpc & BIT0 != 0, true);
    #[cfg(any(config_beamforming, host_rtw_debug_phy_cap_test))]
    {
        #[cfg(any(config_80211ac_vht, host_rtw_debug_phy_cap_test))]
        {
            emit_vx(sel, b"[DFT CAP] VHT MU Bfer : ", c.vht_bf & BIT2 as U16 != 0, false);
            emit_vx(sel, b"[DFT CAP] VHT MU Bfee : ", c.vht_bf & BIT3 as U16 != 0, false);
            emit_vx(sel, b"[DFT CAP] VHT SU Bfer : ", c.vht_bf & BIT0 as U16 != 0, false);
            emit_vx(sel, b"[DFT CAP] VHT SU Bfee : ", c.vht_bf & BIT1 as U16 != 0, false);
        }
        emit_vx(sel, b"[DFT CAP] HT Bfer : ", c.ht_bf & BIT0 != 0, false);
        emit_vx(sel, b"[DFT CAP] HT Bfee : ", c.ht_bf & BIT1 != 0, false);
    }
}

#[no_mangle]
pub extern "C" fn rtw_get_dft_phy_cap(sel: *mut c_void, adapter: *mut c_void) {
    emit_line(sel, b"\n ======== PHY CAP protocol ========\n");
    unsafe {
        rtw_rust_debug_ht_use_default_setting(adapter);
        #[cfg(any(config_80211ac_vht, host_rtw_debug_phy_cap_test))]
        rtw_rust_debug_vht_use_default_setting(adapter);
    }
    rtw_dump_dft_phy_cap(sel, adapter);
}

#[no_mangle]
pub extern "C" fn rtw_dump_drv_phy_cap(sel: *mut c_void, adapter: *mut c_void) {
    let c = caps(adapter);
    emit_line(sel, b"\n ======== DRV's configuration ========\n");
    emit_hex_cap(sel, b"[DRV CAP] STBC Capability : 0x", c.reg_stbc);
    emit_vx(sel, b"[DRV CAP] VHT STBC Tx : ", c.reg_stbc & BIT1 != 0, false);
    emit_vx(sel, b"[DRV CAP] VHT STBC Rx : ", c.reg_stbc & BIT0 != 0, false);
    emit_vx(sel, b"[DRV CAP] HT STBC Tx : ", c.reg_stbc & BIT5 != 0, false);
    emit_vx(sel, b"[DRV CAP] HT STBC Rx : ", c.reg_stbc & BIT4 != 0, true);
    emit_hex_cap(sel, b"[DRV CAP] LDPC Capability : 0x", c.reg_ldpc);
    emit_vx(sel, b"[DRV CAP] VHT LDPC Tx : ", c.reg_ldpc & BIT1 != 0, false);
    emit_vx(sel, b"[DRV CAP] VHT LDPC Rx : ", c.reg_ldpc & BIT0 != 0, false);
    emit_vx(sel, b"[DRV CAP] HT LDPC Tx : ", c.reg_ldpc & BIT5 != 0, false);
    emit_vx(sel, b"[DRV CAP] HT LDPC Rx : ", c.reg_ldpc & BIT4 != 0, true);
    #[cfg(any(config_beamforming, host_rtw_debug_phy_cap_test))]
    {
        emit_hex_cap(sel, b"[DRV CAP] TxBF Capability : 0x", c.reg_bf);
        emit_vx(sel, b"[DRV CAP] VHT MU Bfer : ", c.reg_bf & BIT2 != 0, false);
        emit_vx(sel, b"[DRV CAP] VHT MU Bfee : ", c.reg_bf & BIT3 != 0, false);
        emit_vx(sel, b"[DRV CAP] VHT SU Bfer : ", c.reg_bf & BIT0 != 0, false);
        emit_vx(sel, b"[DRV CAP] VHT SU Bfee : ", c.reg_bf & BIT1 != 0, false);
        emit_vx(sel, b"[DRV CAP] HT Bfer : ", c.reg_bf & BIT4 != 0, false);
        emit_vx(sel, b"[DRV CAP] HT Bfee : ", c.reg_bf & BIT5 != 0, false);
        emit_dec_line(sel, b"[DRV CAP] Tx Bfer rf_num : ", c.reg_bfer_rf);
        emit_dec_line(sel, b"[DRV CAP] Tx Bfee rf_num : ", c.reg_bfee_rf);
    }
}
