// SPDX-License-Identifier: GPL-2.0
//! Tx rate bitmap helpers — Rust port of `core/rtw_xmit_rest.c` (W3-40).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_xmit_test)]
use std::os::raw::c_void;
#[cfg(not(host_xmit_test))]
use core::ffi::c_void;

type U8 = u8;
type U16 = u16;
type U32 = u32;
type U64 = u64;

const CHANNEL_WIDTH_20: U8 = 0;
const CHANNEL_WIDTH_40: U8 = 1;
const CHANNEL_WIDTH_80: U8 = 2;
const CHANNEL_WIDTH_160: U8 = 3;
const CHANNEL_WIDTH_MAX: usize = 7;

const WIFI_ASOC_STATE: i32 = 0x0000_0001;
const MGN_MCS0: U8 = 0x80;
const MGN_MCS31: U8 = 0x9F;
const MGN_VHT1SS_MCS0: U8 = 0xA0;
const MGN_VHT4SS_MCS9: U8 = 0xD9;

const BW_CAP_20M: U8 = 1 << 2;
const BW_CAP_40M: U8 = 1 << 3;
const BW_CAP_80M: U8 = 1 << 4;
const BW_CAP_160M: U8 = 1 << 5;
const BW_CAP_80_80M: U8 = 1 << 6;
const BW_CAP_5M: U8 = 1;
const BW_CAP_10M: U8 = 1 << 1;

const MACID_NUM_SW_LIMIT: usize = 32;
#[cfg(host_xmit_test)]
const CONFIG_IFACE_NUMBER: usize = 2;

static CH_WIDTH_TO_BW_CAP: [U8; CHANNEL_WIDTH_MAX] = [
    BW_CAP_20M,
    BW_CAP_40M,
    BW_CAP_80M,
    BW_CAP_160M,
    BW_CAP_80_80M,
    BW_CAP_5M,
    BW_CAP_10M,
];

#[inline]
fn ch_width_to_bw_cap(bw: U8) -> U8 {
    if (bw as usize) < CHANNEL_WIDTH_MAX {
        CH_WIDTH_TO_BW_CAP[bw as usize]
    } else {
        0
    }
}

#[inline]
fn rtw_min(a: U8, b: U8) -> U8 {
    if a > b { b } else { a }
}

#[inline]
fn is_ht_rate(rate: U8) -> bool {
    rate >= MGN_MCS0 && rate <= MGN_MCS31
}

#[inline]
fn is_vht_rate(rate: U8) -> bool {
    rate >= MGN_VHT1SS_MCS0 && rate <= MGN_VHT4SS_MCS9
}

#[inline]
fn bw_mode_2g(mode: U8) -> U8 {
    mode & 0x0f
}

#[inline]
fn bw_mode_5g(mode: U8) -> U8 {
    mode >> 4
}

#[cfg(host_xmit_test)]
mod host {
    use super::*;

    #[repr(C)]
    struct MacidBmp {
        m0: U32,
    }

    #[repr(C)]
    pub(crate) struct MacidCtl {
        pub(super) num: U8,
        used: MacidBmp,
        bmc: MacidBmp,
        if_g: [MacidBmp; CONFIG_IFACE_NUMBER],
        ch_g: [MacidBmp; 2],
        iface_bmc: [U8; CONFIG_IFACE_NUMBER],
        h2c_msr: [U8; MACID_NUM_SW_LIMIT],
        pub(super) bw: [U8; MACID_NUM_SW_LIMIT],
        pub(super) vht_en: [U8; MACID_NUM_SW_LIMIT],
        pub(super) rate_bmp0: [U32; MACID_NUM_SW_LIMIT],
        pub(super) rate_bmp1: [U32; MACID_NUM_SW_LIMIT],
    }

    #[repr(C)]
    pub(crate) struct RfCtl {
        pub(super) rate_bmp_ht_by_bw: [U32; 2],
        pub(super) rate_bmp_vht_by_bw: [U64; 4],
    }

    #[repr(C)]
    pub struct DvobjPriv {
        pub macid_ctl: MacidCtl,
        pub rf_ctl: RfCtl,
    }

