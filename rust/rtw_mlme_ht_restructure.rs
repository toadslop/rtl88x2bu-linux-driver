// SPDX-License-Identifier: GPL-2.0
//! W3-67 HT IE restructure — host L2 oracle and kernel port.

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    private_interfaces,
    unused_imports,
    missing_docs
)]

#[cfg(host_mlme_ht_restructure_test)]
use std::os::raw::{c_int, c_uint, c_void};

#[cfg(all(not(host_mlme_ht_restructure_test), rust_mlme_ht_restructure))]
use core::ffi::{c_int, c_uint, c_void};

type U8 = u8;
type U16 = u16;
type U32 = u32;

const _TRUE: c_int = 1;
const _FALSE: c_int = 0;
const CHANNEL_WIDTH_20: U8 = 0;
const CHANNEL_WIDTH_40: U8 = 1;
const HAL_PRIME_CHNL_OFFSET_DONT_CARE: U8 = 0;
const HAL_PRIME_CHNL_OFFSET_LOWER: U8 = 1;
const HAL_PRIME_CHNL_OFFSET_UPPER: U8 = 2;
const BW_CAP_40M: U8 = 0x08;
const WIFI_STATION_STATE: U32 = 0x0000_0008;
const SCA: U8 = 1;
const SCB: U8 = 3;
const HT_CAP_IE_LEN: U32 = 26;
const HT_OP_IE_LEN: U32 = 22;
const WLAN_EID_HT_OPERATION: U8 = 61;
const WLAN_EID_HT_CAP: U8 = 45;
const _HT_CAPABILITY_IE_: U8 = 45;
const _HT_ADD_INFO_IE_: U8 = 61;
const IEEE80211_HT_CAP_DSSSCCK40: U16 = 0x1000;
const IEEE80211_HT_CAP_SGI_20: U16 = 0x0020;
const IEEE80211_HT_CAP_SGI_40: U16 = 0x0040;
const IEEE80211_HT_CAP_SUP_WIDTH: U16 = 0x0002;
const IEEE80211_HT_CAP_SM_PS: U16 = 0x000C;
const IEEE80211_HT_CAP_LDPC_CODING: U16 = 0x0001;
const IEEE80211_HT_CAP_TX_STBC: U16 = 0x0080;
const IEEE80211_HT_CAP_MAX_AMSDU: U16 = 0x0800;
const IEEE80211_HT_CAP_AMPDU_FACTOR: U8 = 0x03;
const IEEE80211_HT_CAP_AMPDU_DENSITY: U8 = 0x1C;
const LDPC_HT_ENABLE_RX: U8 = 0x01;
const STBC_HT_ENABLE_TX: U8 = 0x02;
const STBC_HT_ENABLE_RX: U8 = 0x01;
const MCS_RATE_1R: U32 = 0x0000_00ff;
const MCS_RATE_2R: U32 = 0x0000_ffff;
const MCS_RATE_3R: U32 = 0x00ff_ffff;
const MCS_RATE_4R: U32 = 0xffff_ffff;
const _AES_: U32 = 4;
const HAL_DEF_RX_PACKET_OFFSET: U8 = 1;
const HAL_DEF_MAX_RECVBUF_SZ: U8 = 2;
const HAL_DEF_RX_STBC: U8 = 3;
const HW_VAR_MAX_RX_AMPDU_FACTOR: U8 = 4;
const HW_VAR_BEST_AMPDU_DENSITY: U8 = 5;

#[repr(C, packed)]
struct RtwIeee80211HtCap {
    cap_info: U16,
    ampdu_params_info: U8,
    supp_mcs_set: [U8; 16],
    extended_ht_cap_info: U16,
    tx_bf_cap_info: U32,
    antenna_selection_info: U8,
}

fn le_bits(p: *const U8, o: U32, l: U32) -> U8 {
    unsafe { (*p >> o) as U8 & ((1u32 << l) - 1) as U8 }
}

fn set_bits_u8(p: *mut U8, o: U32, l: U32, v: U8) {
    unsafe {
        let mask = (((1u32 << l) - 1) << o) as U8;
        *p = (*p & !mask) | ((v & ((1u32 << l) - 1) as U8) << o);
    }
}

fn test_flag(flag: U8, test: U8) -> bool {
    (flag & test) != 0
}

