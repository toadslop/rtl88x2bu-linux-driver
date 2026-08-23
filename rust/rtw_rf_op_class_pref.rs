// SPDX-License-Identifier: GPL-2.0
//! W3-56 op_class_pref lifecycle — Rust port of `core/rtw_rf_op_class_pref.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_rf_op_class_pref_test)]
use std::os::raw::{c_int, c_void};

#[cfg(not(host_rf_op_class_pref_test))]
use core::ffi::{c_int, c_void};

const _SUCCESS: c_int = 1;
const _FAIL: c_int = 0;
const UNSPECIFIED_MBM: i16 = 32767;

const CHANNEL_WIDTH_20: u8 = 0;
const CHANNEL_WIDTH_40: u8 = 1;
const CHANNEL_WIDTH_80: u8 = 2;
const CHANNEL_WIDTH_160: u8 = 3;
const CHANNEL_WIDTH_MAX: u8 = 7;

const HAL_PRIME_CHNL_OFFSET_DONT_CARE: u8 = 0;
const HAL_PRIME_CHNL_OFFSET_LOWER: u8 = 1;
const HAL_PRIME_CHNL_OFFSET_UPPER: u8 = 2;

const BAND_ON_2_4G: u8 = 0;
const BAND_ON_5G: u8 = 1;
const BAND_MAX: usize = 2;

const BAND_CAP_2G: u8 = 1;
const BAND_CAP_5G: u8 = 2;

const BW_CAP_80M: u8 = 16;
const BW_CAP_160M: u8 = 32;

const OPC_BW20: i32 = 0;
const OPC_BW40PLUS: i32 = 1;
const OPC_BW40MINUS: i32 = 2;
const OPC_BW80: i32 = 3;
const OPC_BW160: i32 = 4;
const OPC_BW80P80: i32 = 5;
const OPC_BW_NUM: usize = 6;

const MAX_CHANNEL_NUM: usize = 59;
const MAX_CHANNEL_NUM_OF_BAND: usize = 28;

const RTW_CHF_NO_IR: u8 = 1 << 0;
const RTW_CHF_DFS: u8 = 1 << 1;
const RTW_CHF_NO_HT40U: u8 = 1 << 4;
const RTW_CHF_NO_HT40L: u8 = 1 << 5;
const RTW_CHF_NO_80MHZ: u8 = 1 << 6;
const RTW_CHF_NO_160MHZ: u8 = 1 << 7;

const REG_TXPWR_CHANGE: u8 = 1;
const REG_CHANGE: u8 = 2;

const WIRELESS_11AC: u8 = 1 << 6;
const WIRELESS_MODE_24G: u8 = 0x0E;
const WIRELESS_MODE_5G: u8 = 0x70;

#[repr(C)]
pub struct OpClassT {
    pub class_id: u8,
    pub band: i32,
    pub bw: i32,
    pub len_ch_attr: *const u8,
}

#[repr(C)]
pub struct OpChT {
    pub ch: u8,
    pub bitfield: u8,
    pub max_txpwr: i16,
}

impl OpChT {
    fn static_non_op(&self) -> u8 {
        self.bitfield & 1
    }
    fn set_static_non_op(&mut self, v: u8) {
        self.bitfield = (self.bitfield & !1) | (v & 1);
    }
    fn no_ir(&self) -> u8 {
        (self.bitfield >> 1) & 1
    }
    fn set_no_ir(&mut self, v: u8) {
        self.bitfield = (self.bitfield & !2) | ((v & 1) << 1);
    }
}

#[repr(C)]
pub struct OpClassPrefT {
    pub class_id: u8,
    pub band: i32,
    pub bw: i32,
    pub ch_num: u8,
    pub op_ch_num: u8,
    pub ir_ch_num: u8,
    pub chs: [OpChT; MAX_CHANNEL_NUM_OF_BAND],
}

#[repr(C)]
pub struct RtChannelInfo {
    pub channel_num: u8,
    pub flags: u8,
}

