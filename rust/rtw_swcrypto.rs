// SPDX-License-Identifier: GPL-2.0
//! Software crypto frame wrappers — Rust port of `core/rtw_swcrypto.c` (W3-01/W3-02).

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
const ETH_ALEN: usize = 6;
const _SUCCESS: i32 = 1;
const _FAIL: i32 = 0;

#[repr(C)]
pub struct Ieee80211Hdr {
    pub frame_control: u16,
    pub duration_id: u16,
    pub addr1: [u8; ETH_ALEN],
    pub addr2: [u8; ETH_ALEN],
    pub addr3: [u8; ETH_ALEN],
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
    fn omac1_aes_128(key: *const u8, data: *const u8, data_len: usize, mac: *mut u8) -> i32;
    fn omac1_aes_256(key: *const u8, data: *const u8, data_len: usize, mac: *mut u8) -> i32;
    fn aes_gmac(
        key: *const u8,
        key_len: usize,
        nonce: *const u8,
        nonce_len: usize,
        data: *const u8,
        data_len: usize,
        tag: *mut u8,
    ) -> i32;
    fn aes_siv_encrypt(
        key: *const u8,
        key_len: usize,
        pw: *const u8,
        pwlen: usize,
        num_elem: usize,
        addr: *const *const u8,
        len: *const usize,
        out: *mut u8,
    ) -> i32;
    fn aes_siv_decrypt(
        key: *const u8,
        key_len: usize,
        iv_crypt: *const u8,
        iv_c_len: usize,
        num_elem: usize,
        addr: *const *const u8,
        len: *const usize,
        out: *mut u8,
    ) -> i32;
    fn rtw_swcrypto_log_err(msg: *const u8);
    fn _rtw_mfree(ptr: *mut c_void, sz: u32);
    fn _rtw_memcpy(dst: *mut c_void, src: *const c_void, n: usize) -> *mut c_void;
}

fn get_addr2_ptr(frame: *const u8) -> *const u8 {
    unsafe { frame.add(10) }
}

fn log_bip_err(msg: &[u8]) {
    unsafe { rtw_swcrypto_log_err(msg.as_ptr()) };
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

#[no_mangle]
pub extern "C" fn _bip_ccmp_protect(
    key: *const u8,
    key_len: usize,
    data: *const u8,
    data_len: usize,
    mic: *mut u8,
) -> u8 {
    let res = if key_len == 16 {
        unsafe { omac1_aes_128(key, data, data_len, mic) }
    } else if key_len == 32 {
        unsafe { omac1_aes_256(key, data, data_len, mic) }
    } else {
        log_bip_err(b"_bip_ccmp_protect : key_len not match!\0");
        return _FAIL as u8;
    };
    if res != 0 {
        if key_len == 16 {
            log_bip_err(b"_bip_ccmp_protect : omac1_aes_128 fail!\0");
        } else {
            log_bip_err(b"_bip_ccmp_protect : omac1_aes_256 fail!\0");
        }
        _FAIL as u8
    } else {
        _SUCCESS as u8
    }
}

#[no_mangle]
pub extern "C" fn _bip_gcmp_protect(
    whdr_pos: *mut u8,
    len: usize,
    key: *const u8,
    key_len: usize,
    data: *const u8,
    data_len: usize,
    mic: *mut u8,
) -> u8 {
    let mic_len = 16usize;
    let mut nonce = [0u8; 12];
    unsafe {
        let gcmp_ipn = whdr_pos.add(len - mic_len - 6);
        _rtw_memcpy(
            nonce.as_mut_ptr() as *mut c_void,
            get_addr2_ptr(whdr_pos) as *const c_void,
            ETH_ALEN,
        );
        nonce[6] = *gcmp_ipn.add(5);
        nonce[7] = *gcmp_ipn.add(4);
        nonce[8] = *gcmp_ipn.add(3);
        nonce[9] = *gcmp_ipn.add(2);
        nonce[10] = *gcmp_ipn.add(1);
        nonce[11] = *gcmp_ipn.add(0);
        if aes_gmac(
            key,
            key_len,
            nonce.as_ptr(),
            nonce.len(),
            data,
            data_len,
            mic,
        ) != 0
        {
            log_bip_err(b"_bip_gcmp_protect : aes_gmac fail!\0");
            return _FAIL as u8;
        }
    }
    _SUCCESS as u8
}

// C gates these on CONFIG_RTW_MESH_AEK; Rust exports unconditionally for a stable
// ABI across kernel configs (same pattern as other swcrypto wrappers in this file).
#[no_mangle]
pub extern "C" fn _aes_siv_encrypt(
    key: *const u8,
    key_len: usize,
    pw: *const u8,
    pwlen: usize,
    num_elem: usize,
    addr: *const *const u8,
    len: *const usize,
    out: *mut u8,
) -> i32 {
    unsafe { aes_siv_encrypt(key, key_len, pw, pwlen, num_elem, addr, len, out) }
}

#[no_mangle]
pub extern "C" fn _aes_siv_decrypt(
    key: *const u8,
    key_len: usize,
    iv_crypt: *const u8,
    iv_c_len: usize,
    num_elem: usize,
    addr: *const *const u8,
    len: *const usize,
    out: *mut u8,
) -> i32 {
    unsafe { aes_siv_decrypt(key, key_len, iv_crypt, iv_c_len, num_elem, addr, len, out) }
}
