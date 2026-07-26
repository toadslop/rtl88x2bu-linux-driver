// SPDX-License-Identifier: GPL-2.0
//! Software crypto frame wrappers — Rust port of `core/rtw_swcrypto.c` (W3-01).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_swcrypto_test)]
use std::os::raw::{c_int, c_void};

#[cfg(not(host_swcrypto_test))]
use core::ffi::{c_int, c_void};

const AES_BLOCK_SIZE: usize = 16;
const _SUCCESS: i32 = 1;
const _FAIL: i32 = 0;

#[repr(C)]
pub struct Ieee80211Hdr {
    pub frame_control: u16,
    pub duration_id: u16,
    pub addr1: [u8; 6],
    pub addr2: [u8; 6],
    pub addr3: [u8; 6],
    pub seq_ctrl: u16,
}

pub type Adapter = c_void;

extern "C" {
    fn ccmp_encrypt(
        padapter: *mut Adapter,
        tk: *const u8,
        frame: *mut u8,
        len: usize,
        hdrlen: usize,
        qos: *mut u8,
        pn: *mut u8,
        keyid: i32,
        encrypted_len: *mut usize,
    ) -> *mut u8;
    fn ccmp_decrypt(
        padapter: *mut Adapter,
        tk: *const u8,
        hdr: *const Ieee80211Hdr,
        data: *const u8,
        data_len: usize,
        decrypted_len: *mut usize,
    ) -> *mut u8;
    fn ccmp_256_encrypt(
        padapter: *mut Adapter,
        tk: *const u8,
        frame: *mut u8,
        len: usize,
        hdrlen: usize,
        qos: *mut u8,
        pn: *mut u8,
        keyid: i32,
        encrypted_len: *mut usize,
    ) -> *mut u8;
    fn ccmp_256_decrypt(
        padapter: *mut Adapter,
        tk: *const u8,
        hdr: *const Ieee80211Hdr,
        data: *const u8,
        data_len: usize,
        decrypted_len: *mut usize,
    ) -> *mut u8;
    fn gcmp_encrypt(
        padapter: *mut Adapter,
        tk: *const u8,
        tk_len: u32,
        frame: *const u8,
        len: usize,
        hdrlen: usize,
        qos: *const u8,
        pn: *const u8,
        keyid: i32,
        encrypted_len: *mut usize,
    ) -> *mut u8;
    fn gcmp_decrypt(
        padapter: *mut Adapter,
        tk: *const u8,
        tk_len: u32,
        hdr: *const Ieee80211Hdr,
        data: *const u8,
        data_len: usize,
        decrypted_len: *mut usize,
    ) -> *mut u8;
    fn _rtw_mfree(ptr: *mut c_void, sz: u32);
    fn _rtw_memcpy(dst: *mut c_void, src: *const c_void, n: usize) -> *mut c_void;
}

#[no_mangle]
pub extern "C" fn rtw_rust_swcrypto_probe() -> c_int {
    0x5301
}

#[no_mangle]
pub extern "C" fn _rtw_ccmp_encrypt(
    padapter: *mut Adapter,
    key: *mut u8,
    key_len: u32,
    hdrlen: u32,
    frame: *mut u8,
    plen: u32,
) -> i32 {
    let mut enc_len: usize = 0;
    let enc = if key_len == 16 {
        unsafe {
            ccmp_encrypt(
                padapter,
                key,
                frame,
                (hdrlen + plen) as usize,
                hdrlen as usize,
                if hdrlen == 26 {
                    frame.add(hdrlen as usize - 2)
                } else {
                    core::ptr::null_mut()
                },
                core::ptr::null_mut(),
                0,
                &mut enc_len,
            )
        }
    } else if key_len == 32 {
        unsafe {
            ccmp_256_encrypt(
                padapter,
                key,
                frame,
                (hdrlen + plen) as usize,
                hdrlen as usize,
                if hdrlen == 26 {
                    frame.add(hdrlen as usize - 2)
                } else {
                    core::ptr::null_mut()
                },
                core::ptr::null_mut(),
                0,
                &mut enc_len,
            )
        }
    } else {
        return _FAIL;
    };

    if enc.is_null() {
        return _FAIL;
    }
    unsafe {
        _rtw_memcpy(frame as *mut c_void, enc as *const c_void, enc_len);
        _rtw_mfree(enc as *mut c_void, (enc_len + AES_BLOCK_SIZE) as u32);
    }
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn _rtw_ccmp_decrypt(
    padapter: *mut Adapter,
    key: *mut u8,
    key_len: u32,
    hdrlen: u32,
    frame: *mut u8,
    plen: u32,
) -> i32 {
    let hdr = frame as *const Ieee80211Hdr;
    let mut plain_len: usize = 0;
    let plain = if key_len == 16 {
        unsafe {
            ccmp_decrypt(
                padapter,
                key,
                hdr,
                frame.add(hdrlen as usize),
                (plen - hdrlen) as usize,
                &mut plain_len,
            )
        }
    } else if key_len == 32 {
        unsafe {
            ccmp_256_decrypt(
                padapter,
                key,
                hdr,
                frame.add(hdrlen as usize),
                (plen - hdrlen) as usize,
                &mut plain_len,
            )
        }
    } else {
        return _FAIL;
    };

    if plain.is_null() {
        return _FAIL;
    }
    unsafe {
        _rtw_memcpy(
            frame.add(hdrlen as usize + 8) as *mut c_void,
            plain as *const c_void,
            plain_len,
        );
        _rtw_mfree(plain as *mut c_void, ((plen - hdrlen) as usize + AES_BLOCK_SIZE) as u32);
    }
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn _rtw_gcmp_encrypt(
    padapter: *mut Adapter,
    key: *mut u8,
    key_len: u32,
    hdrlen: u32,
    frame: *mut u8,
    plen: u32,
) -> i32 {
    let mut enc_len: usize = 0;
    let enc = unsafe {
        gcmp_encrypt(
            padapter,
            key,
            key_len,
            frame,
            (hdrlen + plen) as usize,
            hdrlen as usize,
            if hdrlen == 26 {
                frame.add(hdrlen as usize - 2)
            } else {
                core::ptr::null_mut()
            },
            core::ptr::null_mut(),
            0,
            &mut enc_len,
        )
    };
    if enc.is_null() {
        return _FAIL;
    }
    unsafe {
        _rtw_memcpy(frame as *mut c_void, enc as *const c_void, enc_len);
        _rtw_mfree(enc as *mut c_void, (enc_len + AES_BLOCK_SIZE) as u32);
    }
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn _rtw_gcmp_decrypt(
    padapter: *mut Adapter,
    key: *mut u8,
    key_len: u32,
    hdrlen: u32,
    frame: *mut u8,
    plen: u32,
) -> i32 {
    let hdr = frame as *const Ieee80211Hdr;
    let mut plain_len: usize = 0;
    let plain = unsafe {
        gcmp_decrypt(
            padapter,
            key,
            key_len,
            hdr,
            frame.add(hdrlen as usize),
            (plen - hdrlen) as usize,
            &mut plain_len,
        )
    };
    if plain.is_null() {
        return _FAIL;
    }
    unsafe {
        _rtw_memcpy(
            frame.add(hdrlen as usize + 8) as *mut c_void,
            plain as *const c_void,
            plain_len,
        );
        _rtw_mfree(plain as *mut c_void, ((plen - hdrlen) as usize + AES_BLOCK_SIZE) as u32);
    }
    _SUCCESS
}