#[repr(C)]
pub struct CountryChplan {
    pub alpha2: [u8; 2],
    pub chplan: u8,
    pub en_11ac: u8,
}

#[repr(C)]
pub struct RfCtlT {
    pub country_ent: *const CountryChplan,
    pub channel_set: [RtChannelInfo; MAX_CHANNEL_NUM],
    pub spt_op_class_ch: *mut *mut OpClassPrefT,
    pub cap_spt_op_class_num: u8,
    pub reg_spt_op_class_num: u8,
    pub cur_spt_op_class_num: u8,
}

#[repr(C)]
pub struct RegistryPriv {
    pub wireless_mode: u8,
    pub bw_mode: u8,
    pub vht_enable: u8,
}

#[cfg(host_rf_op_class_pref_test)]
#[repr(C)]
pub struct Adapter {
    pub registrypriv: RegistryPriv,
    pub rf_ctl: RfCtlT,
}

#[cfg(not(host_rf_op_class_pref_test))]
pub type Adapter = c_void;

extern "C" {
    static global_op_class: [OpClassT; 19];
    static global_op_class_num: c_int;
    static _band_to_band_cap: [u8; 3];
    static _opc_bw_to_ch_width: [u8; OPC_BW_NUM];
    static _ch_width_to_bw_cap: [u8; CHANNEL_WIDTH_MAX as usize];

    fn rtw_get_center_ch(ch: u8, bw: u8, offset: u8) -> u8;
    fn rtw_get_op_chs_by_cch_bw(cch: u8, bw: u8, op_chs: *mut *mut u8, op_ch_num: *mut u8) -> u8;
}

#[cfg(host_rf_op_class_pref_test)]
extern "C" {
    fn rtw_zmalloc(sz: u32) -> *mut c_void;
    fn rtw_mfree(p: *mut c_void, sz: u32);
    fn hal_chk_band_cap(adapter: *mut Adapter, cap: u8) -> bool;
    fn host_rf_op_class_pref_hal_spec() -> *mut HalSpec;
}

#[cfg(host_rf_op_class_pref_test)]
fn adapter_hal_chk_band_cap(adapter: *mut Adapter, cap: u8) -> bool {
    unsafe { hal_chk_band_cap(adapter, cap) }
}

#[cfg(host_rf_op_class_pref_test)]
#[repr(C)]
pub struct HalSpec {
    pub bw_cap: u8,
}

#[cfg(not(host_rf_op_class_pref_test))]
mod kernel {
    use super::*;

    pub type RfCtlPtr = *mut c_void;

    extern "C" {
        pub fn rtw_rust_opc_pref_zmalloc(sz: u32) -> *mut c_void;
        pub fn rtw_rust_opc_pref_mfree(p: *mut c_void, sz: u32);
        pub fn rtw_rust_opc_pref_hal_chk_band_cap(adapter: *mut Adapter, cap: u8) -> u8;
        pub fn rtw_rust_opc_pref_hal_bw_cap(adapter: *mut Adapter) -> u8;
        pub fn rtw_rust_opc_pref_wireless_mode(adapter: *mut Adapter) -> u8;
        pub fn rtw_rust_opc_pref_bw_mode(adapter: *mut Adapter) -> u8;
        pub fn rtw_rust_opc_pref_vht_enable(adapter: *mut Adapter) -> u8;
        pub fn rtw_rust_opc_pref_rfctl(adapter: *mut Adapter) -> RfCtlPtr;
        pub fn rtw_rust_opc_pref_spt_op_class_ch_get(rfctl: RfCtlPtr) -> *mut *mut OpClassPrefT;
        pub fn rtw_rust_opc_pref_spt_op_class_ch_set(
            rfctl: RfCtlPtr,
            table: *mut *mut OpClassPrefT,
        );
        pub fn rtw_rust_opc_pref_country_ent(rfctl: RfCtlPtr) -> *const CountryChplan;
        pub fn rtw_rust_opc_pref_cap_spt_op_class_num(rfctl: RfCtlPtr) -> *mut u8;
        pub fn rtw_rust_opc_pref_reg_spt_op_class_num(rfctl: RfCtlPtr) -> *mut u8;
        pub fn rtw_rust_opc_pref_cur_spt_op_class_num(rfctl: RfCtlPtr) -> *mut u8;
        pub fn rtw_rust_opc_pref_get_reg_max_txpwr_mbm(
            rfctl: RfCtlPtr,
            ch: u8,
            bw: u8,
            offset: u8,
            eirp: u8,
        ) -> i16;
        pub fn rtw_rust_opc_pref_dfs_domain_unknown(rfctl: RfCtlPtr) -> u8;
        pub fn rtw_rust_opc_pref_chset_search_ch(rfctl: RfCtlPtr, ch: u32) -> c_int;
        pub fn rtw_rust_opc_pref_chset_flags(rfctl: RfCtlPtr, idx: c_int) -> u8;
    }
}

