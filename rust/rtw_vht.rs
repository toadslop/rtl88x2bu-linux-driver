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

#[cfg(any(host_vht_test, host_vht_restructure_test, host_vht_mcs_rate_test))]
use std::os::raw::c_int;

#[cfg(host_vht_restructure_test)]
use std::os::raw::c_uint;

#[cfg(not(any(host_vht_test, host_vht_restructure_test, host_vht_mcs_rate_test)))]
use core::ffi::c_uint;

#[cfg(not(any(host_vht_test, host_vht_restructure_test, host_vht_mcs_rate_test)))]
use core::ffi::c_int;

mod mcs_rate {
    #[cfg(not(host_vht_mcs_rate_test))]
    use super::c_int;

    const MGN_VHT1SS_MCS0: u8 = 0xA0;
    const MGN_VHT1SS_MCS7: u8 = 0xA7;
    const MGN_VHT4SS_MCS9: u8 = 0xC7;
    const CHANNEL_WIDTH_20: u8 = 0;
    const CHANNEL_WIDTH_80: u8 = 2;
    const EID_VHT_OPERATION: i32 = 192;

    static VHT_MCS_DATA_RATE: [[&[u16; 40]; 2]; 3] = [
        [
            &[
                13, 26, 39, 52, 78, 104, 117, 130, 156, 156, 26, 52, 78, 104, 156, 208, 234, 260,
                312, 312, 39, 78, 117, 156, 234, 312, 351, 390, 468, 520, 52, 104, 156, 208, 312,
                416, 468, 520, 624, 624,
            ],
            &[
                14, 29, 43, 58, 87, 116, 130, 144, 173, 173, 29, 58, 87, 116, 173, 231, 260, 289,
                347, 347, 43, 87, 130, 173, 260, 347, 390, 433, 520, 578, 58, 116, 173, 231, 347,
                462, 520, 578, 693, 693,
            ],
        ],
        [
            &[
                27, 54, 81, 108, 162, 216, 243, 270, 324, 360, 54, 108, 162, 216, 324, 432, 486,
                540, 648, 720, 81, 162, 243, 324, 486, 648, 729, 810, 972, 1080, 108, 216, 324,
                432, 648, 864, 972, 1080, 1296, 1440,
            ],
            &[
                30, 60, 90, 120, 180, 240, 270, 300, 360, 400, 60, 120, 180, 240, 360, 480, 540,
                600, 720, 800, 90, 180, 270, 360, 540, 720, 810, 900, 1080, 1200, 120, 240, 360,
                480, 720, 960, 1080, 1200, 1440, 1600,
            ],
        ],
        [
            &[
                59, 117, 176, 234, 351, 468, 527, 585, 702, 780, 117, 234, 351, 468, 702, 936,
                1053, 1170, 1404, 1560, 176, 351, 527, 702, 1053, 1404, 1580, 1755, 2106, 2340,
                234, 468, 702, 936, 1404, 1872, 2106, 2340, 2808, 3120,
            ],
            &[
                65, 130, 195, 260, 390, 520, 585, 650, 780, 867, 130, 260, 390, 520, 780, 1040,
                1170, 1300, 1560, 1734, 195, 390, 585, 780, 1170, 1560, 1755, 1950, 2340, 2600,
                260, 520, 780, 1040, 1560, 2080, 2340, 2600, 3120, 3467,
            ],
        ],
    ];

    #[cfg(not(host_vht_mcs_rate_test))]
    extern "C" {
        fn rtw_ies_get_chbw(
            ies: *mut u8,
            ies_len: c_int,
            ch: *mut u8,
            bw: *mut u8,
            offset: *mut u8,
            ht: u8,
            vht: u8,
        );
        fn rtw_get_ie(pbuf: *const u8, index: c_int, len: *mut c_int, limit: c_int) -> *mut u8;
    }

    fn set_bits_le_byte(p: *mut u8, offset: u32, length: u32, value: u8) {
        unsafe {
            let mask = (((1u32 << length) - 1) << offset) as u8;
            *p = (*p & !mask) | ((value & ((1u32 << length) - 1) as u8) << offset);
        }
    }

    pub fn rtw_get_vht_highest_rate_impl(pvht_mcs_map: *mut u8) -> u8 {
        let map = unsafe { core::slice::from_raw_parts(pvht_mcs_map, 2) };
        let mut vht_mcs_rate = 0u8;
        for i in 0..2 {
            if map[i] == 0xff {
                continue;
            }
            let mut j = 0;
            while j < 8 {
                let bit_map = (map[i] >> j) & 3;
                if bit_map != 3 {
                    vht_mcs_rate = MGN_VHT1SS_MCS7 + 10 * (j / 2) as u8 + i as u8 * 40 + bit_map;
                }
                j += 2;
            }
        }
        vht_mcs_rate
    }

