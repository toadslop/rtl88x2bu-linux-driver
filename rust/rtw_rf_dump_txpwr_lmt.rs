// SPDX-License-Identifier: GPL-2.0
//! W3-58 dump_txpwr_lmt formatter — Rust port of `core/rtw_rf_dump_txpwr_lmt.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe,
    unused_variables,
    private_interfaces,
    static_mut_refs
)]

#[cfg(all(not(host_rf_dump_txpwr_lmt_test), not(txpwr_limit)))]
compile_error!("rtw_rf_dump_txpwr_lmt requires cfg(txpwr_limit)");

#[cfg(host_rf_dump_txpwr_lmt_test)]
use std::os::raw::{c_char, c_int, c_void};

#[cfg(not(host_rf_dump_txpwr_lmt_test))]
use core::ffi::{c_char, c_int, c_void};

#[cfg(host_rf_dump_txpwr_lmt_test)]
use std::mem::offset_of;

#[cfg(not(host_rf_dump_txpwr_lmt_test))]
use core::mem::offset_of;

const TMP_STR_LEN: usize = 16;

const BAND_ON_2_4G: u8 = 0;
const BAND_ON_5G: u8 = 1;

const CHANNEL_WIDTH_20: u8 = 0;
const CHANNEL_WIDTH_40: u8 = 1;
const CHANNEL_WIDTH_80: u8 = 2;
const CHANNEL_WIDTH_160: u8 = 3;

const MAX_5G_BANDWIDTH_NUM: u8 = 4;
const CENTER_CH_2G_NUM: u8 = 14;
const RF_PATH_MAX: u8 = 4;

const TXPWR_LMT_RS_CCK: u8 = 0;
const TXPWR_LMT_RS_OFDM: u8 = 1;
const TXPWR_LMT_RS_HT: u8 = 2;
const TXPWR_LMT_RS_VHT: u8 = 3;
const TXPWR_LMT_RS_NUM: u8 = 4;

const TXPWR_LMT_HAS_CCK_1T: u8 = 1 << 0;
const TXPWR_LMT_HAS_OFDM_1T: u8 = 1 << 4;

const TXPWR_LMT_REF_HT_FROM_VHT: u8 = 1 << 1;
const TXPWR_LMT_REF_VHT_FROM_HT: u8 = 1 << 0;

const TXPWR_LMT_WW: u8 = 12;

const RF_1TX: u8 = 0;
const RF_PATH_A: u8 = 0;

const CCK: u8 = 0;
const OFDM: u8 = 1;
const HT_1SS: u8 = 2;
const VHT_1SS: u8 = 6;

const MAX_TX_COUNT: u8 = 4;

const LMT_2G_BYTES: usize = 2 * 4 * 14 * 4;
#[cfg(any(host_rf_dump_txpwr_lmt_test, ieee80211_band_5ghz))]
const LMT_5G_BYTES: usize = 4 * 3 * 49 * 4;
#[cfg(not(any(host_rf_dump_txpwr_lmt_test, ieee80211_band_5ghz)))]
const LMT_5G_BYTES: usize = 0;

#[repr(C)]
struct List {
    next: *mut List,
    prev: *mut List,
}

#[repr(C)]
struct Mutex {
    dummy: c_int,
}

#[repr(C)]
struct TxpwrLmtEnt {
    list: List,
}

#[repr(C)]
struct RfCtlT {
    txpwr_lmt_mutex: Mutex,
    reg_exc_list: List,
    regd_exc_num: u8,
    txpwr_lmt_list: List,
    txpwr_regd_num: u8,
    regd_name: *const c_char,
    txpwr_lmt_2g_cck_ofdm_state: u8,
    #[cfg(any(host_rf_dump_txpwr_lmt_test, ieee80211_band_5ghz))]
    txpwr_lmt_5g_cck_ofdm_state: u8,
    #[cfg(any(host_rf_dump_txpwr_lmt_test, ieee80211_band_5ghz))]
    txpwr_lmt_5g_20_40_ref: u8,
}

#[repr(C)]
struct HalDataType {
    max_tx_cnt: u8,
}

#[repr(C)]
struct HalSpecT {
    txgi_max: u8,
    txgi_pdbm: u8,
    rfpath_num_2g: u8,
    rfpath_num_5g: u8,
}

#[repr(C)]
struct Adapter {
    rf_ctl: RfCtlT,
    hal_data: HalDataType,
    band_cap: u8,
    jaguar: u8,
}

