// SPDX-License-Identifier: GPL-2.0
//! W3-83 AP STA HT info helpers — Rust port of `core/rtw_ap_sta_info.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

use core::ffi::c_void;

type U8 = u8;
type U16 = u16;
type Adapter = c_void;
type StaInfo = c_void;

const BEAMFORMING_HT_BEAMFORMER_ENABLE: U8 = 1 << 0;
const BEAMFORMING_HT_BEAMFORMEE_ENABLE: U8 = 1 << 1;

#[inline]
fn test_flag(flag: U8, test: U8) -> bool {
    (flag & test) != 0
}

#[inline]
fn set_flag(flag: &mut U8, set: U8) {
    *flag |= set;
}

#[inline]
fn get_ht_cap_txbf_explicit_comp_steering_cap(tx_bf: u32) -> u32 {
    (tx_bf >> 10) & 1
}

#[inline]
fn get_ht_cap_txbf_explicit_comp_feedback_cap(tx_bf: u32) -> u32 {
    (tx_bf >> 15) & 3
}

#[inline]
fn get_ht_cap_txbf_comp_steering_num_antennas(tx_bf: u32) -> u32 {
    (tx_bf >> 23) & 3
}

#[inline]
fn get_ht_cap_txbf_chnl_estimation_num_antennas(tx_bf: u32) -> u32 {
    (tx_bf >> 27) & 3
}

#[cfg(host_ap_sta_info_test)]
#[repr(C, packed)]
pub struct RtwIeee80211HtCap {
    pub cap_info: U16,
    pub ampdu_params_info: U8,
    pub supp_mcs_set: [U8; 16],
    pub extended_ht_cap_info: U16,
    pub tx_bf_cap_info: u32,
    pub antenna_selection_info: U8,
}

#[cfg(host_ap_sta_info_test)]
#[repr(C)]
pub struct HtPriv {
    pub ht_option: U8,
    pub beamform_cap: U8,
    pub ht_cap: RtwIeee80211HtCap,
}

#[cfg(host_ap_sta_info_test)]
#[repr(C)]
pub struct BfCmnInfo {
    pub ht_beamform_cap: U8,
}

#[cfg(host_ap_sta_info_test)]
#[repr(C)]
pub struct CmnStaInfo {
    pub bf_info: BfCmnInfo,
    pub aid: U16,
}

#[cfg(host_ap_sta_info_test)]
#[repr(C)]
pub struct StaInfoHost {
    pub cmn: CmnStaInfo,
    pub htpriv: HtPriv,
}

#[cfg(host_ap_sta_info_test)]
#[repr(C)]
pub struct MlmePriv {
    pub htpriv: HtPriv,
}

#[cfg(host_ap_sta_info_test)]
#[repr(C)]
pub struct AdapterHost {
    pub mlmepriv: MlmePriv,
}

#[cfg(not(host_ap_sta_info_test))]
extern "C" {
    fn rtw_rust_ap_sta_info_ap_bf_cap(padapter: *mut Adapter) -> U8;
    fn rtw_rust_ap_sta_info_sta_ht_cap(psta: *mut StaInfo) -> *const U8;
    fn rtw_rust_ap_sta_info_set_sta_bf_cap(psta: *mut StaInfo, cap: U8);
    fn rtw_rust_ap_sta_info_set_ht_beamform_cap(psta: *mut StaInfo, cap: U8);
}

fn update_sta_info_apmode_ht_bf_cap_impl(
    ap_bf_cap: U8,
    sta_tx_bf: u32,
    out_sta_bf: &mut U8,
    out_ht_beamform: &mut U8,
) {
    let mut cur_beamform_cap = 0u8;

    if test_flag(ap_bf_cap, BEAMFORMING_HT_BEAMFORMEE_ENABLE)
        && get_ht_cap_txbf_explicit_comp_steering_cap(sta_tx_bf) != 0
    {
        set_flag(&mut cur_beamform_cap, BEAMFORMING_HT_BEAMFORMER_ENABLE);
        set_flag(
            &mut cur_beamform_cap,
            (get_ht_cap_txbf_chnl_estimation_num_antennas(sta_tx_bf) << 6) as U8,
        );
    }

    if test_flag(ap_bf_cap, BEAMFORMING_HT_BEAMFORMER_ENABLE)
        && get_ht_cap_txbf_explicit_comp_feedback_cap(sta_tx_bf) != 0
    {
        set_flag(&mut cur_beamform_cap, BEAMFORMING_HT_BEAMFORMEE_ENABLE);
        set_flag(
            &mut cur_beamform_cap,
            (get_ht_cap_txbf_comp_steering_num_antennas(sta_tx_bf) << 4) as U8,
        );
    }

    *out_sta_bf = cur_beamform_cap;
    *out_ht_beamform = cur_beamform_cap;
}

#[cfg(host_ap_sta_info_test)]
fn update_sta_info_apmode_ht_bf_cap_host(padapter: *mut AdapterHost, psta: *mut StaInfoHost) {
    if padapter.is_null() || psta.is_null() {
        return;
    }
    unsafe {
        let ap_bf = (*padapter).mlmepriv.htpriv.beamform_cap;
        let sta_tx_bf = (*psta).htpriv.ht_cap.tx_bf_cap_info;
        let mut sta_bf = 0u8;
        let mut ht_bf = 0u8;
        update_sta_info_apmode_ht_bf_cap_impl(ap_bf, sta_tx_bf, &mut sta_bf, &mut ht_bf);
        (*psta).htpriv.beamform_cap = sta_bf;
        (*psta).cmn.bf_info.ht_beamform_cap = ht_bf;
    }
}

#[cfg(not(host_ap_sta_info_test))]
fn update_sta_info_apmode_ht_bf_cap_kernel(padapter: *mut Adapter, psta: *mut StaInfo) {
    if padapter.is_null() || psta.is_null() {
        return;
    }
    unsafe {
        let ap_bf = rtw_rust_ap_sta_info_ap_bf_cap(padapter);
        let sta_ht = rtw_rust_ap_sta_info_sta_ht_cap(psta);
        let sta_tx_bf = u32::from_le_bytes([
            *sta_ht.add(21),
            *sta_ht.add(22),
            *sta_ht.add(23),
            *sta_ht.add(24),
        ]);
        let mut sta_bf = 0u8;
        let mut ht_bf = 0u8;
        update_sta_info_apmode_ht_bf_cap_impl(ap_bf, sta_tx_bf, &mut sta_bf, &mut ht_bf);
        rtw_rust_ap_sta_info_set_sta_bf_cap(psta, sta_bf);
        rtw_rust_ap_sta_info_set_ht_beamform_cap(psta, ht_bf);
    }
}

#[no_mangle]
pub extern "C" fn update_sta_info_apmode_ht_bf_cap(padapter: *mut Adapter, psta: *mut StaInfo) {
    #[cfg(host_ap_sta_info_test)]
    update_sta_info_apmode_ht_bf_cap_host(padapter as *mut AdapterHost, psta as *mut StaInfoHost);
    #[cfg(not(host_ap_sta_info_test))]
    update_sta_info_apmode_ht_bf_cap_kernel(padapter, psta);
}