    pub fn rtw_vht_mcsmap_to_nss_impl(pvht_mcs_map: *mut u8) -> u8 {
        let map = unsafe { core::slice::from_raw_parts(pvht_mcs_map, 2) };
        let mut nss = 0u8;
        for i in 0..2 {
            if map[i] == 0xff {
                continue;
            }
            let mut j = 0;
            while j < 8 {
                if ((map[i] >> j) & 3) != 3 {
                    nss = nss.saturating_add(1);
                }
                j += 2;
            }
        }
        nss
    }

    pub fn rtw_vht_mcs_to_data_rate_impl(bw: u8, short_gi: u8, mut vht_mcs_rate: u8) -> u16 {
        if vht_mcs_rate > MGN_VHT4SS_MCS9 {
            vht_mcs_rate = MGN_VHT4SS_MCS9;
        }
        let idx = ((vht_mcs_rate - MGN_VHT1SS_MCS0) & 0x3f) as usize;
        VHT_MCS_DATA_RATE[bw as usize][short_gi as usize][idx]
    }

    pub fn rtw_vht_mcs_map_to_bitmap_impl(mcs_map: *mut u8, nss: u8) -> u64 {
        let map = unsafe { core::slice::from_raw_parts(mcs_map, 2) };
        let mut bitmap = 0u64;
        let bits_nss = nss * 2;
        let mut i = 0u8;
        let mut j = 0u8;
        while i < bits_nss {
            let tmp = (map[(i / 8) as usize] >> i) & 3;
            match tmp {
                2 => bitmap |= 0x03ff_u64 << j,
                1 => bitmap |= 0x01ff_u64 << j,
                0 => bitmap |= 0x00ff_u64 << j,
                _ => {}
            }
            i += 2;
            j += 10;
        }
        bitmap
    }

    #[cfg(not(host_vht_mcs_rate_test))]
    pub fn rtw_check_for_vht20_impl(adapter: *mut u8, ies: *mut u8, ies_len: c_int) {
        unsafe {
            let mut ht_ch = 0u8;
            let mut ht_bw = 0u8;
            let mut ht_offset = 0u8;
            let mut vht_ch = 0u8;
            let mut vht_bw = 0u8;
            let mut vht_offset = 0u8;
            rtw_ies_get_chbw(ies, ies_len, &mut ht_ch, &mut ht_bw, &mut ht_offset, 1, 0);
            rtw_ies_get_chbw(
                ies,
                ies_len,
                &mut vht_ch,
                &mut vht_bw,
                &mut vht_offset,
                1,
                1,
            );
            if ht_bw == CHANNEL_WIDTH_20 && vht_bw >= CHANNEL_WIDTH_80 {
                let mut vht_op_ielen = 0;
                let vht_op_ie = rtw_get_ie(ies, EID_VHT_OPERATION, &mut vht_op_ielen, ies_len);
                if !vht_op_ie.is_null() && vht_op_ielen != 0 {
                    set_bits_le_byte(vht_op_ie.add(2), 0, 8, 0);
                    set_bits_le_byte(vht_op_ie.add(3), 0, 8, 0);
                    set_bits_le_byte(vht_op_ie.add(4), 0, 8, 0);
                }
                let _ = adapter;
            }
        }
    }
}

#[cfg(any(
    not(any(host_vht_test, host_vht_restructure_test)),
    host_vht_mcs_rate_test
))]
#[no_mangle]
pub extern "C" fn rtw_get_vht_highest_rate(pvht_mcs_map: *mut u8) -> u8 {
    mcs_rate::rtw_get_vht_highest_rate_impl(pvht_mcs_map)
}

#[cfg(any(
    not(any(host_vht_test, host_vht_restructure_test)),
    host_vht_mcs_rate_test
))]
#[no_mangle]
pub extern "C" fn rtw_vht_mcsmap_to_nss(pvht_mcs_map: *mut u8) -> u8 {
    mcs_rate::rtw_vht_mcsmap_to_nss_impl(pvht_mcs_map)
}

#[cfg(any(
    not(any(host_vht_test, host_vht_restructure_test)),
    host_vht_mcs_rate_test
))]
#[no_mangle]
pub extern "C" fn rtw_vht_mcs_to_data_rate(bw: u8, short_gi: u8, vht_mcs_rate: u8) -> u16 {
    mcs_rate::rtw_vht_mcs_to_data_rate_impl(bw, short_gi, vht_mcs_rate)
}

#[cfg(any(
    not(any(host_vht_test, host_vht_restructure_test)),
    host_vht_mcs_rate_test
))]
#[no_mangle]
pub extern "C" fn rtw_vht_mcs_map_to_bitmap(mcs_map: *mut u8, nss: u8) -> u64 {
    mcs_rate::rtw_vht_mcs_map_to_bitmap_impl(mcs_map, nss)
}

