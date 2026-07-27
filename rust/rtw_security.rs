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

// ----- WEP frame encrypt/decrypt (W3-06) -----

const _WEP40_: U8 = 0x01;
const _WEP104_: U8 = 0x05;

#[cfg(host_security_test)]
const WEP_TXDESC_OFFSET: usize = 48 + 8;

#[cfg(not(host_security_test))]
const WEP_TXDESC_SIZE: usize = 48;
#[cfg(not(host_security_test))]
const WEP_PACKET_OFFSET_SZ: usize = 8;

#[repr(C)]
#[derive(Copy, Clone)]
pub struct KeyType {
    pub skey: [U8; 32],
}

#[repr(C)]
pub struct SecurityPriv {
    pub dot11_auth_algrthm: U32,
    pub dot11_privacy_algrthm: U32,
    pub dot11_privacy_key_index: U32,
    pub dot11_def_key: [KeyType; 6],
    pub dot11_def_keylen: [U32; 6],
}

#[repr(C)]
pub struct PktAttrib {
    pub type_: U8,
    pub subtype: U8,
    pub bswenc: U8,
    pub dhcp_pkt: U8,
    pub ether_type: u16,
    pub seqnum: u16,
    pub hw_ssn_sel: U8,
    pub pkt_hdrlen: u16,
    pub hdrlen: u16,
    pub pktlen: U32,
    pub last_txcmdsz: U32,
    pub nr_frags: U8,
    pub encrypt: U8,
    pub bmc_camid: U8,
    pub iv_len: U8,
    pub icv_len: U8,
    pub iv: [U8; 18],
    pub icv: [U8; 16],
    pub priority: U8,
    pub ack_policy: U8,
    pub mac_id: U8,
    pub vcs_mode: U8,
    pub dst: [U8; 6],
    pub src: [U8; 6],
    pub ta: [U8; 6],
    pub ra: [U8; 6],
}

#[cfg(host_security_test)]
#[repr(C)]
pub struct HostRxPktAttrib {
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
}

#[cfg(host_security_test)]
#[repr(C)]
pub struct HostRecvFrameHdr {
    pub attrib: HostRxPktAttrib,
    pub len: U32,
    pub rx_data: *mut U8,
}

#[cfg(host_security_test)]
#[repr(C)]
pub struct HostRecvFrame {
    pub hdr: HostRecvFrameHdr,
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
pub struct RecvFrameHdr {
    pub attrib: RxPktAttrib,
    pub len: U32,
    pub rx_data: *mut U8,
}

#[cfg(host_security_test)]
#[repr(C)]
pub struct HostPktAttrib {
    pub encrypt: U8,
    pub nr_frags: U8,
    pub _pad0: U8,
    pub hdrlen: u16,
    pub last_txcmdsz: U32,
    pub iv_len: U8,
    pub icv_len: U8,
    pub _pad1: [U8; 2],
    pub ra: [U8; 6],
}

#[cfg(host_security_test)]
#[repr(C)]
pub struct HostXmitFrame {
    pub attrib: HostPktAttrib,
    pub buf_addr: *mut U8,
    pub pkt_offset: i8,
}

#[cfg(host_security_test)]
#[repr(C)]
pub struct HostSecurityPriv {
    pub dot11_privacy_key_index: U32,
    pub dot11_def_key: [KeyType; 6],
    pub dot11_def_keylen: [U32; 6],
}

#[cfg(host_security_test)]
#[repr(C)]
pub struct HostXmitPriv {
    pub frag_len: U32,
}

#[cfg(host_security_test)]
#[repr(C)]
pub struct HostAdapter {
    pub securitypriv: HostSecurityPriv,
    pub xmitpriv: HostXmitPriv,
}

#[cfg(host_security_test)]
pub type WepAdapter = HostAdapter;

#[cfg(not(host_security_test))]
pub type WepAdapter = core::ffi::c_void;

#[cfg(not(host_security_test))]
fn is_multicast_mac_addr(addr: &[U8; 6]) -> bool {
    (addr[0] & 0x01) == 0x01 && addr[0] != 0xff
}

#[cfg(not(host_security_test))]
fn is_broadcast_mac_addr(addr: &[U8; 6]) -> bool {
    addr[0] == 0xff
        && addr[1] == 0xff
        && addr[2] == 0xff
        && addr[3] == 0xff
        && addr[4] == 0xff
        && addr[5] == 0xff
}

#[cfg(not(host_security_test))]
mod kernel_layout {
    use super::*;