    #[repr(C)]
    struct MlmePriv {
        fw_state: i32,
    }

    #[repr(C)]
    struct MlmeExtPriv {
        cur_channel: U8,
    }

    #[repr(C)]
    struct StaCmn {
        bw_mode: U8,
    }

    #[repr(C)]
    pub struct StaInfo {
        pub cmn: StaCmn,
    }

    #[repr(C)]
    pub struct Adapter {
        pub dvobj: *mut DvobjPriv,
        pub mlmepriv: MlmePriv,
        pub mlmeextpriv: MlmeExtPriv,
        pub driver_tx_bw_mode: U8,
        pub fix_rate: U8,
        pub fix_bw: U8,
        pub iface_id: U8,
    }

    fn macid_is_set(map: &MacidBmp, id: U8) -> bool {
        if id < 32 {
            map.m0 & (1 << id) != 0
        } else {
            false
        }
    }

    pub(super) fn macid_is_used(ctl: &MacidCtl, id: U8) -> bool {
        macid_is_set(&ctl.used, id)
    }

    pub(super) fn macid_is_iface_shared(ctl: &MacidCtl, id: U8) -> bool {
        let mut iface_bmp = 0u8;
        for i in 0..CONFIG_IFACE_NUMBER {
            if macid_is_set(&ctl.if_g[i], id) {
                if iface_bmp != 0 {
                    return true;
                }
                iface_bmp |= 1 << i;
            }
        }
        false
    }

    pub(super) fn macid_is_iface_specific(ctl: &MacidCtl, id: U8, adapter: &Adapter) -> bool {
        let mut iface_bmp = 0u8;
        for i in 0..CONFIG_IFACE_NUMBER {
            if macid_is_set(&ctl.if_g[i], id) {
                if iface_bmp != 0 || i != adapter.iface_id as usize {
                    return false;
                }
                iface_bmp |= 1 << i;
            }
        }
        iface_bmp != 0
    }

    pub(super) fn sta_bw_mode(sta: &StaInfo) -> U8 {
        sta.cmn.bw_mode
    }

    pub(super) fn mlme_state(adapter: &Adapter) -> i32 {
        adapter.mlmepriv.fw_state
    }

    pub(super) fn cur_channel(adapter: &Adapter) -> U8 {
        adapter.mlmeextpriv.cur_channel
    }

    pub(super) fn driver_tx_bw_mode(adapter: &Adapter) -> U8 {
        adapter.driver_tx_bw_mode
    }

    pub(super) fn fix_rate(adapter: &Adapter) -> U8 {
        adapter.fix_rate
    }

    pub(super) fn fix_bw(adapter: &Adapter) -> U8 {
        adapter.fix_bw
    }

    pub(super) fn macid_ctl(dvobj: &DvobjPriv) -> &MacidCtl {
        &dvobj.macid_ctl
    }

    pub(super) fn rfctl(dvobj: &DvobjPriv) -> &RfCtl {
        &dvobj.rf_ctl
    }
}

#[cfg(not(host_xmit_test))]
mod kernel {
    use super::*;