#[cfg(host_rf_op_class_pref_test)]
type RfCtlPtr = *mut RfCtlT;

#[cfg(not(host_rf_op_class_pref_test))]
type RfCtlPtr = kernel::RfCtlPtr;

fn opc_ch_list_len(opc: &OpClassT) -> u8 {
    unsafe { *opc.len_ch_attr }
}

fn opc_ch_list_ch(opc: &OpClassT, i: u8) -> u8 {
    unsafe { *opc.len_ch_attr.add(i as usize + 1) }
}

fn band_to_band_cap(band: i32) -> u8 {
    if band >= 0 && (band as usize) < BAND_MAX {
        unsafe { _band_to_band_cap[band as usize] }
    } else {
        unsafe { _band_to_band_cap[BAND_MAX] }
    }
}

fn opc_bw_to_ch_width(bw: i32) -> u8 {
    if bw >= 0 && (bw as usize) < OPC_BW_NUM {
        unsafe { _opc_bw_to_ch_width[bw as usize] }
    } else {
        CHANNEL_WIDTH_MAX
    }
}

fn ch_width_to_bw_cap(bw: u8) -> u8 {
    if (bw as usize) < CHANNEL_WIDTH_MAX as usize {
        unsafe { _ch_width_to_bw_cap[bw as usize] }
    } else {
        0
    }
}

fn bw_mode_2g(bw_mode: u8) -> u8 {
    bw_mode & 0x0F
}

fn bw_mode_5g(bw_mode: u8) -> u8 {
    bw_mode >> 4
}

fn is_supported_24g(mode: u8) -> bool {
    (mode & WIRELESS_MODE_24G) != 0
}

fn is_supported_5g(mode: u8) -> bool {
    (mode & WIRELESS_MODE_5G) != 0
}

fn is_supported_vht(mode: u8) -> bool {
    (mode & WIRELESS_11AC) != 0
}

fn country_chplan_en_11ac(ent: *const CountryChplan) -> u8 {
    if ent.is_null() {
        0
    } else {
        unsafe { (*ent).en_11ac }
    }
}

#[cfg(host_rf_op_class_pref_test)]
fn adapter_hal_bw_cap(_adapter: *mut Adapter) -> u8 {
    unsafe { (*host_rf_op_class_pref_hal_spec()).bw_cap }
}

#[cfg(not(host_rf_op_class_pref_test))]
fn adapter_hal_bw_cap(adapter: *mut Adapter) -> u8 {
    unsafe { kernel::rtw_rust_opc_pref_hal_bw_cap(adapter) }
}

#[cfg(not(host_rf_op_class_pref_test))]
fn adapter_hal_chk_band_cap(adapter: *mut Adapter, cap: u8) -> bool {
    unsafe { kernel::rtw_rust_opc_pref_hal_chk_band_cap(adapter, cap) != 0 }
}

fn zmalloc(sz: u32) -> *mut c_void {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        return rtw_zmalloc(sz);
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        return kernel::rtw_rust_opc_pref_zmalloc(sz);
    }
}

fn mfree(p: *mut c_void, sz: u32) {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        rtw_mfree(p, sz);
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        kernel::rtw_rust_opc_pref_mfree(p, sz);
    }
}

