// SPDX-License-Identifier: GPL-2.0
// proc rx_ampdu get/set (W3-204); tx AMPDU misc procs (W3-206).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe
)]

use core::ffi::{c_char, c_int, c_long, c_uint, c_void};

type U8 = u8;

const RX_AMPDU_ACCEPT_INVALID: U8 = 0xff;
const RX_AMPDU_SIZE_INVALID: U8 = 0xff;
const RX_AMPDU_DRV_FIXED: U8 = 1;

extern "C" {
    fn rtw_rust_debug_proc_adapter(m: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_proc_adapter_from_data(data: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_print_sel(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_debug_fix_rx_ampdu_accept(adapter: *mut c_void) -> U8;
    fn rtw_rust_debug_fix_rx_ampdu_size(adapter: *mut c_void) -> U8;
    fn rtw_rust_debug_proc_read_user_line(
        buffer: *const c_void,
        count: usize,
        tmp: *mut c_char,
        tmp_size: usize,
    ) -> c_long;
    fn rtw_rx_ampdu_is_accept(adapter: *mut c_void) -> U8;
    fn rtw_rx_ampdu_size(adapter: *mut c_void) -> U8;
    fn rtw_rx_ampdu_set_accept(adapter: *mut c_void, accept: U8, reason: U8) -> U8;
    fn rtw_rx_ampdu_set_size(adapter: *mut c_void, size: U8, reason: U8) -> U8;
    fn rtw_rx_ampdu_apply(adapter: *mut c_void);
    fn dump_regsty_rx_ampdu_size_limit(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_sscanf_hhu_pair(
        tmp: *const u8,
        first: *mut U8,
        second: *mut U8,
    ) -> c_int;
    fn rtw_rust_debug_proc_sscanf_u32(tmp: *const u8, val: *mut c_uint) -> c_int;
    fn rtw_rust_debug_proc_sscanf_u32_pair(
        tmp: *const u8,
        first: *mut c_uint,
        second: *mut c_uint,
    ) -> c_int;
    fn rtw_rust_debug_tx_quick_addba_req(adapter: *mut c_void) -> U8;
    fn rtw_rust_debug_set_tx_quick_addba_req(adapter: *mut c_void, enable: U8);
    #[cfg(config_sdio_tx_enable_aval_int)]
    fn rtw_rust_debug_tx_aval_int_thr_mode(adapter: *mut c_void) -> U8;
    #[cfg(config_sdio_tx_enable_aval_int)]
    fn rtw_rust_debug_tx_aval_int_thr_value(adapter: *mut c_void) -> U8;
    #[cfg(config_sdio_tx_enable_aval_int)]
    fn rtw_rust_debug_set_tx_aval_int_thr_mode(adapter: *mut c_void, mode: U8);
    #[cfg(config_sdio_tx_enable_aval_int)]
    fn rtw_rust_debug_set_tx_aval_int_thr_value(adapter: *mut c_void, val: U8);
}

const EFAULT: c_long = -14;
const PROC_SET_TMP: usize = 32;

fn print_bytes(sel: *mut c_void, bytes: &[u8]) {
    let mut buf = [0i8; 128];
    let n = core::cmp::min(bytes.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = bytes[i] as i8;
    }
    unsafe {
        rtw_rust_debug_print_sel(sel, buf.as_ptr());
    }
}

fn print_line(sel: *mut c_void, line: &[u8]) {
    let mut with_nl = [0u8; 132];
    let n = core::cmp::min(line.len(), with_nl.len().saturating_sub(2));
    with_nl[..n].copy_from_slice(&line[..n]);
    with_nl[n] = b'\n';
    print_bytes(sel, &with_nl[..n + 1]);
}

fn push_byte(out: &mut [u8], pos: &mut usize, b: u8) {
    if *pos < out.len() {
        out[*pos] = b;
        *pos += 1;
    }
}

fn push_str(out: &mut [u8], pos: &mut usize, s: &[u8]) {
    for &b in s {
        push_byte(out, pos, b);
    }
}

fn push_u8_dec(out: &mut [u8], pos: &mut usize, v: U8) {
    if v == 0 {
        push_byte(out, pos, b'0');
        return;
    }
    let mut tmp = [0u8; 3];
    let mut n = 0usize;
    let mut x = v;
    while x > 0 && n < tmp.len() {
        tmp[n] = b'0' + (x % 10);
        x /= 10;
        n += 1;
    }
    while n > 0 {
        n -= 1;
        push_byte(out, pos, tmp[n]);
    }
}

fn push_u8_dec_width_left(out: &mut [u8], pos: &mut usize, v: U8, width: usize) {
    let start = *pos;
    push_u8_dec(out, pos, v);
    while *pos - start < width {
        push_byte(out, pos, b' ');
    }
}

fn emit_get_rx_ampdu(sel: *mut c_void, adapter: *mut c_void) {
    let fix_accept = unsafe { rtw_rust_debug_fix_rx_ampdu_accept(adapter) };
    let fix_size = unsafe { rtw_rust_debug_fix_rx_ampdu_size(adapter) };
    let mut line = [0u8; 48];
    let mut pos;

    print_bytes(sel, b"accept: ");
    pos = 0;
    if fix_accept == RX_AMPDU_ACCEPT_INVALID {
        push_u8_dec(&mut line, &mut pos, unsafe {
            rtw_rx_ampdu_is_accept(adapter)
        });
        push_str(&mut line, &mut pos, b"(auto)");
    } else {
        push_u8_dec(&mut line, &mut pos, fix_accept);
        push_str(&mut line, &mut pos, b"(fixed)");
    }
    print_line(sel, &line[..pos]);

    print_bytes(sel, b"size: ");
    pos = 0;
    if fix_size == RX_AMPDU_SIZE_INVALID {
        push_u8_dec(&mut line, &mut pos, unsafe { rtw_rx_ampdu_size(adapter) });
        push_str(&mut line, &mut pos, b"(auto) with conditional limit:");
        print_line(sel, &line[..pos]);
        unsafe { dump_regsty_rx_ampdu_size_limit(sel, adapter) };
    } else {
        push_u8_dec(&mut line, &mut pos, fix_size);
        push_str(&mut line, &mut pos, b"(fixed)");
        print_line(sel, &line[..pos]);
    }
    print_line(sel, b"");
    print_line(sel, b"fix_rx_ampdu_accept fix_rx_ampdu_size");
    pos = 0;
    push_u8_dec_width_left(&mut line, &mut pos, fix_accept, 19);
    push_byte(&mut line, &mut pos, b' ');
    push_u8_dec_width_left(&mut line, &mut pos, fix_size, 17);
    print_line(sel, &line[..pos]);
}

#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_get_rx_ampdu(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = if m.is_null() {
        core::ptr::null_mut()
    } else {
        unsafe { rtw_rust_debug_proc_adapter(m) }
    };
    if !adapter.is_null() {
        emit_get_rx_ampdu(m, adapter);
    }
    0
}

fn apply_set_rx_ampdu_from_tmp(adapter: *mut c_void, tmp: *const u8) {
    let mut accept = 0u8;
    let mut size = 0u8;
    let num = unsafe { rtw_rust_debug_proc_sscanf_hhu_pair(tmp, &mut accept, &mut size) };
    if num >= 1 {
        unsafe {
            rtw_rx_ampdu_set_accept(adapter, accept, RX_AMPDU_DRV_FIXED);
        }
    }
    if num >= 2 {
        unsafe {
            rtw_rx_ampdu_set_size(adapter, size, RX_AMPDU_DRV_FIXED);
        }
    }
    unsafe { rtw_rx_ampdu_apply(adapter) };
}

#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_set_rx_ampdu(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    if count < 1 {
        return EFAULT;
    }
    if count > PROC_SET_TMP {
        return EFAULT;
    }
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    let mut tmp = [0i8; PROC_SET_TMP];
    if !buffer.is_null() {
        let n = unsafe {
            rtw_rust_debug_proc_read_user_line(buffer, count, tmp.as_mut_ptr(), tmp.len())
        };
        if n > 0 {
            apply_set_rx_ampdu_from_tmp(adapter, tmp.as_ptr() as *const u8);
        }
    }
    count as c_long
}