    extern "C" {
        static rtw_rust_wep_off_adapter_securitypriv: usize;
        static rtw_rust_wep_off_adapter_xmitpriv: usize;
        static rtw_rust_wep_off_xmitpriv_frag_len: usize;
        static rtw_rust_wep_off_xmit_frame_attrib: usize;
        static rtw_rust_wep_off_xmit_frame_buf_addr: usize;
        static rtw_rust_wep_off_xmit_frame_pkt_offset: usize;
        static rtw_rust_wep_off_recv_frame_hdr: usize;
        static rtw_rust_wep_off_recv_frame_hdr_attrib: usize;
        static rtw_rust_wep_off_recv_frame_hdr_len: usize;
        static rtw_rust_wep_off_recv_frame_hdr_rx_data: usize;
        static rtw_rust_wep_off_securitypriv_wep_sw_enc_cnt_bc: usize;
        static rtw_rust_wep_off_securitypriv_wep_sw_enc_cnt_mc: usize;
        static rtw_rust_wep_off_securitypriv_wep_sw_enc_cnt_uc: usize;
        static rtw_rust_wep_off_securitypriv_wep_sw_dec_cnt_bc: usize;
        static rtw_rust_wep_off_securitypriv_wep_sw_dec_cnt_mc: usize;
        static rtw_rust_wep_off_securitypriv_wep_sw_dec_cnt_uc: usize;
    }

    pub unsafe fn adapter_securitypriv(padapter: *mut WepAdapter) -> *mut SecurityPriv {
        unsafe {
            (padapter as *mut u8)
                .add(rtw_rust_wep_off_adapter_securitypriv)
                as *mut SecurityPriv
        }
    }

