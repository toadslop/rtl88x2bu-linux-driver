// SPDX-License-Identifier: GPL-2.0
//! Host L2 frame encrypt/decrypt oracle (W3-142) — mirrors tests/host/wapi_sms4 C oracle.

#![allow(non_snake_case)]

type U8 = u8;
type U16 = u16;
type U32 = u32;

const HOST_TXDESC_SIZE: usize = 48;
const HOST_MAC_HDR: usize = 24;
const HOST_EXT_LEN: usize = 18;
const HOST_MIC_LEN: usize = 16;
const _SUCCESS: U32 = 1;
const _FAIL: U32 = 0;

#[repr(C)]
struct HostWapiKey {
    data_key: [U8; 16],
    key_id: U8,
    b_set: U8,
}

#[repr(C)]
struct HostWapiStaInfo {
    next: *mut HostWapiStaInfo,
    peer_mac: [U8; 6],
    usk: HostWapiKey,
    last_tx_unicast_pn: [U8; 16],
}

#[repr(C)]
struct HostWapiInfo {
    sta_used_head: *mut HostWapiStaInfo,
    b_wapi_enable: U8,
    extra_prefix_len: i32,
    extra_postfix_len: i32,
}

#[repr(C)]
pub struct HostWapiAdapter {
    wapi_support: U8,
    wapi_info: HostWapiInfo,
}

#[repr(C)]
struct HostPktAttrib {
    hdrlen: U16,
    pktlen: U16,
    iv_len: U8,
    icv_len: U8,
}

#[repr(C)]
pub struct HostWapiXmitFrame {
    buf_addr: *mut U8,
    attrib: HostPktAttrib,
}

#[repr(C)]
struct HostWapiSkb {
    data: *mut U8,
    len: usize,
}

#[repr(C)]
struct HostRecvHdr {
    rx_skb: *mut HostWapiSkb,
}

#[repr(C)]
pub struct HostWapiRecvFrame {
    hdr: HostRecvHdr,
}

extern "C" {
    fn host_wapi_sms4_encryption(
        key: *const U8,
        iv: *const U8,
        input: *const U8,
        input_length: U16,
        output: *mut U8,
        output_length: *mut U16,
    );
    fn host_wapi_sms4_decryption(
        key: *const U8,
        iv: *const U8,
        input: *const U8,
        input_length: U16,
        output: *mut U8,
        output_length: *mut U16,
    );
}

fn find_sta<'a>(info: &'a HostWapiInfo, mac: &[U8; 6]) -> Option<&'a mut HostWapiStaInfo> {
    let mut cur = info.sta_used_head;
    while !cur.is_null() {
        let sta = unsafe { &mut *cur };
        if &sta.peer_mac == mac {
            return Some(sta);
        }
        cur = sta.next;
    }
    None
}

#[no_mangle]
pub extern "C" fn host_wapi_sms4_frame_adapter_init(a: *mut HostWapiAdapter) {
    if a.is_null() {
        return;
    }
    let a = unsafe { &mut *a };
    unsafe {
        core::ptr::write_bytes(
            a as *mut HostWapiAdapter as *mut u8,
            0,
            core::mem::size_of::<HostWapiAdapter>(),
        );
    }
    a.wapi_support = 1;
    a.wapi_info.b_wapi_enable = 1;
    a.wapi_info.extra_prefix_len = HOST_EXT_LEN as i32;
    a.wapi_info.extra_postfix_len = HOST_MIC_LEN as i32;
}

#[no_mangle]
pub extern "C" fn host_sec_sw_sms4_encryption(
    padapter: *mut HostWapiAdapter,
    pxmit: *mut HostWapiXmitFrame,
) {
    if padapter.is_null() || pxmit.is_null() {
        return;
    }
    let adapter = unsafe { &*padapter };
    let xmit = unsafe { &*pxmit };
    if xmit.buf_addr.is_null() {
        return;
    }
    let info = &adapter.wapi_info;
    let pframe = unsafe { xmit.buf_addr.add(HOST_TXDESC_SIZE) };
    let data_offset = xmit.attrib.hdrlen as usize + xmit.attrib.iv_len as usize;
    let ra = unsafe { core::slice::from_raw_parts(pframe.add(4), 6) };
    let mut mac = [0u8; 6];
    mac.copy_from_slice(ra);
    let sta = match find_sta(info, &mac) {
        Some(s) => s,
        None => return,
    };
    let crypt_len = xmit.attrib.pktlen - xmit.attrib.icv_len as u16;
    let mut out_len: U16 = 0;
    unsafe {
        host_wapi_sms4_encryption(
            sta.usk.data_key.as_ptr(),
            sta.last_tx_unicast_pn.as_ptr(),
            pframe.add(data_offset),
            crypt_len,
            pframe.add(data_offset),
            &mut out_len,
        );
    }
}

#[no_mangle]
pub extern "C" fn host_sec_sw_sms4_decryption(
    padapter: *mut HostWapiAdapter,
    precv: *mut HostWapiRecvFrame,
) -> U8 {
    if padapter.is_null() || precv.is_null() {
        return 0;
    }
    let adapter = unsafe { &*padapter };
    let info = &adapter.wapi_info;
    let hdr = unsafe { &(*precv).hdr };
    if hdr.rx_skb.is_null() {
        return 0;
    }
    let skb = unsafe { &mut *hdr.rx_skb };
    if skb.data.is_null() {
        return 0;
    }
    let iv_offset = HOST_MAC_HDR;
    let data_offset = iv_offset + info.extra_prefix_len as usize;
    let ta: [U8; 6] = unsafe { core::ptr::read_unaligned(skb.data.add(10) as *const [U8; 6]) };
    let sta = match find_sta(info, &ta) {
        Some(s) => s,
        None => return 0,
    };
    if unsafe { *skb.data.add(4) } & 1 != 0 {
        return 0;
    }
    let p_recv_pn = unsafe { skb.data.add(iv_offset + 2) };
    let p_sec = unsafe { skb.data.add(data_offset) };
    let payload_len = skb.len - data_offset - info.extra_postfix_len as usize;
    let mut out_len: U16 = 0;
    unsafe {
        host_wapi_sms4_decryption(
            sta.usk.data_key.as_ptr(),
            p_recv_pn,
            p_sec,
            payload_len as U16,
            p_sec,
            &mut out_len,
        );
        core::ptr::copy(
            skb.data,
            skb.data.add(info.extra_prefix_len as usize),
            iv_offset,
        );
        skb.data = skb.data.add(info.extra_prefix_len as usize);
        skb.len -= info.extra_prefix_len as usize;
    }
    1
}

#[no_mangle]
pub extern "C" fn host_rtw_sms4_encrypt(
    padapter: *mut HostWapiAdapter,
    pxmit: *mut HostWapiXmitFrame,
) -> U32 {
    let adapter = unsafe { &*padapter };
    if adapter.wapi_support == 0 || adapter.wapi_info.b_wapi_enable == 0 {
        return _FAIL;
    }
    if unsafe { (*pxmit).buf_addr.is_null() } {
        return _FAIL;
    }
    host_sec_sw_sms4_encryption(padapter, pxmit);
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn host_rtw_sms4_decrypt(
    padapter: *mut HostWapiAdapter,
    precv: *mut HostWapiRecvFrame,
) -> U32 {
    let adapter = unsafe { &*padapter };
    if adapter.wapi_support == 0 || adapter.wapi_info.b_wapi_enable == 0 {
        return _FAIL;
    }
    if host_sec_sw_sms4_decryption(padapter, precv) == 0 {
        return _FAIL;
    }
    _SUCCESS
}