fn adapter_regsty(adapter: *mut Adapter) -> *mut RegistryPriv {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        return &mut (*adapter).registrypriv;
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    {
        let _ = adapter;
        core::ptr::null_mut()
    }
}

fn adapter_rfctl(adapter: *mut Adapter) -> RfCtlPtr {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        return &mut (*adapter).rf_ctl;
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        kernel::rtw_rust_opc_pref_rfctl(adapter)
    }
}

fn rfctl_spt_op_class_ch_get(rfctl: RfCtlPtr) -> *mut *mut OpClassPrefT {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        (*rfctl).spt_op_class_ch
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        kernel::rtw_rust_opc_pref_spt_op_class_ch_get(rfctl)
    }
}

fn rfctl_spt_op_class_ch_set(rfctl: RfCtlPtr, table: *mut *mut OpClassPrefT) {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        (*rfctl).spt_op_class_ch = table;
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        kernel::rtw_rust_opc_pref_spt_op_class_ch_set(rfctl, table);
    }
}

fn rfctl_country_ent(rfctl: RfCtlPtr) -> *const CountryChplan {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        (*rfctl).country_ent
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        kernel::rtw_rust_opc_pref_country_ent(rfctl)
    }
}

fn rfctl_cap_spt_op_class_num_ptr(rfctl: RfCtlPtr) -> *mut u8 {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        &mut (*rfctl).cap_spt_op_class_num
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        kernel::rtw_rust_opc_pref_cap_spt_op_class_num(rfctl)
    }
}

fn rfctl_reg_spt_op_class_num_ptr(rfctl: RfCtlPtr) -> *mut u8 {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        &mut (*rfctl).reg_spt_op_class_num
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        kernel::rtw_rust_opc_pref_reg_spt_op_class_num(rfctl)
    }
}

fn rfctl_cur_spt_op_class_num_ptr(rfctl: RfCtlPtr) -> *mut u8 {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        &mut (*rfctl).cur_spt_op_class_num
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        kernel::rtw_rust_opc_pref_cur_spt_op_class_num(rfctl)
    }
}

fn regsty_wireless_mode(adapter: *mut Adapter) -> u8 {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        (*adapter_regsty(adapter)).wireless_mode
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        kernel::rtw_rust_opc_pref_wireless_mode(adapter)
    }
}

fn regsty_bw_mode(adapter: *mut Adapter) -> u8 {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        (*adapter_regsty(adapter)).bw_mode
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        kernel::rtw_rust_opc_pref_bw_mode(adapter)
    }
}

fn regsty_vht_enable(adapter: *mut Adapter) -> u8 {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        (*adapter_regsty(adapter)).vht_enable
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        kernel::rtw_rust_opc_pref_vht_enable(adapter)
    }
}

fn regsty_is_11ac_enable(adapter: *mut Adapter) -> bool {
    regsty_vht_enable(adapter) != 0
}

fn get_reg_max_txpwr_mbm(rfctl: RfCtlPtr, ch: u8, bw: u8, offset: u8) -> i16 {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        extern "C" {
            fn rtw_rfctl_get_reg_max_txpwr_mbm(
                rfctl: *mut RfCtlT,
                ch: u8,
                bw: u8,
                offset: u8,
                eirp: bool,
            ) -> i16;
        }
        return rtw_rfctl_get_reg_max_txpwr_mbm(rfctl, ch, bw, offset, true);
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        kernel::rtw_rust_opc_pref_get_reg_max_txpwr_mbm(rfctl, ch, bw, offset, 1)
    }
}

fn dfs_domain_unknown(rfctl: RfCtlPtr) -> bool {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        extern "C" {
            fn rtw_rfctl_dfs_domain_unknown(rfctl: *mut RfCtlT) -> u8;
        }
        return rtw_rfctl_dfs_domain_unknown(rfctl) != 0;
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        kernel::rtw_rust_opc_pref_dfs_domain_unknown(rfctl) != 0
    }
}

