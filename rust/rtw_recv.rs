// SPDX-License-Identifier: GPL-2.0
//! Recv leaf helpers — Rust port of `core/rtw_recv_rest.c` (W3-39).
//!
//! Defensive null / negative-tid checks and a null `rx_data` early return are
//! intentional additions over the C originals (which fault on invalid inputs).
//! Production call sites always pass live `sta` / populated frames.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(not(host_recv_test))]
use core::ffi::{c_int, c_uint, c_void};
#[cfg(host_recv_test)]
use std::os::raw::{c_int, c_uint, c_void};

const _TRUE: c_int = 1;
const _FALSE: c_int = 0;
const _FAIL: c_int = -1;
const MAX_CONTINUAL_NORXPACKET_COUNT: c_int = 4;
// Matches `sizeof(struct rtw_ieee80211_hdr_3addr)` in `include/rtw_ieee80211.h`.
const HDR_3ADDR_SZ: usize = 24;

#[cfg(host_recv_test)]
const TID_NUM: usize = 16;
#[cfg(host_recv_test)]
const ETH_ALEN: usize = 6;
#[cfg(host_recv_test)]
const SNAP_SIZE: usize = 6;
#[cfg(host_recv_test)]
const ETHHDR_SZ: usize = 14;

#[cfg(host_recv_test)]
const RTW_RX_LLC_KEEP: u8 = 0;
#[cfg(host_recv_test)]
const RTW_RX_LLC_REMOVE: u8 = 1;

#[cfg(host_recv_test)]
const ETH_P_AARP: u16 = 0x80f3;
#[cfg(host_recv_test)]
const ETH_P_IPX: u16 = 0x8137;

#[cfg(host_recv_test)]
const _SUCCESS: c_int = 0;

#[cfg(not(host_recv_test))]
const _SUCCESS: c_int = 0;

#[cfg(host_recv_test)]
const WIFI_AP_STATE: c_int = 0x00000010;
#[cfg(host_recv_test)]
const WIFI_MP_STATE: c_int = 0x00010000;
#[cfg(host_recv_test)]
const WIFI_MONITOR_STATE: c_int = -2147483648;

#[cfg(host_recv_test)]
const WIFI_STATION_STATE: c_int = 0x00000008;

#[cfg(host_recv_test)]
const _AES_: u8 = 0x04;

#[cfg(host_recv_test)]
static RFC1042_HEADER: [u8; 6] = [0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00];
#[cfg(host_recv_test)]
static BRIDGE_TUNNEL_HEADER: [u8; 6] = [0xaa, 0xaa, 0x03, 0x00, 0x00, 0xf8];

#[cfg(host_recv_test)]
#[repr(C)]
pub struct StaStatsHost {
    pub duplicate_cnt: u32,
}

#[cfg(host_recv_test)]
#[repr(C)]
pub struct StainfoRxcache {
    pub tid_rxseq: [u16; TID_NUM],
    pub iv: [[u8; 8]; TID_NUM],
    pub last_tid: u8,
}

#[cfg(host_recv_test)]
#[repr(C)]
pub struct StaRecvPriv {
    pub rxcache: StainfoRxcache,
    pub bmc_tid_rxseq: [u16; TID_NUM],
    pub nonqos_rxseq: u16,
    pub nonqos_bmc_rxseq: u16,
}

#[cfg(host_recv_test)]
#[repr(C)]
pub struct StaInfo {
    pub continual_no_rx_packet: [c_int; TID_NUM],
    pub padapter: *mut Adapter,
    pub sta_recvpriv: StaRecvPriv,
    pub sta_stats: StaStatsHost,
}

#[cfg(host_recv_test)]
#[repr(C)]
pub struct RxPktAttrib {
    pub hdrlen: u8,
    pub encrypt: u8,
    pub iv_len: u8,
    pub icv_len: u8,
    pub qos: u8,
    pub priority: u8,
    pub seq_num: u16,
    pub frag_num: u8,
    pub dst: [u8; ETH_ALEN],
    pub src: [u8; ETH_ALEN],
    pub ra: [u8; ETH_ALEN],
}

#[cfg(host_recv_test)]
#[repr(C)]
pub struct RecvFrameHdr {
    pub _pad: u32,
    pub len: c_uint,
    pub rx_head: *mut u8,
    pub rx_data: *mut u8,
    pub rx_tail: *mut u8,
    pub rx_end: *mut u8,
    pub attrib: RxPktAttrib,
    pub adapter: *mut Adapter,
    pub psta: *mut StaInfo,
}

