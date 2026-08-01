// SPDX-License-Identifier: GPL-2.0
//! SHA256-PRF — Rust port of `core/crypto/sha256-prf.c` (W2-06b).
//!
//! `hmac_sha256_vector` is provided by `sha256.rs` (W2-16) at link time.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(not(host_crypto_test))]
use core::ffi::{c_char, c_int};
#[cfg(host_crypto_test)]
use std::os::raw::{c_char, c_int};

const SHA256_MAC_LEN: usize = 32;

extern "C" {
    fn hmac_sha256_vector(
        key: *const u8,
        key_len: usize,
        num_elem: usize,
        addr: *const *const u8,
        len: *const usize,
        mac: *mut u8,
    ) -> c_int;
}

fn wpa_put_le16(dst: &mut [u8; 2], val: u16) {
    dst[0] = (val & 0xff) as u8;
    dst[1] = ((val >> 8) & 0xff) as u8;
}

fn forced_memzero(ptr: &mut [u8]) {
    for b in ptr.iter_mut() {
        *b = 0;
    }
}

fn label_len(label: *const c_char) -> usize {
    if label.is_null() {
        return 0;
    }
    unsafe {
        let mut p = label;
        while *p != 0 {
            p = p.add(1);
        }
        (p as usize) - (label as usize)
    }
}

/// Typed SHA256-PRF (bits) — oracle: `sha256_prf_bits`.
pub fn sha256_prf_bits_typed(
    key: &[u8],
    label: &str,
    data: &[u8],
    buf: &mut [u8],
    buf_len_bits: usize,
) -> Result<(), ()> {
    let buf_len = (buf_len_bits + 7) / 8;
    if buf.len() < buf_len {
        return Err(());
    }

    let label_bytes = label.as_bytes();
    let mut counter: u16 = 1;
    let mut pos = 0usize;
    let mut hash = [0u8; SHA256_MAC_LEN];
    let mut counter_le = [0u8; 2];
    let mut length_le = [0u8; 2];

    wpa_put_le16(&mut length_le, buf_len_bits as u16);

    while pos < buf_len {
        let plen = buf_len - pos;
        wpa_put_le16(&mut counter_le, counter);

        let addr: [*const u8; 4] = [
            counter_le.as_ptr(),
            label_bytes.as_ptr(),
            data.as_ptr(),
            length_le.as_ptr(),
        ];
        let len: [usize; 4] = [2, label_bytes.len(), data.len(), 2];

        if plen >= SHA256_MAC_LEN {
            let rc = unsafe {
                hmac_sha256_vector(
                    key.as_ptr(),
                    key.len(),
                    4,
                    addr.as_ptr(),
                    len.as_ptr(),
                    buf.as_mut_ptr().add(pos),
                )
            };
            if rc < 0 {
                return Err(());
            }
            pos += SHA256_MAC_LEN;
        } else {
            let rc = unsafe {
                hmac_sha256_vector(
                    key.as_ptr(),
                    key.len(),
                    4,
                    addr.as_ptr(),
                    len.as_ptr(),
                    hash.as_mut_ptr(),
                )
            };
            if rc < 0 {
                return Err(());
            }
            buf[pos..pos + plen].copy_from_slice(&hash[..plen]);
            pos += plen;
            break;
        }
        counter = counter.wrapping_add(1);
    }

    if buf_len_bits % 8 != 0 {
        let mask = 0xffu8 << (8 - buf_len_bits % 8);
        buf[pos - 1] &= mask;
    }

    forced_memzero(&mut hash);
    Ok(())
}

/// C ABI: `sha256_prf_bits` from `core/crypto/sha256-prf.c`.
#[no_mangle]
pub extern "C" fn sha256_prf_bits(
    key: *const u8,
    key_len: usize,
    label: *const c_char,
    data: *const u8,
    data_len: usize,
    buf: *mut u8,
    buf_len_bits: usize,
) -> c_int {
    if buf.is_null() {
        return -1;
    }
    if key_len > 0 && key.is_null() {
        return -1;
    }
    if data_len > 0 && data.is_null() {
        return -1;
    }
    if label.is_null() {
        return -1;
    }

    let key_slice = if key_len == 0 {
        &[][..]
    } else {
        unsafe { core::slice::from_raw_parts(key, key_len) }
    };
    let data_slice = if data_len == 0 {
        &[][..]
    } else {
        unsafe { core::slice::from_raw_parts(data, data_len) }
    };
    let label_str = unsafe {
        core::str::from_utf8_unchecked(core::slice::from_raw_parts(
            label as *const u8,
            label_len(label),
        ))
    };
    let buf_len = (buf_len_bits + 7) / 8;
    let buf_slice = unsafe { core::slice::from_raw_parts_mut(buf, buf_len) };

    match sha256_prf_bits_typed(key_slice, label_str, data_slice, buf_slice, buf_len_bits) {
        Ok(()) => 0,
        Err(()) => -1,
    }
}

/// C ABI: `sha256_prf` from `core/crypto/sha256-prf.c`.
#[no_mangle]
pub extern "C" fn sha256_prf(
    key: *const u8,
    key_len: usize,
    label: *const c_char,
    data: *const u8,
    data_len: usize,
    buf: *mut u8,
    buf_len: usize,
) -> c_int {
    sha256_prf_bits(
        key,
        key_len,
        label,
        data,
        data_len,
        buf,
        buf_len.wrapping_mul(8),
    )
}

/// Link-time probe for L1 (distinct from the exported crypto symbols).
#[no_mangle]
pub extern "C" fn rtw_rust_sha256_prf_probe() -> c_int {
    SHA256_MAC_LEN as c_int
}