#[repr(C)]
pub struct HostSelCapture {
    pub buf: [u8; 16384],
    pub len: usize,
}

#[cfg(host_rf_dump_txpwr_lmt_test)]
extern "C" {
    static mut host_sel_out: HostSelCapture;
}

#[cfg(not(host_rf_dump_txpwr_lmt_test))]
mod kernel {
    use super::*;

    extern "C" {
        pub fn rtw_rust_dump_txpwr_lmt_print_sel(sel: *mut c_void, line: *const c_char);
        pub fn rtw_rust_dump_txpwr_lmt_mutex_enter(rfctl: *mut RfCtlT, irqL: *mut u32);
        pub fn rtw_rust_dump_txpwr_lmt_mutex_exit(rfctl: *mut RfCtlT, irqL: *mut u32);
        pub fn rtw_rust_dump_txpwr_lmt_txpwr_regd_num(rfctl: *mut RfCtlT) -> u8;
        pub fn rtw_rust_dump_txpwr_lmt_regd_name(rfctl: *mut RfCtlT) -> *const c_char;
        pub fn rtw_rust_dump_txpwr_lmt_2g_cck_ofdm_state(rfctl: *mut RfCtlT) -> u8;
        #[cfg(ieee80211_band_5ghz)]
        pub fn rtw_rust_dump_txpwr_lmt_5g_cck_ofdm_state(rfctl: *mut RfCtlT) -> u8;
        #[cfg(ieee80211_band_5ghz)]
        pub fn rtw_rust_dump_txpwr_lmt_5g_20_40_ref(rfctl: *mut RfCtlT) -> u8;
        pub fn rtw_rust_dump_txpwr_lmt_list_head(rfctl: *mut RfCtlT) -> *mut List;
        pub fn rtw_rust_dump_txpwr_lmt_ent_from_list(cur: *mut List) -> *mut TxpwrLmtEnt;
        pub fn rtw_rust_dump_txpwr_lmt_list_next(cur: *mut List) -> *mut List;
        pub fn rtw_rust_dump_txpwr_lmt_list_end(head: *mut List, cur: *mut List) -> bool;
        pub fn rtw_rust_dump_txpwr_lmt_ent_regd_name(ent: *mut TxpwrLmtEnt) -> *const c_char;
        pub fn rtw_rust_dump_txpwr_lmt_is_jaguar(adapter: *mut Adapter) -> bool;
        pub fn rtw_rust_dump_txpwr_lmt_max_tx_cnt(adapter: *mut Adapter) -> u8;
        pub fn rtw_rust_dump_txpwr_lmt_txgi_max(adapter: *mut Adapter) -> u8;
        pub fn rtw_rust_dump_txpwr_lmt_txgi_pdbm(adapter: *mut Adapter) -> u8;
        pub fn rtw_rust_dump_txpwr_lmt_rfpath_num_2g(adapter: *mut Adapter) -> u8;
        pub fn rtw_rust_dump_txpwr_lmt_rfpath_num_5g(adapter: *mut Adapter) -> u8;
        pub fn rtw_rust_dump_txpwr_lmt_hal_is_band_support(adapter: *mut Adapter, band: u8) -> bool;
        pub fn rtw_rust_dump_txpwr_lmt_malloc(sz: u32) -> *mut c_void;
        pub fn rtw_rust_dump_txpwr_lmt_mfree(p: *mut c_void, sz: u32);
    }
}

extern "C" {
    static _band_str: [*const c_char; 3];
    static _ch_width_str: [*const c_char; 7];
    static _txpwr_lmt_rs_str: [*const c_char; 5];
    static _regd_str: [*const c_char; 13];

    fn _dump_regd_exc_list(sel: *mut c_void, rfctl: *mut RfCtlT);
    fn hal_is_band_support(adapter: *mut Adapter, band: u8) -> bool;
    fn center_chs_5g_num(bw: u8) -> u8;
    fn center_chs_5g(bw: u8, id: u8) -> u8;
    fn phy_get_txpwr_lmt(
        adapter: *mut Adapter,
        regd_name: *const c_char,
        band: u8,
        bw: u8,
        tlrs: u8,
        ntx_idx: u8,
        ch: u8,
        lock: u8,
    ) -> i8;
    fn phy_get_txpwr_lmt_diff(
        adapter: *mut Adapter,
        regd_name: *const c_char,
        band: u8,
        bw: u8,
        rfpath: u8,
        rs: u8,
        tlrs: u8,
        ntx_idx: u8,
        ch: u8,
        lock: u8,
    ) -> i8;
    fn phy_get_target_txpwr(adapter: *mut Adapter, band: u8, rfpath: u8, rs: u8) -> u8;
    fn txpwr_idx_get_dbm_str(
        idx: i8,
        txgi_max: u8,
        txgi_pdbm: u8,
        cwidth: usize,
        dbm_str: *mut c_char,
        dbm_str_len: u8,
    );
    fn snprintf(s: *mut c_char, n: usize, fmt: *const c_char, ...) -> c_int;
    fn strcmp(s1: *const c_char, s2: *const c_char) -> c_int;
    fn strlen(s: *const c_char) -> usize;
    fn rtw_malloc(sz: u32) -> *mut c_void;
    fn rtw_mfree(p: *mut c_void, sz: u32);
}