    extern "C" {
        fn rtw_macid_is_used(macid_ctl: *mut c_void, id: U8) -> bool;
        fn rtw_macid_is_iface_specific(macid_ctl: *mut c_void, id: U8, adapter: *mut c_void) -> bool;
        fn rtw_macid_is_iface_shared(macid_ctl: *mut c_void, id: U8) -> bool;
        fn rtw_rust_xmit_adapter_dvobj(adapter: *mut c_void) -> *mut c_void;
        fn rtw_rust_xmit_sta_bw_mode(sta: *mut c_void) -> U8;
        fn rtw_rust_xmit_mlme_state(adapter: *mut c_void) -> i32;
        fn rtw_rust_xmit_cur_channel(adapter: *mut c_void) -> U8;
        fn rtw_rust_xmit_driver_tx_bw_mode(adapter: *mut c_void) -> U8;
        fn rtw_rust_xmit_fix_rate(adapter: *mut c_void) -> U8;
        fn rtw_rust_xmit_fix_bw(adapter: *mut c_void) -> U8;
        fn rtw_rust_xmit_macid_ctl(dvobj: *mut c_void) -> *mut c_void;
        fn rtw_rust_xmit_rfctl(dvobj: *mut c_void) -> *mut c_void;
        fn rtw_rust_xmit_macid_num(macid_ctl: *mut c_void) -> U8;
        fn rtw_rust_xmit_macid_bw(macid_ctl: *mut c_void, id: U8) -> U8;
        fn rtw_rust_xmit_macid_vht_en(macid_ctl: *mut c_void, id: U8) -> U8;
        fn rtw_rust_xmit_macid_rate_bmp0(macid_ctl: *mut c_void, id: U8) -> U32;
        fn rtw_rust_xmit_macid_rate_bmp1(macid_ctl: *mut c_void, id: U8) -> U32;
        fn rtw_rust_xmit_rf_ht_bmp(rfctl: *mut c_void, bw: U8) -> U32;
        fn rtw_rust_xmit_rf_vht_bmp(rfctl: *mut c_void, bw: U8) -> U64;
    }

    pub(super) unsafe fn adapter_dvobj(adapter: *mut c_void) -> *mut c_void {
        unsafe { rtw_rust_xmit_adapter_dvobj(adapter) }
    }
    pub(super) unsafe fn sta_bw_mode(sta: *mut c_void) -> U8 {
        unsafe { rtw_rust_xmit_sta_bw_mode(sta) }
    }
    pub(super) unsafe fn mlme_state(adapter: *mut c_void) -> i32 {
        unsafe { rtw_rust_xmit_mlme_state(adapter) }
    }
    pub(super) unsafe fn cur_channel(adapter: *mut c_void) -> U8 {
        unsafe { rtw_rust_xmit_cur_channel(adapter) }
    }
    pub(super) unsafe fn driver_tx_bw_mode(adapter: *mut c_void) -> U8 {
        unsafe { rtw_rust_xmit_driver_tx_bw_mode(adapter) }
    }
    pub(super) unsafe fn fix_rate(adapter: *mut c_void) -> U8 {
        unsafe { rtw_rust_xmit_fix_rate(adapter) }
    }
    pub(super) unsafe fn fix_bw(adapter: *mut c_void) -> U8 {
        unsafe { rtw_rust_xmit_fix_bw(adapter) }
    }
    pub(super) unsafe fn macid_ctl(dvobj: *mut c_void) -> *mut c_void {
        unsafe { rtw_rust_xmit_macid_ctl(dvobj) }
    }
    pub(super) unsafe fn rfctl(dvobj: *mut c_void) -> *mut c_void {
        unsafe { rtw_rust_xmit_rfctl(dvobj) }
    }
    pub(super) unsafe fn macid_num(macid_ctl: *mut c_void) -> U8 {
        unsafe { rtw_rust_xmit_macid_num(macid_ctl) }
    }
    pub(super) unsafe fn macid_is_used(macid_ctl: *mut c_void, id: U8) -> bool {
        unsafe { rtw_macid_is_used(macid_ctl, id) }
    }
    pub(super) unsafe fn macid_is_iface_specific(
        macid_ctl: *mut c_void,
        id: U8,
        adapter: *mut c_void,
    ) -> bool {
        unsafe { rtw_macid_is_iface_specific(macid_ctl, id, adapter) }
    }
    pub(super) unsafe fn macid_is_iface_shared(macid_ctl: *mut c_void, id: U8) -> bool {
        unsafe { rtw_macid_is_iface_shared(macid_ctl, id) }
    }
    pub(super) unsafe fn macid_bw(macid_ctl: *mut c_void, id: U8) -> U8 {
        unsafe { rtw_rust_xmit_macid_bw(macid_ctl, id) }
    }
    pub(super) unsafe fn macid_vht_en(macid_ctl: *mut c_void, id: U8) -> U8 {
        unsafe { rtw_rust_xmit_macid_vht_en(macid_ctl, id) }
    }
    pub(super) unsafe fn macid_rate_bmp0(macid_ctl: *mut c_void, id: U8) -> U32 {
        unsafe { rtw_rust_xmit_macid_rate_bmp0(macid_ctl, id) }
    }
    pub(super) unsafe fn macid_rate_bmp1(macid_ctl: *mut c_void, id: U8) -> U32 {
        unsafe { rtw_rust_xmit_macid_rate_bmp1(macid_ctl, id) }
    }
    pub(super) unsafe fn rf_ht_bmp(rfctl: *mut c_void, bw: U8) -> U32 {
        unsafe { rtw_rust_xmit_rf_ht_bmp(rfctl, bw) }
    }
    pub(super) unsafe fn rf_vht_bmp(rfctl: *mut c_void, bw: U8) -> U64 {
        unsafe { rtw_rust_xmit_rf_vht_bmp(rfctl, bw) }
    }
}

