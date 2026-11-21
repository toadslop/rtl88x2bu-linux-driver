// SPDX-License-Identifier: GPL-2.0
// Debug dump helpers for reserved page / FIFO (W4-122).

use core::ffi::{c_char, c_void};

type U8 = u8;
type U32 = u32;

const HAL_DEF_TX_PAGE_SIZE: i32 = 25;
const RTW_BUFDUMP_BSIZE: usize = 16;

extern "C" {
    fn rtw_rust_hal_com_print_line(sel: *mut c_void, line: *const c_char);
    fn rtw_hal_get_def_var(adapter: *mut c_void, def: i32, val: *mut c_void);
    fn rtw_zvmalloc(sz: U32) -> *mut U8;
    fn rtw_vmfree(p: *mut c_void, sz: U32);
    fn rtw_hal_get_rsvd_page(
        adapter: *mut c_void,
        page_offset: U32,
        page_num: U32,
        buffer: *mut U8,
        buffer_size: U32,
    ) -> i32;
}

fn print_line(sel: *mut c_void, line: &[u8]) {
    let mut buf = [0i8; 256];
    let n = core::cmp::min(line.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = line[i] as i8;
    }
    unsafe {
        rtw_rust_hal_com_print_line(sel, buf.as_ptr());
    }
}

fn push_decimal(mut n: u32, out: &mut [u8], pos: &mut usize) {
    if n == 0 {
        if *pos < out.len() {
            out[*pos] = b'0';
            *pos += 1;
        }
        return;
    }
    let mut digits = [0u8; 10];
    let mut nd = 0usize;
    while n > 0 {
        digits[nd] = b'0' + (n % 10) as u8;
        n /= 10;
        nd += 1;
    }
    while nd > 0 {
        nd -= 1;
        if *pos < out.len() {
            out[*pos] = digits[nd];
            *pos += 1;
        }
    }
}

fn push_byte(out: &mut [u8], pos: &mut usize, b: u8) -> bool {
    if *pos >= out.len() {
        return false;
    }
    out[*pos] = b;
    *pos += 1;
    true
}

fn push_str(out: &mut [u8], pos: &mut usize, s: &[u8]) -> bool {
    for &b in s {
        if !push_byte(out, pos, b) {
            return false;
        }
    }
    true
}

fn format_page_hdr(page_offset: U8, page_num: U8) -> (usize, [u8; 128]) {
    let mut line = [0u8; 128];
    let mut pos = 0usize;
    let _ = push_str(&mut line, &mut pos, b"page_offset:");
    push_decimal(page_offset as u32, &mut line, &mut pos);
    let _ = push_byte(&mut line, &mut pos, b',');
    let _ = push_byte(&mut line, &mut pos, b' ');
    let _ = push_str(&mut line, &mut pos, b"page_num:");
    push_decimal(page_num as u32, &mut line, &mut pos);
    let _ = push_byte(&mut line, &mut pos, b'\n');
    (pos, line)
}

fn get_tx_page_size(adapter: *mut c_void) -> U32 {
    let mut page_size: U32 = 0;
    unsafe {
        rtw_hal_get_def_var(
            adapter,
            HAL_DEF_TX_PAGE_SIZE,
            (&mut page_size as *mut U32).cast::<c_void>(),
        );
    }
    page_size
}

fn push_hex_u8(out: &mut [u8], pos: &mut usize, val: U8) -> bool {
    const HEX: &[u8; 16] = b"0123456789ABCDEF";
    if *pos + 2 > out.len() {
        return false;
    }
    out[*pos] = HEX[(val >> 4) as usize];
    out[*pos + 1] = HEX[(val & 0x0f) as usize];
    *pos += 2;
    true
}

fn dump_sel(sel: *mut c_void, hexdata: *const U8, hexdatalen: i32) {
    if hexdata.is_null() || hexdatalen <= 0 {
        return;
    }
    let mut line = [0u8; 512];
    let mut pos = 0usize;
    for i in 0..hexdatalen {
        let val = unsafe { *hexdata.add(i as usize) };
        if !push_hex_u8(&mut line, &mut pos, val) {
            break;
        }
        if ((i + 1) % 4) == 0 {
            let _ = push_str(&mut line, &mut pos, b"  ");
        } else {
            let _ = push_byte(&mut line, &mut pos, b' ');
        }
        if (i + 1 < hexdatalen) && ((i + 1) % RTW_BUFDUMP_BSIZE as i32) == 0 {
            let _ = push_byte(&mut line, &mut pos, b'\n');
        }
    }
    let _ = push_byte(&mut line, &mut pos, b'\n');
    print_line(sel, &line[..pos]);
}

#[cfg(any(
    config_rtw_debug,
    config_proc_debug,
    host_hal_com_dump_rsvd_fifo_test
))]
#[no_mangle]
pub extern "C" fn rtw_dump_rsvd_page(
    sel: *mut c_void,
    adapter: *mut c_void,
    page_offset: U8,
    page_num: U8,
) {
    if page_num == 0 {
        return;
    }
    print_line(sel, b"======= RSVD PAGE DUMP =======\n");
    let (n, hdr) = format_page_hdr(page_offset, page_num);
    print_line(sel, &hdr[..n]);

    let page_size = get_tx_page_size(adapter);
    if page_size == 0 {
        print_line(sel, b"ERROR - Tx page size is zero ??\n");
        print_line(sel, b"==========================\n");
        return;
    }
    let buf_size = page_size.wrapping_mul(page_num as U32);
    let buffer = unsafe { rtw_zvmalloc(buf_size) };
    if buffer.is_null() {
        print_line(sel, b"ERROR - rsvd_buf mem allocate failed\n");
        print_line(sel, b"==========================\n");
        return;
    }
    unsafe {
        rtw_hal_get_rsvd_page(adapter, page_offset as U32, page_num as U32, buffer, buf_size);
        dump_sel(sel, buffer, buf_size as i32);
        rtw_vmfree(buffer.cast(), buf_size);
    }
    print_line(sel, b"==========================\n");
}