#[cfg(host_rf_dump_txpwr_lmt_test)]
extern "C" {
    fn host_rf_hal_spec_ptr() -> *mut HalSpecT;
}

unsafe fn band_str(band: u8) -> *const c_char {
    unsafe {
        let idx = if (band as usize) < 2 {
        band as usize
        } else {
        2
        };
        let s = _band_str[idx];
        if s.is_null() {
        b"?\0".as_ptr() as *const c_char
        } else {
        s
        }
    }
}

unsafe fn ch_width_str(bw: u8) -> *const c_char {
    unsafe {
        if (bw as usize) < 7 {
        let s = _ch_width_str[bw as usize];
        if s.is_null() {
        b"CHANNEL_WIDTH_MAX\0".as_ptr() as *const c_char
        } else {
        s
        }
        } else {
        b"CHANNEL_WIDTH_MAX\0".as_ptr() as *const c_char
        }
    }
}

unsafe fn txpwr_lmt_rs_str(rs: u8) -> *const c_char {
    unsafe {
        let idx = if rs >= TXPWR_LMT_RS_NUM {
        TXPWR_LMT_RS_NUM as usize
        } else {
        rs as usize
        };
        _txpwr_lmt_rs_str[idx]
    }
}

unsafe fn regd_str_by_idx(regd: u8) -> *const c_char {
    unsafe {
        let idx = if regd > TXPWR_LMT_WW {
        TXPWR_LMT_WW as usize
        } else {
        regd as usize
        };
        _regd_str[idx]
    }
}

unsafe fn regd_str_ww() -> *const c_char {
    unsafe {
        regd_str_by_idx(TXPWR_LMT_WW)
    }
}

unsafe fn rf_path_char(path: u8) -> u8 {
    unsafe {
        if path >= RF_PATH_MAX {
        b'X'
        } else {
        b'A' + path
        }
    }
}

fn print_sel(sel: *mut c_void, line: *const c_char) {
    #[cfg(host_rf_dump_txpwr_lmt_test)]
    unsafe {
        let mut len = 0usize;
        while *line.add(len) != 0 {
            len += 1;
        }
        let remain = host_sel_out.buf.len().saturating_sub(host_sel_out.len);
        let n = core::cmp::min(len, remain.saturating_sub(1));
        core::ptr::copy_nonoverlapping(
            line as *const u8,
            host_sel_out.buf.as_mut_ptr().add(host_sel_out.len),
            n,
        );
        host_sel_out.len += n;
    }
    #[cfg(not(host_rf_dump_txpwr_lmt_test))]
    unsafe {
        kernel::rtw_rust_dump_txpwr_lmt_print_sel(sel, line);
    }
}

fn print_line(sel: *mut c_void, buf: &[i8]) {
    unsafe {
        print_sel(sel, buf.as_ptr());
    }
}

unsafe fn ent_from_list(cur: *mut List) -> *mut TxpwrLmtEnt {
    unsafe {
        (cur as *mut u8).sub(offset_of!(TxpwrLmtEnt, list)) as *mut TxpwrLmtEnt
    }
}

unsafe fn ent_regd_name_host(ent: *mut TxpwrLmtEnt) -> *const c_char {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        let list_size = std::mem::size_of::<List>();
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        let list_size = core::mem::size_of::<List>();
        let off = list_size + LMT_2G_BYTES + LMT_5G_BYTES;
        (ent as *mut u8).add(off) as *const c_char
    }
}

unsafe fn ent_regd_name(ent: *mut TxpwrLmtEnt) -> *const c_char {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        {
        ent_regd_name_host(ent)
        }
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        {
        kernel::rtw_rust_dump_txpwr_lmt_ent_regd_name(ent)
        }
    }
}