    pub unsafe fn adapter_xmitpriv(padapter: *mut WepAdapter) -> *mut u8 {
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

    pub unsafe fn wep_sw_enc_cnt_inc(psecuritypriv: *mut u8, ra: &[U8; 6]) {
        unsafe {
            let off = if is_broadcast_mac_addr(ra) {
                rtw_rust_wep_off_securitypriv_wep_sw_enc_cnt_bc
            } else if is_multicast_mac_addr(ra) {
                rtw_rust_wep_off_securitypriv_wep_sw_enc_cnt_mc
            } else {
                rtw_rust_wep_off_securitypriv_wep_sw_enc_cnt_uc
            };
            let cnt = (psecuritypriv.add(off)) as *mut u64;
            *cnt = cnt.read().wrapping_add(1);
        }
    }

    pub unsafe fn wep_sw_dec_cnt_inc(psecuritypriv: *mut u8, ra: &[U8; 6]) {
        unsafe {
            let off = if is_broadcast_mac_addr(ra) {
                rtw_rust_wep_off_securitypriv_wep_sw_dec_cnt_bc
            } else if is_multicast_mac_addr(ra) {
                rtw_rust_wep_off_securitypriv_wep_sw_dec_cnt_mc
            } else {
                rtw_rust_wep_off_securitypriv_wep_sw_dec_cnt_uc
            };
            let cnt = (psecuritypriv.add(off)) as *mut u64;
            *cnt = cnt.read().wrapping_add(1);
        }
    }
}

fn cpu_to_le32(v: U32) -> U32 {
    v.to_le()
}

fn le32_to_cpu(v: U32) -> U32 {
    U32::from_le(v)
}

fn rnd4(ptr: usize) -> usize {
    ((ptr >> 2) + if ptr & 3 == 0 { 0 } else { 1 }) << 2
}

unsafe fn rtw_memcpy(dest: *mut u8, src: *const u8, len: usize) {
    unsafe {
        core::ptr::copy_nonoverlapping(src, dest, len);
    }
}

#[cfg(host_security_test)]
fn wep_hw_hdr_offset(pkt_offset: i8) -> usize {
    WEP_TXDESC_OFFSET + (pkt_offset as usize) * 8
}

#[cfg(not(host_security_test))]
unsafe fn wep_hw_hdr_offset(pxmitframe: *mut u8) -> usize {
    unsafe {
        WEP_TXDESC_SIZE
            + (kernel_layout::xmit_frame_pkt_offset(pxmitframe) as usize) * WEP_PACKET_OFFSET_SZ
    }
}

unsafe fn wep_encrypt_inner(
    psecuritypriv: *mut SecurityPriv,
    pxmitpriv_frag_len: U32,
    pattrib: *mut PktAttrib,
    buf_addr: *mut U8,
    hw_hdr_offset: usize,
) {
    unsafe {
    if buf_addr.is_null() {
        return;
    }

    let pattrib = &mut *pattrib;
    let psecuritypriv = &mut *psecuritypriv;

    if pattrib.encrypt != _WEP40_ && pattrib.encrypt != _WEP104_ {
        return;
    }

    let keylength =
        psecuritypriv.dot11_def_keylen[psecuritypriv.dot11_privacy_key_index as usize];
    let mut pframe = buf_addr.add(hw_hdr_offset);
    let mut curfragnum: i32 = 0;

    while curfragnum < pattrib.nr_frags as i32 {
        let iv = pframe.add(pattrib.hdrlen as usize);
        let mut wepkey = [0u8; 16];
        rtw_memcpy(wepkey.as_mut_ptr(), iv, 3);
        rtw_memcpy(
            wepkey.as_mut_ptr().add(3),
            psecuritypriv.dot11_def_key[psecuritypriv.dot11_privacy_key_index as usize]
                .skey
                .as_ptr(),
            keylength as usize,
        );
        let payload = pframe
            .add(pattrib.iv_len as usize)
            .add(pattrib.hdrlen as usize);

        let length = if (curfragnum + 1) == pattrib.nr_frags as i32 {
            pattrib.last_txcmdsz as i32
                - pattrib.hdrlen as i32
                - pattrib.iv_len as i32
                - pattrib.icv_len as i32
        } else {
            pxmitpriv_frag_len as i32
                - pattrib.hdrlen as i32
                - pattrib.iv_len as i32
                - pattrib.icv_len as i32
        };

        let mut crc = [0u8; 4];
        let crc_val = cpu_to_le32(getcrc32(payload, length));
        core::ptr::copy_nonoverlapping(crc_val.to_ne_bytes().as_ptr(), crc.as_mut_ptr(), 4);

        let mut mycontext = arc4context {
            x: 0,
            y: 0,
            state: [0; 256],
        };
        arcfour_init(&mut mycontext, wepkey.as_mut_ptr(), 3 + keylength);
        arcfour_encrypt(&mut mycontext, payload, payload, length as U32);
        arcfour_encrypt(
            &mut mycontext,
            payload.add(length as usize),
            crc.as_mut_ptr(),
            4,
        );

        if (curfragnum + 1) != pattrib.nr_frags as i32 {
            pframe = pframe.add(pxmitpriv_frag_len as usize);
            pframe = rnd4(pframe as usize) as *mut U8;
        }

        curfragnum += 1;
    }
    }
}

#[cfg(host_security_test)]
#[no_mangle]
pub extern "C" fn rtw_wep_encrypt(padapter: *mut HostAdapter, pxmitframe: *mut U8) {
    if padapter.is_null() || pxmitframe.is_null() {
        return;
    }
    unsafe {
    let xmit = &*(pxmitframe as *const HostXmitFrame);
    if xmit.buf_addr.is_null() {
        return;
    }
    let hw = wep_hw_hdr_offset(xmit.pkt_offset);
    let sec = &(*padapter).securitypriv;
    let mut sec_mirror = SecurityPriv {
        dot11_auth_algrthm: 0,
        dot11_privacy_algrthm: 0,
        dot11_privacy_key_index: sec.dot11_privacy_key_index,
        dot11_def_key: sec.dot11_def_key,
        dot11_def_keylen: sec.dot11_def_keylen,
    };
    let mut attrib_mirror = PktAttrib {
        type_: 0,
        subtype: 0,
        bswenc: 0,
        dhcp_pkt: 0,
        ether_type: 0,
        seqnum: 0,
        hw_ssn_sel: 0,
        pkt_hdrlen: 0,
        hdrlen: xmit.attrib.hdrlen,
        pktlen: 0,
        last_txcmdsz: xmit.attrib.last_txcmdsz,
        nr_frags: xmit.attrib.nr_frags,
        encrypt: xmit.attrib.encrypt,
        bmc_camid: 0,
        iv_len: xmit.attrib.iv_len,
        icv_len: xmit.attrib.icv_len,
        iv: [0; 18],
        icv: [0; 16],
        priority: 0,
        ack_policy: 0,
        mac_id: 0,
        vcs_mode: 0,
        dst: [0; 6],
        src: [0; 6],
        ta: [0; 6],
        ra: xmit.attrib.ra,
    };
        wep_encrypt_inner(
            &mut sec_mirror,
            (*padapter).xmitpriv.frag_len,
            &mut attrib_mirror,
            xmit.buf_addr,
            hw,
        );
    }
}

#[cfg(not(host_security_test))]
#[no_mangle]
pub extern "C" fn rtw_wep_encrypt(padapter: *mut WepAdapter, pxmitframe: *mut U8) {
    if padapter.is_null() || pxmitframe.is_null() {
        return;
    }
    unsafe {
        let px = pxmitframe;
        let buf_addr = kernel_layout::xmit_frame_buf_addr(px);
        if buf_addr.is_null() {
            return;
        }
        let hw = wep_hw_hdr_offset(px);
        let psecuritypriv = kernel_layout::adapter_securitypriv(padapter);
        let pxmitpriv = kernel_layout::adapter_xmitpriv(padapter);
        let frag_len = kernel_layout::xmitpriv_frag_len(pxmitpriv);
        let pattrib = kernel_layout::xmit_frame_attrib(px);
        wep_encrypt_inner(psecuritypriv, frag_len, pattrib, buf_addr, hw);
        let encrypt = (*pattrib).encrypt;
        if encrypt == _WEP40_ || encrypt == _WEP104_ {
            kernel_layout::wep_sw_enc_cnt_inc(psecuritypriv as *mut u8, &(*pattrib).ra);
        }
    }
}

unsafe fn wep_decrypt_inner(
    psecuritypriv: *mut SecurityPriv,
    encrypt: U8,
    hdrlen: U8,
    iv_len: U8,
    key_index: U8,
    pframe: *mut U8,
    frame_len: U32,
) {
    unsafe {
    let psecuritypriv = &*psecuritypriv;

    if encrypt != _WEP40_ && encrypt != _WEP104_ {
        return;
    }

    let iv = pframe.add(hdrlen as usize);
    let keylength = psecuritypriv.dot11_def_keylen[key_index as usize];
    let mut wepkey = [0u8; 16];
    rtw_memcpy(wepkey.as_mut_ptr(), iv, 3);
    rtw_memcpy(
        wepkey.as_mut_ptr().add(3),
        psecuritypriv.dot11_def_key[key_index as usize]
            .skey
            .as_ptr(),
        keylength as usize,
    );
    let length = frame_len as i32 - hdrlen as i32 - iv_len as i32;
    let payload = pframe.add(iv_len as usize).add(hdrlen as usize);

    let mut mycontext = arc4context {
        x: 0,
        y: 0,
        state: [0; 256],
    };
    arcfour_init(&mut mycontext, wepkey.as_mut_ptr(), 3 + keylength);
    arcfour_encrypt(&mut mycontext, payload, payload, length as U32);

    let mut crc = [0u8; 4];
    let crc_val = le32_to_cpu(getcrc32(payload, length - 4));
    core::ptr::copy_nonoverlapping(crc_val.to_ne_bytes().as_ptr(), crc.as_mut_ptr(), 4);
    }
}

#[cfg(host_security_test)]
#[no_mangle]
pub extern "C" fn rtw_wep_decrypt(padapter: *mut HostAdapter, precvframe: *mut U8) {
    if padapter.is_null() || precvframe.is_null() {
        return;
    }
    unsafe {
    let recv = &*(precvframe as *const HostRecvFrame);
    let sec = &(*padapter).securitypriv;
    let mut sec_mirror = SecurityPriv {
        dot11_auth_algrthm: 0,
        dot11_privacy_algrthm: 0,
        dot11_privacy_key_index: sec.dot11_privacy_key_index,
        dot11_def_key: sec.dot11_def_key,
        dot11_def_keylen: sec.dot11_def_keylen,
    };
        wep_decrypt_inner(
            &mut sec_mirror,
            recv.hdr.attrib.encrypt,
            recv.hdr.attrib.hdrlen,
            recv.hdr.attrib.iv_len,
            recv.hdr.attrib.key_index,
            recv.hdr.rx_data,
            recv.hdr.len,
        );
    }
}

#[cfg(not(host_security_test))]
#[no_mangle]
pub extern "C" fn rtw_wep_decrypt(padapter: *mut WepAdapter, precvframe: *mut U8) {
    if padapter.is_null() || precvframe.is_null() {
        return;
    }
    unsafe {
        let attrib = kernel_layout::recv_frame_attrib(precvframe);
        let psecuritypriv = kernel_layout::adapter_securitypriv(padapter);
        let encrypt = (*attrib).encrypt;
        let ra = (*attrib).ra;
        wep_decrypt_inner(
            psecuritypriv,
            encrypt,
            (*attrib).hdrlen,
            (*attrib).iv_len,
            (*attrib).key_index,
            kernel_layout::recv_frame_rx_data(precvframe),
            kernel_layout::recv_frame_len(precvframe),
        );
        if encrypt == _WEP40_ || encrypt == _WEP104_ {
            kernel_layout::wep_sw_dec_cnt_inc(psecuritypriv as *mut u8, &ra);
        }
    }
}

// ----- TKIP MIC helpers (W3-07a) -----

#[repr(C)]
pub struct mic_data {
    pub K0: U32,
    pub K1: U32,
    pub L: U32,
    pub R: U32,
    pub M: U32,
    pub nBytesInM: U32,
}

fn rol32(a: U32, n: u32) -> U32 {
    a.rotate_left(n & 31)
}

fn ror32(a: U32, n: u32) -> U32 {
    rol32(a, 32 - n)
}

fn secmicgetuint32(p: &[U8]) -> U32 {
    let mut res = 0u32;
    for (i, &byte) in p.iter().take(4).enumerate() {
        res |= u32::from(byte) << (8 * i);
    }
    res
}

fn secmicputuint32(p: &mut [U8], val: U32) {
    let mut v = val;
    for slot in p.iter_mut().take(4) {
        *slot = (v & 0xff) as U8;
        v >>= 8;
    }
}

fn secmicclear(pmicdata: &mut mic_data) {
    pmicdata.L = pmicdata.K0;
    pmicdata.R = pmicdata.K1;
    pmicdata.nBytesInM = 0;
    pmicdata.M = 0;
}

#[no_mangle]
pub extern "C" fn rtw_secmicsetkey(pmicdata: *mut mic_data, key: *mut U8) {
    if pmicdata.is_null() || key.is_null() {
        return;
    }
    unsafe {
        let pmicdata = &mut *pmicdata;
        let key = core::slice::from_raw_parts(key, 8);
        pmicdata.K0 = secmicgetuint32(key);
        pmicdata.K1 = secmicgetuint32(&key[4..]);
        secmicclear(pmicdata);
    }
}

fn secmicappendbyte_inner(pmicdata: &mut mic_data, b: U8) {
    pmicdata.M |= u32::from(b) << (8 * pmicdata.nBytesInM);
    pmicdata.nBytesInM += 1;
    if pmicdata.nBytesInM >= 4 {
        pmicdata.L ^= pmicdata.M;
        pmicdata.R ^= rol32(pmicdata.L, 17);
        pmicdata.L = pmicdata.L.wrapping_add(pmicdata.R);
        pmicdata.R ^= ((pmicdata.L & 0xff00_ff00) >> 8) | ((pmicdata.L & 0x00ff_00ff) << 8);
        pmicdata.L = pmicdata.L.wrapping_add(pmicdata.R);
        pmicdata.R ^= rol32(pmicdata.L, 3);
        pmicdata.L = pmicdata.L.wrapping_add(pmicdata.R);
        pmicdata.R ^= ror32(pmicdata.L, 2);
        pmicdata.L = pmicdata.L.wrapping_add(pmicdata.R);
        pmicdata.M = 0;
        pmicdata.nBytesInM = 0;
    }
}

#[no_mangle]
pub extern "C" fn rtw_secmicappendbyte(pmicdata: *mut mic_data, b: U8) {
    if pmicdata.is_null() {
        return;
    }
    unsafe {
        secmicappendbyte_inner(&mut *pmicdata, b);
    }
}

#[no_mangle]
pub extern "C" fn rtw_secmicappend(pmicdata: *mut mic_data, src: *mut U8, nbytes: U32) {
    if pmicdata.is_null() || src.is_null() || nbytes == 0 {
        return;
    }
    unsafe {
        let src = core::slice::from_raw_parts(src, nbytes as usize);
        let pmicdata = &mut *pmicdata;
        for &byte in src {
            secmicappendbyte_inner(pmicdata, byte);
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_secgetmic(pmicdata: *mut mic_data, dst: *mut U8) {
    if pmicdata.is_null() || dst.is_null() {
        return;
    }
    unsafe {
        let pmicdata = &mut *pmicdata;
        secmicappendbyte_inner(pmicdata, 0x5a);
        secmicappendbyte_inner(pmicdata, 0);
        secmicappendbyte_inner(pmicdata, 0);
        secmicappendbyte_inner(pmicdata, 0);
        secmicappendbyte_inner(pmicdata, 0);
        while pmicdata.nBytesInM != 0 {
            secmicappendbyte_inner(pmicdata, 0);
        }
        let dst = core::slice::from_raw_parts_mut(dst, 8);
        secmicputuint32(&mut dst[..4], pmicdata.L);
        secmicputuint32(&mut dst[4..], pmicdata.R);
        secmicclear(pmicdata);
    }
}

#[no_mangle]
pub extern "C" fn rtw_seccalctkipmic(
    key: *mut U8,
    header: *mut U8,
    data: *mut U8,
    data_len: U32,
    mic_code: *mut U8,
    pri: U8,
) {
    if key.is_null() || header.is_null() || data.is_null() || mic_code.is_null() {
        return;
    }
    unsafe {
        let header_ptr = header;
        let mut micdata = mic_data {
            K0: 0,
            K1: 0,
            L: 0,
            R: 0,
            M: 0,
            nBytesInM: 0,
        };
        let mut priority = [0u8; 4];
        rtw_secmicsetkey(&mut micdata, key);
        priority[0] = pri;

        if *header_ptr.add(1) & 1 != 0 {
            rtw_secmicappend(&mut micdata, header_ptr.add(16), 6);
            if *header_ptr.add(1) & 2 != 0 {
                rtw_secmicappend(&mut micdata, header_ptr.add(24), 6);
            } else {
                rtw_secmicappend(&mut micdata, header_ptr.add(10), 6);
            }
        } else {
            rtw_secmicappend(&mut micdata, header_ptr.add(4), 6);
            if *header_ptr.add(1) & 2 != 0 {
                rtw_secmicappend(&mut micdata, header_ptr.add(16), 6);
            } else {
                rtw_secmicappend(&mut micdata, header_ptr.add(10), 6);
            }
        }
        rtw_secmicappend(&mut micdata, priority.as_mut_ptr(), 4);
        rtw_secmicappend(&mut micdata, data, data_len);
        rtw_secgetmic(&mut micdata, mic_code);
    }
}

#[cfg(host_security_test)]
#[no_mangle]
pub extern "C" fn host_tkip_secmicsetkey(pmicdata: *mut mic_data, key: *mut U8) {
    rtw_secmicsetkey(pmicdata, key);
}

#[cfg(host_security_test)]
#[no_mangle]
pub extern "C" fn host_tkip_secmicappendbyte(pmicdata: *mut mic_data, b: U8) {
    rtw_secmicappendbyte(pmicdata, b);
}

#[cfg(host_security_test)]
#[no_mangle]
pub extern "C" fn host_tkip_secmicappend(pmicdata: *mut mic_data, src: *mut U8, nbytes: U32) {
    rtw_secmicappend(pmicdata, src, nbytes);
}

#[cfg(host_security_test)]
#[no_mangle]
pub extern "C" fn host_tkip_secgetmic(pmicdata: *mut mic_data, dst: *mut U8) {
    rtw_secgetmic(pmicdata, dst);
}

#[cfg(host_security_test)]
#[no_mangle]
pub extern "C" fn host_tkip_seccalctkipmic(
    key: *mut U8,
    header: *mut U8,
    data: *mut U8,
    data_len: U32,
    mic_code: *mut U8,
    pri: U8,
) {
    rtw_seccalctkipmic(key, header, data, data_len, mic_code, pri);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn probe_constant() {
        assert_eq!(rtw_rust_security_probe(), 0x1e04);
    }
}
