// SPDX-License-Identifier: GPL-2.0
//! GCMP (GCM with GMAC Protocol) — Rust port of `core/crypto/gcmp.c` (W2-02e).
//!
//! Shared logic in `gcmp_support.rs`; this crate root exports the C ABI shims.
//!
//! Domain types are included via `#[path]` because Kbuild compiles each `.rs`
//! as its own crate (same pattern as `domain_types.rs` / `aes_ctr.rs`). That
//! duplicates type code in `88x2bu.ko` for the pilot; consolidate into a
//! shared crate or `include!` only if binary size or drift becomes a concern.
//!
//! The `extern "C"` shims are intentionally stricter than C on invalid `tk_len`:
//! `AesKey::try_from_slice` accepts only 16/24/32-byte keys and returns `NULL`
//! otherwise, whereas C passes `tk_len` straight through to `aes_gcm_*`.

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

#[path = "gcmp_support.rs"]
mod support;

use support::bindings::{aes_gcm_ad, aes_gcm_ae, rtw_registrypriv_amsdu_mode, Adapter};
use support::{gcmp_aad_nonce, os_malloc, rtw_mfree, AES_BLOCK_SIZE, Ieee80211Hdr};
use types::AesKey;

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
