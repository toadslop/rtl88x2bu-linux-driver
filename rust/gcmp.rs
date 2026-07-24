// SPDX-License-Identifier: GPL-2.0
//! GCMP (GCM with GMAC Protocol) — Rust port of `core/crypto/gcmp.c` (W2-02).
//!
//! Typed logic uses domain types where practical; `extern "C"` symbols preserve the
//! C ABI. AES-GCM primitives stay in C (`aes-gcm.c`); adapter field access uses
//! `rtw_registrypriv_amsdu_mode()` from `rtw_crypto_wrap.c`.

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
mod types;

#[cfg(host_crypto_test)]
mod bindings {
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
mod bindings {
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

#[cfg(host_crypto_test)]
fn os_malloc(sz: usize) -> *mut core::ffi::c_void {
    unsafe { bindings::os_malloc(sz) }
}

#[cfg(not(host_crypto_test))]
fn os_malloc(sz: usize) -> *mut core::ffi::c_void {
    unsafe { bindings::_rtw_malloc(sz as u32) }
}

fn rtw_mfree(ptr: *mut core::ffi::c_void, sz: usize) {
    #[cfg(host_crypto_test)]
    unsafe {
        bindings::rtw_mfree(ptr, sz);
    }
    #[cfg(not(host_crypto_test))]
    unsafe {
        bindings::_rtw_mfree(ptr, sz as u32);
    }
}

use bindings::{aes_gcm_ad, aes_gcm_ae, rtw_registrypriv_amsdu_mode, Adapter, AES_BLOCK_SIZE, Ieee80211Hdr};
use types::AesKey;

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

fn gcmp_aad_nonce(
    amsdu_mode: u8,
    hdr: &Ieee80211Hdr,
    data: &[u8],
    aad: &mut [u8; 30],
    nonce: &mut [u8; 12],
) -> usize {
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

fn gcmp_decrypt_inner(
    amsdu_mode: u8,
    tk: AesKey,
    hdr: &Ieee80211Hdr,
    data: &[u8],
    decrypted_len: &mut usize,
) -> *mut u8 {
    if data.len() < 8 + 16 {
        return core::ptr::null_mut();
    }

    let alloc_len = data.len() + AES_BLOCK_SIZE as usize;
    let plain = os_malloc(alloc_len);
    if plain.is_null() {
        return core::ptr::null_mut();
    }

    let mlen = data.len() - 8 - 16;
    let m = &data[8..8 + mlen];
    let tag = &data[8 + mlen..];

    let mut aad = [0u8; 30];
    let mut nonce = [0u8; 12];
    let aad_len = gcmp_aad_nonce(amsdu_mode, hdr, data, &mut aad, &mut nonce);

    let key = tk.as_bytes();
    let rc = unsafe {
        aes_gcm_ad(
            key.as_ptr(),
            key.len(),
            nonce.as_ptr(),
            nonce.len(),
            m.as_ptr(),
            mlen,
            aad.as_ptr(),
            aad_len,
            tag.as_ptr(),
            plain as *mut u8,
        )
    };
    if rc < 0 {
        rtw_mfree(plain, alloc_len);
        return core::ptr::null_mut();
    }

    *decrypted_len = mlen;
    plain as *mut u8
}

fn gcmp_encrypt_inner(
    amsdu_mode: u8,
    tk: AesKey,
    frame: &[u8],
    hdrlen: usize,
    pn: Option<&[u8; 6]>,
    keyid: i32,
    encrypted_len: &mut usize,
) -> *mut u8 {
    if frame.len() < hdrlen || hdrlen < 24 {
        return core::ptr::null_mut();
    }
    let plen = frame.len() - hdrlen;
    let alloc_len = hdrlen + 8 + plen + 16 + AES_BLOCK_SIZE as usize;
    let crypt = os_malloc(alloc_len);
    if crypt.is_null() {
        return core::ptr::null_mut();
    }

    let (hdr_ptr, pos, pdata) = if let Some(pn_bytes) = pn {
        unsafe {
            core::ptr::copy_nonoverlapping(frame.as_ptr(), crypt as *mut u8, hdrlen);
        }
        let mut pos = hdrlen;
        unsafe {
            let c = crypt as *mut u8;
            *c.add(pos) = pn_bytes[5];
            pos += 1;
            *c.add(pos) = pn_bytes[4];
            pos += 1;
            *c.add(pos) = 0x00;
            pos += 1;
            *c.add(pos) = 0x20 | ((keyid as u8) << 6);
            pos += 1;
            *c.add(pos) = pn_bytes[3];
            pos += 1;
            *c.add(pos) = pn_bytes[2];
            pos += 1;
            *c.add(pos) = pn_bytes[1];
            pos += 1;
            *c.add(pos) = pn_bytes[0];
            pos += 1;
        }
        (crypt as *mut Ieee80211Hdr, pos, unsafe { frame.as_ptr().add(hdrlen) })
    } else {
        unsafe {
            core::ptr::copy_nonoverlapping(frame.as_ptr(), crypt as *mut u8, hdrlen + 8);
        }
        (
            crypt as *mut Ieee80211Hdr,
            hdrlen + 8,
            unsafe { frame.as_ptr().add(hdrlen + 8) },
        )
    };

    let mut aad = [0u8; 30];
    let mut nonce = [0u8; 12];
    let data_for_nonce =
        unsafe { core::slice::from_raw_parts((crypt as *const u8).add(hdrlen), 8) };
    let aad_len = unsafe {
        gcmp_aad_nonce(
            amsdu_mode,
            &*hdr_ptr,
            data_for_nonce,
            &mut aad,
            &mut nonce,
        )
    };

    let key = tk.as_bytes();
    let rc = unsafe {
        aes_gcm_ae(
            key.as_ptr(),
            key.len(),
            nonce.as_ptr(),
            nonce.len(),
            pdata,
            plen,
            aad.as_ptr(),
            aad_len,
            (crypt as *mut u8).add(pos),
            (crypt as *mut u8).add(pos + plen),
        )
    };
    if rc < 0 {
        rtw_mfree(crypt, alloc_len);
        return core::ptr::null_mut();
    }

    *encrypted_len = hdrlen + 8 + plen + 16;
    crypt as *mut u8
}

/// C ABI: `gcmp_decrypt` from `core/crypto/gcmp.c`.
#[no_mangle]
pub extern "C" fn gcmp_decrypt(
    padapter: *const Adapter,
    tk: *const u8,
    tk_len: usize,
    hdr: *const Ieee80211Hdr,
    data: *const u8,
    data_len: usize,
    decrypted_len: *mut usize,
) -> *mut u8 {
    if tk.is_null() || hdr.is_null() || data.is_null() || decrypted_len.is_null() {
        return core::ptr::null_mut();
    }

    let aes_key = match AesKey::try_from_slice(unsafe { core::slice::from_raw_parts(tk, tk_len) }) {
        Ok(k) => k,
        Err(_) => return core::ptr::null_mut(),
    };

    let data_slice = unsafe { core::slice::from_raw_parts(data, data_len) };
    let hdr_ref = unsafe { &*hdr };
    let amsdu_mode = unsafe { rtw_registrypriv_amsdu_mode(padapter) };

    gcmp_decrypt_inner(
        amsdu_mode,
        aes_key,
        hdr_ref,
        data_slice,
        unsafe { &mut *decrypted_len },
    )
}

/// C ABI: `gcmp_encrypt` from `core/crypto/gcmp.c`.
#[no_mangle]
pub extern "C" fn gcmp_encrypt(
    padapter: *const Adapter,
    tk: *const u8,
    tk_len: usize,
    frame: *const u8,
    len: usize,
    hdrlen: usize,
    qos: *const u8,
    pn: *const u8,
    keyid: i32,
    encrypted_len: *mut usize,
) -> *mut u8 {
    let _ = qos;
    if tk.is_null() || frame.is_null() || encrypted_len.is_null() {
        return core::ptr::null_mut();
    }

    let aes_key = match AesKey::try_from_slice(unsafe { core::slice::from_raw_parts(tk, tk_len) }) {
        Ok(k) => k,
        Err(_) => return core::ptr::null_mut(),
    };

    let frame_slice = unsafe { core::slice::from_raw_parts(frame, len) };
    let amsdu_mode = unsafe { rtw_registrypriv_amsdu_mode(padapter) };
    let pn_opt = if pn.is_null() {
        None
    } else {
        Some(unsafe { &*(pn as *const [u8; 6]) })
    };

    gcmp_encrypt_inner(
        amsdu_mode,
        aes_key,
        frame_slice,
        hdrlen,
        pn_opt,
        keyid,
        unsafe { &mut *encrypted_len },
    )
}

/// Link-time probe for L1 (distinct from the exported crypto symbols).
#[no_mangle]
pub extern "C" fn rtw_rust_gcmp_probe() -> core::ffi::c_int {
    AES_BLOCK_SIZE as core::ffi::c_int
}
