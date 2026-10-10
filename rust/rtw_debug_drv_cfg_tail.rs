// SPDX-License-Identifier: GPL-2.0
//! W3-138: `dump_drv_cfg` tail — iface/xmit/recv (HCI banners stay in C).

use super::{format_u32, print_kv, print_line};
use core::ffi::c_void;

const TAIL_MI_MBSSID: u32 = 1 << 0;
const TAIL_SWTIMER_TXBCN: u32 = 1 << 1;
const TAIL_FW_HANDLE_TXBCN: u32 = 1 << 2;
const TAIL_CLIENT_PORT: u32 = 1 << 3;
const TAIL_PCI_TX_POLL: u32 = 1 << 4;

extern "C" {
    fn rtw_rust_debug_dump_drv_cfg_hci_banners(sel: *mut c_void);
    fn rtw_rust_debug_drv_cfg_tail_values(
        flags: *mut u32,
        iface_number: *mut i32,
        limited_ap_num: *mut i32,
        up_mapping_rule: *mut i32,
        nr_xmitframe: *mut i32,
        nr_xmitbuff: *mut i32,
        max_xmitbuf_sz: *mut i32,
        nr_xmit_extbuff: *mut i32,
        max_xmit_extbuf_sz: *mut i32,
        max_cmdbuf_sz: *mut i32,
        nr_recvframe: *mut i32,
        nr_recvbuff: *mut i32,
        rtw_recvbuf_nr: *mut u32,
        max_recvbuf_sz: *mut i32,
    );
}

fn print_kv_i32(sel: *mut c_void, key: &[u8], val: i32) {
    print_kv(sel, key, val as u32);
}

fn print_recvbuff_line(sel: *mut c_void, nr_recvbuff: i32, rtw_recvbuf_nr: u32) {
    let mut line = [0u8; 64];
    let pfx = b"NR_RECVBUFF = ";
    line[..pfx.len()].copy_from_slice(pfx);
    let mut pos = pfx.len();
    let (d1, n1) = format_u32(nr_recvbuff as u32);
    for i in 0..n1 {
        if pos >= line.len().saturating_sub(24) {
            break;
        }
        line[pos] = d1[i];
        pos += 1;
    }
    let mid = b", rtw_recvbuf_nr = ";
    for i in 0..mid.len() {
        if pos >= line.len().saturating_sub(12) {
            break;
        }
        line[pos] = mid[i];
        pos += 1;
    }
    let (d2, n2) = format_u32(rtw_recvbuf_nr);
    for i in 0..n2 {
        if pos >= line.len().saturating_sub(1) {
            break;
        }
        line[pos] = d2[i];
        pos += 1;
    }
    print_line(sel, &line[..pos]);
}

#[no_mangle]
pub extern "C" fn rtw_rust_debug_dump_drv_cfg_tail(sel: *mut c_void) {
    unsafe {
        rtw_rust_debug_dump_drv_cfg_hci_banners(sel);
    }

    let mut flags = 0u32;
    let mut iface_number = 0i32;
    let mut limited_ap_num = 0i32;
    let mut up_mapping_rule = 0i32;
    let mut nr_xmitframe = 0i32;
    let mut nr_xmitbuff = 0i32;
    let mut max_xmitbuf_sz = 0i32;
    let mut nr_xmit_extbuff = 0i32;
    let mut max_xmit_extbuf_sz = 0i32;
    let mut max_cmdbuf_sz = 0i32;
    let mut nr_recvframe = 0i32;
    let mut nr_recvbuff = 0i32;
    let mut rtw_recvbuf_nr = 0u32;
    let mut max_recvbuf_sz = 0i32;

    unsafe {
        rtw_rust_debug_drv_cfg_tail_values(
            &mut flags,
            &mut iface_number,
            &mut limited_ap_num,
            &mut up_mapping_rule,
            &mut nr_xmitframe,
            &mut nr_xmitbuff,
            &mut max_xmitbuf_sz,
            &mut nr_xmit_extbuff,
            &mut max_xmit_extbuf_sz,
            &mut max_cmdbuf_sz,
            &mut nr_recvframe,
            &mut nr_recvbuff,
            &mut rtw_recvbuf_nr,
            &mut max_recvbuf_sz,
        );
    }

    print_kv_i32(sel, b"CONFIG_IFACE_NUMBER = ", iface_number);
    if flags & TAIL_MI_MBSSID != 0 {
        print_line(sel, b"CONFIG_MI_WITH_MBSSID_CAM");
    }
    if flags & TAIL_SWTIMER_TXBCN != 0 {
        print_line(sel, b"CONFIG_SWTIMER_BASED_TXBCN");
    }
    if flags & TAIL_FW_HANDLE_TXBCN != 0 {
        print_line(sel, b"CONFIG_FW_HANDLE_TXBCN");
        print_kv_i32(sel, b"CONFIG_LIMITED_AP_NUM = ", limited_ap_num);
    }
    if flags & TAIL_CLIENT_PORT != 0 {
        print_line(sel, b"CONFIG_CLIENT_PORT_CFG");
    }
    if flags & TAIL_PCI_TX_POLL != 0 {
        print_line(sel, b"CONFIG_PCI_TX_POLLING");
    }
    if up_mapping_rule == 1 {
        print_line(sel, b"CONFIG_RTW_UP_MAPPING_RULE = dscp");
    } else {
        print_line(sel, b"CONFIG_RTW_UP_MAPPING_RULE = tos");
    }

    print_line(sel, b"\n=== XMIT-INFO ===");
    print_kv_i32(sel, b"NR_XMITFRAME = ", nr_xmitframe);
    print_kv_i32(sel, b"NR_XMITBUFF = ", nr_xmitbuff);
    print_kv_i32(sel, b"MAX_XMITBUF_SZ = ", max_xmitbuf_sz);
    print_kv_i32(sel, b"NR_XMIT_EXTBUFF = ", nr_xmit_extbuff);
    print_kv_i32(sel, b"MAX_XMIT_EXTBUF_SZ = ", max_xmit_extbuf_sz);
    print_kv_i32(sel, b"MAX_CMDBUF_SZ = ", max_cmdbuf_sz);

    print_line(sel, b"\n=== RECV-INFO ===");
    print_kv_i32(sel, b"NR_RECVFRAME = ", nr_recvframe);
    print_recvbuff_line(sel, nr_recvbuff, rtw_recvbuf_nr);
    print_kv_i32(sel, b"MAX_RECVBUF_SZ = ", max_recvbuf_sz);
}
