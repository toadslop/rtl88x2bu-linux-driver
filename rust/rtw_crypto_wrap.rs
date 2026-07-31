// SPDX-License-Identifier: GPL-2.0
//! Crypto OS wrappers — Rust port of `core/crypto/rtw_crypto_wrap.c` (W2-06e).
//!
//! Thin `extern "C"` shims for WPA/crypto helpers used across `core/crypto/*`.

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
use std::os::raw::{c_char, c_int, c_void};
#[cfg(not(host_crypto_test))]
use core::ffi::{c_char, c_int, c_void};

#[cfg(host_crypto_test)]
mod bindings {
    use super::*;

    #[repr(C)]
    pub struct RegistryPriv {
        pub amsdu_mode: u8,
    }

    #[repr(C)]
    pub struct Adapter {
        pub registrypriv: RegistryPriv,
    }

    unsafe extern "C" {
        pub fn _rtw_memcmp2(dst: *const c_void, src: *const c_void, sz: u32) -> c_int;
        pub fn rtw_malloc(sz: usize) -> *mut c_void;
        pub fn rtw_mfree(ptr: *mut c_void, sz: usize);
    }

    pub fn rtw_memset(s: *mut c_void, c: c_int, n: usize) -> *mut c_void {
        unsafe {
            core::ptr::write_bytes(s as *mut u8, c as u8, n);
            s
        }
    }

    pub fn rtw_memcpy(dest: *mut c_void, src: *const c_void, n: usize) -> *mut c_void {
        unsafe {
            core::ptr::copy_nonoverlapping(src as *const u8, dest as *mut u8, n);
            dest
        }
    }
}

#[cfg(not(host_crypto_test))]
mod bindings {
    use super::*;

    unsafe extern "C" {
        pub fn _rtw_memcmp2(dst: *const c_void, src: *const c_void, sz: u32) -> c_int;
        pub fn _rtw_memset(s: *mut c_void, c: c_int, n: usize) -> *mut c_void;
        pub fn _rtw_memcpy(dest: *mut c_void, src: *const c_void, n: usize) -> *mut c_void;
        pub fn _rtw_malloc(sz: u32) -> *mut c_void;
        pub fn _rtw_mfree(ptr: *mut c_void, sz: u32);
    }

    /// `offsetof(struct _ADAPTER, registrypriv.amsdu_mode)` for this driver's
    /// `include/drv_types.h` layout (verified via `llvm-objdump` on the C TU).
    /// Re-run L1 (`make rust-check-symbols OLD=core/crypto/rtw_crypto_wrap.o
    /// NEW=rust/rtw_crypto_wrap.o`) after any `_adapter` / `registry_priv` layout change.
    pub const AMSDU_MODE_OFFSET: usize = 0x3859;
}

const RTW_AMSDU_MODE_NON_SPP: u8 = 0;

#[cfg(host_crypto_test)]
fn wrap_malloc(sz: u32) -> *mut c_void {
    unsafe { bindings::rtw_malloc(sz as usize) }
}

#[cfg(not(host_crypto_test))]
fn wrap_malloc(sz: u32) -> *mut c_void {
    unsafe { bindings::_rtw_malloc(sz) }
}

#[cfg(host_crypto_test)]
fn wrap_mfree(ptr: *mut c_void, len: usize) {
    unsafe {
        bindings::rtw_mfree(ptr, len);
    }
}

#[cfg(not(host_crypto_test))]
fn wrap_mfree(ptr: *mut c_void, len: usize) {
    unsafe {
        bindings::_rtw_mfree(ptr, len as u32);
    }
}

#[cfg(host_crypto_test)]
fn wrap_memcpy(dest: *mut c_void, src: *const c_void, n: usize) {
    bindings::rtw_memcpy(dest, src, n);
}

#[cfg(not(host_crypto_test))]
fn wrap_memcpy(dest: *mut c_void, src: *const c_void, n: usize) {
    unsafe {
        bindings::_rtw_memcpy(dest, src, n);
    }
}

#[cfg(host_crypto_test)]
fn wrap_memset(ptr: *mut c_void, len: usize) {
    bindings::rtw_memset(ptr, 0, len);
}

#[cfg(not(host_crypto_test))]
fn wrap_memset(ptr: *mut c_void, len: usize) {
    unsafe {
        bindings::_rtw_memset(ptr, 0, len);
    }
}

