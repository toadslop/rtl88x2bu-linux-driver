// SPDX-License-Identifier: GPL-2.0
//! Shared GCMP helpers: bindings, alloc shims, and AAD/nonce construction (W2-02d).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_crypto_test)]
pub mod bindings {
    use std::os::raw::{c_int, c_void};

    pub const AES_BLOCK_SIZE: u32 = 16;

    #[repr(C, packed)]
    pub struct Ieee80211Hdr {
        pub frame_control: u16,
        pub duration_id: u16,
        pub addr1: [u8; 6],
        pub addr2: [u8; 6],
        pub addr3: [u8; 6],
        pub seq_ctrl: u16,
        pub addr4: [u8; 6],
    }

    pub type Adapter = c_void;

    unsafe extern "C" {
        pub fn aes_gcm_ae(
            key: *const u8,
            key_len: usize,
            iv: *const u8,
            iv_len: usize,
            plain: *const u8,
            plain_len: usize,
            aad: *const u8,
            aad_len: usize,
            crypt: *mut u8,
            tag: *mut u8,
        ) -> c_int;
        pub fn aes_gcm_ad(
            key: *const u8,
            key_len: usize,
            iv: *const u8,
            iv_len: usize,
            crypt: *const u8,
            crypt_len: usize,
            aad: *const u8,
            aad_len: usize,
            tag: *const u8,
            plain: *mut u8,
        ) -> c_int;
        pub fn os_malloc(sz: usize) -> *mut c_void;
        pub fn rtw_mfree(ptr: *mut c_void, sz: usize);
        pub fn rtw_registrypriv_amsdu_mode(padapter: *const Adapter) -> u8;
    }
}

#[cfg(not(host_crypto_test))]
pub mod bindings {
    include!("bindings/generated.rs");

    #[repr(C, packed)]
    pub struct Ieee80211Hdr {
        pub frame_control: u16,
        pub duration_id: u16,
        pub addr1: [u8; 6],
        pub addr2: [u8; 6],
        pub addr3: [u8; 6],
        pub seq_ctrl: u16,
        pub addr4: [u8; 6],
    }

    pub type Adapter = core::ffi::c_void;

    unsafe extern "C" {
        pub fn aes_gcm_ae(
            key: *const u8,
            key_len: usize,
            iv: *const u8,
            iv_len: usize,
            plain: *const u8,
            plain_len: usize,
            aad: *const u8,
            aad_len: usize,
            crypt: *mut u8,
            tag: *mut u8,
        ) -> core::ffi::c_int;
        pub fn aes_gcm_ad(
            key: *const u8,
            key_len: usize,
            iv: *const u8,
            iv_len: usize,
            crypt: *const u8,
            crypt_len: usize,
            aad: *const u8,
            aad_len: usize,
            tag: *const u8,
            plain: *mut u8,
        ) -> core::ffi::c_int;
        pub fn _rtw_malloc(sz: u32) -> *mut core::ffi::c_void;
        pub fn _rtw_mfree(ptr: *mut core::ffi::c_void, sz: u32);
        pub fn rtw_registrypriv_amsdu_mode(padapter: *const Adapter) -> u8;
    }
}

pub use self::bindings::{Ieee80211Hdr, AES_BLOCK_SIZE};

#[cfg(host_crypto_test)]
pub fn os_malloc(sz: usize) -> *mut core::ffi::c_void {
    unsafe { bindings::os_malloc(sz) }
}

#[cfg(not(host_crypto_test))]
pub fn os_malloc(sz: usize) -> *mut core::ffi::c_void {
    unsafe { bindings::_rtw_malloc(sz as u32) }
}

pub fn rtw_mfree(ptr: *mut core::ffi::c_void, sz: usize) {
    #[cfg(host_crypto_test)]
    unsafe {
        bindings::rtw_mfree(ptr, sz);
    }
    #[cfg(not(host_crypto_test))]
    unsafe {
        bindings::_rtw_mfree(ptr, sz as u32);
    }
}