struct HalView {
    max_tx_cnt: u8,
    txgi_max: u8,
    txgi_pdbm: u8,
    rfpath_num_2g: u8,
    rfpath_num_5g: u8,
}

unsafe fn hal_view(adapter: *mut Adapter) -> HalView {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        {
        let hal = &(*adapter).hal_data;
        let spec = &*host_rf_hal_spec_ptr();
        HalView {
        max_tx_cnt: hal.max_tx_cnt,
        txgi_max: spec.txgi_max,
        txgi_pdbm: spec.txgi_pdbm,
        rfpath_num_2g: spec.rfpath_num_2g,
        rfpath_num_5g: spec.rfpath_num_5g,
        }
        }
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        {
        HalView {
        max_tx_cnt: kernel::rtw_rust_dump_txpwr_lmt_max_tx_cnt(adapter),
        txgi_max: kernel::rtw_rust_dump_txpwr_lmt_txgi_max(adapter),
        txgi_pdbm: kernel::rtw_rust_dump_txpwr_lmt_txgi_pdbm(adapter),
        rfpath_num_2g: kernel::rtw_rust_dump_txpwr_lmt_rfpath_num_2g(adapter),
        rfpath_num_5g: kernel::rtw_rust_dump_txpwr_lmt_rfpath_num_5g(adapter),
        }
        }
    }
}

unsafe fn adapter_band_supported(adapter: *mut Adapter, band: u8) -> bool {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        {
        hal_is_band_support(adapter, band)
        }
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        {
        kernel::rtw_rust_dump_txpwr_lmt_hal_is_band_support(adapter, band)
        }
    }
}

unsafe fn is_jaguar(adapter: *mut Adapter) -> bool {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        {
        (*adapter).jaguar != 0
        }
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        {
        kernel::rtw_rust_dump_txpwr_lmt_is_jaguar(adapter)
        }
    }
}

unsafe fn mutex_enter(rfctl: *mut RfCtlT, irqL: *mut u32) {
    unsafe {
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        kernel::rtw_rust_dump_txpwr_lmt_mutex_enter(rfctl, irqL);
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        let _ = (rfctl, irqL);
    }
}

unsafe fn mutex_exit(rfctl: *mut RfCtlT, irqL: *mut u32) {
    unsafe {
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        kernel::rtw_rust_dump_txpwr_lmt_mutex_exit(rfctl, irqL);
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        let _ = (rfctl, irqL);
    }
}

unsafe fn txpwr_regd_num(rfctl: *mut RfCtlT) -> u8 {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        {
        (*rfctl).txpwr_regd_num
        }
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        {
        kernel::rtw_rust_dump_txpwr_lmt_txpwr_regd_num(rfctl)
        }
    }
}

unsafe fn regd_name_ptr(rfctl: *mut RfCtlT) -> *const c_char {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        {
        (*rfctl).regd_name
        }
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        {
        kernel::rtw_rust_dump_txpwr_lmt_regd_name(rfctl)
        }
    }
}

unsafe fn cck_ofdm_state_2g(rfctl: *mut RfCtlT) -> u8 {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        {
        (*rfctl).txpwr_lmt_2g_cck_ofdm_state
        }
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        {
        kernel::rtw_rust_dump_txpwr_lmt_2g_cck_ofdm_state(rfctl)
        }
    }
}

#[cfg(any(host_rf_dump_txpwr_lmt_test, ieee80211_band_5ghz))]
unsafe fn cck_ofdm_state_5g(rfctl: *mut RfCtlT) -> u8 {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        {
        (*rfctl).txpwr_lmt_5g_cck_ofdm_state
        }
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        {
        kernel::rtw_rust_dump_txpwr_lmt_5g_cck_ofdm_state(rfctl)
        }
    }
}

#[cfg(any(host_rf_dump_txpwr_lmt_test, ieee80211_band_5ghz))]
unsafe fn txpwr_lmt_5g_ref(rfctl: *mut RfCtlT) -> u8 {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        {
        (*rfctl).txpwr_lmt_5g_20_40_ref
        }
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        {
        kernel::rtw_rust_dump_txpwr_lmt_5g_20_40_ref(rfctl)
        }
    }
}

unsafe fn list_head(rfctl: *mut RfCtlT) -> *mut List {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        {
        &mut (*rfctl).txpwr_lmt_list
        }
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        {
        kernel::rtw_rust_dump_txpwr_lmt_list_head(rfctl)
        }
    }
}

