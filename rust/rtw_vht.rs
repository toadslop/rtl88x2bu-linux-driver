// SPDX-License-Identifier: GPL-2.0
//! VHT helpers — Rust port of `core/rtw_vht_rest.c` (W3-35, W3-36).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_vht_test)]
use std::os::raw::c_int;

#[cfg(host_vht_restructure_test)]
use std::os::raw::c_int;

#[cfg(not(any(host_vht_test, host_vht_restructure_test)))]
use core::ffi::c_int;

#[no_mangle]
pub extern "C" fn rtw_vht_nss_to_mcsmap(nss: u8, target_mcs_map: *mut u8, cur_mcs_map: *mut u8) {
    let cur = unsafe { core::slice::from_raw_parts(cur_mcs_map, 2) };
    let target = unsafe { core::slice::from_raw_parts_mut(target_mcs_map, 2) };
    for i in 0..2 {
        target[i] = 0;
        for j in (0..8).step_by(2) {
            let cur_rate = (cur[i] >> j) & 3;
            let target_rate = if cur_rate == 3 {
                3
            } else if nss <= (j / 2) as u8 + (i as u8) * 4 {
                3
            } else {
                cur_rate
            };
            target[i] |= target_rate << j;
        }
    }
}

#[cfg(any(host_vht_test, roku_private))]
#[no_mangle]
pub extern "C" fn VHT_get_ss_from_map(vht_mcs_map: *mut u8) -> u8 {
    let map = unsafe { core::slice::from_raw_parts(vht_mcs_map, 2) };
    let mut ss = 0u8;
    for i in 0..2 {
        if map[i] == 0xff {
            continue;
        }
        for j in (0..8).step_by(2) {
            if ((map[i] >> j) & 0x03) == 0x03 {
                break;
            }
            ss = ss.saturating_add(1);
        }
    }
    ss
}

#[cfg(host_vht_restructure_test)]
mod restructure {
    use super::c_int;
    use std::os::raw::c_uint;

    const CHANNEL_WIDTH_20: u8 = 0;
    const CHANNEL_WIDTH_40: u8 = 1;
    const CHANNEL_WIDTH_80: u8 = 2;
    const HAL_PRIME_CHNL_OFFSET_DONT_CARE: u8 = 0;
    const HAL_PRIME_CHNL_OFFSET_LOWER: u8 = 1;
    const HAL_PRIME_CHNL_OFFSET_UPPER: u8 = 2;
    const WLAN_EID_HT_OPERATION: c_int = 61;
    const EID_VHTCapability: c_int = 191;
    const EID_VHTOperation: c_int = 192;
    const HT_OP_IE_LEN: u32 = 22;
    const VHT_CAP_IE_LEN: u32 = 12;
    const VHT_OP_IE_LEN: u32 = 5;
    const SCA: u8 = 1;
    const SCB: u8 = 3;
    const _TRUE: u8 = 1;

    extern "C" {
        fn rtw_rust_vht_channel_set(padapter: *mut u8) -> *mut u8;
        fn rtw_rust_vht_regsty_bw_5g(padapter: *mut u8) -> u8;
        fn rtw_rust_vht_vht_option(padapter: *mut u8) -> *mut u8;
        fn rtw_rust_vht_get_ie(pbuf: *const u8, index: c_int, len: *mut u32, limit: c_int) -> *mut u8;
        fn rtw_set_ie(
            pbuf: *mut u8,
            index: c_int,
            len: c_uint,
            source: *const u8,
            frlen: *mut c_uint,
        ) -> *mut u8;
        fn rtw_vht_use_default_setting(padapter: *mut u8);
        fn rtw_build_vht_cap_ie(padapter: *mut u8, pbuf: *mut u8) -> u32;
        fn rtw_build_vht_op_mode_notify_ie(padapter: *mut u8, pbuf: *mut u8, bw: u8) -> u32;
        fn hal_largest_bw(padapter: *mut u8, bw_cap: u8) -> u8;
        fn rtw_rust_vht_chset_is_chbw_valid(
            ch_set: *mut u8,
            ch: u8,
            bw: u8,
            offset: u8,
            a: u8,
            b: u8,
        ) -> u8;
        fn rtw_rust_vht_chset_is_chbw_non_ocp(ch_set: *mut u8, ch: u8, bw: u8, offset: u8) -> u8;
        fn rtw_rust_vht_rfctl(padapter: *mut u8) -> *mut u8;
        fn rtw_rust_vht_is_dfs_slave_with_rd(rfctl: *mut u8) -> u8;
        fn rtw_rust_vht_rfctl_dfs_domain_unknown(rfctl: *mut u8) -> u8;
        fn rtw_get_center_ch(ch: u8, bw: u8, offset: u8) -> u8;
    }

    fn warn_on(_cond: c_int) {}

    fn le_bits(p: *const u8, o: u32, l: u32) -> u8 {
        unsafe { (*p >> o) as u8 & ((1u32 << l) - 1) as u8 }
    }

    fn set_bits(p: *mut u8, o: u32, l: u32, v: u8) {
        unsafe {
            let mask = (((1u32 << l) - 1) << o) as u8;
            *p = (*p & !mask) | ((v & ((1u32 << l) - 1) as u8) << o);
        }
    }

