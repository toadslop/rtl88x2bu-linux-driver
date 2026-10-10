// SPDX-License-Identifier: GPL-2.0
//! W3-134 tx/rx debug dump leaves from `core/rtw_debug_rest.c`.

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

use core::ffi::{c_char, c_int, c_void};

type U8 = u8;
type U16 = u16;
type U32 = u32;
type U64 = u64;

const RX_AMPDU_SIZE_INVALID: U8 = 0xff;
const CHANNEL_WIDTH_20: U8 = 0;
const CHANNEL_WIDTH_40: U8 = 1;
const CHANNEL_WIDTH_160: U8 = 3;
const PROTO_CAP_11AC: U8 = 8;

const RATE_BMP_CCK: U16 = 0x000f;
const RATE_BMP_OFDM: U16 = 0x00f0;
const RATE_BMP_HT_1SS: U32 = 0x000000ff;
const RATE_BMP_HT_2SS: U32 = 0x0000ff00;
const RATE_BMP_HT_3SS: U32 = 0x00ff0000;
const RATE_BMP_HT_4SS: U32 = 0xff000000;
const RATE_BMP_VHT_1SS: U64 = 0x00000003ff;
const RATE_BMP_VHT_2SS: U64 = 0x00000ffc00;
const RATE_BMP_VHT_3SS: U64 = 0x003ff00000;
const RATE_BMP_VHT_4SS: U64 = 0xffc0000000;