unsafe fn list_next(cur: *mut List) -> *mut List {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        {
        (*cur).next
        }
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        {
        kernel::rtw_rust_dump_txpwr_lmt_list_next(cur)
        }
    }
}

unsafe fn list_end(head: *mut List, cur: *mut List) -> bool {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        {
        head == cur
        }
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        {
        kernel::rtw_rust_dump_txpwr_lmt_list_end(head, cur)
        }
    }
}

unsafe fn ent_from_list_node(cur: *mut List) -> *mut TxpwrLmtEnt {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        {
        ent_from_list(cur)
        }
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        {
        kernel::rtw_rust_dump_txpwr_lmt_ent_from_list(cur)
        }
    }
}

unsafe fn lmt_alloc(sz: u32) -> *mut c_void {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        {
        rtw_malloc(sz)
        }
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        {
        kernel::rtw_rust_dump_txpwr_lmt_malloc(sz)
        }
    }
}

unsafe fn lmt_free(p: *mut c_void, sz: u32) {
    unsafe {
        #[cfg(host_rf_dump_txpwr_lmt_test)]
        {
        rtw_mfree(p, sz);
        }
        #[cfg(not(host_rf_dump_txpwr_lmt_test))]
        {
        kernel::rtw_rust_dump_txpwr_lmt_mfree(p, sz);
        }
    }
}

unsafe fn print_regd_header(
    sel: *mut c_void,
    head: *mut List,
    rf_regd: *const c_char,
    rfpath_num: u8,
) {
    unsafe {
    let mut fmt = [0i8; 16];
    let mut tmp_str = [0i8; TMP_STR_LEN];
    let mut line = [0i8; 512];

    snprintf(
        line.as_mut_ptr(),
        line.len(),
        b"%3s \0".as_ptr() as *const c_char,
        b"ch\0".as_ptr() as *const c_char,
    );
    print_line(sel, &line);

    let mut cur = list_next(head);
    while !list_end(head, cur) {
        let ent = ent_from_list_node(cur);
        let name = ent_regd_name(ent);
        let name_len = strlen(name);
        let pad = if name_len >= 6 { 1 } else { 6 - name_len };
        snprintf(
            fmt.as_mut_ptr(),
            fmt.len(),
            b"%%%zus%%s \0".as_ptr() as *const c_char,
            pad,
        );
        let star = if strcmp(name, rf_regd) == 0 {
            b"*\0".as_ptr() as *const c_char
        } else {
            b"\0".as_ptr() as *const c_char
        };
        snprintf(
            tmp_str.as_mut_ptr(),
            tmp_str.len(),
            fmt.as_ptr(),
            star,
            name,
        );
        snprintf(
            line.as_mut_ptr(),
            line.len(),
            b"%s\0".as_ptr() as *const c_char,
            tmp_str.as_ptr(),
        );
        print_line(sel, &line);
        cur = list_next(cur);
    }

    let ww = regd_str_ww();
    let ww_len = strlen(ww);
    let pad = if ww_len >= 6 { 1 } else { 6 - ww_len };
    snprintf(
        fmt.as_mut_ptr(),
        fmt.len(),
        b"%%%zus%%s \0".as_ptr() as *const c_char,
        pad,
    );
    let star = if strcmp(rf_regd, ww) == 0 {
        b"*\0".as_ptr() as *const c_char
    } else {
        b"\0".as_ptr() as *const c_char
    };
    snprintf(
        tmp_str.as_mut_ptr(),
        tmp_str.len(),
        fmt.as_ptr(),
        star,
        ww,
    );
    snprintf(
        line.as_mut_ptr(),
        line.len(),
        b"%s\0".as_ptr() as *const c_char,
        tmp_str.as_ptr(),
    );
    print_line(sel, &line);

    for path in 0..RF_PATH_MAX {
        if path >= rfpath_num {
            break;
        }
        snprintf(line.as_mut_ptr(), line.len(), b"|\0".as_ptr() as *const c_char);
        print_line(sel, &line);

        let mut cur = list_next(head);
        while !list_end(head, cur) {
            let ent = ent_from_list_node(cur);
            let name = ent_regd_name(ent);
            let ch = if strcmp(name, rf_regd) == 0 {
                rf_path_char(path) as c_int
            } else {
                b' ' as c_int
            };
            snprintf(
                line.as_mut_ptr(),
                line.len(),
                b"%3c \0".as_ptr() as *const c_char,
                ch,
            );
            print_line(sel, &line);
            cur = list_next(cur);
        }
        let ch = if strcmp(rf_regd, ww) == 0 {
            rf_path_char(path) as c_int
        } else {
            b' ' as c_int
        };
        snprintf(
            line.as_mut_ptr(),
            line.len(),
            b"%3c \0".as_ptr() as *const c_char,
            ch,
        );
        print_line(sel, &line);
    }
    snprintf(line.as_mut_ptr(), line.len(), b"\n\0".as_ptr() as *const c_char);
    print_line(sel, &line);
    }
}