#[cfg(host_recv_test)]
#[repr(C)]
pub struct RecvFrame {
    pub hdr: RecvFrameHdr,
}

#[cfg(host_recv_test)]
#[repr(C)]
pub struct MlmePriv {
    pub fw_state: c_int,
}

#[cfg(host_recv_test)]
#[repr(C)]
pub struct SecurityPriv {
    pub iv_seq: [[u8; 8]; 4],
}

#[cfg(host_recv_test)]
#[repr(C)]
pub struct Adapter {
    pub mlmepriv: MlmePriv,
    pub host_linked: u8,
    pub securitypriv: SecurityPriv,
}

#[cfg(not(host_recv_test))]
mod kernel {
    use super::*;
    extern "C" {
        fn rtw_rust_recv_continual_no_rx(sta: *mut c_void, tid: c_int) -> *mut c_int;
        fn rtw_rust_atomic_inc_return(v: *mut c_int) -> c_int;
        fn rtw_rust_atomic_set(v: *mut c_int, val: c_int);
        fn rtw_rust_del_wfd_ie(ies: *mut u8, len: c_uint, msg: *const u8) -> c_uint;
        fn rtw_rust_recv_frame_rx_data(rframe: *mut c_void) -> *mut u8;
        fn rtw_rust_recv_frame_len(rframe: *mut c_void) -> c_uint;
        fn rtw_rust_recv_frame_set_len(rframe: *mut c_void, len: c_uint);
        fn rtw_rust_recv_frame_attrib(rframe: *mut c_void) -> *mut c_void;
        fn rtw_rust_recv_frame_pull_tail(rframe: *mut c_void, sz: c_int);
        fn rtw_rust_recv_frame_pull(rframe: *mut c_void, sz: c_int) -> *mut u8;
        fn rtw_rust_rframe_set_os_pkt(rframe: *mut c_void);
        fn rtw_rust_adapter_fw_state(adapter: *mut c_void) -> c_int;
        fn rtw_rust_adapter_linked(adapter: *mut c_void) -> u8;
        fn rtw_rust_adapter_simple_config(adapter: *mut c_void) -> u8;
        fn rtw_rust_attrib_mesh_ctrl_len(attrib: *mut c_void) -> u8;
        fn rtw_rust_recv_frame_psta(rframe: *mut c_void) -> *mut c_void;
        fn rtw_rust_recv_frame_adapter(rframe: *mut c_void) -> *mut c_void;
        fn rtw_rust_recv_tid_rxseq(sta: *mut c_void, tid: c_int) -> *mut u16;
        fn rtw_rust_recv_bmc_tid_rxseq(sta: *mut c_void, tid: c_int) -> *mut u16;
        fn rtw_rust_recv_nonqos_rxseq(sta: *mut c_void) -> *mut u16;
        fn rtw_rust_recv_nonqos_bmc_rxseq(sta: *mut c_void) -> *mut u16;
        fn rtw_rust_recv_sta_iv(sta: *mut c_void, tid: c_int) -> *mut u8;
        fn rtw_rust_recv_sta_last_tid(sta: *mut c_void) -> *mut u8;
        fn rtw_rust_recv_sta_duplicate_cnt(sta: *mut c_void) -> *mut u32;
        fn rtw_rust_recv_sec_iv_seq(adapter: *mut c_void, key_id: u8) -> *mut u8;
    }
    pub(super) unsafe fn continual_no_rx(sta: *mut c_void, tid: c_int) -> *mut c_int {
        unsafe { rtw_rust_recv_continual_no_rx(sta, tid) }
    }
    pub(super) unsafe fn atomic_inc(v: *mut c_int) -> c_int {
        unsafe { rtw_rust_atomic_inc_return(v) }
    }
    pub(super) unsafe fn atomic_set(v: *mut c_int, val: c_int) {
        unsafe { rtw_rust_atomic_set(v, val) };
    }
    pub(super) unsafe fn del_wfd_ie(ies: *mut u8, len: c_uint) -> c_uint {
        unsafe { rtw_rust_del_wfd_ie(ies, len, core::ptr::null()) }
    }
    pub(super) unsafe fn frame_rx_data(rframe: *mut c_void) -> *mut u8 {
        unsafe { rtw_rust_recv_frame_rx_data(rframe) }
    }
    pub(super) unsafe fn frame_len(rframe: *mut c_void) -> c_uint {
        unsafe { rtw_rust_recv_frame_len(rframe) }
    }
    pub(super) unsafe fn frame_set_len(rframe: *mut c_void, len: c_uint) {
        unsafe { rtw_rust_recv_frame_set_len(rframe, len) };
    }
    pub(super) unsafe fn frame_attrib(rframe: *mut c_void) -> *mut c_void {
        unsafe { rtw_rust_recv_frame_attrib(rframe) }
    }
    pub(super) unsafe fn frame_pull_tail(rframe: *mut c_void, sz: c_int) {
        unsafe { rtw_rust_recv_frame_pull_tail(rframe, sz) };
    }
    pub(super) unsafe fn frame_pull(rframe: *mut c_void, sz: c_int) -> *mut u8 {
        unsafe { rtw_rust_recv_frame_pull(rframe, sz) }
    }
    pub(super) unsafe fn rframe_set_os_pkt(rframe: *mut c_void) {
        unsafe { rtw_rust_rframe_set_os_pkt(rframe) };
    }
    pub(super) unsafe fn adapter_fw_state(adapter: *mut c_void) -> c_int {
        unsafe { rtw_rust_adapter_fw_state(adapter) }
    }
    pub(super) unsafe fn adapter_linked(adapter: *mut c_void) -> u8 {
        unsafe { rtw_rust_adapter_linked(adapter) }
    }
    pub(super) unsafe fn adapter_simple_config(adapter: *mut c_void) -> u8 {
        unsafe { rtw_rust_adapter_simple_config(adapter) }
    }
    pub(super) unsafe fn attrib_mesh_ctrl_len(attrib: *mut c_void) -> u8 {
        unsafe { rtw_rust_attrib_mesh_ctrl_len(attrib) }
    }
    pub(super) unsafe fn frame_psta(rframe: *mut c_void) -> *mut c_void {
        unsafe { rtw_rust_recv_frame_psta(rframe) }
    }
    pub(super) unsafe fn frame_adapter(rframe: *mut c_void) -> *mut c_void {
        unsafe { rtw_rust_recv_frame_adapter(rframe) }
    }
    pub(super) unsafe fn tid_rxseq(sta: *mut c_void, tid: c_int) -> *mut u16 {
        unsafe { rtw_rust_recv_tid_rxseq(sta, tid) }
    }
    pub(super) unsafe fn bmc_tid_rxseq(sta: *mut c_void, tid: c_int) -> *mut u16 {
        unsafe { rtw_rust_recv_bmc_tid_rxseq(sta, tid) }
    }
    pub(super) unsafe fn nonqos_rxseq(sta: *mut c_void) -> *mut u16 {
        unsafe { rtw_rust_recv_nonqos_rxseq(sta) }
    }
    pub(super) unsafe fn nonqos_bmc_rxseq(sta: *mut c_void) -> *mut u16 {
        unsafe { rtw_rust_recv_nonqos_bmc_rxseq(sta) }
    }
    pub(super) unsafe fn sta_iv(sta: *mut c_void, tid: c_int) -> *mut u8 {
        unsafe { rtw_rust_recv_sta_iv(sta, tid) }
    }
    pub(super) unsafe fn sta_last_tid(sta: *mut c_void) -> *mut u8 {
        unsafe { rtw_rust_recv_sta_last_tid(sta) }
    }
    pub(super) unsafe fn sta_duplicate_cnt(sta: *mut c_void) -> *mut u32 {
        unsafe { rtw_rust_recv_sta_duplicate_cnt(sta) }
    }
    pub(super) unsafe fn sec_iv_seq(adapter: *mut c_void, key_id: u8) -> *mut u8 {
        unsafe { rtw_rust_recv_sec_iv_seq(adapter, key_id) }
    }
}