    pub fn restructure_vht_ie_impl(
        padapter: *mut u8,
        in_ie: *mut u8,
        out_ie: *mut u8,
        in_len: c_uint,
        pout_len: *mut c_uint,
    ) -> u32 {
        unsafe {
            let rfctl = rtw_rust_vht_rfctl(padapter);
            let chset = rtw_rust_vht_channel_set(padapter);
            let vht_option = rtw_rust_vht_vht_option(padapter);
            let mut ielen: u32 = 0;
            let mut oper_bw = CHANNEL_WIDTH_20;
            let mut oper_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;

            rtw_vht_use_default_setting(padapter);

            let ht_op_ie = rtw_rust_vht_get_ie(in_ie.add(12), WLAN_EID_HT_OPERATION, &mut ielen, (in_len - 12) as c_int);
            if ht_op_ie.is_null() || ielen != HT_OP_IE_LEN {
                return u32::from(*vht_option);
            }
            let vht_cap_ie = rtw_rust_vht_get_ie(in_ie.add(12), EID_VHTCapability, &mut ielen, (in_len - 12) as c_int);
            if vht_cap_ie.is_null() || ielen != VHT_CAP_IE_LEN {
                return u32::from(*vht_option);
            }
            let vht_op_ie = rtw_rust_vht_get_ie(in_ie.add(12), EID_VHTOperation, &mut ielen, (in_len - 12) as c_int);
            if vht_op_ie.is_null() || ielen != VHT_OP_IE_LEN {
                return u32::from(*vht_option);
            }

            *pout_len += rtw_build_vht_cap_ie(padapter, out_ie.add(*pout_len as usize));
            let out_vht_op_ie = out_ie.add(*pout_len as usize);
            rtw_set_ie(out_vht_op_ie, EID_VHTOperation, VHT_OP_IE_LEN, vht_op_ie.add(2), pout_len);

            let oper_ch = le_bits(ht_op_ie.add(2), 0, 8);
            let max_bw = hal_largest_bw(padapter, rtw_rust_vht_regsty_bw_5g(padapter));

            if max_bw >= CHANNEL_WIDTH_40 {
                if le_bits(ht_op_ie.add(2).add(1), 2, 1) != 0 {
                    match le_bits(ht_op_ie.add(2).add(1), 0, 2) {
                        SCA => {
                            oper_bw = CHANNEL_WIDTH_40;
                            oper_offset = HAL_PRIME_CHNL_OFFSET_LOWER;
                        }
                        SCB => {
                            oper_bw = CHANNEL_WIDTH_40;
                            oper_offset = HAL_PRIME_CHNL_OFFSET_UPPER;
                        }
                        _ => {}
                    }
                }
                if oper_bw == CHANNEL_WIDTH_40 {
                    if matches!(le_bits(vht_op_ie.add(2), 0, 8), 1 | 2 | 3) {
                        oper_bw = CHANNEL_WIDTH_80;
                    }
                    oper_bw = core::cmp::min(oper_bw, max_bw);
                    while rtw_rust_vht_chset_is_chbw_valid(chset, oper_ch, oper_bw, oper_offset, 1, 1) == 0
                        || (rtw_rust_vht_is_dfs_slave_with_rd(rfctl) != 0
                            && rtw_rust_vht_rfctl_dfs_domain_unknown(rfctl) == 0
                            && rtw_rust_vht_chset_is_chbw_non_ocp(chset, oper_ch, oper_bw, oper_offset) != 0)
                    {
                        oper_bw = oper_bw.saturating_sub(1);
                        if oper_bw == CHANNEL_WIDTH_20 {
                            oper_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
                            break;
                        }
                    }
                }
            }

            warn_on((rtw_rust_vht_chset_is_chbw_valid(chset, oper_ch, oper_bw, oper_offset, 1, 1) == 0) as c_int);
            if rtw_rust_vht_is_dfs_slave_with_rd(rfctl) != 0
                && rtw_rust_vht_rfctl_dfs_domain_unknown(rfctl) == 0
            {
                warn_on((rtw_rust_vht_chset_is_chbw_non_ocp(chset, oper_ch, oper_bw, oper_offset) != 0)
                    as c_int);
            }

            if oper_bw < CHANNEL_WIDTH_80 {
                set_bits(out_vht_op_ie.add(2), 0, 8, 0);
                set_bits(out_vht_op_ie.add(3), 0, 8, 0);
                set_bits(out_vht_op_ie.add(4), 0, 8, 0);
            } else if oper_bw == CHANNEL_WIDTH_80 {
                let cch = rtw_get_center_ch(oper_ch, oper_bw, oper_offset);
                set_bits(out_vht_op_ie.add(2), 0, 8, 1);
                set_bits(out_vht_op_ie.add(3), 0, 8, cch);
                set_bits(out_vht_op_ie.add(4), 0, 8, 0);
            } else {
                warn_on(1);
            }

            *pout_len += rtw_build_vht_op_mode_notify_ie(padapter, out_ie.add(*pout_len as usize), oper_bw);
            *vht_option = _TRUE;
            u32::from(*vht_option)
        }
    }
}

#[cfg(host_vht_restructure_test)]
#[no_mangle]
pub extern "C" fn rtw_restructure_vht_ie(
    padapter: *mut u8,
    in_ie: *mut u8,
    out_ie: *mut u8,
    in_len: std::os::raw::c_uint,
    pout_len: *mut std::os::raw::c_uint,
) -> u32 {
    restructure::restructure_vht_ie_impl(padapter, in_ie, out_ie, in_len, pout_len)
}

#[no_mangle]
pub extern "C" fn rtw_rust_vht_probe() -> c_int {
    0x1e35
}