#[cfg(not(host_mlme_ht_restructure_test))]
extern "C" {
    fn _rtw_memset(s: *mut c_void, c: c_int, n: usize) -> *mut c_void;
    fn _rtw_memcpy(d: *mut c_void, s: *const c_void, n: usize) -> *mut c_void;
}

#[cfg(host_mlme_ht_restructure_test)]
fn memset_bytes(s: *mut u8, c: u8, n: usize) {
    unsafe {
        core::ptr::write_bytes(s, c, n);
    }
}

#[cfg(not(host_mlme_ht_restructure_test))]
fn memset_bytes(s: *mut u8, c: u8, n: usize) {
    unsafe {
        _rtw_memset(s as *mut c_void, c as c_int, n);
    }
}

#[cfg(host_mlme_ht_restructure_test)]
fn memcpy_bytes(dst: *mut u8, src: *const u8, n: usize) {
    unsafe {
        core::ptr::copy_nonoverlapping(src, dst, n);
    }
}

#[cfg(not(host_mlme_ht_restructure_test))]
fn memcpy_bytes(dst: *mut u8, src: *const u8, n: usize) {
    unsafe {
        _rtw_memcpy(dst as *mut c_void, src as *const c_void, n);
    }
}

extern "C" {
    fn rtw_rust_ht_channel_set(padapter: *mut U8) -> *mut U8;
    fn rtw_rust_ht_rfctl(padapter: *mut U8) -> *mut U8;
    fn rtw_rust_ht_is_dfs_slave_with_rd(rfctl: *mut U8) -> U8;
    fn rtw_rust_ht_rfctl_dfs_domain_unknown(rfctl: *mut U8) -> U8;
    fn rtw_rust_ht_regsty_bw_2g(padapter: *mut U8) -> U8;
    fn rtw_rust_ht_regsty_bw_5g(padapter: *mut U8) -> U8;
    fn rtw_rust_ht_ht_option(padapter: *mut U8) -> *mut U8;
    fn rtw_rust_ht_get_ie(pbuf: *const U8, index: c_int, len: *mut U32, limit: c_int) -> *mut U8;
    fn rtw_rust_ht_chset_is_chbw_valid(
        ch_set: *mut U8,
        ch: U8,
        bw: U8,
        offset: U8,
        a: U8,
        b: U8,
    ) -> U8;
    fn rtw_rust_ht_chset_is_chbw_non_ocp(ch_set: *mut U8, ch: U8, bw: U8, offset: U8) -> U8;
    fn rtw_rust_ht_warn_on(condition: c_int);
    fn rtw_rust_ht_hal_chk_bw_cap(padapter: *mut U8, cap: U8) -> U8;
    fn rtw_rust_ht_hal_get_def_var(padapter: *mut U8, def_var: U8, val: *mut c_void);
    fn rtw_rust_ht_set_mcs_rate_by_mask(mcs_set: *mut U8, mask: U32);
    fn rtw_rust_ht_fw_state(padapter: *mut U8) -> U32;
    fn rtw_rust_ht_cur_bwmode(padapter: *mut U8) -> U8;
    fn rtw_rust_ht_default_mcs(padapter: *mut U8) -> *mut U8;
    fn rtw_rust_ht_sgi_20m(padapter: *mut U8) -> U8;
    fn rtw_rust_ht_sgi_40m(padapter: *mut U8) -> U8;
    fn rtw_rust_ht_ldpc_cap(padapter: *mut U8) -> U8;
    fn rtw_rust_ht_stbc_cap(padapter: *mut U8) -> U8;
    fn rtw_rust_ht_rx_stbc(padapter: *mut U8) -> U8;
    fn rtw_rust_ht_wifi_spec(padapter: *mut U8) -> U8;
    fn rtw_rust_ht_rx_nss(padapter: *mut U8) -> U8;
    fn rtw_rust_ht_driver_rx_ampdu_factor(padapter: *mut U8) -> U8;
    fn rtw_rust_ht_driver_rx_ampdu_spacing(padapter: *mut U8) -> U8;
    fn rtw_rust_ht_dot11_privacy(padapter: *mut U8) -> U32;
    fn rtw_rust_ht_set_ie(
        pbuf: *mut U8,
        index: c_int,
        len: U32,
        source: *mut U8,
        frlen: *mut U32,
    ) -> *mut U8;
}

mod access {
    use super::*;

    pub(super) fn regsty_bw_2g(p: *mut U8) -> U8 {
        unsafe { rtw_rust_ht_regsty_bw_2g(p) }
    }