#[cfg(host_recv_test)]
extern "C" {
    fn rtw_del_wfd_ie(ies: *mut u8, ies_len_ori: c_uint, msg: *const u8) -> c_uint;
    fn rtw_rframe_set_os_pkt(rframe: *mut RecvFrame);
    fn rtw_linked_check(adapter: *mut Adapter) -> c_int;
}

#[no_mangle]
pub extern "C" fn rtw_inc_and_chk_continual_no_rx_packet(
    sta: *mut c_void,
    tid_index: c_int,
) -> c_int {
    if sta.is_null() || tid_index < 0 {
        return _FALSE;
    }
    #[cfg(host_recv_test)]
    {
        let tid = tid_index as usize;
        if tid >= TID_NUM {
            return _FALSE;
        }
        let sta = unsafe { &mut *(sta as *mut StaInfo) };
        let value = sta.continual_no_rx_packet[tid] + 1;
        sta.continual_no_rx_packet[tid] = value;
        return if value >= MAX_CONTINUAL_NORXPACKET_COUNT {
            _TRUE
        } else {
            _FALSE
        };
    }
    #[cfg(not(host_recv_test))]
    {
        let counter = unsafe { kernel::continual_no_rx(sta, tid_index) };
        if counter.is_null() {
            return _FALSE;
        }
        let value = unsafe { kernel::atomic_inc(counter) };
        if value >= MAX_CONTINUAL_NORXPACKET_COUNT {
            _TRUE
        } else {
            _FALSE
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_reset_continual_no_rx_packet(sta: *mut c_void, tid_index: c_int) {
    if sta.is_null() || tid_index < 0 {
        return;
    }
    #[cfg(host_recv_test)]
    {
        let tid = tid_index as usize;
        if tid < TID_NUM {
            unsafe { (&mut *(sta as *mut StaInfo)).continual_no_rx_packet[tid] = 0 };
        }
    }
    #[cfg(not(host_recv_test))]
    {
        let counter = unsafe { kernel::continual_no_rx(sta, tid_index) };
        if !counter.is_null() {
            unsafe { kernel::atomic_set(counter, 0) };
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_rframe_del_wfd_ie(rframe: *mut c_void, ies_offset: u8) -> bool {
    if rframe.is_null() {
        return false;
    }
    let (rx_data, len) = {
        #[cfg(host_recv_test)]
        {
            let rf = unsafe { &mut *(rframe as *mut RecvFrame) };
            (rf.hdr.rx_data, rf.hdr.len)
        }
        #[cfg(not(host_recv_test))]
        {
            (unsafe { kernel::frame_rx_data(rframe) }, unsafe {
                kernel::frame_len(rframe)
            })
        }
    };
    if rx_data.is_null() {
        return false;
    }
    let ies = unsafe { rx_data.add(HDR_3ADDR_SZ + ies_offset as usize) };
    let ies_len_ori = len - (ies as u32 - rx_data as u32);
    let ies_len = {
        #[cfg(host_recv_test)]
        {
            unsafe { rtw_del_wfd_ie(ies, ies_len_ori, core::ptr::null()) }
        }
        #[cfg(not(host_recv_test))]
        {
            unsafe { kernel::del_wfd_ie(ies, ies_len_ori) }
        }
    };
    let new_len = len - (ies_len_ori - ies_len);
    #[cfg(host_recv_test)]
    {
        unsafe { (*(rframe as *mut RecvFrame)).hdr.len = new_len };
    }
    #[cfg(not(host_recv_test))]
    {
        unsafe { kernel::frame_set_len(rframe, new_len) };
    }
    ies_len_ori != ies_len
}

#[cfg(host_recv_test)]
fn be16(bytes: &[u8]) -> u16 {
    ((bytes[0] as u16) << 8) | (bytes[1] as u16)
}

#[cfg(host_recv_test)]
fn memeq(a: &[u8], b: &[u8]) -> bool {
    a.len() == b.len() && a.iter().zip(b.iter()).all(|(x, y)| x == y)
}

#[cfg(host_recv_test)]
fn host_htons(v: u16) -> u16 {
    v.to_be()
}

#[cfg(host_recv_test)]
fn host_recv_llc_parse(msdu: &[u8]) -> c_int {
    if msdu.len() < 8 {
        return RTW_RX_LLC_KEEP as c_int;
    }
    let eth_type = be16(&msdu[SNAP_SIZE..SNAP_SIZE + 2]);
    if (memeq(&msdu[..SNAP_SIZE], &RFC1042_HEADER)
        && eth_type != ETH_P_AARP
        && eth_type != ETH_P_IPX)
        || memeq(&msdu[..SNAP_SIZE], &BRIDGE_TUNNEL_HEADER)
    {
        RTW_RX_LLC_REMOVE as c_int
    } else {
        RTW_RX_LLC_KEEP as c_int
    }
}

#[cfg(not(host_recv_test))]
fn recv_llc_parse(msdu: *mut u8, msdu_len: u16) -> u8 {
    if msdu.is_null() || msdu_len < 8 {
        return 0;
    }
    let slice = unsafe { core::slice::from_raw_parts(msdu, msdu_len as usize) };
    static RFC1042: [u8; 6] = [0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00];
    static BRIDGE: [u8; 6] = [0xaa, 0xaa, 0x03, 0x00, 0x00, 0xf8];
    let eth_type = ((slice[6] as u16) << 8) | (slice[7] as u16);
    if (slice[..6] == RFC1042 && eth_type != 0x80f3 && eth_type != 0x8137) || slice[..6] == BRIDGE {
        1
    } else {
        0
    }
}

#[cfg(host_recv_test)]
unsafe fn host_frame_pull(rf: &mut RecvFrame, sz: c_int) -> Option<*mut u8> {
    let sz = sz as usize;
    let data = rf.hdr.rx_data;
    rf.hdr.rx_data = data.add(sz);
    if rf.hdr.rx_data > rf.hdr.rx_tail {
        rf.hdr.rx_data = data;
        return None;
    }
    rf.hdr.len -= sz as c_uint;
    Some(rf.hdr.rx_data)
}

#[cfg(host_recv_test)]
unsafe fn host_frame_pull_tail(rf: &mut RecvFrame, sz: c_int) {
    rf.hdr.rx_tail = rf.hdr.rx_tail.sub(sz as usize);
    if rf.hdr.rx_tail < rf.hdr.rx_data {
        return;
    }
    rf.hdr.len -= sz as c_uint;
}

#[cfg(host_recv_test)]
unsafe fn host_wlanhdr_to_ethhdr(rframe: *mut RecvFrame, llc_hdl: u8) -> c_int {
    let rf = &mut *rframe;

    if rf.hdr.attrib.encrypt != 0 {
        host_frame_pull_tail(rf, rf.hdr.attrib.icv_len as c_int);
    }

    let rmv_len = rf.hdr.attrib.hdrlen as c_int
        + rf.hdr.attrib.iv_len as c_int
        + if llc_hdl != 0 { SNAP_SIZE as c_int } else { 0 };
    let len = rf.hdr.len as c_int - rmv_len;

    let pull = rmv_len - ETHHDR_SZ as c_int + if llc_hdl != 0 { 2 } else { 0 };
    let ptr = match host_frame_pull(rf, pull) {
        Some(p) => p,
        None => return _FAIL,
    };

    core::ptr::copy_nonoverlapping(rf.hdr.attrib.dst.as_ptr(), ptr, ETH_ALEN);
    core::ptr::copy_nonoverlapping(rf.hdr.attrib.src.as_ptr(), ptr.add(ETH_ALEN), ETH_ALEN);

    if llc_hdl == 0 {
        let nlen = host_htons(len as u16);
        core::ptr::copy_nonoverlapping(&nlen as *const u16 as *const u8, ptr.add(12), 2);
    }

    rtw_rframe_set_os_pkt(rframe);
    _SUCCESS
}

#[cfg(not(host_recv_test))]
unsafe fn wlanhdr_to_ethhdr_kernel(rframe: *mut c_void, llc_hdl: u8) -> c_int {
    let attrib = unsafe { kernel::frame_attrib(rframe) };
    if attrib.is_null() {
        return -1;
    }
    let a = unsafe { &mut *(attrib as *mut RxPktAttribKernel) };
    if a.encrypt != 0 {
        unsafe { kernel::frame_pull_tail(rframe, a.icv_len as c_int) };
    }
    let mctrl_len = unsafe { kernel::attrib_mesh_ctrl_len(attrib) } as c_int;
    let rmv_len =
        a.hdrlen as c_int + a.iv_len as c_int + mctrl_len + if llc_hdl != 0 { 6 } else { 0 };
    let len = unsafe { kernel::frame_len(rframe) as c_int } - rmv_len;
    let pull = rmv_len - 14 + if llc_hdl != 0 { 2 } else { 0 };
    let ptr = unsafe { kernel::frame_pull(rframe, pull) };
    if ptr.is_null() {
        return -1;
    }
    unsafe {
        core::ptr::copy_nonoverlapping(a.dst.as_ptr(), ptr, 6);
        core::ptr::copy_nonoverlapping(a.src.as_ptr(), ptr.add(6), 6);
    }
    if llc_hdl == 0 {
        let nlen = (len as u16).to_be();
        unsafe {
            core::ptr::copy_nonoverlapping(&nlen as *const u16 as *const u8, ptr.add(12), 2);
        }
    }
    unsafe { kernel::rframe_set_os_pkt(rframe) };
    0
}

/// Kernel `struct rx_pkt_attrib` through `bssid` — matches `include/rtw_recv.h`
/// field order (same layout as `rust/rtw_security.rs` `RxPktAttrib`).
#[cfg(not(host_recv_test))]
#[repr(C)]
struct RxPktAttribKernel {
    pkt_len: u16,
    physt: u8,
    drvinfo_sz: u8,
    shift_sz: u8,
    hdrlen: u8,
    to_fr_ds: u8,
    amsdu: u8,
    qos: u8,
    priority: u8,
    pw_save: u8,
    mdata: u8,
    seq_num: u16,
    frag_num: u8,
    mfrag: u8,
    order: u8,
    privacy: u8,
    bdecrypted: u8,
    encrypt: u8,
    iv_len: u8,
    icv_len: u8,
    crc_err: u8,
    icv_err: u8,
    dst: [u8; 6],
    src: [u8; 6],
    ta: [u8; 6],
    ra: [u8; 6],
    bssid: [u8; 6],
}

#[no_mangle]
pub extern "C" fn rtw_recv_llc_parse(msdu: *mut u8, msdu_len: u16) -> c_int {
    #[cfg(host_recv_test)]
    {
        if msdu.is_null() {
            return RTW_RX_LLC_KEEP as c_int;
        }
        let slice = unsafe { core::slice::from_raw_parts(msdu, msdu_len as usize) };
        host_recv_llc_parse(slice)
    }
    #[cfg(not(host_recv_test))]
    {
        recv_llc_parse(msdu, msdu_len) as c_int
    }
}

#[no_mangle]
pub extern "C" fn wlanhdr_to_ethhdr(rframe: *mut c_void, llc_hdl: u8) -> c_int {
    if rframe.is_null() {
        return _FAIL;
    }
    #[cfg(host_recv_test)]
    {
        unsafe { host_wlanhdr_to_ethhdr(rframe as *mut RecvFrame, llc_hdl) }
    }
    #[cfg(not(host_recv_test))]
    {
        unsafe { wlanhdr_to_ethhdr_kernel(rframe, llc_hdl) }
    }
}

#[no_mangle]
pub extern "C" fn adapter_allow_bmc_data_rx(adapter: *mut c_void) -> u8 {
    if adapter.is_null() {
        return 0;
    }
    #[cfg(host_recv_test)]
    {
        let ad = unsafe { &*(adapter as *mut Adapter) };
        if ad.mlmepriv.fw_state & (WIFI_MONITOR_STATE | WIFI_MP_STATE) != 0 {
            return 1;
        }
        if ad.mlmepriv.fw_state & WIFI_AP_STATE != 0 {
            return 0;
        }
        if unsafe { rtw_linked_check(adapter as *mut Adapter) } == _FALSE {
            return 0;
        }
        1
    }
    #[cfg(not(host_recv_test))]
    {
        let fw = unsafe { kernel::adapter_fw_state(adapter) } as u32;
        if fw & (0x80000000 | 0x00010000) != 0 {
            return 1;
        }
        if fw & 0x00000010 != 0 {
            if unsafe { kernel::adapter_simple_config(adapter) } != 0 {
                return 1;
            }
            return 0;
        }
        if unsafe { kernel::adapter_linked(adapter) } == 0 {
            return 0;
        }
        1
    }
}

const AES_ENCRYPT: u8 = 0x04;
const WIFI_STA_STATE: u32 = 0x00000008;

fn ccmph_2_pn(ch: u64) -> u64 {
    (ch & 0xffff) | ((ch & 0xffffffff00000000) >> 16)
}

fn ccmph_2_keyid(ch: u64) -> u8 {
    ((ch & 0x00000000c0000000) >> 30) as u8
}

fn pn_less_chk(a: u64, b: u64) -> bool {
    (a.wrapping_sub(b) & 0x800000000000) != 0
}

fn valid_pn_chk(new_pn: u64, old_pn: u64) -> bool {
    old_pn == 0 || pn_less_chk(old_pn, new_pn)
}

fn is_mcast_ra(ra: &[u8; 6]) -> bool {
    ra[0] & 0x01 != 0
}

fn read_le64(ptr: *const u8) -> u64 {
    let mut buf = [0u8; 8];
    unsafe {
        core::ptr::copy_nonoverlapping(ptr, buf.as_mut_ptr(), 8);
    }
    u64::from_le_bytes(buf)
}

fn write_le64(ptr: *mut u8, val: u64) {
    let bytes = val.to_le_bytes();
    unsafe {
        core::ptr::copy_nonoverlapping(bytes.as_ptr(), ptr, 8);
    }
}

#[cfg(host_recv_test)]
unsafe fn host_recv_decache(rframe: *mut RecvFrame) -> c_int {
    let rf = &mut *rframe;
    if rf.hdr.psta.is_null() {
        return _FAIL;
    }
    let psta = &mut *rf.hdr.psta;
    let tid = rf.hdr.attrib.priority as c_int;
    if tid > 15 {
        return _FAIL;
    }
    let seq_ctrl = ((rf.hdr.attrib.seq_num as u16) << 4) | (rf.hdr.attrib.frag_num as u16 & 0xf);
    let prxseq = if rf.hdr.attrib.qos != 0 {
        if is_mcast_ra(&rf.hdr.attrib.ra) {
            &mut psta.sta_recvpriv.bmc_tid_rxseq[tid as usize]
        } else {
            &mut psta.sta_recvpriv.rxcache.tid_rxseq[tid as usize]
        }
    } else if is_mcast_ra(&rf.hdr.attrib.ra) {
        &mut psta.sta_recvpriv.nonqos_bmc_rxseq
    } else {
        &mut psta.sta_recvpriv.nonqos_rxseq
    };
    if seq_ctrl == *prxseq {
        psta.sta_stats.duplicate_cnt += 1;
        return _FAIL;
    }
    *prxseq = seq_ctrl;
    _SUCCESS
}

#[cfg(not(host_recv_test))]
fn kernel_recv_decache(rframe: *mut c_void) -> c_int {
    unsafe {
        let psta = kernel::frame_psta(rframe);
        if psta.is_null() {
            return _FAIL;
        }
        let attrib = kernel::frame_attrib(rframe);
        if attrib.is_null() {
            return _FAIL;
        }
        let a = &*(attrib as *const RxPktAttribKernel);
        let tid = a.priority as c_int;
        if tid > 15 {
            return _FAIL;
        }
        let seq_ctrl = ((a.seq_num as u16) << 4) | (a.frag_num as u16 & 0xf);
        let prxseq = if a.qos != 0 {
            if is_mcast_ra(&a.ra) {
                kernel::bmc_tid_rxseq(psta, tid)
            } else {
                kernel::tid_rxseq(psta, tid)
            }
        } else if is_mcast_ra(&a.ra) {
            kernel::nonqos_bmc_rxseq(psta)
        } else {
            kernel::nonqos_rxseq(psta)
        };
        if prxseq.is_null() {
            return _FAIL;
        }
        if seq_ctrl == *prxseq {
            let dup = kernel::sta_duplicate_cnt(psta);
            if !dup.is_null() {
                *dup += 1;
            }
            return _FAIL;
        }
        *prxseq = seq_ctrl;
        _SUCCESS
    }
}

#[no_mangle]
pub extern "C" fn recv_decache(rframe: *mut c_void) -> c_int {
    if rframe.is_null() {
        return _FAIL;
    }
    #[cfg(host_recv_test)]
    {
        unsafe { host_recv_decache(rframe as *mut RecvFrame) }
    }
    #[cfg(not(host_recv_test))]
    {
        kernel_recv_decache(rframe)
    }
}

#[cfg(host_recv_test)]
unsafe fn host_recv_ucast_pn_decache(rframe: *mut RecvFrame) -> c_int {
    let rf = &mut *rframe;
    if rf.hdr.psta.is_null() {
        return _FAIL;
    }
    let psta = &mut *rf.hdr.psta;
    let tid = rf.hdr.attrib.priority as c_int;
    if tid > 15 {
        return _FAIL;
    }
    if rf.hdr.attrib.encrypt != _AES_ {
        return _SUCCESS;
    }
    if rf.hdr.rx_data.is_null() {
        return _SUCCESS;
    }
    let iv_off = rf.hdr.attrib.hdrlen as usize;
    let pkt_pn = ccmph_2_pn(read_le64(rf.hdr.rx_data.add(iv_off)));
    let curr_pn = ccmph_2_pn(read_le64(
        psta.sta_recvpriv.rxcache.iv[tid as usize].as_ptr(),
    ));
    if valid_pn_chk(pkt_pn, curr_pn) {
        psta.sta_recvpriv.rxcache.last_tid = tid as u8;
        core::ptr::copy_nonoverlapping(
            rf.hdr.rx_data.add(iv_off),
            psta.sta_recvpriv.rxcache.iv[tid as usize].as_mut_ptr(),
            8,
        );
    }
    _SUCCESS
}

#[cfg(not(host_recv_test))]
fn kernel_recv_ucast_pn_decache(rframe: *mut c_void) -> c_int {
    unsafe {
        let psta = kernel::frame_psta(rframe);
        if psta.is_null() {
            return _FAIL;
        }
        let attrib = kernel::frame_attrib(rframe);
        if attrib.is_null() {
            return _FAIL;
        }
        let a = &*(attrib as *const RxPktAttribKernel);
        let tid = a.priority as c_int;
        if tid > 15 {
            return _FAIL;
        }
        if a.encrypt != AES_ENCRYPT {
            return _SUCCESS;
        }
        let pdata = kernel::frame_rx_data(rframe);
        if pdata.is_null() {
            return _SUCCESS;
        }
        let pkt_pn = ccmph_2_pn(read_le64(pdata.add(a.hdrlen as usize)));
        let iv = kernel::sta_iv(psta, tid);
        if iv.is_null() {
            return _SUCCESS;
        }
        let curr_pn = ccmph_2_pn(read_le64(iv));
        if valid_pn_chk(pkt_pn, curr_pn) {
            let last_tid = kernel::sta_last_tid(psta);
            if !last_tid.is_null() {
                *last_tid = tid as u8;
            }
            core::ptr::copy_nonoverlapping(pdata.add(a.hdrlen as usize), iv, 8);
        }
        _SUCCESS
    }
}

#[no_mangle]
pub extern "C" fn recv_ucast_pn_decache(rframe: *mut c_void) -> c_int {
    if rframe.is_null() {
        return _FAIL;
    }
    #[cfg(host_recv_test)]
    {
        unsafe { host_recv_ucast_pn_decache(rframe as *mut RecvFrame) }
    }
    #[cfg(not(host_recv_test))]
    {
        kernel_recv_ucast_pn_decache(rframe)
    }
}

#[cfg(host_recv_test)]
unsafe fn host_recv_bcast_pn_decache(rframe: *mut RecvFrame) -> c_int {
    let rf = &mut *rframe;
    if rf.hdr.adapter.is_null() {
        return _SUCCESS;
    }
    let adapter = &mut *rf.hdr.adapter;
    if rf.hdr.attrib.encrypt != _AES_ {
        return _SUCCESS;
    }
    if adapter.mlmepriv.fw_state & WIFI_STATION_STATE == 0 {
        return _SUCCESS;
    }
    if rf.hdr.rx_data.is_null() {
        return _SUCCESS;
    }
    let iv_off = rf.hdr.attrib.hdrlen as usize;
    let tmp_iv = read_le64(rf.hdr.rx_data.add(iv_off));
    let key_id = ccmph_2_keyid(tmp_iv);
    let pkt_pn = ccmph_2_pn(tmp_iv);
    let mut curr_pn = read_le64(adapter.securitypriv.iv_seq[key_id as usize].as_ptr());
    curr_pn &= 0x0000ffffffffffff;
    if !valid_pn_chk(pkt_pn, curr_pn) {
        return _FAIL;
    }
    write_le64(
        adapter.securitypriv.iv_seq[key_id as usize].as_mut_ptr(),
        pkt_pn,
    );
    _SUCCESS
}

#[cfg(not(host_recv_test))]
fn kernel_recv_bcast_pn_decache(rframe: *mut c_void) -> c_int {
    unsafe {
        let adapter = kernel::frame_adapter(rframe);
        if adapter.is_null() {
            return _SUCCESS;
        }
        let attrib = kernel::frame_attrib(rframe);
        if attrib.is_null() {
            return _SUCCESS;
        }
        let a = &*(attrib as *const RxPktAttribKernel);
        if a.encrypt != AES_ENCRYPT {
            return _SUCCESS;
        }
        if kernel::adapter_fw_state(adapter) & WIFI_STA_STATE as c_int == 0 {
            return _SUCCESS;
        }
        let pdata = kernel::frame_rx_data(rframe);
        if pdata.is_null() {
            return _SUCCESS;
        }
        let tmp_iv = read_le64(pdata.add(a.hdrlen as usize));
        let key_id = ccmph_2_keyid(tmp_iv);
        let pkt_pn = ccmph_2_pn(tmp_iv);
        let iv_seq = kernel::sec_iv_seq(adapter, key_id);
        if iv_seq.is_null() {
            return _SUCCESS;
        }
        let mut curr_pn = read_le64(iv_seq);
        curr_pn &= 0x0000ffffffffffff;
        if !valid_pn_chk(pkt_pn, curr_pn) {
            return _FAIL;
        }
        write_le64(iv_seq, pkt_pn);
        _SUCCESS
    }
}

#[no_mangle]
pub extern "C" fn recv_bcast_pn_decache(rframe: *mut c_void) -> c_int {
    if rframe.is_null() {
        return _SUCCESS;
    }
    #[cfg(host_recv_test)]
    {
        unsafe { host_recv_bcast_pn_decache(rframe as *mut RecvFrame) }
    }
    #[cfg(not(host_recv_test))]
    {
        kernel_recv_bcast_pn_decache(rframe)
    }
}
