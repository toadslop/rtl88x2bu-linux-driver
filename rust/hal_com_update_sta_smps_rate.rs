// SPDX-License-Identifier: GPL-2.0

use core::ffi::c_void;

type U8 = u8;
type S8 = i8;
type U32 = u32;
type U64 = u64;
type Adapter = c_void;
type StaInfo = c_void;

const SM_PS_DISABLE: U8 = 3;
const _TRUE: U8 = 1;

#[cfg(host_hal_com_update_sta_smps_rate_test)]
#[repr(C)]
struct HostAdapter {
    ht_enable: U8,
    wireless_mode: U8,
    ap_state: U8,
    hal_tx_nss: S8,
}

#[cfg(host_hal_com_update_sta_smps_rate_test)]
#[repr(C)]
struct HostSta {
    mac_id: U8,
    sm_ps: U8,
    ht_option: U8,
    smps_cap: U8,
    vht_option: U8,
    init_rate: U8,
    ramask: U64,
    bssrateset: [U8; 16],
    supp_mcs_set: [U8; 16],
    vht_mcs_map: [U8; 2],
}

#[cfg(not(host_hal_com_update_sta_smps_rate_test))]
extern "C" {
    fn rtw_rust_hal_com_sta_set_sm_ps(p: *mut StaInfo, v: U8);
    fn rtw_rust_hal_com_sta_mac_id(p: *mut StaInfo) -> U8;
    fn rtw_rust_hal_com_log_sta_sm_ps(mac: U8, sm: U8);
    fn rtw_rust_hal_com_adapter_ht_enabled(a: *mut Adapter) -> U8;
    fn rtw_rust_hal_com_adapter_wireless_mode(a: *mut Adapter) -> U8;
    fn rtw_rust_hal_com_get_hal_tx_nss(a: *mut Adapter) -> U8;
    fn rtw_rust_hal_com_mlmepriv_is_ap(a: *mut Adapter) -> U8;
    fn rtw_rust_hal_com_sta_bssrate(p: *mut StaInfo, i: usize) -> U8;
    fn rtw_rust_hal_com_sta_ht_option(p: *mut StaInfo) -> U8;
    fn rtw_rust_hal_com_sta_ht_smps_cap(p: *mut StaInfo) -> U8;
    fn rtw_rust_hal_com_sta_ht_supp_mcs(p: *mut StaInfo, i: usize) -> U8;
    fn rtw_rust_hal_com_sta_vht_option(p: *mut StaInfo) -> U8;
    fn rtw_rust_hal_com_sta_vht_mcs_map(p: *mut StaInfo, i: usize) -> U8;
    fn rtw_rust_hal_com_sta_set_ramask(p: *mut StaInfo, m: U64);
    fn rtw_rust_hal_com_sta_set_init_rate(p: *mut StaInfo, r: U8);
    fn rtw_get_bit_value_from_ieee_value(v: U8) -> i32;
    fn rtw_ht_mcs_set_to_bitmap(m: *mut U8, nss: U8) -> U32;
    fn rtw_vht_mcs_map_to_bitmap(m: *mut U8, nss: U8) -> U64;
    fn get_highest_rate_idx(m: U64) -> U8;
}

#[cfg(host_hal_com_update_sta_smps_rate_test)]
const DOT11: [u8; 13] = [2, 4, 11, 22, 12, 18, 24, 36, 48, 72, 96, 108, 0];

#[cfg(host_hal_com_update_sta_smps_rate_test)]
fn ieee_bit(val: U8) -> u64 {
    let mut bit = 1u64;
    for &r in DOT11.iter() {
        if r == 0 {
            break;
        }
        if r == val {
            return bit;
        }
        bit <<= 1;
    }
    0
}

#[cfg(host_hal_com_update_sta_smps_rate_test)]
fn ht_bitmap(m: &[U8; 16], nss: U8) -> U32 {
    let mut b = 0u32;
    for i in 0..nss as usize {
        b |= (m[i] as u32) << (i * 8);
    }
    b
}

#[cfg(host_hal_com_update_sta_smps_rate_test)]
fn vht_bitmap(m: &[U8; 2], nss: U8) -> U64 {
    let mut b = 0u64;
    let bits = nss * 2;
    let mut j = 0u8;
    let mut i = 0u8;
    while i < bits {
        let t = (m[(i / 8) as usize] >> i) & 3;
        match t {
            2 => b |= 0x3ffu64 << j,
            1 => b |= 0x1ffu64 << j,
            0 => b |= 0xffu64 << j,
            _ => {}
        }
        i += 2;
        j += 10;
    }
    b
}

#[cfg(host_hal_com_update_sta_smps_rate_test)]
fn highest_rate(m: U64) -> U8 {
    for i in (0..64).rev() {
        if (m >> i) & 1 != 0 {
            return i as U8;
        }
    }
    0
}

fn is_supported_ht(mode: U8) -> bool {
    const W24: U8 = 1 << 3;
    const W5: U8 = 1 << 4;
    (mode & (W24 | W5)) != 0
}

