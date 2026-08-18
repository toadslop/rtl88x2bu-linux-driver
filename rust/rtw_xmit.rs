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

#[cfg(not(any(host_xmit_test, host_xmit_sctx_test)))]
use core::ffi::c_void;
#[cfg(any(host_xmit_test, host_xmit_sctx_test))]
use std::os::raw::c_void;

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
    if a > b {
        b
    } else {
        a
    }
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
    struct HtPriv {
        pub(super) sgi_20m: U8,
        pub(super) sgi_40m: U8,
    }

    #[repr(C)]
    struct VhtPriv {
        pub(super) vht_option: U8,
        pub(super) sgi_80m: U8,
    }

    #[repr(C)]
    struct StaCmn {
        bw_mode: U8,
    }

    #[repr(C)]
    pub struct StaInfo {
        pub cmn: StaCmn,
        pub htpriv: HtPriv,
        pub vhtpriv: VhtPriv,
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
        pub hal_bw_cap: U8,
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

    pub(super) fn hal_is_bw_support(adapter: &Adapter, bw: U8) -> bool {
        adapter.hal_bw_cap & ch_width_to_bw_cap(bw) != 0
    }

    pub(super) fn sta_ra_sgi(sta: &StaInfo) -> (U8, U8, U8) {
        let sgi_80m = if sta.vhtpriv.vht_option != 0 {
            sta.vhtpriv.sgi_80m
        } else {
            0
        };
        (sta.htpriv.sgi_20m, sta.htpriv.sgi_40m, sgi_80m)
    }
}

#[cfg(not(host_xmit_test))]
mod kernel {
    use super::*;