fn chset_search_ch(rfctl: RfCtlPtr, ch: u32) -> c_int {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        extern "C" {
            fn rtw_chset_search_ch(ch_set: *mut RtChannelInfo, ch: u32) -> c_int;
        }
        return rtw_chset_search_ch((*rfctl).channel_set.as_mut_ptr(), ch);
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        kernel::rtw_rust_opc_pref_chset_search_ch(rfctl, ch)
    }
}

fn chset_flags(rfctl: RfCtlPtr, idx: c_int) -> u8 {
    #[cfg(host_rf_op_class_pref_test)]
    unsafe {
        (*rfctl).channel_set[idx as usize].flags
    }
    #[cfg(not(host_rf_op_class_pref_test))]
    unsafe {
        kernel::rtw_rust_opc_pref_chset_flags(rfctl, idx)
    }
}

fn opc_pref_alloc(class_id: u8) -> *mut OpClassPrefT {
    let mut idx = 0usize;
    while idx < unsafe { global_op_class.len() } {
        let ent = unsafe { &global_op_class[idx] };
        if ent.class_id == class_id {
            break;
        }
        idx += 1;
    }
    if idx >= unsafe { global_op_class.len() } {
        return core::ptr::null_mut();
    }
    let ent = unsafe { &global_op_class[idx] };
    let opc_pref = zmalloc(core::mem::size_of::<OpClassPrefT>() as u32) as *mut OpClassPrefT;
    if opc_pref.is_null() {
        return core::ptr::null_mut();
    }
    unsafe {
        (*opc_pref).class_id = ent.class_id;
        (*opc_pref).band = ent.band;
        (*opc_pref).bw = ent.bw;
        let ch_len = opc_ch_list_len(ent);
        let mut j = 0u8;
        while j < ch_len {
            (*opc_pref).chs[j as usize].ch = opc_ch_list_ch(ent, j);
            (*opc_pref).chs[j as usize].set_static_non_op(1);
            (*opc_pref).chs[j as usize].set_no_ir(1);
            (*opc_pref).chs[j as usize].max_txpwr = UNSPECIFIED_MBM;
            j += 1;
        }
        (*opc_pref).ch_num = ch_len;
    }
    opc_pref
}

fn opc_pref_free(opc_pref: *mut OpClassPrefT) {
    if !opc_pref.is_null() {
        mfree(
            opc_pref as *mut c_void,
            core::mem::size_of::<OpClassPrefT>() as u32,
        );
    }
}

fn opc_bw_to_bw_offset(opc_bw: i32) -> (u8, u8) {
    match opc_bw {
        OPC_BW20 => (CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE),
        OPC_BW40PLUS => (CHANNEL_WIDTH_40, HAL_PRIME_CHNL_OFFSET_LOWER),
        OPC_BW40MINUS => (CHANNEL_WIDTH_40, HAL_PRIME_CHNL_OFFSET_UPPER),
        OPC_BW80 => (CHANNEL_WIDTH_80, HAL_PRIME_CHNL_OFFSET_DONT_CARE),
        OPC_BW160 => (CHANNEL_WIDTH_160, HAL_PRIME_CHNL_OFFSET_DONT_CARE),
        _ => (CHANNEL_WIDTH_MAX, HAL_PRIME_CHNL_OFFSET_DONT_CARE),
    }
}

