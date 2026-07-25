// SPDX-License-Identifier: GPL-2.0
//! CCMP (CTR with CBC-MAC Protocol) — Rust port of `core/crypto/ccmp.c` (W2-09/W2-10).

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

#[path = "ccmp_support.rs"]
mod support;

use support::bindings::{aes_ccm_ad, aes_ccm_ae, Adapter};
use support::{
    ccmp_aad_nonce, ccmp_aad_nonce_pv1, os_malloc, rtw_mfree, AES_BLOCK_SIZE, Ieee80211Hdr,
};
use types::AesKey;

const WLAN_FC_ISWEP: u16 = 0x4000;
const CCMP_HDR_LEN: usize = 8;
const CCMP_MIC_LEN: usize = 8;
const CCMP_256_MIC_LEN: usize = 16;
const CCMP_KEY_LEN: usize = 16;
const CCMP_256_KEY_LEN: usize = 32;

fn ccmp_decrypt_inner(
    amsdu_mode: u8,
    tk: &AesKey,
    hdr: &Ieee80211Hdr,
    data: &[u8],
    mic_len: usize,
    decrypted_len: &mut usize,
) -> *mut u8 {
    if data.len() < 8 + mic_len {
        return core::ptr::null_mut();
    }

    let alloc_len = data.len() + AES_BLOCK_SIZE as usize;
    let plain = os_malloc(alloc_len);
    if plain.is_null() {
        return core::ptr::null_mut();
    }

    let mlen = data.len() - 8 - mic_len;
    let crypt = &data[8..8 + mlen];
    let tag = &data[8 + mlen..];

    let mut aad: [u8; 30] = Default::default();
    let mut nonce: [u8; 13] = Default::default();
    let aad_len = ccmp_aad_nonce(amsdu_mode, hdr, data, &mut aad, &mut nonce);

    let key = tk.as_bytes();
    let rc = unsafe {
        aes_ccm_ad(
            key.as_ptr(),
            key.len(),
            nonce.as_ptr(),
            mic_len,
            crypt.as_ptr(),
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

fn ccmp_decrypt_128_inner(
    amsdu_mode: u8,
    tk: &AesKey,
    hdr: &Ieee80211Hdr,
    data: &[u8],
    decrypted_len: &mut usize,
) -> *mut u8 {
    ccmp_decrypt_inner(amsdu_mode, tk, hdr, data, CCMP_MIC_LEN, decrypted_len)
}

fn ccmp_encrypt_inner(
    amsdu_mode: u8,
    tk: &AesKey,
    frame: &[u8],
    hdrlen: usize,
    pn: Option<&[u8; 6]>,
    keyid: i32,
    mic_len: usize,
    encrypted_len: &mut usize,
) -> *mut u8 {
    if frame.len() < hdrlen || hdrlen < 24 {
        return core::ptr::null_mut();
    }
    let plen = frame.len() - hdrlen;
    let alloc_len = hdrlen + CCMP_HDR_LEN + plen + mic_len + AES_BLOCK_SIZE as usize;
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
            core::ptr::copy_nonoverlapping(frame.as_ptr(), crypt as *mut u8, hdrlen + CCMP_HDR_LEN);
        }
        (
            crypt as *mut Ieee80211Hdr,
            hdrlen + CCMP_HDR_LEN,
            unsafe { frame.as_ptr().add(hdrlen + CCMP_HDR_LEN) },
        )
    };

    unsafe {
        (*hdr_ptr).frame_control |= WLAN_FC_ISWEP;
    }

    let mut aad = [0u8; 30];
    let mut nonce = [0u8; 13];
    let data_for_nonce =
        unsafe { core::slice::from_raw_parts((crypt as *const u8).add(hdrlen), CCMP_HDR_LEN) };
    let aad_len = unsafe {
        ccmp_aad_nonce(
            amsdu_mode,
            &*hdr_ptr,
            data_for_nonce,
            &mut aad,
            &mut nonce,
        )
    };

    let key = tk.as_bytes();
    let rc = unsafe {
        aes_ccm_ae(
            key.as_ptr(),
            key.len(),
            nonce.as_ptr(),
            mic_len,
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

    *encrypted_len = hdrlen + CCMP_HDR_LEN + plen + mic_len;
    crypt as *mut u8
}

/// C ABI: `ccmp_get_pn` from `core/crypto/ccmp.c`.
#[no_mangle]
pub extern "C" fn ccmp_get_pn(pn: *mut u8, data: *const u8) {
    if pn.is_null() || data.is_null() {
        return;
    }
    let data = unsafe { core::slice::from_raw_parts(data, 8) };
    let pn = unsafe { core::slice::from_raw_parts_mut(pn, 6) };
    pn[0] = data[7];
    pn[1] = data[6];
    pn[2] = data[5];
    pn[3] = data[4];
    pn[4] = data[1];
    pn[5] = data[0];
}

/// C ABI: `ccmp_decrypt` from `core/crypto/ccmp.c`.
#[no_mangle]
pub extern "C" fn ccmp_decrypt(
    padapter: *const Adapter,
    tk: *const u8,
    hdr: *const Ieee80211Hdr,
    data: *const u8,
    data_len: usize,
    decrypted_len: *mut usize,
) -> *mut u8 {
    if padapter.is_null() || tk.is_null() || hdr.is_null() || data.is_null()
        || decrypted_len.is_null()
    {
        return core::ptr::null_mut();
    }

    let aes_key = match AesKey::try_from_slice(unsafe {
        core::slice::from_raw_parts(tk, CCMP_KEY_LEN)
    }) {
        Ok(k) => k,
        Err(_) => return core::ptr::null_mut(),
    };

    let data_slice = unsafe { core::slice::from_raw_parts(data, data_len) };
    let hdr_ref = unsafe { &*hdr };
    let amsdu_mode = unsafe { support::bindings::rtw_registrypriv_amsdu_mode(padapter) };

    ccmp_decrypt_128_inner(
        amsdu_mode,
        &aes_key,
        hdr_ref,
        data_slice,
        unsafe { &mut *decrypted_len },
    )
}

/// C ABI: `ccmp_256_decrypt` from `core/crypto/ccmp.c`.
#[no_mangle]
pub extern "C" fn ccmp_256_decrypt(
    padapter: *const Adapter,
    tk: *const u8,
    hdr: *const Ieee80211Hdr,
    data: *const u8,
    data_len: usize,
    decrypted_len: *mut usize,
) -> *mut u8 {
    if padapter.is_null() || tk.is_null() || hdr.is_null() || data.is_null()
        || decrypted_len.is_null()
    {
        return core::ptr::null_mut();
    }

    let aes_key = match AesKey::try_from_slice(unsafe {
        core::slice::from_raw_parts(tk, CCMP_256_KEY_LEN)
    }) {
        Ok(k) => k,
        Err(_) => return core::ptr::null_mut(),
    };

    let data_slice = unsafe { core::slice::from_raw_parts(data, data_len) };
    let hdr_ref = unsafe { &*hdr };
    let amsdu_mode = unsafe { support::bindings::rtw_registrypriv_amsdu_mode(padapter) };

    ccmp_decrypt_inner(
        amsdu_mode,
        &aes_key,
        hdr_ref,
        data_slice,
        CCMP_256_MIC_LEN,
        unsafe { &mut *decrypted_len },
    )
}

/// C ABI: `ccmp_encrypt` from `core/crypto/ccmp.c`.
#[no_mangle]
pub extern "C" fn ccmp_encrypt(
    padapter: *const Adapter,
    tk: *const u8,
    frame: *mut u8,
    len: usize,
    hdrlen: usize,
    qos: *mut u8,
    pn: *mut u8,
    keyid: i32,
    encrypted_len: *mut usize,
) -> *mut u8 {
    let _ = qos;
    if padapter.is_null() || tk.is_null() || frame.is_null() || encrypted_len.is_null() {
        return core::ptr::null_mut();
    }

    let aes_key = match AesKey::try_from_slice(unsafe {
        core::slice::from_raw_parts(tk, CCMP_KEY_LEN)
    }) {
        Ok(k) => k,
        Err(_) => return core::ptr::null_mut(),
    };

    let frame_slice = unsafe { core::slice::from_raw_parts(frame, len) };
    let amsdu_mode = unsafe { support::bindings::rtw_registrypriv_amsdu_mode(padapter) };
    let pn_opt = if pn.is_null() {
        None
    } else {
        Some(unsafe { &*(pn as *const [u8; 6]) })
    };

    ccmp_encrypt_inner(
        amsdu_mode,
        &aes_key,
        frame_slice,
        hdrlen,
        pn_opt,
        keyid,
        CCMP_MIC_LEN,
        unsafe { &mut *encrypted_len },
    )
}

/// C ABI: `ccmp_encrypt_pv1` from `core/crypto/ccmp.c`.
#[no_mangle]
pub extern "C" fn ccmp_encrypt_pv1(
    tk: *const u8,
    a1: *const u8,
    a2: *const u8,
    a3: *const u8,
    frame: *const u8,
    len: usize,
    hdrlen: usize,
    pn: *const u8,
    _keyid: i32,
    encrypted_len: *mut usize,
) -> *mut u8 {
    if tk.is_null() || a1.is_null() || a2.is_null() || frame.is_null() || pn.is_null()
        || encrypted_len.is_null()
    {
        return core::ptr::null_mut();
    }

    if len < hdrlen || hdrlen < 12 {
        return core::ptr::null_mut();
    }

    let aes_key = match AesKey::try_from_slice(unsafe {
        core::slice::from_raw_parts(tk, CCMP_KEY_LEN)
    }) {
        Ok(k) => k,
        Err(_) => return core::ptr::null_mut(),
    };

    let frame_slice = unsafe { core::slice::from_raw_parts(frame, len) };
    let plen = frame_slice.len() - hdrlen;
    let alloc_len = hdrlen + plen + CCMP_MIC_LEN + AES_BLOCK_SIZE as usize;
    let crypt = os_malloc(alloc_len);
    if crypt.is_null() {
        return core::ptr::null_mut();
    }

    unsafe {
        core::ptr::copy_nonoverlapping(frame_slice.as_ptr(), crypt as *mut u8, hdrlen);
    }
    let hdr_ptr = crypt as *mut Ieee80211Hdr;
    unsafe {
        (*hdr_ptr).frame_control |= 1 << 12;
    }
    let pos = hdrlen;

    let a1_arr = unsafe { &*(a1 as *const [u8; 6]) };
    let a2_arr = unsafe { &*(a2 as *const [u8; 6]) };
    let a3_opt = if a3.is_null() {
        None
    } else {
        Some(unsafe { &*(a3 as *const [u8; 6]) })
    };
    let pn_arr = unsafe { &*(pn as *const [u8; 6]) };
    let hdr_bytes = unsafe { core::slice::from_raw_parts(crypt as *const u8, hdrlen) };

    let mut aad = [0u8; 24];
    let mut nonce = [0u8; 13];
    let aad_len = ccmp_aad_nonce_pv1(hdr_bytes, a1_arr, a2_arr, a3_opt, pn_arr, &mut aad, &mut nonce);

    let key = aes_key.as_bytes();
    let rc = unsafe {
        aes_ccm_ae(
            key.as_ptr(),
            key.len(),
            nonce.as_ptr(),
            CCMP_MIC_LEN,
            frame_slice.as_ptr().add(hdrlen),
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

    unsafe {
        *encrypted_len = hdrlen + plen + CCMP_MIC_LEN;
    }
    crypt as *mut u8
}

/// C ABI: `ccmp_256_encrypt` from `core/crypto/ccmp.c`.
#[no_mangle]
pub extern "C" fn ccmp_256_encrypt(
    padapter: *const Adapter,
    tk: *const u8,
    frame: *mut u8,
    len: usize,
    hdrlen: usize,
    qos: *mut u8,
    pn: *mut u8,
    keyid: i32,
    encrypted_len: *mut usize,
) -> *mut u8 {
    let _ = qos;
    if padapter.is_null() || tk.is_null() || frame.is_null() || encrypted_len.is_null() {
        return core::ptr::null_mut();
    }

    let aes_key = match AesKey::try_from_slice(unsafe {
        core::slice::from_raw_parts(tk, CCMP_256_KEY_LEN)
    }) {
        Ok(k) => k,
        Err(_) => return core::ptr::null_mut(),
    };

    let frame_slice = unsafe { core::slice::from_raw_parts(frame, len) };
    let amsdu_mode = unsafe { support::bindings::rtw_registrypriv_amsdu_mode(padapter) };
    let pn_opt = if pn.is_null() {
        None
    } else {
        Some(unsafe { &*(pn as *const [u8; 6]) })
    };

    ccmp_encrypt_inner(
        amsdu_mode,
        &aes_key,
        frame_slice,
        hdrlen,
        pn_opt,
        keyid,
        CCMP_256_MIC_LEN,
        unsafe { &mut *encrypted_len },
    )
}

/// Link-time probe for L1 (distinct from the exported crypto symbols).
#[no_mangle]
pub extern "C" fn rtw_rust_ccmp_probe() -> core::ffi::c_int {
    AES_BLOCK_SIZE as core::ffi::c_int
}
