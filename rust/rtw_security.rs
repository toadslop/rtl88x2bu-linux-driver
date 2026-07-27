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
const _TKIP_: U8 = 0x02;

const _SUCCESS: U32 = 0;
const _FAIL: U32 = 1;
const _FALSE: U8 = 0;

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
    pub ta: [U8; 6],
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
    pub ta: [U8; 6],
    pub dot118021x_UncstKey: KeyType,
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
    pub dot118021XGrpKeyid: U32,
    pub dot118021XGrpKey: [KeyType; 4],
    pub binstall_grpkey: U8,
}

#[cfg(host_security_test)]
#[repr(C)]
pub struct HostXmitPriv {
    pub frag_len: U32,
}

#[cfg(host_security_test)]
#[repr(C)]
pub struct HostStaInfo {
    pub used: U8,
    pub ta: [U8; 6],
    pub dot118021x_UncstKey: KeyType,
}

#[cfg(host_security_test)]
#[repr(C)]
pub struct HostStapriv {
    pub stas: [HostStaInfo; 4],
}

#[cfg(host_security_test)]
#[repr(C)]
pub struct HostAdapter {
    pub securitypriv: HostSecurityPriv,
    pub xmitpriv: HostXmitPriv,
    pub stapriv: HostStapriv,
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
        static rtw_rust_tkip_off_securitypriv_dot118021XGrpKeyid: usize;
        static rtw_rust_tkip_off_securitypriv_dot118021XGrpKey: usize;
        static rtw_rust_tkip_off_pkt_attrib_dot118021x_UncstKey: usize;
        static rtw_rust_tkip_off_securitypriv_tkip_sw_enc_cnt_bc: usize;
        static rtw_rust_tkip_off_securitypriv_tkip_sw_enc_cnt_mc: usize;
        static rtw_rust_tkip_off_securitypriv_tkip_sw_enc_cnt_uc: usize;
        static rtw_rust_tkip_off_securitypriv_binstallGrpkey: usize;
        static rtw_rust_tkip_off_securitypriv_tkip_sw_dec_cnt_bc: usize;
        static rtw_rust_tkip_off_securitypriv_tkip_sw_dec_cnt_mc: usize;
        static rtw_rust_tkip_off_securitypriv_tkip_sw_dec_cnt_uc: usize;
        static rtw_rust_tkip_off_adapter_stapriv: usize;
        static rtw_rust_tkip_off_sta_info_dot118021x_UncstKey: usize;
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

    pub unsafe fn tkip_sw_enc_cnt_inc(psecuritypriv: *mut u8, ra: &[U8; 6]) {
        unsafe {
            let off = if is_broadcast_mac_addr(ra) {
                rtw_rust_tkip_off_securitypriv_tkip_sw_enc_cnt_bc
            } else if is_multicast_mac_addr(ra) {
                rtw_rust_tkip_off_securitypriv_tkip_sw_enc_cnt_mc
            } else {
                rtw_rust_tkip_off_securitypriv_tkip_sw_enc_cnt_uc
            };
            let cnt = (psecuritypriv.add(off)) as *mut u64;
            *cnt = cnt.read().wrapping_add(1);
        }
    }

    pub unsafe fn securitypriv_binstall_grpkey(psecuritypriv: *mut u8) -> U8 {
        unsafe {
            *((psecuritypriv.add(rtw_rust_tkip_off_securitypriv_binstallGrpkey)) as *const U8)
        }
    }

    pub unsafe fn adapter_stapriv(padapter: *mut WepAdapter) -> *mut u8 {
        unsafe { (padapter as *mut u8).add(rtw_rust_tkip_off_adapter_stapriv) }
    }

    pub unsafe fn sta_info_unicast_key_skey(stainfo: *mut u8) -> *mut U8 {
        unsafe {
            (stainfo.add(rtw_rust_tkip_off_sta_info_dot118021x_UncstKey)) as *mut U8
        }
    }