#[no_mangle]
pub extern "C" fn op_class_pref_init(adapter: *mut Adapter) -> c_int {
    if adapter.is_null() {
        return _FAIL;
    }
    let rfctl = adapter_rfctl(adapter);
    let hal_cap = adapter_hal_bw_cap(adapter);
    let wireless_mode = regsty_wireless_mode(adapter);
    let bw_mode = regsty_bw_mode(adapter);
    let mut band_bmp = 0u8;
    let mut bw_bmp = [0u8; BAND_MAX];
    let mut op_class_num = 0u8;

    unsafe {
        let n = global_op_class_num as usize;
        let table = zmalloc((core::mem::size_of::<*mut OpClassPrefT>() * n) as u32)
            as *mut *mut OpClassPrefT;
        if table.is_null() {
            return _FAIL;
        }
        rfctl_spt_op_class_ch_set(rfctl, table);

        if is_supported_24g(wireless_mode) && adapter_hal_chk_band_cap(adapter, BAND_CAP_2G) {
            band_bmp |= BAND_CAP_2G;
        }
        if is_supported_5g(wireless_mode) && adapter_hal_chk_band_cap(adapter, BAND_CAP_5G) {
            band_bmp |= BAND_CAP_5G;
        }

        bw_bmp[BAND_ON_2_4G as usize] = (ch_width_to_bw_cap(bw_mode_2g(bw_mode) + 1) - 1) & hal_cap;
        bw_bmp[BAND_ON_5G as usize] = (ch_width_to_bw_cap(bw_mode_5g(bw_mode) + 1) - 1) & hal_cap;
        if !regsty_is_11ac_enable(adapter) || !is_supported_vht(wireless_mode) {
            bw_bmp[BAND_ON_5G as usize] &= !(BW_CAP_80M | BW_CAP_160M);
        }

        for i in 0..n {
            let g = &global_op_class[i];
            if (band_bmp & band_to_band_cap(g.band)) == 0 {
                continue;
            }
            let ch_w = opc_bw_to_ch_width(g.bw);
            if ch_w == CHANNEL_WIDTH_MAX || g.bw == OPC_BW80P80 {
                continue;
            }
            if g.band < 0 || (g.band as usize) >= BAND_MAX {
                continue;
            }
            if (bw_bmp[g.band as usize] & ch_width_to_bw_cap(ch_w)) == 0 {
                continue;
            }
            let opc_pref = opc_pref_alloc(g.class_id);
            if opc_pref.is_null() {
                op_class_pref_deinit(adapter);
                return _FAIL;
            }
            if (*opc_pref).ch_num != 0 {
                *rfctl_spt_op_class_ch_get(rfctl).add(i) = opc_pref;
                op_class_num = op_class_num.wrapping_add(1);
            } else {
                opc_pref_free(opc_pref);
            }
        }
        *rfctl_cap_spt_op_class_num_ptr(rfctl) = op_class_num;
    }
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn op_class_pref_deinit(adapter: *mut Adapter) {
    if adapter.is_null() {
        return;
    }
    let rfctl = adapter_rfctl(adapter);
    unsafe {
        let table = rfctl_spt_op_class_ch_get(rfctl);
        if table.is_null() {
            return;
        }
        let n = global_op_class_num as usize;
        for i in 0..n {
            let slot = table.add(i);
            if !(*slot).is_null() {
                opc_pref_free(*slot);
                *slot = core::ptr::null_mut();
            }
        }
        mfree(
            table as *mut c_void,
            (core::mem::size_of::<*mut OpClassPrefT>() * n) as u32,
        );
        rfctl_spt_op_class_ch_set(rfctl, core::ptr::null_mut());
    }
}

#[no_mangle]
pub extern "C" fn op_class_pref_apply_regulatory(adapter: *mut Adapter, reason: u8) {
    if adapter.is_null() {
        return;
    }
    let rfctl = adapter_rfctl(adapter);
    unsafe {
        let table = rfctl_spt_op_class_ch_get(rfctl);
        if table.is_null() {
            return;
        }
        let mut reg_op_class_num = 0u8;
        let mut op_class_num = 0u8;
        let n = global_op_class_num as usize;

        for i in 0..n {
            let slot = *table.add(i);
            if slot.is_null() {
                continue;
            }
            let opc_pref = slot;
            let mut j = 0usize;
            while j < MAX_CHANNEL_NUM_OF_BAND && (*opc_pref).chs[j].ch != 0 {
                if reason >= REG_CHANGE {
                    (*opc_pref).chs[j].set_static_non_op(1);
                }
                if reason != REG_TXPWR_CHANGE {
                    (*opc_pref).chs[j].set_no_ir(1);
                }
                if reason >= REG_TXPWR_CHANGE {
                    (*opc_pref).chs[j].max_txpwr = UNSPECIFIED_MBM;
                }
                j += 1;
            }
            if reason >= REG_CHANGE {
                (*opc_pref).op_ch_num = 0;
            }
            if reason != REG_TXPWR_CHANGE {
                (*opc_pref).ir_ch_num = 0;
            }

            let (bw, offset) = opc_bw_to_bw_offset((*opc_pref).bw);
            if bw == CHANNEL_WIDTH_MAX {
                continue;
            }
            let country_ent = rfctl_country_ent(rfctl);
            if !country_ent.is_null()
                && country_chplan_en_11ac(country_ent) == 0
                && (bw == CHANNEL_WIDTH_80 || bw == CHANNEL_WIDTH_160)
            {
                continue;
            }

            j = 0;
            while j < MAX_CHANNEL_NUM_OF_BAND && (*opc_pref).chs[j].ch != 0 {
                let ch_ent = &mut (*opc_pref).chs[j];
                let ch = ch_ent.ch;

                if reason >= REG_TXPWR_CHANGE {
                    ch_ent.max_txpwr = get_reg_max_txpwr_mbm(rfctl, ch, bw, offset);
                }
                if reason == REG_TXPWR_CHANGE {
                    j += 1;
                    continue;
                }

                let cch = rtw_get_center_ch(ch, bw, offset);
                if cch == 0 {
                    j += 1;
                    continue;
                }

                let mut op_chs: *mut u8 = core::ptr::null_mut();
                let mut op_ch_num = 0u8;
                if rtw_get_op_chs_by_cch_bw(cch, bw, &mut op_chs, &mut op_ch_num) == 0 {
                    j += 1;
                    continue;
                }

                let mut k = 0u8;
                let mut l = 0u8;
                while (k as usize) < op_ch_num as usize {
                    let op_ch = *op_chs.add(k as usize);
                    let chset_idx = chset_search_ch(rfctl, op_ch as u32);
                    if chset_idx == -1 {
                        break;
                    }
                    let ch_flags = chset_flags(rfctl, chset_idx);
                    if bw >= CHANNEL_WIDTH_40 {
                        if (ch_flags & RTW_CHF_NO_HT40U) != 0 && k % 2 == 0 {
                            break;
                        }
                        if (ch_flags & RTW_CHF_NO_HT40L) != 0 && k % 2 == 1 {
                            break;
                        }
                    }
                    if bw >= CHANNEL_WIDTH_80 && (ch_flags & RTW_CHF_NO_80MHZ) != 0 {
                        break;
                    }
                    if bw >= CHANNEL_WIDTH_160 && (ch_flags & RTW_CHF_NO_160MHZ) != 0 {
                        break;
                    }
                    if (ch_flags & RTW_CHF_DFS) != 0 && dfs_domain_unknown(rfctl) {
                        k += 1;
                        continue;
                    }
                    if (ch_flags & RTW_CHF_NO_IR) != 0 {
                        k += 1;
                        continue;
                    }
                    l += 1;
                    k += 1;
                }
                if (k as usize) < op_ch_num as usize {
                    j += 1;
                    continue;
                }

                if reason >= REG_CHANGE {
                    ch_ent.set_static_non_op(0);
                    (*opc_pref).op_ch_num = (*opc_pref).op_ch_num.wrapping_add(1);
                }
                if l >= op_ch_num {
                    ch_ent.set_no_ir(0);
                    (*opc_pref).ir_ch_num = (*opc_pref).ir_ch_num.wrapping_add(1);
                }
                j += 1;
            }

            if (*opc_pref).op_ch_num != 0 {
                reg_op_class_num = reg_op_class_num.wrapping_add(1);
            }
            if (*opc_pref).ir_ch_num != 0 {
                op_class_num = op_class_num.wrapping_add(1);
            }
        }
        *rfctl_reg_spt_op_class_num_ptr(rfctl) = reg_op_class_num;
        *rfctl_cur_spt_op_class_num_ptr(rfctl) = op_class_num;
    }
}