    pub(super) fn regsty_bw_5g(p: *mut U8) -> U8 {
        unsafe { rtw_rust_ht_regsty_bw_5g(p) }
    }

    pub(super) fn hal_chk_bw_cap(p: *mut U8, cap: U8) -> bool {
        unsafe { rtw_rust_ht_hal_chk_bw_cap(p, cap) != 0 }
    }

    pub(super) fn hal_get_def_var(p: *mut U8, def_var: U8, val: *mut c_void) {
        unsafe { rtw_rust_ht_hal_get_def_var(p, def_var, val) };
    }

    pub(super) fn set_mcs(mcs: *mut U8, mask: U32) {
        unsafe { rtw_rust_ht_set_mcs_rate_by_mask(mcs, mask) };
    }

    pub(super) fn fw_state(p: *mut U8) -> U32 {
        unsafe { rtw_rust_ht_fw_state(p) }
    }

    pub(super) fn cur_bwmode(p: *mut U8) -> U8 {
        unsafe { rtw_rust_ht_cur_bwmode(p) }
    }

    pub(super) fn default_mcs(p: *mut U8) -> *mut U8 {
        unsafe { rtw_rust_ht_default_mcs(p) }
    }

    pub(super) fn sgi_20m(p: *mut U8) -> U8 {
        unsafe { rtw_rust_ht_sgi_20m(p) }
    }

    pub(super) fn sgi_40m(p: *mut U8) -> U8 {
        unsafe { rtw_rust_ht_sgi_40m(p) }
    }

    pub(super) fn ldpc_cap(p: *mut U8) -> U8 {
        unsafe { rtw_rust_ht_ldpc_cap(p) }
    }

    pub(super) fn stbc_cap(p: *mut U8) -> U8 {
        unsafe { rtw_rust_ht_stbc_cap(p) }
    }

    pub(super) fn rx_stbc(p: *mut U8) -> U8 {
        unsafe { rtw_rust_ht_rx_stbc(p) }
    }

    pub(super) fn wifi_spec(p: *mut U8) -> U8 {
        unsafe { rtw_rust_ht_wifi_spec(p) }
    }

    pub(super) fn rx_nss(p: *mut U8) -> U8 {
        unsafe { rtw_rust_ht_rx_nss(p) }
    }

    pub(super) fn driver_rx_ampdu_factor(p: *mut U8) -> U8 {
        unsafe { rtw_rust_ht_driver_rx_ampdu_factor(p) }
    }

    pub(super) fn driver_rx_ampdu_spacing(p: *mut U8) -> U8 {
        unsafe { rtw_rust_ht_driver_rx_ampdu_spacing(p) }
    }

    pub(super) fn dot11_privacy(p: *mut U8) -> U32 {
        unsafe { rtw_rust_ht_dot11_privacy(p) }
    }
}