#[cfg(host_rtw_debug_proc_ampdu_test)]
#[no_mangle]
pub extern "C" fn host_proc_set_rx_ampdu_from_buf(
    adapter: *mut c_void,
    buffer: *const u8,
    count: usize,
) -> c_long {
    if buffer.is_null() || count < 1 {
        return EFAULT;
    }
    if count > PROC_SET_TMP {
        return EFAULT;
    }
    apply_set_rx_ampdu_from_tmp(adapter, buffer);
    count as c_long
}

fn adapter_from_seq(m: *mut c_void) -> *mut c_void {
    if m.is_null() {
        core::ptr::null_mut()
    } else {
        unsafe { rtw_rust_debug_proc_adapter(m) }
    }
}

fn push_u8_hex(out: &mut [u8], pos: &mut usize, v: U8) {
    const HEX: &[u8; 16] = b"0123456789abcdef";
    if v == 0 {
        push_byte(out, pos, b'0');
        return;
    }
    let mut started = false;
    for shift in [4i32, 0] {
        let nibble = (v >> shift) & 0x0f;
        if nibble != 0 || started {
            push_byte(out, pos, HEX[nibble as usize]);
            started = true;
        }
    }
}

fn emit_get_tx_quick_addba(sel: *mut c_void, adapter: *mut c_void) {
    let val = unsafe { rtw_rust_debug_tx_quick_addba_req(adapter) };
    let mut line = [0u8; 48];
    let mut pos = 0usize;
    push_str(&mut line, &mut pos, b"tx_quick_addba_req = ");
    push_u8_hex(&mut line, &mut pos, val);
    print_line(sel, &line[..pos]);
}

