// SPDX-License-Identifier: GPL-2.0
// Debug dump helpers for reserved page / FIFO (W4-122).

use core::ffi::{c_char, c_void};

type U8 = u8;
type U32 = u32;

const HAL_DEF_TX_PAGE_SIZE: i32 = 25;
const RTW_BUFDUMP_BSIZE: usize = 16;

const _DRV_ALWAYS_: u32 = 1;

extern "C" {
    fn rtw_rust_hal_com_print_line(sel: *mut c_void, line: *const c_char);
    fn rtw_hal_get_def_var(adapter: *mut c_void, def: i32, val: *mut c_void);
    #[cfg(config_rtw_debug)]
    static mut rtw_drv_log_level: U32;
    fn rtw_hal_get_rsvd_page(
        adapter: *mut c_void,
        page_offset: U32,
        page_num: U32,
        buffer: *mut U8,
        buffer_size: U32,
    ) -> i32;
}

#[cfg(not(host_hal_com_dump_rsvd_fifo_test))]
extern "C" {
    fn rtw_rust_hal_com_zvmalloc(sz: U32) -> *mut U8;
    fn rtw_rust_hal_com_vmfree(p: *mut c_void, sz: U32);
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_log_invalid_fifo_sel(fifo_sel: U8);
    fn rtw_halmac_dump_fifo(
        d: *mut c_void,
        fifo_sel: U8,
        addr: U32,
        size: U32,
        buffer: *mut U8,
    ) -> i32;
}

#[cfg(host_hal_com_dump_rsvd_fifo_test)]
extern "C" {
    fn rtw_zvmalloc(sz: U32) -> *mut U8;
    fn rtw_vmfree(p: *mut c_void, sz: U32);
    fn rtw_halmac_dump_fifo(
        d: *mut c_void,
        fifo_sel: U8,
        addr: U32,
        size: U32,
        buffer: *mut U8,
    ) -> i32;
}

fn alloc_buf(sz: U32) -> *mut U8 {
    #[cfg(not(host_hal_com_dump_rsvd_fifo_test))]
    {
        return unsafe { rtw_rust_hal_com_zvmalloc(sz) };
    }
    #[cfg(host_hal_com_dump_rsvd_fifo_test)]
    {
        return unsafe { rtw_zvmalloc(sz) };
    }
}