    pub unsafe fn tkip_sw_dec_cnt_inc(psecuritypriv: *mut u8, ra: &[U8; 6]) {
        unsafe {
            let off = if is_broadcast_mac_addr(ra) {
                rtw_rust_tkip_off_securitypriv_tkip_sw_dec_cnt_bc
            } else if is_multicast_mac_addr(ra) {
                rtw_rust_tkip_off_securitypriv_tkip_sw_dec_cnt_mc
            } else {
                rtw_rust_tkip_off_securitypriv_tkip_sw_dec_cnt_uc
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

// ----- TKIP frame encrypt (W3-10) -----

fn is_mcast_ra(ra: &[U8; 6]) -> bool {
    (ra[0] & 0x01) != 0
}

unsafe fn get_tkip_pn(iv: *const U8) -> (U16, U32) {
    unsafe {
        let iv = core::slice::from_raw_parts(iv, 8);
        let pn_val = (iv[2] as u64)
            | ((iv[0] as u64) << 8)
            | ((iv[4] as u64) << 16)
            | ((iv[5] as u64) << 24)
            | ((iv[6] as u64) << 32)
            | ((iv[7] as u64) << 40);
        let pnl = pn_val as U16;
        let pnh = (pn_val >> 16) as U32;
        (pnl, pnh)
    }
}

unsafe fn tkip_encrypt_inner(
    prwskey: *const U8,
    ta: &[U8; 6],
    pxmitpriv_frag_len: U32,
    pattrib: &PktAttrib,
    buf_addr: *mut U8,
    hw_hdr_offset: usize,
) -> U32 {
    unsafe {
        if buf_addr.is_null() || prwskey.is_null() {
            return _FAIL;
        }

        if pattrib.encrypt != _TKIP_ {
            return _SUCCESS;
        }

        let mut tk = [0u8; 16];
        rtw_memcpy(tk.as_mut_ptr(), prwskey, 16);

        let mut pframe = buf_addr.add(hw_hdr_offset);
        let mut curfragnum: i32 = 0;

        while curfragnum < pattrib.nr_frags as i32 {
            let iv = pframe.add(pattrib.hdrlen as usize);
            let payload = pframe
                .add(pattrib.iv_len as usize)
                .add(pattrib.hdrlen as usize);

            let (pnl, pnh) = get_tkip_pn(iv);
            let mut p1k = [0u16; 5];
            let mut rc4key = [0u8; 16];
            phase1_inner(&mut p1k, &tk, ta, pnh);
            phase2_inner(&mut rc4key, &tk, &p1k, pnl);

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
            arcfour_init(&mut mycontext, rc4key.as_mut_ptr(), 16);
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

        _SUCCESS
    }
}

#[cfg(host_security_test)]
#[no_mangle]
pub extern "C" fn rtw_tkip_encrypt(padapter: *mut HostAdapter, pxmitframe: *mut U8) -> U32 {
    if padapter.is_null() || pxmitframe.is_null() {
        return _FAIL;
    }
    unsafe {
        let xmit = &*(pxmitframe as *const HostXmitFrame);
        if xmit.buf_addr.is_null() {
            return _FAIL;
        }
        let sec = &(*padapter).securitypriv;
        let hw = wep_hw_hdr_offset(xmit.pkt_offset);
        let prwskey = if is_mcast_ra(&xmit.attrib.ra) {
            sec.dot118021XGrpKey[sec.dot118021XGrpKeyid as usize]
                .skey
                .as_ptr()
        } else {
            xmit.attrib.dot118021x_UncstKey.skey.as_ptr()
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
            ta: xmit.attrib.ta,
            ra: xmit.attrib.ra,
        };
        tkip_encrypt_inner(
            prwskey,
            &xmit.attrib.ta,
            (*padapter).xmitpriv.frag_len,
            &mut attrib_mirror,
            xmit.buf_addr,
            hw,
        )
    }
}

#[cfg(not(host_security_test))]
#[no_mangle]
pub extern "C" fn rtw_tkip_encrypt(padapter: *mut WepAdapter, pxmitframe: *mut U8) -> U32 {
    if padapter.is_null() || pxmitframe.is_null() {
        return _FAIL;
    }
    unsafe {
        let px = pxmitframe;
        let buf_addr = kernel_layout::xmit_frame_buf_addr(px);
        if buf_addr.is_null() {
            return _FAIL;
        }
        let hw = wep_hw_hdr_offset(px);
        let psecuritypriv = kernel_layout::adapter_securitypriv(padapter);
        let sec_base = psecuritypriv as *mut u8;
        let pxmitpriv = kernel_layout::adapter_xmitpriv(padapter);
        let frag_len = kernel_layout::xmitpriv_frag_len(pxmitpriv);
        let pattrib = kernel_layout::xmit_frame_attrib(px);
        let encrypt = (*pattrib).encrypt;
        if encrypt != _TKIP_ {
            return _SUCCESS;
        }
        let ra = (*pattrib).ra;
        let prwskey = if is_mcast_ra(&ra) {
            let kid = kernel_layout::securitypriv_grp_keyid(sec_base) as usize;
            kernel_layout::securitypriv_grp_key_skey(sec_base, kid)
        } else {
            kernel_layout::pkt_attrib_unicast_key_skey(pattrib)
        };
        let res = tkip_encrypt_inner(
            prwskey,
            &(*pattrib).ta,
            frag_len,
            &*pattrib,
            buf_addr,
            hw,
        );
        if res == _SUCCESS {
            kernel_layout::tkip_sw_enc_cnt_inc(sec_base, &ra);
        }
        res
    }
}

#[cfg(host_security_test)]
unsafe fn host_get_stainfo<'a>(stapriv: &'a HostStapriv, ta: &[U8; 6]) -> Option<&'a HostStaInfo> {
    for sta in &stapriv.stas {
        if sta.used != 0 && sta.ta == *ta {
            return Some(sta);
        }
    }
    None
}

#[cfg(not(host_security_test))]
mod tkip_decrypt_ffi {
    use super::*;

    extern "C" {
        fn rtw_get_stainfo(stapriv: *mut core::ffi::c_void, hwaddr: *const U8) -> *mut core::ffi::c_void;
    }

    pub unsafe fn lookup(stapriv: *mut u8, ta: &[U8; 6]) -> *mut u8 {
        unsafe { rtw_get_stainfo(stapriv as *mut core::ffi::c_void, ta.as_ptr()) as *mut u8 }
    }
}

unsafe fn tkip_decrypt_inner(
    prwskey: *const U8,
    ta: &[U8; 6],
    encrypt: U8,
    hdrlen: U8,
    iv_len: U8,
    pframe: *mut U8,
    frame_len: U32,
) -> U32 {
    unsafe {
        if encrypt != _TKIP_ {
            return _SUCCESS;
        }
        if prwskey.is_null() {
            return _FAIL;
        }

        let mut tk = [0u8; 16];
        rtw_memcpy(tk.as_mut_ptr(), prwskey, 16);

        let iv = pframe.add(hdrlen as usize);
        let payload = pframe.add(iv_len as usize).add(hdrlen as usize);
        let length = frame_len as i32 - hdrlen as i32 - iv_len as i32;

        let (pnl, pnh) = get_tkip_pn(iv);
        let mut p1k = [0u16; 5];
        let mut rc4key = [0u8; 16];
        phase1_inner(&mut p1k, &tk, ta, pnh);
        phase2_inner(&mut rc4key, &tk, &p1k, pnl);

        let mut mycontext = arc4context {
            x: 0,
            y: 0,
            state: [0; 256],
        };
        arcfour_init(&mut mycontext, rc4key.as_mut_ptr(), 16);
        arcfour_encrypt(&mut mycontext, payload, payload, length as U32);

        let mut crc = [0u8; 4];
        let crc_val = le32_to_cpu(getcrc32(payload, length - 4));
        core::ptr::copy_nonoverlapping(crc_val.to_ne_bytes().as_ptr(), crc.as_mut_ptr(), 4);

        if crc[3] != *payload.add((length - 1) as usize)
            || crc[2] != *payload.add((length - 2) as usize)
            || crc[1] != *payload.add((length - 3) as usize)
            || crc[0] != *payload.add((length - 4) as usize)
        {
            return _FAIL;
        }

        _SUCCESS
    }
}

#[cfg(host_security_test)]
#[no_mangle]
pub extern "C" fn rtw_tkip_decrypt(padapter: *mut HostAdapter, precvframe: *mut U8) -> U32 {
    if padapter.is_null() || precvframe.is_null() {
        return _FAIL;
    }
    unsafe {
        let recv = &*(precvframe as *const HostRecvFrame);
        let adapter = &*padapter;
        let stainfo = match host_get_stainfo(&adapter.stapriv, &recv.hdr.attrib.ta) {
            Some(sta) => sta,
            None => return _FAIL,
        };
        let prwskey = if is_mcast_ra(&recv.hdr.attrib.ra) {
            if adapter.securitypriv.binstall_grpkey == _FALSE {
                return _FAIL;
            }
            adapter.securitypriv.dot118021XGrpKey[recv.hdr.attrib.key_index as usize]
                .skey
                .as_ptr()
        } else {
            stainfo.dot118021x_UncstKey.skey.as_ptr()
        };
        tkip_decrypt_inner(
            prwskey,
            &recv.hdr.attrib.ta,
            recv.hdr.attrib.encrypt,
            recv.hdr.attrib.hdrlen,
            recv.hdr.attrib.iv_len,
            recv.hdr.rx_data,
            recv.hdr.len,
        )
    }
}

#[cfg(not(host_security_test))]
#[no_mangle]
pub extern "C" fn rtw_tkip_decrypt(padapter: *mut WepAdapter, precvframe: *mut U8) -> U32 {
    if padapter.is_null() || precvframe.is_null() {
        return _FAIL;
    }
    unsafe {
        let psecuritypriv = kernel_layout::adapter_securitypriv(padapter);
        let sec_base = psecuritypriv as *mut u8;
        let attrib = kernel_layout::recv_frame_attrib(precvframe);
        let encrypt = (*attrib).encrypt;
        if encrypt != _TKIP_ {
            return _SUCCESS;
        }
        let ra = (*attrib).ra;
        let ta = (*attrib).ta;
        let stapriv = kernel_layout::adapter_stapriv(padapter);
        let stainfo = tkip_decrypt_ffi::lookup(stapriv, &ta);
        if stainfo.is_null() {
            return _FAIL;
        }
        let stainfo_key = kernel_layout::sta_info_unicast_key_skey(stainfo);
        let prwskey = if is_mcast_ra(&ra) {
            if kernel_layout::securitypriv_binstall_grpkey(sec_base) == _FALSE {
                return _FAIL;
            }
            kernel_layout::securitypriv_grp_key_skey(sec_base, (*attrib).key_index as usize)
        } else {
            stainfo_key
        };
        let res = tkip_decrypt_inner(
            prwskey,
            &ta,
            encrypt,
            (*attrib).hdrlen,
            (*attrib).iv_len,
            kernel_layout::recv_frame_rx_data(precvframe),
            kernel_layout::recv_frame_len(precvframe),
        );
        if res == _SUCCESS {
            kernel_layout::tkip_sw_dec_cnt_inc(sec_base, &ra);
        }
        res
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

// ----- TKIP phase1/phase2 (W3-07b) -----

type U16 = u16;

const PHASE1_LOOP_CNT: Sint = 8;

static SBOX1: [[U16; 256]; 2] = [
    [
        0xC6A5, 0xF884, 0xEE99, 0xF68D, 0xFF0D, 0xD6BD, 0xDEB1, 0x9154, 0x6050, 0x0203,
        0xCEA9, 0x567D, 0xE719, 0xB562, 0x4DE6, 0xEC9A, 0x8F45, 0x1F9D, 0x8940, 0xFA87,
        0xEF15, 0xB2EB, 0x8EC9, 0xFB0B, 0x41EC, 0xB367, 0x5FFD, 0x45EA, 0x23BF, 0x53F7,
        0xE496, 0x9B5B, 0x75C2, 0xE11C, 0x3DAE, 0x4C6A, 0x6C5A, 0x7E41, 0xF502, 0x834F,
        0x685C, 0x51F4, 0xD134, 0xF908, 0xE293, 0xAB73, 0x6253, 0x2A3F, 0x080C, 0x9552,
        0x4665, 0x9D5E, 0x3028, 0x37A1, 0x0A0F, 0x2FB5, 0x0E09, 0x2436, 0x1B9B, 0xDF3D,
        0xCD26, 0x4E69, 0x7FCD, 0xEA9F, 0x121B, 0x1D9E, 0x5874, 0x342E, 0x362D, 0xDCB2,
        0xB4EE, 0x5BFB, 0xA4F6, 0x764D, 0xB761, 0x7DCE, 0x527B, 0xDD3E, 0x5E71, 0x1397,
        0xA6F5, 0xB968, 0x0000, 0xC12C, 0x4060, 0xE31F, 0x79C8, 0xB6ED, 0xD4BE, 0x8D46,
        0x67D9, 0x724B, 0x94DE, 0x98D4, 0xB0E8, 0x854A, 0xBB6B, 0xC52A, 0x4FE5, 0xED16,
        0x86C5, 0x9AD7, 0x6655, 0x1194, 0x8ACF, 0xE910, 0x0406, 0xFE81, 0xA0F0, 0x7844,
        0x25BA, 0x4BE3, 0xA2F3, 0x5DFE, 0x80C0, 0x058A, 0x3FAD, 0x21BC, 0x7048, 0xF104,
        0x63DF, 0x77C1, 0xAF75, 0x4263, 0x2030, 0xE51A, 0xFD0E, 0xBF6D, 0x814C, 0x1814,
        0x2635, 0xC32F, 0xBEE1, 0x35A2, 0x88CC, 0x2E39, 0x9357, 0x55F2, 0xFC82, 0x7A47,
        0xC8AC, 0xBAE7, 0x322B, 0xE695, 0xC0A0, 0x1998, 0x9ED1, 0xA37F, 0x4466, 0x547E,
        0x3BAB, 0x0B83, 0x8CCA, 0xC729, 0x6BD3, 0x283C, 0xA779, 0xBCE2, 0x161D, 0xAD76,
        0xDB3B, 0x6456, 0x744E, 0x141E, 0x92DB, 0x0C0A, 0x486C, 0xB8E4, 0x9F5D, 0xBD6E,
        0x43EF, 0xC4A6, 0x39A8, 0x31A4, 0xD337, 0xF28B, 0xD532, 0x8B43, 0x6E59, 0xDAB7,
        0x018C, 0xB164, 0x9CD2, 0x49E0, 0xD8B4, 0xACFA, 0xF307, 0xCF25, 0xCAAF, 0xF48E,
        0x47E9, 0x1018, 0x6FD5, 0xF088, 0x4A6F, 0x5C72, 0x3824, 0x57F1, 0x73C7, 0x9751,
        0xCB23, 0xA17C, 0xE89C, 0x3E21, 0x96DD, 0x61DC, 0x0D86, 0x0F85, 0xE090, 0x7C42,
        0x71C4, 0xCCAA, 0x90D8, 0x0605, 0xF701, 0x1C12, 0xC2A3, 0x6A5F, 0xAEF9, 0x69D0,
        0x1791, 0x9958, 0x3A27, 0x27B9, 0xD938, 0xEB13, 0x2BB3, 0x2233, 0xD2BB, 0xA970,
        0x0789, 0x33A7, 0x2DB6, 0x3C22, 0x1592, 0xC920, 0x8749, 0xAAFF, 0x5078, 0xA57A,
        0x038F, 0x59F8, 0x0980, 0x1A17, 0x65DA, 0xD731, 0x84C6, 0xD0B8, 0x82C3, 0x29B0,
        0x5A77, 0x1E11, 0x7BCB, 0xA8FC, 0x6DD6, 0x2C3A,
    ],
    [
        0xA5C6, 0x84F8, 0x99EE, 0x8DF6, 0x0DFF, 0xBDD6, 0xB1DE, 0x5491, 0x5060, 0x0302,
        0xA9CE, 0x7D56, 0x19E7, 0x62B5, 0xE64D, 0x9AEC, 0x458F, 0x9D1F, 0x4089, 0x87FA,
        0x15EF, 0xEBB2, 0xC98E, 0x0BFB, 0xEC41, 0x67B3, 0xFD5F, 0xEA45, 0xBF23, 0xF753,
        0x96E4, 0x5B9B, 0xC275, 0x1CE1, 0xAE3D, 0x6A4C, 0x5A6C, 0x417E, 0x02F5, 0x4F83,
        0x5C68, 0xF451, 0x34D1, 0x08F9, 0x93E2, 0x73AB, 0x5362, 0x3F2A, 0x0C08, 0x5295,
        0x6546, 0x5E9D, 0x2830, 0xA137, 0x0F0A, 0xB52F, 0x090E, 0x3624, 0x9B1B, 0x3DDF,
        0x26CD, 0x694E, 0xCD7F, 0x9FEA, 0x1B12, 0x9E1D, 0x7458, 0x2E34, 0x2D36, 0xB2DC,
        0xEEB4, 0xFB5B, 0xF6A4, 0x4D76, 0x61B7, 0xCE7D, 0x7B52, 0x3EDD, 0x715E, 0x9713,
        0xF5A6, 0x68B9, 0x0000, 0x2CC1, 0x6040, 0x1FE3, 0xC879, 0xEDB6, 0xBED4, 0x468D,
        0xD967, 0x4B72, 0xDE94, 0xD498, 0xE8B0, 0x4A85, 0x6BBB, 0x2AC5, 0xE54F, 0x16ED,
        0xC586, 0xD79A, 0x5566, 0x9411, 0xCF8A, 0x10E9, 0x0604, 0x81FE, 0xF0A0, 0x4478,
        0xBA25, 0xE34B, 0xF3A2, 0xFE5D, 0xC080, 0x8A05, 0xAD3F, 0xBC21, 0x4870, 0x04F1,
        0xDF63, 0xC177, 0x75AF, 0x6342, 0x3020, 0x1AE5, 0x0EFD, 0x6DBF, 0x4C81, 0x1418,
        0x3526, 0x2FC3, 0xE1BE, 0xA235, 0xCC88, 0x392E, 0x5793, 0xF255, 0x82FC, 0x477A,
        0xACC8, 0xE7BA, 0x2B32, 0x95E6, 0xA0C0, 0x9819, 0xD19E, 0x7FA3, 0x6644, 0x7E54,
        0xAB3B, 0x830B, 0xCA8C, 0x29C7, 0xD36B, 0x3C28, 0x79A7, 0xE2BC, 0x1D16, 0x76AD,
        0x3BDB, 0x5664, 0x4E74, 0x1E14, 0xDB92, 0x0A0C, 0x6C48, 0xE4B8, 0x5D9F, 0x6EBD,
        0xEF43, 0xA6C4, 0xA839, 0xA431, 0x37D3, 0x8BF2, 0x32D5, 0x438B, 0x596E, 0xB7DA,
        0x8C01, 0x64B1, 0xD29C, 0xE049, 0xB4D8, 0xFAAC, 0x07F3, 0x25CF, 0xAFCA, 0x8EF4,
        0xE947, 0x1810, 0xD56F, 0x88F0, 0x6F4A, 0x725C, 0x2438, 0xF157, 0xC773, 0x5197,
        0x23CB, 0x7CA1, 0x9CE8, 0x213E, 0xDD96, 0xDC61, 0x860D, 0x850F, 0x90E0, 0x427C,
        0xC471, 0xAACC, 0xD890, 0x0506, 0x01F7, 0x121C, 0xA3C2, 0x5F6A, 0xF9AE, 0xD069,
        0x9117, 0x5899, 0x273A, 0xB927, 0x38D9, 0x13EB, 0xB32B, 0x3322, 0xBBD2, 0x70A9,
        0x8907, 0xA733, 0xB62D, 0x223C, 0x9215, 0x20C9, 0x4987, 0xFFAA, 0x7850, 0x7AA5,
        0x8F03, 0xF859, 0x8009, 0x171A, 0xDA65, 0x31D7, 0xC684, 0xB8D0, 0xC382, 0xB029,
        0x775A, 0x111E, 0xCB7B, 0xFCA8, 0xD66D, 0x3A2C,
    ],
];

fn rot_r1(v16: U16) -> U16 {
    ((v16 >> 1) & 0x7FFF) ^ ((v16 & 1) << 15)
}

fn lo8(v16: U16) -> U8 {
    (v16 & 0x00FF) as U8
}

fn hi8(v16: U16) -> U8 {
    ((v16 >> 8) & 0x00FF) as U8
}

fn lo16(v32: U32) -> U16 {
    (v32 & 0xFFFF) as U16
}

fn hi16(v32: U32) -> U16 {
    ((v32 >> 16) & 0xFFFF) as U16
}

fn mk16(hi: U8, lo: U8) -> U16 {
    (lo as U16) ^ ((hi as U16) << 8)
}

fn tk16(tk: &[U8; 16], n: usize) -> U16 {
    mk16(tk[2 * n + 1], tk[2 * n])
}

fn s_lookup(v16: U16) -> U16 {
    SBOX1[0][lo8(v16) as usize] ^ SBOX1[1][hi8(v16) as usize]
}

fn phase1_inner(p1k: &mut [U16; 5], tk: &[U8; 16], ta: &[U8; 6], iv32: U32) {
    p1k[0] = lo16(iv32);
    p1k[1] = hi16(iv32);
    p1k[2] = mk16(ta[1], ta[0]);
    p1k[3] = mk16(ta[3], ta[2]);
    p1k[4] = mk16(ta[5], ta[4]);

    for i in 0..PHASE1_LOOP_CNT {
        let i = i as usize;
        p1k[0] = p1k[0].wrapping_add(s_lookup(p1k[4] ^ tk16(tk, (i & 1) + 0)));
        p1k[1] = p1k[1].wrapping_add(s_lookup(p1k[0] ^ tk16(tk, (i & 1) + 2)));
        p1k[2] = p1k[2].wrapping_add(s_lookup(p1k[1] ^ tk16(tk, (i & 1) + 4)));
        p1k[3] = p1k[3].wrapping_add(s_lookup(p1k[2] ^ tk16(tk, (i & 1) + 6)));
        p1k[4] = p1k[4].wrapping_add(s_lookup(p1k[3] ^ tk16(tk, (i & 1) + 0)));
        p1k[4] = p1k[4].wrapping_add(i as U16);
    }
}

fn phase2_inner(rc4key: &mut [U8; 16], tk: &[U8; 16], p1k: &[U16; 5], iv16: U16) {
    let mut ppk = [0u16; 6];
    ppk[..5].copy_from_slice(p1k);
    ppk[5] = p1k[4].wrapping_add(iv16);

    ppk[0] = ppk[0].wrapping_add(s_lookup(ppk[5] ^ tk16(tk, 0)));
    ppk[1] = ppk[1].wrapping_add(s_lookup(ppk[0] ^ tk16(tk, 1)));
    ppk[2] = ppk[2].wrapping_add(s_lookup(ppk[1] ^ tk16(tk, 2)));
    ppk[3] = ppk[3].wrapping_add(s_lookup(ppk[2] ^ tk16(tk, 3)));
    ppk[4] = ppk[4].wrapping_add(s_lookup(ppk[3] ^ tk16(tk, 4)));
    ppk[5] = ppk[5].wrapping_add(s_lookup(ppk[4] ^ tk16(tk, 5)));

    ppk[0] = ppk[0].wrapping_add(rot_r1(ppk[5] ^ tk16(tk, 6)));
    ppk[1] = ppk[1].wrapping_add(rot_r1(ppk[0] ^ tk16(tk, 7)));
    ppk[2] = ppk[2].wrapping_add(rot_r1(ppk[1]));
    ppk[3] = ppk[3].wrapping_add(rot_r1(ppk[2]));
    ppk[4] = ppk[4].wrapping_add(rot_r1(ppk[3]));
    ppk[5] = ppk[5].wrapping_add(rot_r1(ppk[4]));

    rc4key[0] = hi8(iv16);
    rc4key[1] = (hi8(iv16) | 0x20) & 0x7F;
    rc4key[2] = lo8(iv16);
    rc4key[3] = lo8((ppk[5] ^ tk16(tk, 0)) >> 1);

    for i in 0..6 {
        rc4key[4 + 2 * i] = lo8(ppk[i]);
        rc4key[5 + 2 * i] = hi8(ppk[i]);
    }
}

#[no_mangle]
pub extern "C" fn phase1(p1k: *mut U16, tk: *const U8, ta: *const U8, iv32: U32) {
    if p1k.is_null() || tk.is_null() || ta.is_null() {
        return;
    }
    unsafe {
        let mut tk_fixed = [0u8; 16];
        let mut ta_fixed = [0u8; 6];
        let mut p1k_fixed = [0u16; 5];
        tk_fixed.copy_from_slice(core::slice::from_raw_parts(tk, 16));
        ta_fixed.copy_from_slice(core::slice::from_raw_parts(ta, 6));
        phase1_inner(&mut p1k_fixed, &tk_fixed, &ta_fixed, iv32);
        core::ptr::copy_nonoverlapping(p1k_fixed.as_ptr(), p1k, 5);
    }
}

#[no_mangle]
pub extern "C" fn phase2(rc4key: *mut U8, tk: *const U8, p1k: *const U16, iv16: U16) {
    if rc4key.is_null() || tk.is_null() || p1k.is_null() {
        return;
    }
    unsafe {
        let mut tk_fixed = [0u8; 16];
        let mut p1k_fixed = [0u16; 5];
        let mut rc4key_fixed = [0u8; 16];
        tk_fixed.copy_from_slice(core::slice::from_raw_parts(tk, 16));
        p1k_fixed.copy_from_slice(core::slice::from_raw_parts(p1k, 5));
        phase2_inner(&mut rc4key_fixed, &tk_fixed, &p1k_fixed, iv16);
        core::ptr::copy_nonoverlapping(rc4key_fixed.as_ptr(), rc4key, 16);
    }
}

#[cfg(host_security_test)]
#[no_mangle]
pub extern "C" fn host_tkip_phase1(p1k: *mut U16, tk: *const U8, ta: *const U8, iv32: U32) {
    phase1(p1k, tk, ta, iv32);
}

#[cfg(host_security_test)]
#[no_mangle]
pub extern "C" fn host_tkip_phase2(
    rc4key: *mut U8,
    tk: *const U8,
    p1k: *const U16,
    iv16: U16,
) {
    phase2(rc4key, tk, p1k, iv16);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn probe_constant() {
        assert_eq!(rtw_rust_security_probe(), 0x1e04);
    }
}