fn restructure_ht_ie_impl(
    padapter: *mut U8,
    in_ie: *mut U8,
    out_ie: *mut U8,
    in_len: c_uint,
    pout_len: *mut c_uint,
    channel: U8,
) -> U32 {
    unsafe {
        let ht_option = rtw_rust_ht_ht_option(padapter);
        let rfctl = rtw_rust_ht_rfctl(padapter);
        let chset = rtw_rust_ht_channel_set(padapter);
        let mut ielen: U32 = 0;
        let mut ht_capie: RtwIeee80211HtCap = core::mem::zeroed();
        let mut cbw40_enable: U8 = 0;
        let mut rx_stbc_nss: U8 = 0;

        *ht_option = _FALSE as U8;
        memset_bytes(
            &mut ht_capie as *mut _ as *mut U8,
            0,
            core::mem::size_of::<RtwIeee80211HtCap>(),
        );

        ht_capie.cap_info = IEEE80211_HT_CAP_DSSSCCK40;
        if access::sgi_20m(padapter) != 0 {
            ht_capie.cap_info |= IEEE80211_HT_CAP_SGI_20;
        }

        if access::hal_chk_bw_cap(padapter, BW_CAP_40M) {
            cbw40_enable = if channel > 14 {
                (access::regsty_bw_5g(padapter) >= CHANNEL_WIDTH_40) as U8
            } else {
                (access::regsty_bw_2g(padapter) >= CHANNEL_WIDTH_40) as U8
            };
        }

        if cbw40_enable != 0 {
            let mut oper_bw = CHANNEL_WIDTH_20;
            let mut oper_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;

            if in_ie.is_null() {
                if (access::fw_state(padapter) & WIFI_STATION_STATE) != 0 {
                    oper_bw = access::cur_bwmode(padapter);
                    if oper_bw > CHANNEL_WIDTH_40 {
                        oper_bw = CHANNEL_WIDTH_40;
                    }
                } else {
                    oper_bw = CHANNEL_WIDTH_40;
                }
            } else {
                let ht_op =
                    rtw_rust_ht_get_ie(in_ie, WLAN_EID_HT_OPERATION as c_int, &mut ielen, in_len as c_int);
                if !ht_op.is_null() && ielen == HT_OP_IE_LEN && le_bits(ht_op.add(2).add(1), 2, 1) != 0 {
                    oper_bw = CHANNEL_WIDTH_40;
                    oper_offset = match le_bits(ht_op.add(2).add(1), 0, 2) {
                        SCA => HAL_PRIME_CHNL_OFFSET_LOWER,
                        SCB => HAL_PRIME_CHNL_OFFSET_UPPER,
                        _ => HAL_PRIME_CHNL_OFFSET_DONT_CARE,
                    };
                }
                if oper_bw == CHANNEL_WIDTH_40 {
                    let ht_cap =
                        rtw_rust_ht_get_ie(in_ie, WLAN_EID_HT_CAP as c_int, &mut ielen, in_len as c_int);
                    if !ht_cap.is_null() && ielen == HT_CAP_IE_LEN {
                        oper_bw = if le_bits(ht_cap.add(2), 1, 1) != 0 {
                            CHANNEL_WIDTH_40
                        } else {
                            CHANNEL_WIDTH_20
                        };
                        if oper_bw == CHANNEL_WIDTH_20 {
                            oper_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
                        }
                    }
                }
            }

            if oper_bw == CHANNEL_WIDTH_40
                && oper_offset != HAL_PRIME_CHNL_OFFSET_DONT_CARE
                && (rtw_rust_ht_chset_is_chbw_valid(chset, channel, oper_bw, oper_offset, 1, 1) == 0
                    || (rtw_rust_ht_is_dfs_slave_with_rd(rfctl) != 0
                        && rtw_rust_ht_rfctl_dfs_domain_unknown(rfctl) == 0
                        && rtw_rust_ht_chset_is_chbw_non_ocp(chset, channel, oper_bw, oper_offset) != 0))
            {
                oper_bw = CHANNEL_WIDTH_20;
                oper_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
                rtw_rust_ht_warn_on(
                    (rtw_rust_ht_chset_is_chbw_valid(chset, channel, oper_bw, oper_offset, 1, 1) == 0)
                        as c_int,
                );
                if rtw_rust_ht_is_dfs_slave_with_rd(rfctl) != 0
                    && rtw_rust_ht_rfctl_dfs_domain_unknown(rfctl) == 0
                {
                    rtw_rust_ht_warn_on(
                        (rtw_rust_ht_chset_is_chbw_non_ocp(chset, channel, oper_bw, oper_offset) != 0)
                            as c_int,
                    );
                }
            }

            if oper_bw == CHANNEL_WIDTH_40 {
                ht_capie.cap_info |= IEEE80211_HT_CAP_SUP_WIDTH;
                if access::sgi_40m(padapter) != 0 {
                    ht_capie.cap_info |= IEEE80211_HT_CAP_SGI_40;
                }
            }
            cbw40_enable = if oper_bw == CHANNEL_WIDTH_40 { 1 } else { 0 };
            let _ = cbw40_enable;
        }

        ht_capie.cap_info |= IEEE80211_HT_CAP_SM_PS;

        if test_flag(access::ldpc_cap(padapter), LDPC_HT_ENABLE_RX) {
            ht_capie.cap_info |= IEEE80211_HT_CAP_LDPC_CODING;
        }
        if test_flag(access::stbc_cap(padapter), STBC_HT_ENABLE_TX) {
            ht_capie.cap_info |= IEEE80211_HT_CAP_TX_STBC;
        }
        if test_flag(access::stbc_cap(padapter), STBC_HT_ENABLE_RX) {
            let rx_stbc_reg = access::rx_stbc(padapter);
            if rx_stbc_reg == 0x3
                || (channel <= 14 && rx_stbc_reg == 0x1)
                || (channel > 14 && rx_stbc_reg == 0x2)
                || access::wifi_spec(padapter) == 1
            {
                access::hal_get_def_var(
                    padapter,
                    HAL_DEF_RX_STBC,
                    &mut rx_stbc_nss as *mut U8 as *mut c_void,
                );
                set_bits_u8(
                    (&mut ht_capie as *mut RtwIeee80211HtCap as *mut U8).add(1),
                    0,
                    2,
                    rx_stbc_nss,
                );
            }
        }

        memcpy_bytes(
            ht_capie.supp_mcs_set.as_mut_ptr(),
            access::default_mcs(padapter),
            16,
        );

        match access::rx_nss(padapter) {
            1 => access::set_mcs(ht_capie.supp_mcs_set.as_mut_ptr(), MCS_RATE_1R),
            2 => access::set_mcs(ht_capie.supp_mcs_set.as_mut_ptr(), MCS_RATE_2R),
            3 => access::set_mcs(ht_capie.supp_mcs_set.as_mut_ptr(), MCS_RATE_3R),
            4 => access::set_mcs(ht_capie.supp_mcs_set.as_mut_ptr(), MCS_RATE_4R),
            _ => {}
        }

        let mut rx_packet_offset: U32 = 0;
        let mut max_recvbuf_sz: U32 = 0;
        access::hal_get_def_var(
            padapter,
            HAL_DEF_RX_PACKET_OFFSET,
            &mut rx_packet_offset as *mut U32 as *mut c_void,
        );
        access::hal_get_def_var(
            padapter,
            HAL_DEF_MAX_RECVBUF_SZ,
            &mut max_recvbuf_sz as *mut U32 as *mut c_void,
        );
        if max_recvbuf_sz - rx_packet_offset >= (8191 - 256) {
            ht_capie.cap_info |= IEEE80211_HT_CAP_MAX_AMSDU;
        }

        let mut max_rx_ampdu_factor: U8 = 0;
        if access::driver_rx_ampdu_factor(padapter) != 0xff {
            max_rx_ampdu_factor = access::driver_rx_ampdu_factor(padapter);
        } else {
            access::hal_get_def_var(
                padapter,
                HW_VAR_MAX_RX_AMPDU_FACTOR,
                &mut max_rx_ampdu_factor as *mut U8 as *mut c_void,
            );
        }
        ht_capie.ampdu_params_info = max_rx_ampdu_factor & IEEE80211_HT_CAP_AMPDU_FACTOR;

        if access::driver_rx_ampdu_spacing(padapter) != 0xff {
            ht_capie.ampdu_params_info |= (access::driver_rx_ampdu_spacing(padapter) & 0x07) << 2;
        } else if access::dot11_privacy(padapter) == _AES_ {
            let mut best_ampdu_density: U8 = 0;
            access::hal_get_def_var(
                padapter,
                HW_VAR_BEST_AMPDU_DENSITY,
                &mut best_ampdu_density as *mut U8 as *mut c_void,
            );
            ht_capie.ampdu_params_info |= IEEE80211_HT_CAP_AMPDU_DENSITY & (best_ampdu_density << 2);
        }

        rtw_rust_ht_set_ie(
            out_ie.add(*pout_len as usize),
            _HT_CAPABILITY_IE_ as c_int,
            core::mem::size_of::<RtwIeee80211HtCap>() as U32,
            &mut ht_capie as *mut RtwIeee80211HtCap as *mut U8,
            pout_len,
        );

        *ht_option = _TRUE as U8;

        if !in_ie.is_null() {
            let ht_add = rtw_rust_ht_get_ie(in_ie, _HT_ADD_INFO_IE_ as c_int, &mut ielen, in_len as c_int);
            if !ht_add.is_null() && ielen == 22 {
                rtw_rust_ht_set_ie(
                    out_ie.add(*pout_len as usize),
                    _HT_ADD_INFO_IE_ as c_int,
                    ielen,
                    ht_add.add(2),
                    pout_len,
                );
            }
        }

        u32::from(*ht_option)
    }
}

#[no_mangle]
pub extern "C" fn rtw_restructure_ht_ie(
    padapter: *mut U8,
    in_ie: *mut U8,
    out_ie: *mut U8,
    in_len: c_uint,
    pout_len: *mut c_uint,
    channel: U8,
) -> U32 {
    restructure_ht_ie_impl(padapter, in_ie, out_ie, in_len, pout_len, channel)
}

#[no_mangle]
pub extern "C" fn rtw_rust_ht_probe() -> c_int {
    0x1e67
}