fn free_buf(p: *mut c_void, sz: U32) {
    #[cfg(not(host_hal_com_dump_rsvd_fifo_test))]
    unsafe {
        rtw_rust_hal_com_vmfree(p, sz);
    }
    #[cfg(host_hal_com_dump_rsvd_fifo_test)]
    unsafe {
        rtw_vmfree(p, sz);
    }
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

/// Lowercase hex, minimum four digits (matches `%04x` for typical FIFO addresses).
fn push_hex_lower(n: u32, out: &mut [u8], pos: &mut usize) {
    const HEX: &[u8; 16] = b"0123456789abcdef";
    let mut digits = [0u8; 8];
    let mut nd = 0usize;
    let mut v = n;
    if v == 0 {
        digits[nd] = b'0';
        nd = 1;
    } else {
        while v > 0 && nd < digits.len() {
            digits[nd] = HEX[(v & 0xf) as usize];
            nd += 1;
            v >>= 4;
        }
    }
    while nd < 4 {
        for i in (1..=nd).rev() {
            digits[i] = digits[i - 1];
        }
        digits[0] = b'0';
        nd += 1;
    }
    for i in (0..nd).rev() {
        if *pos < out.len() {
            out[*pos] = digits[i];
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

#[cfg(host_hal_com_dump_rsvd_fifo_test)]
fn dump_log_enabled() -> bool {
    true
}

#[cfg(all(config_rtw_debug, not(host_hal_com_dump_rsvd_fifo_test)))]
fn dump_log_enabled() -> bool {
    unsafe { _DRV_ALWAYS_ <= rtw_drv_log_level }
}

#[cfg(not(any(host_hal_com_dump_rsvd_fifo_test, config_rtw_debug)))]
fn dump_log_enabled() -> bool {
    false
}

/// Match `RTW_BUF_DUMP_SEL` byte layout; one `print_line` per 16-byte row so
/// `RTW_DBGDUMP` (`printk`) gets one `RTW: ` prefix per row like `KERN_CONT`.
fn dump_sel(sel: *mut c_void, hexdata: *const U8, hexdatalen: i32) {
    if hexdata.is_null() || hexdatalen <= 0 || !dump_log_enabled() {
        return;
    }
    let mut row = [0u8; 64];
    let mut row_pos = 0usize;
    let mut row_bytes = 0usize;

    for i in 0..hexdatalen {
        let val = unsafe { *hexdata.add(i as usize) };
        let tail = if ((i + 1) % 4) == 0 {
            b"  " as &[u8]
        } else {
            b" "
        };
        if !push_hex_u8(&mut row, &mut row_pos, val) {
            return;
        }
        let _ = push_str(&mut row, &mut row_pos, tail);
        row_bytes += 1;

        if row_bytes == RTW_BUFDUMP_BSIZE && (i + 1) < hexdatalen {
            let _ = push_byte(&mut row, &mut row_pos, b'\n');
            print_line(sel, &row[..row_pos]);
            row_pos = 0;
            row_bytes = 0;
        }
    }
    if row_bytes > 0 {
        let _ = push_byte(&mut row, &mut row_pos, b'\n');
        print_line(sel, &row[..row_pos]);
    }
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
    let buffer = alloc_buf(buf_size);
    if buffer.is_null() {
        print_line(sel, b"ERROR - rsvd_buf mem allocate failed\n");
        print_line(sel, b"==========================\n");
        return;
    }
    unsafe {
        rtw_hal_get_rsvd_page(adapter, page_offset as U32, page_num as U32, buffer, buf_size);
        dump_sel(sel, buffer, buf_size as i32);
        free_buf(buffer.cast(), buf_size);
    }
    print_line(sel, b"==========================\n");
}

fn rnd4(x: U32) -> U32 {
    ((x >> 2) + if x & 3 == 0 { 0 } else { 1 }) << 2
}

const FIFO_SEL_STR: &[&[u8]; 6] = &[
    b"TX",
    b"RX",
    b"RSVD_PAGE",
    b"REPORT",
    b"LLT",
    b"RXBUF_FW",
];

#[cfg(any(config_support_fifo_dump, host_hal_com_dump_rsvd_fifo_test))]
#[no_mangle]
pub extern "C" fn rtw_dump_fifo(
    sel: *mut c_void,
    adapter: *mut c_void,
    fifo_sel: U8,
    fifo_addr: U32,
    fifo_size: U32,
) {
    if fifo_sel > 5 {
        #[cfg(not(host_hal_com_dump_rsvd_fifo_test))]
        unsafe {
            rtw_rust_hal_com_log_invalid_fifo_sel(fifo_sel);
        }
        return;
    }
    print_line(sel, b"========= FIFO DUMP =========\n");
    let mut line = [0u8; 96];
    let mut pos = 0usize;
    let _ = push_str(&mut line, &mut pos, FIFO_SEL_STR[fifo_sel as usize]);
    let _ = push_str(&mut line, &mut pos, b" FIFO DUMP [start_addr:0x");
    push_hex_lower(fifo_addr, &mut line, &mut pos);
    let _ = push_str(&mut line, &mut pos, b" , size:");
    push_decimal(fifo_size, &mut line, &mut pos);
    let _ = push_str(&mut line, &mut pos, b"]\n");
    print_line(sel, &line[..pos]);

    let mut buff_size = 0u32;
    let buffer = if fifo_size != 0 {
        buff_size = rnd4(fifo_size);
        alloc_buf(buff_size)
    } else {
        core::ptr::null_mut()
    };
    if fifo_size != 0 && buffer.is_null() {
        buff_size = 0;
    }

    #[cfg(not(host_hal_com_dump_rsvd_fifo_test))]
    {
        let dvobj = unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) };
        unsafe {
            rtw_halmac_dump_fifo(dvobj, fifo_sel, fifo_addr, buff_size, buffer);
        }
    }

    #[cfg(host_hal_com_dump_rsvd_fifo_test)]
    unsafe {
        rtw_halmac_dump_fifo(
            core::ptr::null_mut(),
            fifo_sel,
            fifo_addr,
            buff_size,
            buffer,
        );
    }

    if !buffer.is_null() {
        unsafe {
            dump_sel(sel, buffer, fifo_size as i32);
            free_buf(buffer.cast(), buff_size);
        }
    }
    print_line(sel, b"==========================\n");
}

