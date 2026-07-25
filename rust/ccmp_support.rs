// SPDX-License-Identifier: GPL-2.0
//! Shared CCMP helpers: bindings, alloc shims, and AAD/nonce construction (W2-09).

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

    extern "C" {
        pub fn aes_ccm_ae(
            key: *const u8,
            key_len: usize,
            nonce: *const u8,
            m: usize,
            plain: *const u8,
            plain_len: usize,
            aad: *const u8,
            aad_len: usize,
            crypt: *mut u8,
            auth: *mut u8,
        ) -> c_int;
        pub fn aes_ccm_ad(
            key: *const u8,
            key_len: usize,
            nonce: *const u8,
            m: usize,
            crypt: *const u8,
            crypt_len: usize,
            aad: *const u8,
            aad_len: usize,
            auth: *const u8,
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

    extern "C" {
        pub fn aes_ccm_ae(
            key: *const u8,
            key_len: usize,
            nonce: *const u8,
            m: usize,
            plain: *const u8,
            plain_len: usize,
            aad: *const u8,
            aad_len: usize,
            crypt: *mut u8,
            auth: *mut u8,
        ) -> core::ffi::c_int;
        pub fn aes_ccm_ad(
            key: *const u8,
            key_len: usize,
            nonce: *const u8,
            m: usize,
            crypt: *const u8,
            crypt_len: usize,
            aad: *const u8,
            aad_len: usize,
            auth: *const u8,
            plain: *mut u8,
        ) -> core::ffi::c_int;
        pub fn _rtw_malloc(sz: u32) -> *mut core::ffi::c_void;
        pub fn _rtw_mfree(ptr: *mut core::ffi::c_void, sz: u32);
        pub fn rtw_registrypriv_amsdu_mode(padapter: *const Adapter) -> u8;
    }
}

pub use self::bindings::{AES_BLOCK_SIZE, Ieee80211Hdr};

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
const WLAN_FC_ISWEP: u16 = 0x4000;
const WLAN_FC_TYPE_DATA: u16 = 0x0008;
const WLAN_FC_TYPE_MGMT: u16 = 0x0000;
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

/// Build CCMP AAD and nonce from an 802.11 header and CCMP header prefix.
pub fn ccmp_aad_nonce(
    amsdu_mode: u8,
    hdr: &Ieee80211Hdr,
    data: &[u8],
    aad: &mut [u8; 30],
    nonce: &mut [u8; 13],
) -> usize {
    if data.len() < 8 {
        debug_assert!(false, "ccmp_aad_nonce: data must contain at least 8 PN bytes");
        return 0;
    }

    nonce[0] = 0;

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
            let hdr_bytes = hdr as *const _ as *const u8;
            let mut qc = unsafe { hdr_bytes.add(24) };
            if addr4 {
                qc = unsafe { qc.add(ETH_ALEN) };
            }
            nonce[0] = unsafe { *qc } & 0x0f;
        }
    } else if wlan_fc_get_type(fc) == WLAN_FC_TYPE_MGMT {
        nonce[0] |= 0x10;
    }

    fc &= !(WLAN_FC_RETRY | WLAN_FC_PWRMGT | WLAN_FC_MOREDATA);
    fc |= WLAN_FC_ISWEP;
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

    nonce[1..1 + ETH_ALEN].copy_from_slice(&hdr.addr2);
    nonce[7] = data[7];
    nonce[8] = data[6];
    nonce[9] = data[5];
    nonce[10] = data[4];
    nonce[11] = data[1];
    nonce[12] = data[0];

    pos
}

/// Build CCMP PV1 AAD and nonce.
pub fn ccmp_aad_nonce_pv1(
    hdr: &[u8],
    a1: &[u8; 6],
    a2: &[u8; 6],
    a3: Option<&[u8; 6]>,
    pn: &[u8; 6],
    aad: &mut [u8; 24],
    nonce: &mut [u8; 13],
) -> usize {
    nonce[0] = 1 << 5;

    let fc = (hdr[0] as u16) | ((hdr[1] as u16) << 8);
    let fc_type = (fc & (1 << 2 | 1 << 3 | 1 << 4)) >> 2;

    if fc_type == 1 {
        nonce[0] |= 0x10;
    }

    let mut fc_masked = fc;
    fc_masked &= !(1 << 10 | 1 << 11 | 1 << 13 | 1 << 14 | 1 << 15);
    fc_masked |= 1 << 12;
    wpa_put_le16(&mut aad[..2], fc_masked);
    let mut pos = 2usize;

    if fc_type == 0 || fc_type == 3 {
        aad[pos..pos + ETH_ALEN].copy_from_slice(a1);
        pos += ETH_ALEN;
        aad[pos..pos + ETH_ALEN].copy_from_slice(a2);
        pos += ETH_ALEN;

        let sc = if fc_type == 0 {
            &hdr[2 + 2 + ETH_ALEN..]
        } else {
            &hdr[2 + 2 * ETH_ALEN..]
        };
        aad[pos] = sc[0] & 0x0f;
        pos += 1;
        aad[pos] = 0;
        pos += 1;

        if let Some(a3_bytes) = a3 {
            aad[pos..pos + ETH_ALEN].copy_from_slice(a3_bytes);
            pos += ETH_ALEN;
        }
    }

    nonce[1..1 + ETH_ALEN].copy_from_slice(a2);
    nonce[7] = pn[5];
    nonce[8] = pn[4];
    nonce[9] = pn[3];
    nonce[10] = pn[2];
    nonce[11] = pn[1];
    nonce[12] = pn[0];

    pos
}