#[cfg(config_sdio_tx_enable_aval_int)]
fn emit_get_tx_aval_th(sel: *mut c_void, adapter: *mut c_void) {
    let mode = unsafe { rtw_rust_debug_tx_aval_int_thr_mode(adapter) };
    match mode {
        0 => {
            let mut line = [0u8; 64];
            let mut pos = 0usize;
            push_str(&mut line, &mut pos, b"tx_aval_int_thr_mode = ");
            push_u8_dec(&mut line, &mut pos, mode);
            push_str(&mut line, &mut pos, b" (auto) ");
            print_line(sel, &line[..pos]);
        }
        1 => {
            let val = unsafe { rtw_rust_debug_tx_aval_int_thr_value(adapter) };
            let mut line = [0u8; 64];
            let mut pos = 0usize;
            push_str(&mut line, &mut pos, b"tx_aval_int_thr_mode = ");
            push_u8_dec(&mut line, &mut pos, mode);
            push_str(&mut line, &mut pos, b" (fixed)");
            print_line(sel, &line[..pos]);
            pos = 0;
            push_str(&mut line, &mut pos, b"tx_aval_threshold = 0x");
            push_u8_hex(&mut line, &mut pos, val);
            print_line(sel, &line[..pos]);
        }
        2 => {
            let mut line = [0u8; 72];
            let mut pos = 0usize;
            push_str(&mut line, &mut pos, b"tx_aval_int_thr_mode = ");
            push_u8_dec(&mut line, &mut pos, mode);
            push_str(&mut line, &mut pos, b"(by sdio_tx_max_len)");
            print_line(sel, &line[..pos]);
        }
        _ => {}
    }
}

fn proc_set_read_tmp(buffer: *const c_void, count: usize, tmp: &mut [i8; PROC_SET_TMP]) -> bool {
    if count < 1 || count > PROC_SET_TMP || buffer.is_null() {
        return false;
    }
    let n =
        unsafe { rtw_rust_debug_proc_read_user_line(buffer, count, tmp.as_mut_ptr(), tmp.len()) };
    n > 0
}

#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_get_tx_quick_addba_req(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        emit_get_tx_quick_addba(m, adapter);
    }
    0
}

#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_set_tx_quick_addba_req(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    if count < 1 || count > PROC_SET_TMP {
        return EFAULT;
    }
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    let mut tmp = [0i8; PROC_SET_TMP];
    if proc_set_read_tmp(buffer, count, &mut tmp) && !adapter.is_null() {
        let mut enable = 0u32;
        let num = unsafe { rtw_rust_debug_proc_sscanf_u32(tmp.as_ptr() as *const u8, &mut enable) };
        if num == 1 {
            unsafe { rtw_rust_debug_set_tx_quick_addba_req(adapter, enable as U8) };
        }
    }
    count as c_long
}

#[cfg(all(
    any(config_rtw_debug, config_proc_debug),
    config_sdio_tx_enable_aval_int
))]
#[no_mangle]
pub extern "C" fn proc_get_tx_aval_th(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        emit_get_tx_aval_th(m, adapter);
    }
    0
}

#[cfg(all(
    any(config_rtw_debug, config_proc_debug),
    config_sdio_tx_enable_aval_int
))]
#[no_mangle]
pub extern "C" fn proc_set_tx_aval_th(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    if count < 1 || count > PROC_SET_TMP {
        return EFAULT;
    }
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    let mut tmp = [0i8; PROC_SET_TMP];
    if proc_set_read_tmp(buffer, count, &mut tmp) {
        let mut mode = 0u32;
        let mut threshold = 0u32;
        let num = unsafe {
            rtw_rust_debug_proc_sscanf_u32_pair(
                tmp.as_ptr() as *const u8,
                &mut mode,
                &mut threshold,
            )
        };
        if num >= 1 {
            unsafe { rtw_rust_debug_set_tx_aval_int_thr_mode(adapter, mode as U8) };
        }
        if num >= 2 {
            unsafe { rtw_rust_debug_set_tx_aval_int_thr_value(adapter, threshold as U8) };
        }
    }
    count as c_long
}

#[cfg(host_rtw_debug_proc_ampdu_test)]
#[no_mangle]
pub extern "C" fn host_proc_set_tx_quick_addba_req_from_buf(
    adapter: *mut c_void,
    buffer: *const u8,
    count: usize,
) -> c_long {
    proc_set_tx_quick_addba_req(
        core::ptr::null_mut(),
        buffer as *const c_void,
        count,
        core::ptr::null_mut(),
        adapter,
    )
}

#[cfg(all(host_rtw_debug_proc_ampdu_test, config_sdio_tx_enable_aval_int))]
#[no_mangle]
pub extern "C" fn host_proc_set_tx_aval_th_from_buf(
    adapter: *mut c_void,
    buffer: *const u8,
    count: usize,
) -> c_long {
    proc_set_tx_aval_th(
        core::ptr::null_mut(),
        buffer as *const c_void,
        count,
        core::ptr::null_mut(),
        adapter,
    )
}