    extern "C" {
        fn rtw_macid_is_used(macid_ctl: *mut c_void, id: U8) -> bool;
        fn rtw_macid_is_iface_specific(
            macid_ctl: *mut c_void,
            id: U8,
            adapter: *mut c_void,
        ) -> bool;
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
        fn rtw_rust_xmit_attrib_hdrlen(pattrib: *mut c_void) -> U16;
        fn rtw_rust_xmit_attrib_iv_len(pattrib: *mut c_void) -> U8;
        fn rtw_rust_xmit_attrib_pktlen(pattrib: *mut c_void) -> U32;
        fn rtw_rust_xmit_attrib_encrypt(pattrib: *mut c_void) -> U8;
        fn rtw_rust_xmit_attrib_bswenc(pattrib: *mut c_void) -> U8;
        fn rtw_rust_xmit_attrib_icv_len(pattrib: *mut c_void) -> U8;
        fn rtw_rust_xmit_attrib_meshctrl_len(pattrib: *mut c_void) -> U8;
        fn rtw_rust_xmit_hal_is_bw_support(adapter: *mut c_void, bw: U8) -> bool;
        fn rtw_rust_xmit_sta_ht_sgi_20m(sta: *mut c_void) -> U8;
        fn rtw_rust_xmit_sta_ht_sgi_40m(sta: *mut c_void) -> U8;
        fn rtw_rust_xmit_sta_vht_option(sta: *mut c_void) -> U8;
        fn rtw_rust_xmit_sta_vht_sgi_80m(sta: *mut c_void) -> U8;
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
    pub(super) unsafe fn attrib_hdrlen(pattrib: *mut c_void) -> U16 {
        unsafe { rtw_rust_xmit_attrib_hdrlen(pattrib) }
    }
    pub(super) unsafe fn attrib_iv_len(pattrib: *mut c_void) -> U8 {
        unsafe { rtw_rust_xmit_attrib_iv_len(pattrib) }
    }
    pub(super) unsafe fn attrib_pktlen(pattrib: *mut c_void) -> U32 {
        unsafe { rtw_rust_xmit_attrib_pktlen(pattrib) }
    }
    pub(super) unsafe fn attrib_encrypt(pattrib: *mut c_void) -> U8 {
        unsafe { rtw_rust_xmit_attrib_encrypt(pattrib) }
    }
    pub(super) unsafe fn attrib_bswenc(pattrib: *mut c_void) -> U8 {
        unsafe { rtw_rust_xmit_attrib_bswenc(pattrib) }
    }
    pub(super) unsafe fn attrib_icv_len(pattrib: *mut c_void) -> U8 {
        unsafe { rtw_rust_xmit_attrib_icv_len(pattrib) }
    }
    pub(super) unsafe fn attrib_meshctrl_len(pattrib: *mut c_void) -> U8 {
        unsafe { rtw_rust_xmit_attrib_meshctrl_len(pattrib) }
    }
    pub(super) unsafe fn hal_is_bw_support(adapter: *mut c_void, bw: U8) -> bool {
        unsafe { rtw_rust_xmit_hal_is_bw_support(adapter, bw) }
    }
    pub(super) unsafe fn sta_ht_sgi_20m(sta: *mut c_void) -> U8 {
        unsafe { rtw_rust_xmit_sta_ht_sgi_20m(sta) }
    }
    pub(super) unsafe fn sta_ht_sgi_40m(sta: *mut c_void) -> U8 {
        unsafe { rtw_rust_xmit_sta_ht_sgi_40m(sta) }
    }
    pub(super) unsafe fn sta_vht_option(sta: *mut c_void) -> U8 {
        unsafe { rtw_rust_xmit_sta_vht_option(sta) }
    }
    pub(super) unsafe fn sta_vht_sgi_80m(sta: *mut c_void) -> U8 {
        unsafe { rtw_rust_xmit_sta_vht_sgi_80m(sta) }
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
                bw = rtw_min(
                    bw,
                    bw_mode_2g(unsafe { kernel::driver_tx_bw_mode(adapter) }),
                );
            } else {
                bw = rtw_min(
                    bw,
                    bw_mode_5g(unsafe { kernel::driver_tx_bw_mode(adapter) }),
                );
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
        if unsafe { kernel::fix_rate(adapter) } != 0xff
            && unsafe { kernel::fix_bw(adapter) } != 0xff
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

#[no_mangle]
pub extern "C" fn rtw_get_adapter_tx_rate_bmp(
    adapter: *mut c_void,
    r_bmp_cck_ofdm: *mut U16,
    r_bmp_ht: *mut U32,
    r_bmp_vht: *mut U64,
) {
    if adapter.is_null() {
        return;
    }
    #[cfg(host_xmit_test)]
    {
        let adapter_ref = unsafe { &*(adapter as *const host::Adapter) };
        for bw in CHANNEL_WIDTH_20..=CHANNEL_WIDTH_160 {
            let mut bmp_cck_ofdm = 0u16;
            let mut bmp_ht = 0u32;
            let mut bmp_vht = 0u64;
            if host::hal_is_bw_support(adapter_ref, bw) {
                let mut tmp_cck = 0u16;
                let mut tmp_ht = 0u32;
                let mut tmp_vht = 0u64;
                rtw_get_adapter_tx_rate_bmp_by_bw(
                    adapter,
                    bw,
                    &mut tmp_cck,
                    &mut tmp_ht,
                    &mut tmp_vht,
                );
                bmp_cck_ofdm |= tmp_cck;
                bmp_ht |= tmp_ht;
                bmp_vht |= tmp_vht;
                rtw_get_shared_macid_tx_rate_bmp_by_bw(
                    adapter_ref.dvobj as *mut c_void,
                    bw,
                    &mut tmp_cck,
                    &mut tmp_ht,
                    &mut tmp_vht,
                );
                bmp_cck_ofdm |= tmp_cck;
                bmp_ht |= tmp_ht;
                bmp_vht |= tmp_vht;
            }
            if bw == CHANNEL_WIDTH_20 && !r_bmp_cck_ofdm.is_null() {
                unsafe { *r_bmp_cck_ofdm.add(bw as usize) = bmp_cck_ofdm };
            }
            if bw <= CHANNEL_WIDTH_40 && !r_bmp_ht.is_null() {
                unsafe { *r_bmp_ht.add(bw as usize) = bmp_ht };
            }
            if bw <= CHANNEL_WIDTH_160 && !r_bmp_vht.is_null() {
                unsafe { *r_bmp_vht.add(bw as usize) = bmp_vht };
            }
        }
    }
    #[cfg(not(host_xmit_test))]
    {
        let dvobj = unsafe { kernel::adapter_dvobj(adapter) };
        if dvobj.is_null() {
            return;
        }
        for bw in CHANNEL_WIDTH_20..=CHANNEL_WIDTH_160 {
            let mut bmp_cck_ofdm = 0u16;
            let mut bmp_ht = 0u32;
            let mut bmp_vht = 0u64;
            if unsafe { kernel::hal_is_bw_support(adapter, bw) } {
                let mut tmp_cck = 0u16;
                let mut tmp_ht = 0u32;
                let mut tmp_vht = 0u64;
                rtw_get_adapter_tx_rate_bmp_by_bw(
                    adapter,
                    bw,
                    &mut tmp_cck,
                    &mut tmp_ht,
                    &mut tmp_vht,
                );
                bmp_cck_ofdm |= tmp_cck;
                bmp_ht |= tmp_ht;
                bmp_vht |= tmp_vht;
                rtw_get_shared_macid_tx_rate_bmp_by_bw(
                    dvobj,
                    bw,
                    &mut tmp_cck,
                    &mut tmp_ht,
                    &mut tmp_vht,
                );
                bmp_cck_ofdm |= tmp_cck;
                bmp_ht |= tmp_ht;
                bmp_vht |= tmp_vht;
            }
            if bw == CHANNEL_WIDTH_20 && !r_bmp_cck_ofdm.is_null() {
                unsafe { *r_bmp_cck_ofdm.add(bw as usize) = bmp_cck_ofdm };
            }
            if bw <= CHANNEL_WIDTH_40 && !r_bmp_ht.is_null() {
                unsafe { *r_bmp_ht.add(bw as usize) = bmp_ht };
            }
            if bw <= CHANNEL_WIDTH_160 && !r_bmp_vht.is_null() {
                unsafe { *r_bmp_vht.add(bw as usize) = bmp_vht };
            }
        }
    }
}

#[no_mangle]
pub extern "C" fn query_ra_short_GI(psta: *mut c_void, bw: U8) -> U8 {
    if psta.is_null() {
        return 0;
    }
    let (sgi_20m, sgi_40m, sgi_80m) = {
        #[cfg(host_xmit_test)]
        {
            let sta = unsafe { &*(psta as *const host::StaInfo) };
            host::sta_ra_sgi(sta)
        }
        #[cfg(not(host_xmit_test))]
        {
            let sgi_80m = if unsafe { kernel::sta_vht_option(psta) } != 0 {
                unsafe { kernel::sta_vht_sgi_80m(psta) }
            } else {
                0
            };
            (
                unsafe { kernel::sta_ht_sgi_20m(psta) },
                unsafe { kernel::sta_ht_sgi_40m(psta) },
                sgi_80m,
            )
        }
    };
    match bw {
        CHANNEL_WIDTH_80 => sgi_80m,
        CHANNEL_WIDTH_40 => sgi_40m,
        _ => sgi_20m,
    }
}

#[no_mangle]
pub extern "C" fn qos_acm(acm_mask: U8, priority: U8) -> U8 {
    let mut change_priority = priority;
    match priority {
        0 | 3 if acm_mask & (1 << 1) != 0 => change_priority = 1,
        4 | 5 if acm_mask & (1 << 2) != 0 => change_priority = 0,
        6 | 7 if acm_mask & (1 << 3) != 0 => change_priority = 5,
        _ => {}
    }
    change_priority
}

#[no_mangle]
pub extern "C" fn tos_to_up(tos: U8) -> U8 {
    #[cfg(not(rtw_up_mapping_dscp))]
    {
        return tos >> 5;
    }
    #[cfg(rtw_up_mapping_dscp)]
    {
        let dscp = tos >> 2;
        if dscp == 0 {
            0
        } else if dscp <= 9 {
            1
        } else if dscp <= 16 {
            2
        } else if dscp <= 23 {
            3
        } else if dscp <= 31 {
            4
        } else if dscp >= 33 && dscp <= 40 {
            5
        } else if (dscp >= 41 && dscp <= 47) || dscp == 32 {
            6
        } else {
            7
        }
    }
}

const P802_1H_OUI: [U8; 3] = [0x00, 0x00, 0xf8];
const RFC1042_OUI: [U8; 3] = [0x00, 0x00, 0x00];
const SNAP_HDR_SIZE: usize = 6;
const _TKIP_: U8 = 0x02;

#[cfg(host_xmit_qos_test)]
#[repr(C)]
struct PktAttrib {
    hdrlen: U8,
    iv_len: U8,
    meshctrl_len: U8,
    pktlen: U32,
    encrypt: U8,
    bswenc: U8,
    icv_len: U8,
}

#[cfg(any(not(host_xmit_test), host_xmit_qos_test))]
#[no_mangle]
pub extern "C" fn rtw_calculate_wlan_pkt_size_by_attribue(pattrib: *mut c_void) -> U32 {
    if pattrib.is_null() {
        return 0;
    }
    #[cfg(host_xmit_qos_test)]
    {
        let a = unsafe { &*(pattrib as *const PktAttrib) };
        return a.hdrlen as U32
            + a.iv_len as U32
            + a.meshctrl_len as U32
            + (SNAP_HDR_SIZE + 2) as U32
            + a.pktlen
            + if a.encrypt == _TKIP_ { 8 } else { 0 }
            + if a.bswenc != 0 { a.icv_len as U32 } else { 0 };
    }
    #[cfg(all(not(host_xmit_qos_test), not(host_xmit_test)))]
    {
        unsafe {
            kernel::attrib_hdrlen(pattrib) as U32
                + kernel::attrib_iv_len(pattrib) as U32
                + kernel::attrib_meshctrl_len(pattrib) as U32
                + (SNAP_HDR_SIZE + 2) as U32
                + kernel::attrib_pktlen(pattrib)
                + if kernel::attrib_encrypt(pattrib) == _TKIP_ {
                    8
                } else {
                    0
                }
                + if kernel::attrib_bswenc(pattrib) != 0 {
                    kernel::attrib_icv_len(pattrib) as U32
                } else {
                    0
                }
        }
    }
}

#[cfg(any(not(host_xmit_test), host_xmit_qos_test))]
#[no_mangle]
pub extern "C" fn rtw_put_snap(data: *mut U8, h_proto: U16) -> i32 {
    if data.is_null() {
        return 0;
    }
    let data = unsafe { core::slice::from_raw_parts_mut(data, SNAP_HDR_SIZE + 2) };
    data[0] = 0xaa;
    data[1] = 0xaa;
    data[2] = 0x03;
    let oui = if h_proto == 0x8137 || h_proto == 0x80f3 {
        P802_1H_OUI
    } else {
        RFC1042_OUI
    };
    data[3] = oui[0];
    data[4] = oui[1];
    data[5] = oui[2];
    let be = h_proto.to_be().to_ne_bytes();
    data[SNAP_HDR_SIZE] = be[0];
    data[SNAP_HDR_SIZE + 1] = be[1];
    (SNAP_HDR_SIZE + 2) as i32
}

const RTW_SCTX_SUBMITTED: i32 = -1;
const RTW_SCTX_DONE_SUCCESS: i32 = 0;
const RTW_SCTX_DONE_UNKNOWN: i32 = 1;
const RTW_SCTX_DONE_BUF_ALLOC: i32 = 3;
const RTW_SCTX_DONE_BUF_FREE: i32 = 4;
const RTW_SCTX_DONE_DRV_STOP: i32 = 9;
const RTW_SCTX_DONE_DEV_REMOVE: i32 = 10;
const RTW_SCTX_DONE_TIMEOUT: i32 = 2;

const _SUCCESS: i32 = 1;
const _FAIL: i32 = 0;
const _TRUE: i32 = 1;

#[cfg(not(any(host_xmit_test, host_xmit_sctx_test)))]
mod sctx_kernel {
    use super::*;

    type Systime = U64;

    extern "C" {
        fn rtw_rust_sctx_get_current_time() -> Systime;
        fn rtw_rust_sctx_field_init(sctx: *mut c_void, timeout_ms: i32, submit_time: Systime);
        fn rtw_rust_sctx_field_set_status(sctx: *mut c_void, status: i32);
        fn rtw_rust_sctx_field_get_status(sctx: *mut c_void) -> i32;
        fn rtw_rust_sctx_field_get_timeout_ms(sctx: *mut c_void) -> U32;
        fn rtw_rust_sctx_msecs_to_jiffies(ms: i32) -> U64;
        fn rtw_rust_sctx_max_schedule_timeout() -> U64;
        fn rtw_rust_sctx_wait_done(sctx: *mut c_void, expire: U64) -> U64;
        fn rtw_rust_sctx_complete_done(sctx: *mut c_void);
        fn rtw_rust_sctx_log_timeout(msg: *const u8);
        fn rtw_rust_sctx_log_warning_status(status: i32);
    }

    pub(super) fn init(sctx: *mut c_void, timeout_ms: i32) {
        if sctx.is_null() {
            return;
        }
        unsafe {
            rtw_rust_sctx_field_init(sctx, timeout_ms, rtw_rust_sctx_get_current_time());
        }
    }

    pub(super) fn wait(sctx: *mut c_void, msg: *const u8) -> i32 {
        if sctx.is_null() {
            return _FAIL;
        }
        let status = unsafe {
            let timeout_ms = rtw_rust_sctx_field_get_timeout_ms(sctx) as i32;
            let expire = if timeout_ms != 0 {
                rtw_rust_sctx_msecs_to_jiffies(timeout_ms)
            } else {
                rtw_rust_sctx_max_schedule_timeout()
            };
            if rtw_rust_sctx_wait_done(sctx, expire) == 0 {
                if !msg.is_null() {
                    rtw_rust_sctx_log_timeout(msg);
                }
                RTW_SCTX_DONE_TIMEOUT
            } else {
                rtw_rust_sctx_field_get_status(sctx)
            }
        };
        if status == RTW_SCTX_DONE_SUCCESS {
            _SUCCESS
        } else {
            _FAIL
        }
    }

    pub(super) fn done_err(sctx: *mut *mut c_void, status: i32) {
        if sctx.is_null() {
            return;
        }
        unsafe {
            if (*sctx).is_null() {
                return;
            }
            if rtw_sctx_chk_waring_status(status) != 0 {
                rtw_rust_sctx_log_warning_status(status);
            }
            rtw_rust_sctx_field_set_status(*sctx, status);
            rtw_rust_sctx_complete_done(*sctx);
            *sctx = core::ptr::null_mut();
        }
    }
}

#[cfg(not(any(host_xmit_test, host_xmit_sctx_test)))]
#[no_mangle]
pub extern "C" fn rtw_sctx_chk_waring_status(status: i32) -> i32 {
    match status {
        RTW_SCTX_DONE_UNKNOWN
        | RTW_SCTX_DONE_BUF_ALLOC
        | RTW_SCTX_DONE_BUF_FREE
        | RTW_SCTX_DONE_DRV_STOP
        | RTW_SCTX_DONE_DEV_REMOVE => _TRUE,
        _ => 0,
    }
}

#[cfg(not(any(host_xmit_test, host_xmit_sctx_test)))]
#[no_mangle]
pub extern "C" fn rtw_sctx_init(sctx: *mut c_void, timeout_ms: i32) {
    sctx_kernel::init(sctx, timeout_ms);
}

#[cfg(not(any(host_xmit_test, host_xmit_sctx_test)))]
#[no_mangle]
pub extern "C" fn rtw_sctx_wait(sctx: *mut c_void, msg: *const u8) -> i32 {
    sctx_kernel::wait(sctx, msg)
}

#[cfg(not(any(host_xmit_test, host_xmit_sctx_test)))]
#[no_mangle]
pub extern "C" fn rtw_sctx_done_err(sctx: *mut *mut c_void, status: i32) {
    sctx_kernel::done_err(sctx, status);
}

#[cfg(not(any(host_xmit_test, host_xmit_sctx_test)))]
#[no_mangle]
pub extern "C" fn rtw_sctx_done(sctx: *mut *mut c_void) {
    sctx_kernel::done_err(sctx, RTW_SCTX_DONE_SUCCESS);
}

#[cfg(host_xmit_sctx_test)]
// Host L2 builds `librust_xmit_sctx.a` with `host_xmit_sctx_test`; this stub mirrors
// `host_xmit_sctx_types.h` and is what `test_xmit_sctx_rust` exercises. Kernel
// production uses `sctx_kernel` + `rtw_rust_sctx_*` shims instead — drift there is
// not caught by the current sctx vectors (same pattern as xmit/qos harnesses).
mod sctx_host {
    use super::*;

    #[repr(C)]
    struct HostCompletion {
        completed: u32,
    }

    #[repr(C)]
    struct SubmitCtx {
        submit_time: U64,
        timeout_ms: U32,
        status: i32,
        done: HostCompletion,
    }

    fn ctx(sctx: *mut c_void) -> *mut SubmitCtx {
        sctx as *mut SubmitCtx
    }

    fn init_completion(c: *mut HostCompletion) {
        unsafe {
            (*c).completed = 0;
        }
    }

    fn complete(c: *mut HostCompletion) {
        unsafe {
            (*c).completed = 1;
        }
    }

    fn wait_done(c: *mut HostCompletion, _expire: U64) -> U64 {
        unsafe {
            if (*c).completed != 0 {
                1
            } else {
                0
            }
        }
    }

    pub(super) fn init(sctx: *mut c_void, timeout_ms: i32) {
        if sctx.is_null() {
            return;
        }
        unsafe {
            let s = ctx(sctx);
            (*s).timeout_ms = timeout_ms as U32;
            (*s).submit_time = 0;
            init_completion(&mut (*s).done);
            (*s).status = RTW_SCTX_SUBMITTED;
        }
    }

    pub(super) fn wait(sctx: *mut c_void, _msg: *const u8) -> i32 {
        if sctx.is_null() {
            return _FAIL;
        }
        let status = unsafe {
            let s = ctx(sctx);
            let expire = if (*s).timeout_ms != 0 {
                (*s).timeout_ms as U64
            } else {
                u64::MAX
            };
            if wait_done(&mut (*s).done, expire) == 0 {
                RTW_SCTX_DONE_TIMEOUT
            } else {
                (*s).status
            }
        };
        if status == RTW_SCTX_DONE_SUCCESS {
            _SUCCESS
        } else {
            _FAIL
        }
    }

    pub(super) fn done_err(sctx: *mut *mut c_void, status: i32) {
        if sctx.is_null() {
            return;
        }
        unsafe {
            if (*sctx).is_null() {
                return;
            }
            let s = ctx(*sctx);
            (*s).status = status;
            complete(&mut (*s).done);
            *sctx = core::ptr::null_mut();
        }
    }
}

#[cfg(host_xmit_sctx_test)]
#[no_mangle]
pub extern "C" fn rtw_sctx_chk_waring_status(status: i32) -> i32 {
    match status {
        RTW_SCTX_DONE_UNKNOWN
        | RTW_SCTX_DONE_BUF_ALLOC
        | RTW_SCTX_DONE_BUF_FREE
        | RTW_SCTX_DONE_DRV_STOP
        | RTW_SCTX_DONE_DEV_REMOVE => _TRUE,
        _ => 0,
    }
}

#[cfg(host_xmit_sctx_test)]
#[no_mangle]
pub extern "C" fn rtw_sctx_init(sctx: *mut c_void, timeout_ms: i32) {
    sctx_host::init(sctx, timeout_ms);
}

#[cfg(host_xmit_sctx_test)]
#[no_mangle]
pub extern "C" fn rtw_sctx_wait(sctx: *mut c_void, msg: *const u8) -> i32 {
    sctx_host::wait(sctx, msg)
}

#[cfg(host_xmit_sctx_test)]
#[no_mangle]
pub extern "C" fn rtw_sctx_done_err(sctx: *mut *mut c_void, status: i32) {
    sctx_host::done_err(sctx, status);
}

#[cfg(host_xmit_sctx_test)]
#[no_mangle]
pub extern "C" fn rtw_sctx_done(sctx: *mut *mut c_void) {
    sctx_host::done_err(sctx, RTW_SCTX_DONE_SUCCESS);
}