#[cfg(host_hal_com_update_sta_smps_rate_test)]
#[no_mangle]
pub extern "C" fn rtw_hal_update_sta_smps_cap(adapter: *mut Adapter, psta: *mut StaInfo) {
    let _ = adapter;
    if psta.is_null() {
        return;
    }
    unsafe {
        (*psta.cast::<HostSta>()).sm_ps = SM_PS_DISABLE;
    }
}

#[cfg(not(host_hal_com_update_sta_smps_rate_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_update_sta_smps_cap(adapter: *mut Adapter, psta: *mut StaInfo) {
    let _ = adapter;
    if psta.is_null() {
        return;
    }
    unsafe {
        rtw_rust_hal_com_sta_set_sm_ps(psta, SM_PS_DISABLE);
        let mac = rtw_rust_hal_com_sta_mac_id(psta);
        rtw_rust_hal_com_log_sta_sm_ps(mac, SM_PS_DISABLE);
    }
}

#[cfg(host_hal_com_update_sta_smps_rate_test)]
#[no_mangle]
pub extern "C" fn rtw_hal_update_sta_rate_mask(adapter: *mut Adapter, psta: *mut StaInfo) {
    if adapter.is_null() || psta.is_null() {
        return;
    }
    let (a, p) = unsafe { (&*adapter.cast::<HostAdapter>(), &mut *psta.cast::<HostSta>()) };
    let mut mask = 0u64;
    for i in 0..16 {
        let r = p.bssrateset[i];
        if r != 0 {
            mask |= ieee_bit(r & 0x7f);
        }
    }
    if a.ht_enable != 0 && is_supported_ht(a.wireless_mode) {
        let mut tx = a.hal_tx_nss.max(0) as U8;
        if p.vht_option != 0 {
            mask |= vht_bitmap(&p.vht_mcs_map, tx) << 12;
        } else if p.ht_option != 0 {
            if a.ap_state != 0 && (p.smps_cap == 0 || p.smps_cap == 1) && tx > 1 {
                tx = 1;
            }
            mask |= (ht_bitmap(&p.supp_mcs_set, tx) as u64) << 12;
        }
    }
    p.ramask = mask;
    p.init_rate = highest_rate(mask) & 0x3f;
}

#[cfg(not(host_hal_com_update_sta_smps_rate_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_update_sta_rate_mask(adapter: *mut Adapter, psta: *mut StaInfo) {
    if psta.is_null() {
        return;
    }
    let mut mask = 0u64;
    for i in 0..16usize {
        let r = unsafe { rtw_rust_hal_com_sta_bssrate(psta, i) };
        if r != 0 {
            mask |= unsafe { rtw_get_bit_value_from_ieee_value(r & 0x7f) as u64 };
        }
    }
    // Kernel path: always fold HT/VHT MCS into ramask (matches C with CONFIG_80211N_HT
    // and CONFIG_80211AC_VHT from autoconf.h). Module rustc also sees config_80211*
    // via rust/generated_cfgs (T21); do not gate this block on those cfgs.
    let ht_en = unsafe { rtw_rust_hal_com_adapter_ht_enabled(adapter) };
    let mode = unsafe { rtw_rust_hal_com_adapter_wireless_mode(adapter) };
    if ht_en != 0 && is_supported_ht(mode) {
        let mut tx = unsafe { rtw_rust_hal_com_get_hal_tx_nss(adapter) };
        let vht = unsafe { rtw_rust_hal_com_sta_vht_option(psta) };
        if vht != 0 {
            let mut map = [0u8; 2];
            map[0] = unsafe { rtw_rust_hal_com_sta_vht_mcs_map(psta, 0) };
            map[1] = unsafe { rtw_rust_hal_com_sta_vht_mcs_map(psta, 1) };
            mask |= unsafe { rtw_vht_mcs_map_to_bitmap(map.as_mut_ptr(), tx) << 12 };
        } else if unsafe { rtw_rust_hal_com_sta_ht_option(psta) } != 0 {
            let mut mcs = [0u8; 16];
            for i in 0..16 {
                mcs[i] = unsafe { rtw_rust_hal_com_sta_ht_supp_mcs(psta, i) };
            }
            if unsafe { rtw_rust_hal_com_mlmepriv_is_ap(adapter) } != 0 {
                let sm = unsafe { rtw_rust_hal_com_sta_ht_smps_cap(psta) };
                if (sm == 0 || sm == 1) && tx > 1 {
                    tx = 1;
                }
            }
            let ht = unsafe { rtw_ht_mcs_set_to_bitmap(mcs.as_mut_ptr(), tx) as u64 };
            mask |= ht << 12;
        }
    }
    let ir = unsafe { get_highest_rate_idx(mask) & 0x3f };
    unsafe {
        rtw_rust_hal_com_sta_set_ramask(psta, mask);
        rtw_rust_hal_com_sta_set_init_rate(psta, ir);
    }
}