#[no_mangle]
pub extern "C" fn rtw_get_tx_bw_mode(adapter: *mut c_void, sta: *mut c_void) -> U8 {
    if adapter.is_null() || sta.is_null() {
        return 0;
    }
    #[cfg(host_xmit_test)]
    {
        let adapter = unsafe { &*(adapter as *const host::Adapter) };
        let sta = unsafe { &*(sta as *const host::StaInfo) };
        let mut bw = host::sta_bw_mode(sta);
        if host::mlme_state(adapter) & WIFI_ASOC_STATE != 0 {
            if host::cur_channel(adapter) <= 14 {
                bw = rtw_min(bw, bw_mode_2g(host::driver_tx_bw_mode(adapter)));
            } else {
                bw = rtw_min(bw, bw_mode_5g(host::driver_tx_bw_mode(adapter)));
            }
        }
        bw
    }
    #[cfg(not(host_xmit_test))]
    {
        let mut bw = unsafe { kernel::sta_bw_mode(sta) };
        if unsafe { kernel::mlme_state(adapter) } & WIFI_ASOC_STATE != 0 {
            if unsafe { kernel::cur_channel(adapter) } <= 14 {
                bw = rtw_min(bw, bw_mode_2g(unsafe { kernel::driver_tx_bw_mode(adapter) }));
            } else {
                bw = rtw_min(bw, bw_mode_5g(unsafe { kernel::driver_tx_bw_mode(adapter) }));
            }
        }
        bw
    }
}

fn accumulate_rate_bmp(
    bw: U8,
    fix_bw: U8,
    macid_bw: U8,
    vht_en: U8,
    rate_bmp0: U32,
    rate_bmp1: U32,
    bmp_cck_ofdm: &mut U16,
    bmp_ht: &mut U32,
    bmp_vht: &mut U64,
) {
    if bw == CHANNEL_WIDTH_20 {
        *bmp_cck_ofdm |= (rate_bmp0 & 0x0000_0fff) as U16;
    }
    if (fix_bw != 0xff && fix_bw != bw) || (fix_bw == 0xff && macid_bw != bw) {
        return;
    }
    if vht_en != 0 {
        *bmp_vht |= ((rate_bmp0 >> 12) as U64) | ((rate_bmp1 as U64) << 20);
    } else {
        *bmp_ht |= (rate_bmp0 >> 12) | (rate_bmp1 << 20);
    }
}

