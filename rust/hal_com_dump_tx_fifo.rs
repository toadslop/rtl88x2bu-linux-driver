// SPDX-License-Identifier: GPL-2.0
// dump_TX_FIFO WoW debug helper (W4-42).

use core::ffi::{c_char, c_void};

type U8 = u8;
type U16 = u16;
type U32 = u32;

extern "C" {
    fn rtw_rust_hal_com_log_info(line: *const c_char);
    fn rtw_rust_hal_com_read8(adapter: *mut c_void, addr: U32) -> U8;
    fn rtw_rust_hal_com_write8(adapter: *mut c_void, addr: U32, val: U8);
    fn rtw_rust_hal_com_read32(adapter: *mut c_void, addr: U32) -> U32;
    fn rtw_rust_hal_com_write32(adapter: *mut c_void, addr: U32, val: U32);
    fn rtw_rust_hal_com_printk_reg_pair(a: U32, b: U32);
    fn rtw_rust_hal_com_printk_reg_pair_nl(a: U32, b: U32);
}

fn log_bytes(line: &[u8]) {
    let mut buf = [0i8; 128];
    let n = core::cmp::min(line.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = line[i] as i8;
    }
    unsafe {
        rtw_rust_hal_com_log_info(buf.as_ptr());
    }
}

fn push_hex_u8(out: &mut [u8], pos: &mut usize, val: U8) -> bool {
    const HEX: &[u8; 16] = b"0123456789abcdef";
    if *pos + 2 > out.len() {
        return false;
    }
    out[*pos] = HEX[(val >> 4) as usize];
    out[*pos + 1] = HEX[(val & 0x0f) as usize];
    *pos += 2;
    true
}

fn log_reg_u8(prefix: &[u8], val: U8) {
    let mut line = [0u8; 32];
    let mut pos = 0usize;
    for &b in prefix {
        if pos >= line.len() {
            return;
        }
        line[pos] = b;
        pos += 1;
    }
    if !push_hex_u8(&mut line, &mut pos, val) {
        return;
    }
    if pos < line.len() {
        line[pos] = b'\n';
        pos += 1;
    }
    log_bytes(&line[..pos]);
}

#[no_mangle]
pub extern "C" fn dump_TX_FIFO(adapter: *mut c_void, page_num: U8, page_size: U16) {
    if page_num == 0 {
        log_bytes(b"!!dump_TX_FIFO: incorrect input page_num paramter!\n");
        return;
    }
    if page_size < 128 || page_size > 512 {
        log_bytes(b"!!dump_TX_FIFO: incorrect input page_size paramter!\n");
        return;
    }

    log_bytes(b"+dump_TX_FIFO+\n");

    let val = unsafe { rtw_rust_hal_com_read8(adapter, 0x106) };
    unsafe {
        rtw_rust_hal_com_write8(adapter, 0x106, 0x69);
    }
    log_reg_u8(b"0x106: 0x", val);

    let base = unsafe { rtw_rust_hal_com_read8(adapter, 0x209) };
    log_reg_u8(b"0x209: 0x", base);

    let count = (page_size / 8) as U32;
    let addr = (base as U32 * page_size as U32) / 8;
    let limit = page_num as U32 * count;
    let mut i = 0u32;
    while i < limit {
        unsafe {
            rtw_rust_hal_com_write32(adapter, 0x140, addr + i);
            let a = rtw_rust_hal_com_read32(adapter, 0x144);
            let b = rtw_rust_hal_com_read32(adapter, 0x148);
            rtw_rust_hal_com_printk_reg_pair(a, b);
            rtw_rust_hal_com_write32(adapter, 0x140, addr + i + 1);
            let c = rtw_rust_hal_com_read32(adapter, 0x144);
            let d = rtw_rust_hal_com_read32(adapter, 0x148);
            rtw_rust_hal_com_printk_reg_pair_nl(c, d);
        }
        i += 2;
    }
}