#[no_mangle]
pub extern "C" fn dump_txpwr_lmt(sel: *mut c_void, adapter: *mut Adapter) {
    unsafe {
        let rfctl = &mut (*adapter).rf_ctl;
        let hal = hal_view(adapter);
        let mut irqL: u32 = 0;
        let mut line = [0i8; 512];

        mutex_enter(rfctl, &mut irqL);
        _dump_regd_exc_list(sel, rfctl);
        snprintf(line.as_mut_ptr(), line.len(), b"\n\0".as_ptr() as *const c_char);
        print_line(sel, &line);

        let regd_num = txpwr_regd_num(rfctl);
        if regd_num == 0 {
            mutex_exit(rfctl, &mut irqL);
            return;
        }

        let lmt_idx = lmt_alloc(RF_PATH_MAX as u32 * regd_num as u32) as *mut i8;
        if lmt_idx.is_null() {
            mutex_exit(rfctl, &mut irqL);
            return;
        }

        snprintf(
            line.as_mut_ptr(),
            line.len(),
            b"txpwr_lmt_2g_cck_ofdm_state:0x%02x\n\0".as_ptr() as *const c_char,
            cck_ofdm_state_2g(rfctl) as c_int,
        );
        print_line(sel, &line);

        #[cfg(any(host_rf_dump_txpwr_lmt_test, ieee80211_band_5ghz))]
        if is_jaguar(adapter) {
            snprintf(
                line.as_mut_ptr(),
                line.len(),
                b"txpwr_lmt_5g_cck_ofdm_state:0x%02x\n\0".as_ptr() as *const c_char,
                cck_ofdm_state_5g(rfctl) as c_int,
            );
            print_line(sel, &line);
            snprintf(
                line.as_mut_ptr(),
                line.len(),
                b"txpwr_lmt_5g_20_40_ref:0x%02x\n\0".as_ptr() as *const c_char,
                txpwr_lmt_5g_ref(rfctl) as c_int,
            );
            print_line(sel, &line);
        }

        snprintf(line.as_mut_ptr(), line.len(), b"\n\0".as_ptr() as *const c_char);
        print_line(sel, &line);

        let rf_regd = regd_name_ptr(rfctl);
        let head = list_head(rfctl);

        for band in BAND_ON_2_4G..=BAND_ON_5G {
            if !adapter_band_supported(adapter, band) {
                continue;
            }

            let rfpath_num = if band == BAND_ON_2_4G {
                hal.rfpath_num_2g
            } else {
                hal.rfpath_num_5g
            };

            for bw in 0..MAX_5G_BANDWIDTH_NUM {
                if bw >= CHANNEL_WIDTH_160 {
                    break;
                }
                if band == BAND_ON_2_4G && bw >= CHANNEL_WIDTH_80 {
                    break;
                }

                let ch_num = if band == BAND_ON_2_4G {
                    CENTER_CH_2G_NUM
                } else {
                    center_chs_5g_num(bw)
                };

                if ch_num == 0 {
                    break;
                }

                for tlrs in TXPWR_LMT_RS_CCK..TXPWR_LMT_RS_NUM {
                    if band == BAND_ON_2_4G && tlrs == TXPWR_LMT_RS_VHT {
                        continue;
                    }
                    if band == BAND_ON_5G && tlrs == TXPWR_LMT_RS_CCK {
                        continue;
                    }
                    if bw > CHANNEL_WIDTH_20
                        && (tlrs == TXPWR_LMT_RS_CCK || tlrs == TXPWR_LMT_RS_OFDM)
                    {
                        continue;
                    }
                    if bw > CHANNEL_WIDTH_40 && tlrs == TXPWR_LMT_RS_HT {
                        continue;
                    }
                    if tlrs == TXPWR_LMT_RS_VHT && !is_jaguar(adapter) {
                        continue;
                    }

                    for ntx_idx in RF_1TX..MAX_TX_COUNT {
                        if ntx_idx + 1 > hal.max_tx_cnt {
                            continue;
                        }

                        if tlrs == TXPWR_LMT_RS_CCK && ntx_idx > RF_1TX {
                            if band == BAND_ON_2_4G {
                                let st = cck_ofdm_state_2g(rfctl);
                                if st & (TXPWR_LMT_HAS_CCK_1T << ntx_idx) == 0 {
                                    continue;
                                }
                            }
                        }

                        if tlrs == TXPWR_LMT_RS_OFDM && ntx_idx > RF_1TX {
                            if band == BAND_ON_2_4G {
                                let st = cck_ofdm_state_2g(rfctl);
                                if st & (TXPWR_LMT_HAS_OFDM_1T << ntx_idx) == 0 {
                                    continue;
                                }
                            }
                            #[cfg(any(host_rf_dump_txpwr_lmt_test, ieee80211_band_5ghz))]
                            if band == BAND_ON_5G {
                                let st = cck_ofdm_state_5g(rfctl);
                                if st & (TXPWR_LMT_HAS_OFDM_1T << ntx_idx) == 0 {
                                    continue;
                                }
                            }
                        }

                        #[cfg(any(host_rf_dump_txpwr_lmt_test, ieee80211_band_5ghz))]
                        if band == BAND_ON_5G
                            && (bw == CHANNEL_WIDTH_20 || bw == CHANNEL_WIDTH_40)
                        {
                            let ref_mode = txpwr_lmt_5g_ref(rfctl);
                            if ref_mode == TXPWR_LMT_REF_HT_FROM_VHT && tlrs == TXPWR_LMT_RS_HT {
                                continue;
                            }
                            if ref_mode == TXPWR_LMT_REF_VHT_FROM_HT
                                && tlrs == TXPWR_LMT_RS_VHT
                                && bw <= CHANNEL_WIDTH_40
                            {
                                continue;
                            }
                        }

                        let rs = if tlrs == TXPWR_LMT_RS_CCK {
                            CCK
                        } else if tlrs == TXPWR_LMT_RS_OFDM {
                            OFDM
                        } else if tlrs == TXPWR_LMT_RS_HT {
                            HT_1SS + ntx_idx
                        } else if tlrs == TXPWR_LMT_RS_VHT {
                            VHT_1SS + ntx_idx
                        } else {
                            continue;
                        };

                        snprintf(
                            line.as_mut_ptr(),
                            line.len(),
                            b"[%s][%s][%s][%uT]\n\0".as_ptr() as *const c_char,
                            band_str(band),
                            ch_width_str(bw),
                            txpwr_lmt_rs_str(tlrs),
                            (ntx_idx + 1) as c_int,
                        );
                        print_line(sel, &line);

                        print_regd_header(sel, head, rf_regd, rfpath_num);

                        for n in 0..ch_num {
                            let ch = if band == BAND_ON_2_4G {
                                n + 1
                            } else {
                                center_chs_5g(bw, n)
                            };

                            if ch == 0 {
                                break;
                            }

                            snprintf(
                                line.as_mut_ptr(),
                                line.len(),
                                b"%3u \0".as_ptr() as *const c_char,
                                ch as c_int,
                            );
                            print_line(sel, &line);

                            let mut cur = list_next(head);
                            while !list_end(head, cur) {
                                let ent = ent_from_list_node(cur);
                                let name = ent_regd_name(ent);
                                let lmt = phy_get_txpwr_lmt(
                                    adapter, name, band, bw, tlrs, ntx_idx, ch, 0,
                                );
                                let mut tmp_str = [0i8; TMP_STR_LEN];
                                txpwr_idx_get_dbm_str(
                                    lmt,
                                    hal.txgi_max,
                                    hal.txgi_pdbm,
                                    strlen(name),
                                    tmp_str.as_mut_ptr(),
                                    TMP_STR_LEN as u8,
                                );
                                snprintf(
                                    line.as_mut_ptr(),
                                    line.len(),
                                    b"%s \0".as_ptr() as *const c_char,
                                    tmp_str.as_ptr(),
                                );
                                print_line(sel, &line);
                                cur = list_next(cur);
                            }

                            let ww = regd_str_ww();
                            let lmt = phy_get_txpwr_lmt(
                                adapter, ww, band, bw, tlrs, ntx_idx, ch, 0,
                            );
                            let mut tmp_str = [0i8; TMP_STR_LEN];
                            txpwr_idx_get_dbm_str(
                                lmt,
                                hal.txgi_max,
                                hal.txgi_pdbm,
                                strlen(ww),
                                tmp_str.as_mut_ptr(),
                                TMP_STR_LEN as u8,
                            );
                            snprintf(
                                line.as_mut_ptr(),
                                line.len(),
                                b"%s \0".as_ptr() as *const c_char,
                                tmp_str.as_ptr(),
                            );
                            print_line(sel, &line);

                            for path in RF_PATH_A..RF_PATH_MAX {
                                if path >= rfpath_num {
                                    break;
                                }

                                let base = phy_get_target_txpwr(adapter, band, path, rs);
                                snprintf(line.as_mut_ptr(), line.len(), b"|\0".as_ptr() as *const c_char);
                                print_line(sel, &line);

                                let mut cur = list_next(head);
                                let mut i = 0u8;
                                while !list_end(head, cur) {
                                    let ent = ent_from_list_node(cur);
                                    let name = ent_regd_name(ent);
                                    let lmt_offset = phy_get_txpwr_lmt_diff(
                                        adapter, name, band, bw, path, rs, tlrs, ntx_idx, ch, 0,
                                    );
                                    if lmt_offset as u8 == hal.txgi_max {
                                        *lmt_idx.add((i as usize) * (RF_PATH_MAX as usize)
                                            + path as usize) = hal.txgi_max as i8;
                                        snprintf(
                                            line.as_mut_ptr(),
                                            line.len(),
                                            b"%3s \0".as_ptr() as *const c_char,
                                            b"NA\0".as_ptr() as *const c_char,
                                        );
                                    } else {
                                        *lmt_idx.add((i as usize) * (RF_PATH_MAX as usize)
                                            + path as usize) =
                                            lmt_offset + base as i8;
                                        snprintf(
                                            line.as_mut_ptr(),
                                            line.len(),
                                            b"%3d \0".as_ptr() as *const c_char,
                                            lmt_offset as c_int,
                                        );
                                    }
                                    print_line(sel, &line);
                                    i += 1;
                                    cur = list_next(cur);
                                }

                                let lmt_offset = phy_get_txpwr_lmt_diff(
                                    adapter, ww, band, bw, path, rs, tlrs, ntx_idx, ch, 0,
                                );
                                if lmt_offset as u8 == hal.txgi_max {
                                    snprintf(
                                        line.as_mut_ptr(),
                                        line.len(),
                                        b"%3s \0".as_ptr() as *const c_char,
                                        b"NA\0".as_ptr() as *const c_char,
                                    );
                                } else {
                                    snprintf(
                                        line.as_mut_ptr(),
                                        line.len(),
                                        b"%3d \0".as_ptr() as *const c_char,
                                        lmt_offset as c_int,
                                    );
                                }
                                print_line(sel, &line);
                            }

                            if rfpath_num > 1 {
                                for i in 0..regd_num {
                                    let mut path = 0u8;
                                    while path < RF_PATH_MAX {
                                        if path >= rfpath_num {
                                            break;
                                        }
                                        let a = *lmt_idx.add(
                                            (i as usize) * (RF_PATH_MAX as usize) + path as usize,
                                        );
                                        let b = *lmt_idx.add(
                                            (i as usize) * (RF_PATH_MAX as usize)
                                                + ((path + 1) % rfpath_num) as usize,
                                        );
                                        if a != b {
                                            break;
                                        }
                                        path += 1;
                                    }
                                    if path >= rfpath_num {
                                        snprintf(
                                            line.as_mut_ptr(),
                                            line.len(),
                                            b" \0".as_ptr() as *const c_char,
                                        );
                                    } else {
                                        snprintf(
                                            line.as_mut_ptr(),
                                            line.len(),
                                            b"x\0".as_ptr() as *const c_char,
                                        );
                                    }
                                    print_line(sel, &line);
                                }
                            }
                            snprintf(line.as_mut_ptr(), line.len(), b"\n\0".as_ptr() as *const c_char);
                            print_line(sel, &line);
                        }
                        snprintf(line.as_mut_ptr(), line.len(), b"\n\0".as_ptr() as *const c_char);
                        print_line(sel, &line);
                    }
                }
            }
        }

        lmt_free(lmt_idx as *mut c_void, RF_PATH_MAX as u32 * regd_num as u32);
        mutex_exit(rfctl, &mut irqL);
    }
}