#[no_mangle]
pub extern "C" fn rtw_get_adapter_tx_rate_bmp_by_bw(
    adapter: *mut c_void,
    bw: U8,
    r_bmp_cck_ofdm: *mut U16,
    r_bmp_ht: *mut U32,
    r_bmp_vht: *mut U64,
) {
    if adapter.is_null() {
        return;
    }
    let mut bmp_cck_ofdm = 0u16;
    let mut bmp_ht = 0u32;
    let mut bmp_vht = 0u64;
    #[cfg(host_xmit_test)]
    {
        let adapter = unsafe { &*(adapter as *const host::Adapter) };
        let dvobj = unsafe { &*adapter.dvobj };
        let macid_ctl = host::macid_ctl(dvobj);
        let mut fix_bw = 0xffu8;
        if host::fix_rate(adapter) != 0xff && host::fix_bw(adapter) != 0xff {
            fix_bw = host::fix_bw(adapter);
        }
        for i in 0..macid_ctl.num {
            let i = i as usize;
            if !host::macid_is_used(macid_ctl, i as U8) {
                continue;
            }
            if !host::macid_is_iface_specific(macid_ctl, i as U8, adapter) {
                continue;
            }
            accumulate_rate_bmp(
                bw,
                fix_bw,
                macid_ctl.bw[i],
                macid_ctl.vht_en[i],
                macid_ctl.rate_bmp0[i],
                macid_ctl.rate_bmp1[i],
                &mut bmp_cck_ofdm,
                &mut bmp_ht,
                &mut bmp_vht,
            );
        }
    }
    #[cfg(not(host_xmit_test))]
    {
        let dvobj = unsafe { kernel::adapter_dvobj(adapter) };
        if dvobj.is_null() {
            return;
        }
        let macid_ctl = unsafe { kernel::macid_ctl(dvobj) };
        if macid_ctl.is_null() {
            return;
        }
        let mut fix_bw = 0xffu8;
        if unsafe { kernel::fix_rate(adapter) } != 0xff && unsafe { kernel::fix_bw(adapter) } != 0xff
        {
            fix_bw = unsafe { kernel::fix_bw(adapter) };
        }
        let num = unsafe { kernel::macid_num(macid_ctl) };
        for i in 0..num {
            if !unsafe { kernel::macid_is_used(macid_ctl, i) } {
                continue;
            }
            if !unsafe { kernel::macid_is_iface_specific(macid_ctl, i, adapter) } {
                continue;
            }
            accumulate_rate_bmp(
                bw,
                fix_bw,
                unsafe { kernel::macid_bw(macid_ctl, i) },
                unsafe { kernel::macid_vht_en(macid_ctl, i) },
                unsafe { kernel::macid_rate_bmp0(macid_ctl, i) },
                unsafe { kernel::macid_rate_bmp1(macid_ctl, i) },
                &mut bmp_cck_ofdm,
                &mut bmp_ht,
                &mut bmp_vht,
            );
        }
    }
    if !r_bmp_cck_ofdm.is_null() {
        unsafe { *r_bmp_cck_ofdm = bmp_cck_ofdm };
    }
    if !r_bmp_ht.is_null() {
        unsafe { *r_bmp_ht = bmp_ht };
    }
    if !r_bmp_vht.is_null() {
        unsafe { *r_bmp_vht = bmp_vht };
    }
}

#[no_mangle]
pub extern "C" fn rtw_get_shared_macid_tx_rate_bmp_by_bw(
    dvobj: *mut c_void,
    bw: U8,
    r_bmp_cck_ofdm: *mut U16,
    r_bmp_ht: *mut U32,
    r_bmp_vht: *mut U64,
) {
    if dvobj.is_null() {
        return;
    }
    let mut bmp_cck_ofdm = 0u16;
    let mut bmp_ht = 0u32;
    let mut bmp_vht = 0u64;
    #[cfg(host_xmit_test)]
    {
        let dvobj = unsafe { &*(dvobj as *const host::DvobjPriv) };
        let macid_ctl = host::macid_ctl(dvobj);
        for i in 0..macid_ctl.num {
            let i = i as usize;
            if !host::macid_is_used(macid_ctl, i as U8) {
                continue;
            }
            if !host::macid_is_iface_shared(macid_ctl, i as U8) {
                continue;
            }
            accumulate_rate_bmp(
                bw,
                0xff,
                macid_ctl.bw[i],
                macid_ctl.vht_en[i],
                macid_ctl.rate_bmp0[i],
                macid_ctl.rate_bmp1[i],
                &mut bmp_cck_ofdm,
                &mut bmp_ht,
                &mut bmp_vht,
            );
        }
    }
    #[cfg(not(host_xmit_test))]
    {
        let macid_ctl = unsafe { kernel::macid_ctl(dvobj) };
        if macid_ctl.is_null() {
            return;
        }
        let num = unsafe { kernel::macid_num(macid_ctl) };
        for i in 0..num {
            if !unsafe { kernel::macid_is_used(macid_ctl, i) } {
                continue;
            }
            if !unsafe { kernel::macid_is_iface_shared(macid_ctl, i) } {
                continue;
            }
            accumulate_rate_bmp(
                bw,
                0xff,
                unsafe { kernel::macid_bw(macid_ctl, i) },
                unsafe { kernel::macid_vht_en(macid_ctl, i) },
                unsafe { kernel::macid_rate_bmp0(macid_ctl, i) },
                unsafe { kernel::macid_rate_bmp1(macid_ctl, i) },
                &mut bmp_cck_ofdm,
                &mut bmp_ht,
                &mut bmp_vht,
            );
        }
    }
    if !r_bmp_cck_ofdm.is_null() {
        unsafe { *r_bmp_cck_ofdm = bmp_cck_ofdm };
    }
    if !r_bmp_ht.is_null() {
        unsafe { *r_bmp_ht = bmp_ht };
    }
    if !r_bmp_vht.is_null() {
        unsafe { *r_bmp_vht = bmp_vht };
    }
}

