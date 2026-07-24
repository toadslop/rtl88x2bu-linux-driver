// SPDX-License-Identifier: GPL-2.0
//! AES-OMAC1 (CMAC) — Rust port of `core/crypto/aes-omac1.c` (W2-01).
//!
//! Typed logic uses domain types; `extern "C"` symbols preserve the C ABI for
//! remaining callers. AES block operations stay in C (`aes-internal*.c`).
//!
//! Domain types are included via `#[path]` because Kbuild compiles each `.rs`
//! as its own crate (same pattern as `domain_types.rs` / `aes_ctr.rs`). That
//! duplicates type code in `88x2bu.ko` for the pilot; consolidate into a
//! shared crate or `include!` only if binary size or drift becomes a concern.
//!
//! The `extern "C"` shims are intentionally stricter than C on invalid inputs
//! (`num_elem == 0`, null pointers, non-zero-length fragments with null data):
//! they return `-1` instead of invoking UB. In-tree callers only pass valid
//! single-fragment inputs today.

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

    extern "C" {
        pub fn aes_encrypt_init(key: *const u8, len: usize) -> *mut c_void;
        pub fn aes_encrypt(ctx: *mut c_void, plain: *const u8, crypt: *mut u8) -> c_int;
        pub fn aes_encrypt_deinit(ctx: *mut c_void);
    }
}

#[cfg(not(host_crypto_test))]
mod bindings {
    include!("bindings/generated.rs");
}

use bindings::{aes_encrypt, aes_encrypt_deinit, aes_encrypt_init, AES_BLOCK_SIZE};
use types::{AesKey, AesMac};

#[cfg(host_crypto_test)]
use std::os::raw::c_int;
#[cfg(not(host_crypto_test))]
use core::ffi::c_int;

fn gf_mulx(pad: &mut [u8; 16]) {
    let carry = pad[0] & 0x80 != 0;
    let block_size = AES_BLOCK_SIZE as usize;
    for i in 0..block_size - 1 {
        pad[i] = (pad[i] << 1) | (pad[i + 1] >> 7);
    }
    pad[block_size - 1] <<= 1;
    if carry {
        pad[block_size - 1] ^= 0x87;
    }
}

/// Walks one or more message fragments byte-by-byte (slice or C vector layout).
struct FragCursor<'a> {
    slices: Option<&'a [&'a [u8]]>,
    addr: *const *const u8,
    len: *const usize,
    num_elem: usize,
    elem: usize,
    pos: usize,
}

impl<'a> FragCursor<'a> {
    fn from_slices(fragments: &'a [&'a [u8]]) -> Self {
        Self {
            slices: Some(fragments),
            addr: core::ptr::null(),
            len: core::ptr::null(),
            num_elem: 0,
            elem: 0,
            pos: 0,
        }
    }

    fn from_c(addr: *const *const u8, len: *const usize, num_elem: usize) -> Self {
        Self {
            slices: None,
            addr,
            len,
            num_elem,
            elem: 0,
            pos: 0,
        }
    }

    fn num_fragments(&self) -> usize {
        match self.slices {
            Some(s) => s.len(),
            None => self.num_elem,
        }
    }

    fn current_end(&self) -> usize {
        match self.slices {
            Some(s) => {
                if self.elem < s.len() {
                    s[self.elem].len()
                } else {
                    0
                }
            }
            None => unsafe {
                if self.elem < self.num_elem {
                    *self.len.add(self.elem)
                } else {
                    0
                }
            },
        }
    }

    fn read_byte(&mut self) -> Result<u8, ()> {
        let end = self.current_end();
        if self.elem >= self.num_fragments() {
            return Err(());
        }
        if self.pos >= end {
            self.elem += 1;
            self.pos = 0;
            return self.read_byte();
        }
        let byte = match self.slices {
            Some(s) => s[self.elem][self.pos],
            None => unsafe {
                let ptr = *self.addr.add(self.elem);
                *ptr.add(self.pos)
            },
        };
        self.pos += 1;
        Ok(byte)
    }
}