/// Constant-time memory compare — oracle: `os_memcmp_const`.
pub fn os_memcmp_const_typed(a: &[u8], b: &[u8]) -> u8 {
    let mut res = 0u8;
    for i in 0..a.len() {
        res |= a[i] ^ b[i];
    }
    res
}

/// C ABI: `os_memcmp_const` from `core/crypto/rtw_crypto_wrap.c`.
#[unsafe(no_mangle)]
pub extern "C" fn os_memcmp_const(a: *const c_void, b: *const c_void, len: usize) -> c_int {
    let a_slice = unsafe { core::slice::from_raw_parts(a as *const u8, len) };
    let b_slice = unsafe { core::slice::from_raw_parts(b as *const u8, len) };
    os_memcmp_const_typed(a_slice, b_slice) as c_int
}

/// C ABI: `os_memcmp` from `core/crypto/rtw_crypto_wrap.c`.
#[unsafe(no_mangle)]
pub extern "C" fn os_memcmp(s1: *const c_void, s2: *const c_void, n: usize) -> c_int {
    unsafe { bindings::_rtw_memcmp2(s1, s2, n as u32) }
}

/// C ABI: `os_strlen` from `core/crypto/rtw_crypto_wrap.c`.
#[unsafe(no_mangle)]
pub extern "C" fn os_strlen(s: *const c_char) -> usize {
    unsafe {
        let mut p = s;
        while *p != 0 {
            p = p.add(1);
        }
        (p as usize) - (s as usize)
    }
}

/// C ABI: `os_memdup` from `core/crypto/rtw_crypto_wrap.c`.
#[unsafe(no_mangle)]
pub extern "C" fn os_memdup(src: *const c_void, sz: u32) -> *mut c_void {
    let r = wrap_malloc(sz);
    if !r.is_null() && !src.is_null() && sz > 0 {
        wrap_memcpy(r, src, sz as usize);
    }
    r
}

/// C ABI: `forced_memzero` from `core/crypto/rtw_crypto_wrap.c`.
#[unsafe(no_mangle)]
pub extern "C" fn forced_memzero(ptr: *mut c_void, len: usize) {
    wrap_memset(ptr, len);
}

/// C ABI: `bin_clear_free` from `core/crypto/rtw_crypto_wrap.c`.
#[unsafe(no_mangle)]
pub extern "C" fn bin_clear_free(bin: *mut c_void, len: usize) {
    if bin.is_null() {
        return;
    }
    wrap_memset(bin, len);
    wrap_mfree(bin, len);
}

/// C ABI: `rtw_registrypriv_amsdu_mode` from `core/crypto/rtw_crypto_wrap.c`.
#[unsafe(no_mangle)]
pub extern "C" fn rtw_registrypriv_amsdu_mode(padapter: *const c_void) -> u8 {
    if padapter.is_null() {
        return RTW_AMSDU_MODE_NON_SPP;
    }
    #[cfg(host_crypto_test)]
    {
        let adapter = padapter as *const bindings::Adapter;
        unsafe { (*adapter).registrypriv.amsdu_mode }
    }
    #[cfg(not(host_crypto_test))]
    {
        let field = unsafe { (padapter as *const u8).add(bindings::AMSDU_MODE_OFFSET) };
        unsafe { *field }
    }
}

/// Debug printf stub — oracle: `wpa_printf` (no-op unless DEBUG_CRYPTO in C).
#[unsafe(no_mangle)]
pub extern "C" fn wpa_printf(_level: c_int, _fmt: *const c_char) {}

/// Debug hexdump stub — oracle: `wpa_hexdump`.
#[unsafe(no_mangle)]
pub extern "C" fn wpa_hexdump(
    _level: c_int,
    _title: *const c_char,
    _buf: *const c_void,
    _len: usize,
) {
}

/// Debug key hexdump stub — oracle: `wpa_hexdump_key`.
#[unsafe(no_mangle)]
pub extern "C" fn wpa_hexdump_key(
    _level: c_int,
    _title: *const c_char,
    _buf: *const c_void,
    _len: usize,
) {
}

/// Link-time probe for L1 (distinct from the exported crypto symbols).
#[unsafe(no_mangle)]
pub extern "C" fn rtw_rust_rtw_crypto_wrap_probe() -> c_int {
    RTW_AMSDU_MODE_NON_SPP as c_int
}