const ETH_ALEN: usize = 6;
const WLAN_FC_TODS: u16 = 0x0100;
const WLAN_FC_FROMDS: u16 = 0x0200;
const WLAN_FC_RETRY: u16 = 0x0800;
const WLAN_FC_PWRMGT: u16 = 0x1000;
const WLAN_FC_MOREDATA: u16 = 0x2000;
const WLAN_FC_ORDER: u16 = 0x8000;
const WLAN_FC_TYPE_DATA: u16 = 0x0008;
const WLAN_FC_STYPE_QOS_DATA: u16 = 0x0080;
const RTW_AMSDU_MODE_SPP: u8 = 1;

#[cfg(target_endian = "little")]
fn le_to_host16(v: u16) -> u16 {
    v
}

#[cfg(target_endian = "big")]
fn le_to_host16(v: u16) -> u16 {
    v.swap_bytes()
}

fn wpa_put_le16(out: &mut [u8], val: u16) {
    out[1] = (val >> 8) as u8;
    out[0] = (val & 0xff) as u8;
}

fn get_addr1_ptr(hdr: *const u8) -> *const u8 {
    unsafe { hdr.add(4) }
}

fn wlan_fc_get_stype(fc: u16) -> u16 {
    fc & 0x00f0
}

fn wlan_fc_get_type(fc: u16) -> u16 {
    fc & 0x000c
}

/// Build GCMP AAD and nonce from an 802.11 header and PN prefix.
///
/// `data` must contain at least 8 bytes (the packet number). Returns `0` when
/// `data` is too short; otherwise returns the AAD length in bytes.
pub fn gcmp_aad_nonce(
    amsdu_mode: u8,
    hdr: &Ieee80211Hdr,
    data: &[u8],
    aad: &mut [u8; 30],
    nonce: &mut [u8; 12],
) -> usize {
    if data.len() < 8 {
        debug_assert!(
            false,
            "gcmp_aad_nonce: data must contain at least 8 PN bytes"
        );
        return 0;
    }

    let mut fc = le_to_host16(hdr.frame_control);
    let stype = wlan_fc_get_stype(fc);
    let mut addr4 = false;
    let mut qos = false;

    if (fc & (WLAN_FC_TODS | WLAN_FC_FROMDS)) == (WLAN_FC_TODS | WLAN_FC_FROMDS) {
        addr4 = true;
    }

    if wlan_fc_get_type(fc) == WLAN_FC_TYPE_DATA {
        fc &= !0x0070;
        if (stype & WLAN_FC_STYPE_QOS_DATA) != 0 {
            qos = true;
            fc &= !WLAN_FC_ORDER;
        }
    }

    fc &= !(WLAN_FC_RETRY | WLAN_FC_PWRMGT | WLAN_FC_MOREDATA);
    wpa_put_le16(&mut aad[..2], fc);
    let mut pos = 2usize;

    unsafe {
        core::ptr::copy_nonoverlapping(
            get_addr1_ptr(hdr as *const _ as *const u8),
            aad.as_mut_ptr().add(pos),
            3 * ETH_ALEN,
        );
    }
    pos += 3 * ETH_ALEN;

    let mut seq = le_to_host16(hdr.seq_ctrl);
    seq &= !0xfff0;
    wpa_put_le16(&mut aad[pos..pos + 2], seq);
    pos += 2;

    let hdr_bytes = hdr as *const _ as *const u8;
    let copy_len = (if addr4 { ETH_ALEN } else { 0 }) + if qos { 2 } else { 0 };
    if copy_len > 0 {
        unsafe {
            core::ptr::copy_nonoverlapping(hdr_bytes.add(24), aad.as_mut_ptr().add(pos), copy_len);
        }
        pos += if addr4 { ETH_ALEN } else { 0 };
        if qos {
            aad[pos] &= !0x70;
            if amsdu_mode != RTW_AMSDU_MODE_SPP {
                aad[pos] &= !0x80;
            }
            pos += 1;
            aad[pos] = 0x00;
            pos += 1;
        }
    }

    nonce[..6].copy_from_slice(&hdr.addr2);
    nonce[6] = data[7];
    nonce[7] = data[6];
    nonce[8] = data[5];
    nonce[9] = data[4];
    nonce[10] = data[1];
    nonce[11] = data[0];

    pos
}
