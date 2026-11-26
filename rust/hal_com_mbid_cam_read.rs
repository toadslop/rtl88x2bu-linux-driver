// SPDX-License-Identifier: GPL-2.0
// CONFIG_MBSSID_CAM — HW CAM read + dump (W4-141).

use core::ffi::{c_char, c_void};

type U8 = u8;
type U16 = u16;
type U32 = u32;

const REG_MBIDCAMCFG_1: U32 = 0x0628;
const REG_MBIDCAMCFG_2: U32 = 0x062c;
const BIT_MBIDCAM_POLL: U32 = 1 << 31;
const MBIDCAM_ADDR_MASK: U32 = 0x1f;
const MBIDCAM_ADDR_SHIFT: U32 = 24;
const ETH_ALEN: usize = 6;
const TOTAL_MBID_CAM_NUM: U8 = 8;

extern "C" {
    fn rtw_rust_hal_com_read32(adapter: *mut c_void, addr: U32) -> U32;
    fn rtw_rust_hal_com_read16(adapter: *mut c_void, addr: U32) -> U16;
    fn rtw_rust_hal_com_write32(adapter: *mut c_void, addr: U32, val: U32);
    fn rtw_rust_hal_com_cannot_run(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_print_line(sel: *mut c_void, line: *const c_char);
    fn _rtw_memset(dst: *mut c_void, val: i32, len: usize) -> *mut c_void;
}

fn read_line(sel: *mut c_void, bytes: &[u8]) {
    let mut buf = [0i8; 128];
    let n = core::cmp::min(bytes.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = bytes[i] as i8;
    }
    unsafe {
        rtw_rust_hal_com_print_line(sel, buf.as_ptr());
    }
}

fn read_print_u8(sel: *mut c_void, mut n: U8) {
    let mut digits = [0u8; 3];
    let mut len = 0usize;
    if n == 0 {
        read_line(sel, b"0");
        return;
    }
    while n > 0 {
        digits[len] = b'0' + (n % 10);
        len += 1;
        n /= 10;
    }
    let mut line = [0u8; 4];
    for i in 0..len {
        line[i] = digits[len - 1 - i];
    }
    read_line(sel, &line[..len]);
}

fn read_print_hex8(sel: *mut c_void, n: U8) {
    const HEX: &[u8; 16] = b"0123456789abcdef";
    let line = [HEX[(n >> 4) as usize], HEX[(n & 0xf) as usize]];
    read_line(sel, &line);
}

fn read_print_cstr(sel: *mut c_void, p: *const c_char) {
    if p.is_null() {
        return;
    }
    unsafe {
        let mut cur = p;
        while *cur != 0 {
            let byte = [*cur as u8];
            read_line(sel, &byte);
            cur = cur.add(1);
        }
    }
}

fn read_mbssid_cam_inner(adapter: *mut c_void, cam_addr: U8, mac: *mut U8) {
    if adapter.is_null() || mac.is_null() {
        return;
    }
    if unsafe { rtw_rust_hal_com_cannot_run(adapter) } != 0 {
        return;
    }
    unsafe {
        let poll_cmd = BIT_MBIDCAM_POLL
            | ((u32::from(cam_addr) & MBIDCAM_ADDR_MASK) << MBIDCAM_ADDR_SHIFT);
        rtw_rust_hal_com_write32(adapter, REG_MBIDCAMCFG_2, poll_cmd);

        let mut poll: U8 = 1;
        let mut cam_ready = false;
        loop {
            if (rtw_rust_hal_com_read32(adapter, REG_MBIDCAMCFG_2) & BIT_MBIDCAM_POLL) == 0 {
                cam_ready = true;
                break;
            }
            poll = poll.wrapping_add(1);
            if poll % 10 == 0 || rtw_rust_hal_com_cannot_run(adapter) != 0 {
                break;
            }
        }
        if !cam_ready {
            return;
        }

        let cam_data1 = rtw_rust_hal_com_read32(adapter, REG_MBIDCAMCFG_1);
        *mac = (cam_data1 & 0xff) as U8;
        *mac.add(1) = ((cam_data1 >> 8) & 0xff) as U8;
        *mac.add(2) = ((cam_data1 >> 16) & 0xff) as U8;
        *mac.add(3) = ((cam_data1 >> 24) & 0xff) as U8;

        let cam_data2 = rtw_rust_hal_com_read16(adapter, REG_MBIDCAMCFG_2);
        *mac.add(4) = (cam_data2 & 0xff) as U8;
        *mac.add(5) = ((cam_data2 >> 8) & 0xff) as U8;
    }
}

#[cfg(host_hal_com_mbid_cam_read_test)]
#[no_mangle]
pub extern "C" fn read_mbssid_cam_probe(adapter: *mut c_void, cam_addr: U8, mac: *mut U8) {
    read_mbssid_cam_inner(adapter, cam_addr, mac);
}

#[cfg(any(config_mbssid_cam, host_hal_com_mbid_cam_read_test))]
#[no_mangle]
pub extern "C" fn rtw_mbid_cam_dump(
    sel: *mut c_void,
    fun_name: *const c_char,
    adapter: *mut c_void,
) -> i32 {
    if adapter.is_null() {
        return 0;
    }
    read_line(sel, b"\n== MBSSID HW-CAM DUMP (");
    read_print_cstr(sel, fun_name);
    read_line(sel, b")==\n");

    let mut mac_addr = [0u8; ETH_ALEN];
    for i in 0..TOTAL_MBID_CAM_NUM {
        read_line(sel, b"CAM_ID = ");
        read_print_u8(sel, i);
        read_line(sel, b"\t");
        unsafe {
            _rtw_memset(mac_addr.as_mut_ptr().cast(), 0, ETH_ALEN);
        }
        read_mbssid_cam_inner(adapter, i, mac_addr.as_mut_ptr());
        read_line(sel, b"MAC Addr:");
        for (j, byte) in mac_addr.iter().enumerate() {
            if j > 0 {
                read_line(sel, b":");
            }
            read_print_hex8(sel, *byte);
        }
        read_line(sel, b"\n");
    }
    0
}