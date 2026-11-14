// SPDX-License-Identifier: GPL-2.0
// CONFIG_FW_C2H_PKT pre/handle dispatch (W4-77).

use core::ffi::c_void;

type U8 = u8;
type U16 = u16;
type S32 = i32;

const SUCCESS: U8 = 1;
const FAIL: U8 = 0;
const TRUE: S32 = 1;

static FN_PRE: &[u8] = b"rtw_hal_c2h_pkt_pre_hdl\0";
static FN_HDL: &[u8] = b"rtw_hal_c2h_pkt_hdl\0";

extern "C" {
    #[cfg(config_rtw_halmac)]
    fn rtw_rust_hal_com_hal_mac_c2h_handler(adapter: *mut c_void, buf: *mut U8, len: U16);
    fn rtw_hal_c2h_pkt_hdr_parse(
        adapter: *mut c_void,
        buf: *mut U8,
        len: U16,
        id: *mut U8,
        seq: *mut U8,
        plen: *mut U8,
        payload: *mut *mut U8,
    ) -> U8;
    fn rtw_hal_c2h_id_handle_directly(
        adapter: *mut c_void,
        id: U8,
        seq: U8,
        plen: U8,
        payload: *mut U8,
    ) -> S32;
    fn rtw_hal_c2h_handler(
        adapter: *mut c_void,
        id: U8,
        seq: U8,
        plen: U8,
        payload: *mut U8,
    ) -> S32;
    fn rtw_c2h_packet_wk_cmd(adapter: *mut c_void, buf: *mut U8, len: U16) -> U8;
    fn rtw_rust_hal_com_err_c2h_pkt_parse(func: *const U8, buf: *mut c_void, len: U16);
    fn rtw_rust_hal_com_log_c2h_pkt_status(
        func: *const U8,
        id: U8,
        seq: U8,
        plen: U8,
        action: *const U8,
        result: *const U8,
    );
    fn rtw_rust_hal_com_dump_c2h_pkt_buf(buf: *mut U8, len: U16);
    fn rtw_rust_hal_com_dbg_c2h_pkt(is_pre: U8) -> i32;
    #[cfg(config_wowlan)]
    fn rtw_rust_ps_rdy_get_wowlan_mode(adapter: *mut c_void) -> U8;
}

#[cfg(not(config_rtw_halmac))]
#[inline]
fn wowlan_skip(adapter: *mut c_void) -> bool {
    #[cfg(config_wowlan)]
    return unsafe { rtw_rust_ps_rdy_get_wowlan_mode(adapter) == 1 };
    #[cfg(not(config_wowlan))]
    {
        let _ = adapter;
        false
    }
}

#[cfg(not(config_rtw_halmac))]
fn log_exit(
    func: *const U8,
    id: U8,
    seq: U8,
    plen: U8,
    action: &[u8],
    ret: S32,
    dbg: i32,
    buf: *mut U8,
    len: U16,
    extra_log: bool,
) {
    let result = if ret == SUCCESS as S32 { b"ok\0" } else { b"fail\0" };
    if extra_log || ret != SUCCESS as S32 || dbg > 0 {
        unsafe {
            rtw_rust_hal_com_log_c2h_pkt_status(
                func,
                id,
                seq,
                plen,
                action.as_ptr(),
                result.as_ptr(),
            );
        }
        if dbg >= 2 {
            unsafe {
                rtw_rust_hal_com_dump_c2h_pkt_buf(buf, len);
            }
        }
    }
}

#[cfg(not(config_rtw_halmac))]
fn pre_hdl(adapter: *mut c_void, buf: *mut U8, len: U16) {
    let mut id = 0u8;
    let mut seq = 0u8;
    let mut plen = 0u8;
    let mut payload = core::ptr::null_mut();
    let ok = unsafe {
        rtw_hal_c2h_pkt_hdr_parse(adapter, buf, len, &mut id, &mut seq, &mut plen, &mut payload)
            == SUCCESS
    };
    if !ok {
        unsafe { rtw_rust_hal_com_err_c2h_pkt_parse(FN_PRE.as_ptr(), buf as *mut c_void, len) };
        return;
    }
    let hdl_here =
        unsafe { rtw_hal_c2h_id_handle_directly(adapter, id, seq, plen, payload) == TRUE };
    let ret = if hdl_here {
        unsafe { rtw_hal_c2h_handler(adapter, id, seq, plen, payload) }
    } else {
        unsafe { rtw_c2h_packet_wk_cmd(adapter, buf, len) as S32 }
    };
    let action = if hdl_here { b"handle\0" } else { b"enqueue\0" };
    let dbg = unsafe { rtw_rust_hal_com_dbg_c2h_pkt(1) };
    log_exit(
        FN_PRE.as_ptr(),
        id,
        seq,
        plen,
        action,
        ret,
        dbg,
        buf,
        len,
        false,
    );
}

#[cfg(not(config_rtw_halmac))]
fn pkt_hdl(adapter: *mut c_void, buf: *mut U8, len: U16) {
    let mut id = 0u8;
    let mut seq = 0u8;
    let mut plen = 0u8;
    let mut payload = core::ptr::null_mut();
    let ok = unsafe {
        rtw_hal_c2h_pkt_hdr_parse(adapter, buf, len, &mut id, &mut seq, &mut plen, &mut payload)
            == SUCCESS
    };
    if !ok {
        unsafe { rtw_rust_hal_com_err_c2h_pkt_parse(FN_HDL.as_ptr(), buf as *mut c_void, len) };
        return;
    }
    let bypass = wowlan_skip(adapter);
    let ret = if bypass {
        SUCCESS as S32
    } else {
        unsafe { rtw_hal_c2h_handler(adapter, id, seq, plen, payload) }
    };
    let action = if bypass { b"bypass\0" } else { b"handle\0" };
    let dbg = unsafe { rtw_rust_hal_com_dbg_c2h_pkt(0) };
    log_exit(
        FN_HDL.as_ptr(),
        id,
        seq,
        plen,
        action,
        ret,
        dbg,
        buf,
        len,
        bypass,
    );
}

#[no_mangle]
pub extern "C" fn rtw_hal_c2h_pkt_pre_hdl(adapter: *mut c_void, buf: *mut U8, len: U16) {
    #[cfg(config_rtw_halmac)]
    {
        let _ = (adapter, buf, len);
    }
    #[cfg(not(config_rtw_halmac))]
    pre_hdl(adapter, buf, len);
}

#[no_mangle]
pub extern "C" fn rtw_hal_c2h_pkt_hdl(adapter: *mut c_void, buf: *mut U8, len: U16) {
    #[cfg(config_rtw_halmac)]
    unsafe {
        rtw_rust_hal_com_hal_mac_c2h_handler(adapter, buf, len);
    }
    #[cfg(not(config_rtw_halmac))]
    pkt_hdl(adapter, buf, len);
}