fn omac1_aes_compute(key: AesKey, cursor: &mut FragCursor<'_>, total_len: usize) -> Result<AesMac, ()> {
    let block_size = AES_BLOCK_SIZE as usize;
    let ctx = unsafe { aes_encrypt_init(key.as_bytes().as_ptr(), key.key_len()) };
    if ctx.is_null() {
        return Err(());
    }

    let mut cbc = [0u8; 16];
    let mut pad = [0u8; 16];
    let mut left = total_len;
    let mut end = cursor.current_end();

    while left >= block_size {
        for i in 0..block_size {
            if cursor.num_fragments() == 0 {
                break;
            }
            cbc[i] ^= cursor.read_byte()?;
            if cursor.pos >= end {
                if i + 1 == block_size && left == block_size {
                    break;
                }
                end = cursor.current_end();
            }
        }
        if left > block_size {
            let rc = unsafe { aes_encrypt(ctx, cbc.as_ptr(), cbc.as_mut_ptr()) };
            if rc != 0 {
                unsafe { aes_encrypt_deinit(ctx) };
                return Err(());
            }
        }
        left -= block_size;
    }

    pad.fill(0);
    let rc = unsafe { aes_encrypt(ctx, pad.as_ptr(), pad.as_mut_ptr()) };
    if rc != 0 {
        unsafe { aes_encrypt_deinit(ctx) };
        return Err(());
    }
    gf_mulx(&mut pad);

    if left != 0 || total_len == 0 {
        if cursor.num_fragments() != 0 {
            for i in 0..left {
                cbc[i] ^= cursor.read_byte()?;
                if cursor.pos >= end {
                    if i + 1 == left {
                        break;
                    }
                    end = cursor.current_end();
                }
            }
        }
        cbc[left] ^= 0x80;
        gf_mulx(&mut pad);
    }

    for i in 0..block_size {
        pad[i] ^= cbc[i];
    }

    let mut mac = [0u8; 16];
    let rc = unsafe { aes_encrypt(ctx, pad.as_ptr(), mac.as_mut_ptr()) };
    unsafe { aes_encrypt_deinit(ctx) };
    if rc != 0 {
        return Err(());
    }
    Ok(AesMac::from_bytes(mac))
}

/// OMAC1 over one or more message fragments (oracle: `omac1_aes_vector`).
pub fn omac1_aes_vector_typed(key: AesKey, fragments: &[&[u8]]) -> Result<AesMac, ()> {
    let total_len: usize = fragments.iter().map(|f| f.len()).sum();
    let mut cursor = FragCursor::from_slices(fragments);
    omac1_aes_compute(key, &mut cursor, total_len)
}

/// C ABI: `omac1_aes_vector` from `core/crypto/aes-omac1.c`.
#[no_mangle]
pub extern "C" fn omac1_aes_vector(
    key: *const u8,
    key_len: usize,
    num_elem: usize,
    addr: *const *const u8,
    len: *const usize,
    mac: *mut u8,
) -> c_int {
    let aes_key = match AesKey::try_from_slice(unsafe { core::slice::from_raw_parts(key, key_len) }) {
        Ok(k) => k,
        Err(_) => return -1,
    };

    if num_elem == 0 || addr.is_null() || len.is_null() || mac.is_null() {
        return -1;
    }

    for i in 0..num_elem {
        let slice_len = unsafe { *len.add(i) };
        let ptr = unsafe { *addr.add(i) };
        if slice_len > 0 && ptr.is_null() {
            return -1;
        }
    }

    let total_len: usize = (0..num_elem)
        .map(|i| unsafe { *len.add(i) })
        .sum();
    let mut cursor = FragCursor::from_c(addr, len, num_elem);

    match omac1_aes_compute(aes_key, &mut cursor, total_len) {
        Ok(out) => {
            unsafe {
                core::ptr::copy_nonoverlapping(out.as_bytes().as_ptr(), mac, AesMac::SIZE);
            }
            0
        }
        Err(()) => -1,
    }
}

/// C ABI: `omac1_aes_128_vector` from `core/crypto/aes-omac1.c`.
#[no_mangle]
pub extern "C" fn omac1_aes_128_vector(
    key: *const u8,
    num_elem: usize,
    addr: *const *const u8,
    len: *const usize,
    mac: *mut u8,
) -> c_int {
    omac1_aes_vector(key, 16, num_elem, addr, len, mac)
}

/// C ABI: `omac1_aes_128` from `core/crypto/aes-omac1.c`.
#[no_mangle]
pub extern "C" fn omac1_aes_128(
    key: *const u8,
    data: *const u8,
    data_len: usize,
    mac: *mut u8,
) -> c_int {
    omac1_aes_vector(key, 16, 1, &data, &data_len, mac)
}

/// C ABI: `omac1_aes_256` from `core/crypto/aes-omac1.c`.
#[no_mangle]
pub extern "C" fn omac1_aes_256(
    key: *const u8,
    data: *const u8,
    data_len: usize,
    mac: *mut u8,
) -> c_int {
    omac1_aes_vector(key, 32, 1, &data, &data_len, mac)
}

/// Link-time probe for L1 (distinct from the exported crypto symbols).
#[no_mangle]
pub extern "C" fn rtw_rust_aes_omac1_probe() -> c_int {
    AES_BLOCK_SIZE as c_int
}