extern "C" {
    fn rtw_rust_debug_print_sel(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_debug_log_info(line: *const c_char);
    fn rtw_rust_debug_recv_sink_udpport(adapter: *mut c_void) -> U16;
    fn rtw_rust_debug_recv_pre_rtp_rxseq(adapter: *mut c_void) -> U16;
    fn rtw_rust_debug_recv_cur_rtp_rxseq(adapter: *mut c_void) -> U16;
    fn rtw_rust_debug_recv_set_pre_rtp_rxseq(adapter: *mut c_void, seq: U16);
    fn rtw_rust_debug_recv_set_cur_rtp_rxseq(adapter: *mut c_void, seq: U16);
    fn rtw_rust_debug_sta_reorder_get(
        sta: *mut c_void,
        tid: c_int,
        enable: *mut U8,
        ampdu_size: *mut U8,
        indicate_seq: *mut U16,
    );
    fn rtw_rust_debug_dvobj_primary_adapter(dvobj: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_dvobj_rfctl(dvobj: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_hal_chk_proto_cap(adapter: *mut c_void, cap: U8) -> U8;
    fn rtw_rust_debug_hal_is_bw_support(adapter: *mut c_void, bw: U8) -> U8;
    fn rtw_rust_debug_ch_width_str(bw: U8) -> *const c_char;
    fn rtw_rust_debug_rfctl_rate_bmp_ht(rfctl: *mut c_void, bw: U8) -> U32;
    fn rtw_rust_debug_rfctl_rate_bmp_vht(rfctl: *mut c_void, bw: U8) -> U64;
    fn rtw_rust_debug_rfctl_rate_bmp_cck_ofdm(rfctl: *mut c_void) -> U16;
}

fn c_str_bytes(ptr: *const c_char) -> &'static [u8] {
    if ptr.is_null() {
        return b"";
    }
    unsafe {
        let mut len = 0usize;
        while *ptr.add(len) != 0 {
            len += 1;
        }
        core::slice::from_raw_parts(ptr as *const u8, len)
    }
}

fn print_bytes(sel: *mut c_void, bytes: &[u8]) {
    let mut buf = [0i8; 256];
    let n = core::cmp::min(bytes.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = bytes[i] as i8;
    }
    unsafe {
        rtw_rust_debug_print_sel(sel, buf.as_ptr());
    }
}

fn print_line(sel: *mut c_void, line: &[u8]) {
    let mut with_nl = [0u8; 260];
    let n = core::cmp::min(line.len(), with_nl.len().saturating_sub(2));
    with_nl[..n].copy_from_slice(&line[..n]);
    with_nl[n] = b'\n';
    print_bytes(sel, &with_nl[..n + 1]);
}

fn push_byte(out: &mut [u8], pos: &mut usize, b: U8) {
    if *pos < out.len() {
        out[*pos] = b;
        *pos += 1;
    }
}

fn push_str(out: &mut [u8], pos: &mut usize, s: &[u8]) {
    for &b in s {
        push_byte(out, pos, b);
    }
}

fn push_u16_hex3(out: &mut [u8], pos: &mut usize, v: U16) {
    for shift in [8i32, 4, 0] {
        let nib = ((v >> shift) & 0xf) as U8;
        push_byte(
            out,
            pos,
            if nib < 10 {
                b'0' + nib
            } else {
                b'a' + (nib - 10)
            },
        );
    }
}

fn push_u16_hex2(out: &mut [u8], pos: &mut usize, v: U16) {
    for shift in [4i32, 0] {
        let nib = ((v >> shift) & 0xf) as U8;
        push_byte(
            out,
            pos,
            if nib < 10 {
                b'0' + nib
            } else {
                b'a' + (nib - 10)
            },
        );
    }
}

fn push_u16_dec(out: &mut [u8], pos: &mut usize, v: U16) {
    let mut tmp = [0u8; 5];
    let mut n = 0usize;
    let mut x = v;
    if x == 0 {
        push_byte(out, pos, b'0');
        return;
    }
    while x > 0 && n < tmp.len() {
        tmp[n] = b'0' + (x % 10) as U8;
        x /= 10;
        n += 1;
    }
    while n > 0 {
        n -= 1;
        push_byte(out, pos, tmp[n]);
    }
}

#[no_mangle]
pub extern "C" fn rtw_sink_rtp_seq_dbg(adapter: *mut c_void, ehdr_pos: *mut U8) {
    if adapter.is_null() || ehdr_pos.is_null() {
        return;
    }
    let sink_port = unsafe { rtw_rust_debug_recv_sink_udpport(adapter) };
    if sink_port == 0 {
        return;
    }
    let pkt_port = unsafe { core::ptr::read_unaligned(ehdr_pos.add(0x24) as *const U16) };
    if pkt_port != sink_port.to_be() {
        return;
    }
    let pre = unsafe { rtw_rust_debug_recv_cur_rtp_rxseq(adapter) };
    let cur = unsafe { U16::from_be(core::ptr::read_unaligned(ehdr_pos.add(0x2c) as *const U16)) };
    unsafe {
        rtw_rust_debug_recv_set_pre_rtp_rxseq(adapter, pre);
        rtw_rust_debug_recv_set_cur_rtp_rxseq(adapter, cur);
    }
    if pre + 1 == cur {
        return;
    }
    if pre == 65535 && cur == 0 {
        return;
    }
    let mut line = [0u8; 96];
    let mut pos = 0usize;
    push_str(
        &mut line,
        &mut pos,
        b"rtw_sink_rtp_seq_dbg : RTP Seq num from ",
    );
    push_u16_dec(&mut line, &mut pos, pre);
    push_str(&mut line, &mut pos, b" to ");
    push_u16_dec(&mut line, &mut pos, cur);
    push_byte(&mut line, &mut pos, b'\n');
    line[pos] = 0;
    unsafe {
        rtw_rust_debug_log_info(line.as_ptr() as *const c_char);
    }
}

#[no_mangle]
pub extern "C" fn sta_rx_reorder_ctl_dump(sel: *mut c_void, sta: *mut c_void) {
    if sta.is_null() {
        return;
    }
    for i in 0..16 {
        let mut enable = 0u8;
        let mut ampdu_size = 0u8;
        let mut indicate_seq = 0u16;
        unsafe {
            rtw_rust_debug_sta_reorder_get(sta, i, &mut enable, &mut ampdu_size, &mut indicate_seq);
        }
        if ampdu_size == RX_AMPDU_SIZE_INVALID && indicate_seq == 0xffff {
            continue;
        }
        let mut line = [0u8; 80];
        let mut pos = 0usize;
        push_str(&mut line, &mut pos, b"tid=");
        push_u16_dec(&mut line, &mut pos, i as U16);
        push_str(&mut line, &mut pos, b", enable=");
        push_byte(&mut line, &mut pos, b'0' + enable);
        push_str(&mut line, &mut pos, b", ampdu_size=");
        push_u16_dec(&mut line, &mut pos, ampdu_size as U16);
        push_str(&mut line, &mut pos, b", indicate_seq=");
        push_u16_dec(&mut line, &mut pos, indicate_seq);
        print_line(sel, &line[..pos]);
    }
}

#[no_mangle]
pub extern "C" fn dump_tx_rate_bmp(sel: *mut c_void, dvobj: *mut c_void) {
    if dvobj.is_null() {
        return;
    }
    let adapter = unsafe { rtw_rust_debug_dvobj_primary_adapter(dvobj) };
    let rfctl = unsafe { rtw_rust_debug_dvobj_rfctl(dvobj) };
    if adapter.is_null() || rfctl.is_null() {
        return;
    }
    let vht_cap =
        unsafe { rtw_rust_debug_hal_chk_proto_cap(adapter, PROTO_CAP_11AC) != 0 };
    let mut hdr = [0u8; 64];
    let mut pos = 0usize;
    push_str(&mut hdr, &mut pos, b"bw    ");
    if vht_cap {
        push_str(&mut hdr, &mut pos, b" vht           ");
    }
    push_str(&mut hdr, &mut pos, b" ht          ofdm cck");
    print_line(sel, &hdr[..pos]);

    let cck_ofdm = unsafe { rtw_rust_debug_rfctl_rate_bmp_cck_ofdm(rfctl) };
    let mut bw = CHANNEL_WIDTH_20;
    while bw <= CHANNEL_WIDTH_160 {
        if unsafe { rtw_rust_debug_hal_is_bw_support(adapter, bw) == 0 } {
            bw += 1;
            continue;
        }
        let bw_label = c_str_bytes(unsafe { rtw_rust_debug_ch_width_str(bw) });
        let mut line = [0u8; 128];
        pos = 0usize;
        push_str(&mut line, &mut pos, bw_label);
        while pos < 6 {
            push_byte(&mut line, &mut pos, b' ');
        }
        if vht_cap {
            let bmp_vht = unsafe { rtw_rust_debug_rfctl_rate_bmp_vht(rfctl, bw) };
            push_byte(&mut line, &mut pos, b' ');
            push_u16_hex3(&mut line, &mut pos, ((bmp_vht & RATE_BMP_VHT_4SS) >> 30) as U16);
            push_byte(&mut line, &mut pos, b' ');
            push_u16_hex3(&mut line, &mut pos, ((bmp_vht & RATE_BMP_VHT_3SS) >> 20) as U16);
            push_byte(&mut line, &mut pos, b' ');
            push_u16_hex3(&mut line, &mut pos, ((bmp_vht & RATE_BMP_VHT_2SS) >> 10) as U16);
            push_byte(&mut line, &mut pos, b' ');
            push_u16_hex3(&mut line, &mut pos, (bmp_vht & RATE_BMP_VHT_1SS) as U16);
        }
        let bmp_ht = unsafe { rtw_rust_debug_rfctl_rate_bmp_ht(rfctl, bw) };
        let ht4 = if bw <= CHANNEL_WIDTH_40 {
            ((bmp_ht & RATE_BMP_HT_4SS) >> 24) as U16
        } else {
            0
        };
        let ht3 = if bw <= CHANNEL_WIDTH_40 {
            ((bmp_ht & RATE_BMP_HT_3SS) >> 16) as U16
        } else {
            0
        };
        let ht2 = if bw <= CHANNEL_WIDTH_40 {
            ((bmp_ht & RATE_BMP_HT_2SS) >> 8) as U16
        } else {
            0
        };
        let ht1 = if bw <= CHANNEL_WIDTH_40 {
            (bmp_ht & RATE_BMP_HT_1SS) as U16
        } else {
            0
        };
        push_byte(&mut line, &mut pos, b' ');
        push_u16_hex2(&mut line, &mut pos, ht4);
        push_byte(&mut line, &mut pos, b' ');
        push_u16_hex2(&mut line, &mut pos, ht3);
        push_byte(&mut line, &mut pos, b' ');
        push_u16_hex2(&mut line, &mut pos, ht2);
        push_byte(&mut line, &mut pos, b' ');
        push_u16_hex2(&mut line, &mut pos, ht1);
        let ofdm = if bw <= CHANNEL_WIDTH_20 {
            (cck_ofdm & RATE_BMP_OFDM) >> 4
        } else {
            0
        };
        let cck = if bw <= CHANNEL_WIDTH_20 {
            cck_ofdm & RATE_BMP_CCK
        } else {
            0
        };
        push_str(&mut line, &mut pos, b"  ");
        push_u16_hex3(&mut line, &mut pos, ofdm);
        push_str(&mut line, &mut pos, b"   ");
        push_u16_hex2(&mut line, &mut pos, cck);
        print_line(sel, &line[..pos]);
        bw += 1;
    }
}