#[cfg(not(any(host_vht_test, host_vht_restructure_test, host_vht_mcs_rate_test)))]
#[no_mangle]
pub extern "C" fn rtw_check_for_vht20(adapter: *mut u8, ies: *mut u8, ies_len: c_int) {
    mcs_rate::rtw_check_for_vht20_impl(adapter, ies, ies_len);
}

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

#[cfg(any(
    host_vht_restructure_test,
    all(not(host_vht_test), not(host_vht_mcs_rate_test))
))]
mod restructure {
    use super::c_int;
    use super::c_uint;

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
        fn rtw_rust_vht_get_ie(
            pbuf: *const u8,
            index: c_int,
            len: *mut u32,
            limit: c_int,
        ) -> *mut u8;
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

    #[cfg(not(host_vht_restructure_test))]
    mod kernel {
        use super::*;

        extern "C" {
            fn rtw_rust_vht_warn_on(condition: c_int);
        }

        pub(super) fn warn_on(condition: c_int) {
            unsafe { rtw_rust_vht_warn_on(condition) };
        }
    }

    #[cfg(host_vht_restructure_test)]
    mod kernel {
        use super::c_int;

        pub(super) fn warn_on(_condition: c_int) {}
    }

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

            let ht_op_ie = rtw_rust_vht_get_ie(
                in_ie.add(12),
                WLAN_EID_HT_OPERATION,
                &mut ielen,
                (in_len - 12) as c_int,
            );
            if ht_op_ie.is_null() || ielen != HT_OP_IE_LEN {
                return u32::from(*vht_option);
            }
            let vht_cap_ie = rtw_rust_vht_get_ie(
                in_ie.add(12),
                EID_VHTCapability,
                &mut ielen,
                (in_len - 12) as c_int,
            );
            if vht_cap_ie.is_null() || ielen != VHT_CAP_IE_LEN {
                return u32::from(*vht_option);
            }
            let vht_op_ie = rtw_rust_vht_get_ie(
                in_ie.add(12),
                EID_VHTOperation,
                &mut ielen,
                (in_len - 12) as c_int,
            );
            if vht_op_ie.is_null() || ielen != VHT_OP_IE_LEN {
                return u32::from(*vht_option);
            }

            *pout_len += rtw_build_vht_cap_ie(padapter, out_ie.add(*pout_len as usize));
            let out_vht_op_ie = out_ie.add(*pout_len as usize);
            rtw_set_ie(
                out_vht_op_ie,
                EID_VHTOperation,
                VHT_OP_IE_LEN,
                vht_op_ie.add(2),
                pout_len,
            );

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
                    while rtw_rust_vht_chset_is_chbw_valid(
                        chset,
                        oper_ch,
                        oper_bw,
                        oper_offset,
                        1,
                        1,
                    ) == 0
                        || (rtw_rust_vht_is_dfs_slave_with_rd(rfctl) != 0
                            && rtw_rust_vht_rfctl_dfs_domain_unknown(rfctl) == 0
                            && rtw_rust_vht_chset_is_chbw_non_ocp(
                                chset,
                                oper_ch,
                                oper_bw,
                                oper_offset,
                            ) != 0)
                    {
                        oper_bw = oper_bw.saturating_sub(1);
                        if oper_bw == CHANNEL_WIDTH_20 {
                            oper_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
                            break;
                        }
                    }
                }
            }

            kernel::warn_on(
                (rtw_rust_vht_chset_is_chbw_valid(chset, oper_ch, oper_bw, oper_offset, 1, 1) == 0)
                    as c_int,
            );
            if rtw_rust_vht_is_dfs_slave_with_rd(rfctl) != 0
                && rtw_rust_vht_rfctl_dfs_domain_unknown(rfctl) == 0
            {
                kernel::warn_on(
                    (rtw_rust_vht_chset_is_chbw_non_ocp(chset, oper_ch, oper_bw, oper_offset) != 0)
                        as c_int,
                );
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
                kernel::warn_on(1);
            }

            *pout_len +=
                rtw_build_vht_op_mode_notify_ie(padapter, out_ie.add(*pout_len as usize), oper_bw);
            *vht_option = _TRUE;
            u32::from(*vht_option)
        }
    }
}

#[cfg(any(
    host_vht_restructure_test,
    all(not(host_vht_test), not(host_vht_mcs_rate_test))
))]
#[no_mangle]
pub extern "C" fn rtw_restructure_vht_ie(
    padapter: *mut u8,
    in_ie: *mut u8,
    out_ie: *mut u8,
    in_len: c_uint,
    pout_len: *mut c_uint,
) -> u32 {
    restructure::restructure_vht_ie_impl(padapter, in_ie, out_ie, in_len, pout_len)
}

#[no_mangle]
pub extern "C" fn rtw_rust_vht_probe() -> c_int {
    0x1e35
}
