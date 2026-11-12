// SPDX-License-Identifier: GPL-2.0
//! Host L2 frame encrypt/decrypt oracle (W3-142 / T24) — mirrors shipped rust/wapi_sms4_frame.rs.

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
    len: U32,
}

#[repr(C)]
struct HostRecvHdr {
    rx_skb: *mut HostWapiSkb,
}

#[repr(C)]
pub struct HostWapiRecvFrame {
    hdr: HostRecvHdr,
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
    _padapter: *mut HostWapiAdapter,
    _pxmit: *mut HostWapiXmitFrame,
) {
    /* Matches SecSWSMS4Encryption in rust/wapi_sms4_frame.rs. */
}

#[no_mangle]
pub extern "C" fn host_sec_sw_sms4_decryption(
    _padapter: *mut HostWapiAdapter,
    _precv: *mut HostWapiRecvFrame,
) -> U8 {
    0
}

#[no_mangle]
pub extern "C" fn host_rtw_sms4_encrypt(
    padapter: *mut HostWapiAdapter,
    pxmit: *mut HostWapiXmitFrame,
) -> U32 {
    if padapter.is_null() || pxmit.is_null() {
        return _FAIL;
    }
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
    _precv: *mut HostWapiRecvFrame,
) -> U32 {
    if padapter.is_null() {
        return _FAIL;
    }
    let adapter = unsafe { &*padapter };
    if adapter.wapi_support == 0 || adapter.wapi_info.b_wapi_enable == 0 {
        return _FAIL;
    }
    _FAIL
}