#[no_mangle]
pub extern "C" fn rtw_get_tx_bw_bmp_of_ht_rate(dvobj: *mut c_void, rate: U8, max_bw: U8) -> U8 {
    if dvobj.is_null() || !is_ht_rate(rate) {
        return 0;
    }
    let rate_bmp = 1u32 << (rate - MGN_MCS0);
    let mut max_bw = max_bw;
    if max_bw > CHANNEL_WIDTH_40 {
        max_bw = CHANNEL_WIDTH_40;
    }
    let mut bw_bmp = 0u8;
    #[cfg(host_xmit_test)]
    {
        let dvobj = unsafe { &*(dvobj as *const host::DvobjPriv) };
        let rf = host::rfctl(dvobj);
        for bw in CHANNEL_WIDTH_20..=max_bw {
            if rf.rate_bmp_ht_by_bw[bw as usize] >= rate_bmp {
                bw_bmp |= ch_width_to_bw_cap(bw);
            }
        }
    }
    #[cfg(not(host_xmit_test))]
    {
        let rf = unsafe { kernel::rfctl(dvobj) };
        if rf.is_null() {
            return 0;
        }
        for bw in CHANNEL_WIDTH_20..=max_bw {
            if unsafe { kernel::rf_ht_bmp(rf, bw) } >= rate_bmp {
                bw_bmp |= ch_width_to_bw_cap(bw);
            }
        }
    }
    bw_bmp
}

#[no_mangle]
pub extern "C" fn rtw_get_tx_bw_bmp_of_vht_rate(dvobj: *mut c_void, rate: U8, max_bw: U8) -> U8 {
    if dvobj.is_null() || !is_vht_rate(rate) {
        return 0;
    }
    let rate_bmp = 1u64 << (rate - MGN_VHT1SS_MCS0);
    let mut max_bw = max_bw;
    if max_bw > CHANNEL_WIDTH_160 {
        max_bw = CHANNEL_WIDTH_160;
    }
    let mut bw_bmp = 0u8;
    #[cfg(host_xmit_test)]
    {
        let dvobj = unsafe { &*(dvobj as *const host::DvobjPriv) };
        let rf = host::rfctl(dvobj);
        for bw in CHANNEL_WIDTH_20..=max_bw {
            if rf.rate_bmp_vht_by_bw[bw as usize] >= rate_bmp {
                bw_bmp |= ch_width_to_bw_cap(bw);
            }
        }
    }
    #[cfg(not(host_xmit_test))]
    {
        let rf = unsafe { kernel::rfctl(dvobj) };
        if rf.is_null() {
            return 0;
        }
        for bw in CHANNEL_WIDTH_20..=max_bw {
            if unsafe { kernel::rf_vht_bmp(rf, bw) } >= rate_bmp {
                bw_bmp |= ch_width_to_bw_cap(bw);
            }
        }
    }
    bw_bmp
}
