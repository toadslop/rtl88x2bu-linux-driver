// SPDX-License-Identifier: GPL-2.0
//! W3-124/W3-125 ioctl helpers (host L2 Rust oracles + kernel leaf setters).
#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_snake_case,
    unreachable_pub
)]

#[cfg(rust_ioctl_set_leaf)]
use core::ffi::{c_int, c_ulong, c_void};

#[cfg(any(rust_ioctl_set_leaf, host_ioctl_max_rate_test))]
mod max_rate_legacy {
    /// Must match `NumRates` in `include/rtw_rf.h` (kernel FFI layout).
    pub const NUM_RATES: usize = 13;

    #[repr(C)]
    pub struct MaxRateLegacyIn {
        pub fw_state: u32,
        pub has_sta: u8,
        pub sta_mode: u8,
        pub ap_rates: [u8; NUM_RATES],
        pub sta_rates: [u8; NUM_RATES],
        pub sta_rate_len: u8,
    }

    const IEEE80211_BASIC_RATE_MASK: u8 = 0x80;

    pub fn calc(in_: &MaxRateLegacyIn) -> u16 {
        if in_.has_sta == 0 {
            return 0;
        }
        let mut i = 0usize;
        let mut max_rate: u16 = 0;
        let sta_len = in_.sta_rate_len as usize;
        while i < in_.ap_rates.len() && in_.ap_rates[i] != 0 && in_.ap_rates[i] != 0xff {
            let rate = (in_.ap_rates[i] & 0x7f) as u16;
            if in_.sta_mode != 0 {
                for j in 0..sta_len {
                    let sr = in_.sta_rates[j] as u16;
                    if (rate | IEEE80211_BASIC_RATE_MASK as u16)
                        == (sr | IEEE80211_BASIC_RATE_MASK as u16)
                    {
                        if rate > max_rate {
                            max_rate = rate;
                        }
                        break;
                    }
                }
            } else if rate > max_rate {
                max_rate = rate;
            }
            i += 1;
        }
        max_rate * 10 / 2
    }
}

#[cfg(rust_ioctl_set_leaf)]
mod kernel {
    use super::{c_int, c_ulong, c_void, max_rate_legacy::MaxRateLegacyIn};

    extern "C" {
        fn rtw_rust_ioctl_scan_mode_ptr(adapter: *mut c_void) -> *mut i32;
        fn rtw_rust_ioctl_setband_ptr(adapter: *mut c_void) -> *mut u32;
        fn rtw_rust_ioctl_regsty_ptr(adapter: *mut c_void) -> *mut c_void;
        fn rtw_rust_ioctl_regd_from_os(regsty: *mut c_void) -> i32;
        fn rtw_rust_ioctl_set_country_kbuild_enabled() -> i32;
        fn rtw_set_chplan_cmd(adapter: *mut c_void, flags: i32, chplan: u8, swconfig: u8) -> u8;
        fn rtw_set_country_cmd(
            adapter: *mut c_void,
            flags: i32,
            country_code: *const u8,
            swconfig: u8,
        ) -> u8;
        fn rtw_rust_ioctl_mlme_lock_ptr(adapter: *mut c_void) -> *mut c_void;
        fn rtw_rust_ioctl_enter_critical_bh(lock: *mut c_void, irqL: *mut c_ulong);
        fn rtw_rust_ioctl_exit_critical_bh(lock: *mut c_void, irqL: *mut c_ulong);
        fn rtw_sitesurvey_cmd(adapter: *mut c_void, pparm: *mut c_void) -> u8;
        fn rtw_rust_ioctl_disassociate_if_assoc(adapter: *mut c_void);
        fn rtw_rust_ioctl_infra_mode_ptr(adapter: *mut c_void) -> *mut u32;
        fn rtw_rust_ioctl_join_res_ptr(adapter: *mut c_void) -> *mut i32;
        fn rtw_rust_ioctl_fw_state_ptr(adapter: *mut c_void) -> *mut u32;
        fn rtw_rust_ioctl_clr_fwstate_mask(adapter: *mut c_void, state: c_int);
        fn rtw_rust_ioctl_set_fwstate(adapter: *mut c_void, state: c_int);
        fn rtw_rust_ioctl_stop_ap_mode(adapter: *mut c_void);
        fn rtw_rust_ioctl_start_ap_mode(adapter: *mut c_void);
        fn rtw_rust_ioctl_disassoc_cmd(adapter: *mut c_void, flags: u8);
        fn rtw_rust_ioctl_free_assoc_resources_cmd(adapter: *mut c_void, flags: u8);
        fn rtw_rust_ioctl_indicate_disconnect(adapter: *mut c_void);
        fn rtw_rust_ioctl_init_bcmc_stainfo(adapter: *mut c_void);
    }

    pub unsafe fn scan_mode_ptr(adapter: *mut c_void) -> *mut i32 {
        unsafe { rtw_rust_ioctl_scan_mode_ptr(adapter) }
    }

    pub unsafe fn setband_ptr(adapter: *mut c_void) -> *mut u32 {
        unsafe { rtw_rust_ioctl_setband_ptr(adapter) }
    }

    pub unsafe fn regsty_ptr(adapter: *mut c_void) -> *mut c_void {
        unsafe { rtw_rust_ioctl_regsty_ptr(adapter) }
    }

    pub unsafe fn regd_from_os(regsty: *mut c_void) -> bool {
        unsafe { rtw_rust_ioctl_regd_from_os(regsty) != 0 }
    }

    pub unsafe fn set_country_kbuild_enabled() -> bool {
        unsafe { rtw_rust_ioctl_set_country_kbuild_enabled() != 0 }
    }

    pub unsafe fn set_chplan_cmd(adapter: *mut c_void, flags: i32, chplan: u8, swconfig: u8) -> u8 {
        unsafe { rtw_set_chplan_cmd(adapter, flags, chplan, swconfig) }
    }

    pub unsafe fn set_country_cmd(
        adapter: *mut c_void,
        flags: i32,
        country_code: *const u8,
        swconfig: u8,
    ) -> u8 {
        unsafe { rtw_set_country_cmd(adapter, flags, country_code, swconfig) }
    }

    pub unsafe fn mlme_lock_ptr(adapter: *mut c_void) -> *mut c_void {
        unsafe { rtw_rust_ioctl_mlme_lock_ptr(adapter) }
    }

    pub unsafe fn enter_critical_bh(lock: *mut c_void, irqL: *mut c_ulong) {
        unsafe { rtw_rust_ioctl_enter_critical_bh(lock, irqL) }
    }

    pub unsafe fn exit_critical_bh(lock: *mut c_void, irqL: *mut c_ulong) {
        unsafe { rtw_rust_ioctl_exit_critical_bh(lock, irqL) }
    }

    pub unsafe fn sitesurvey_cmd(adapter: *mut c_void, pparm: *mut c_void) -> u8 {
        unsafe { rtw_sitesurvey_cmd(adapter, pparm) }
    }

    extern "C" {
        fn rtw_rust_ioctl_max_rate_legacy_fill(adapter: *mut c_void, out: *mut MaxRateLegacyIn);
    }

    pub unsafe fn max_rate_legacy_kernel(adapter: *mut c_void) -> u16 {
        let mut in_ = MaxRateLegacyIn {
            fw_state: 0,
            has_sta: 0,
            sta_mode: 0,
            ap_rates: [0; super::max_rate_legacy::NUM_RATES],
            sta_rates: [0; super::max_rate_legacy::NUM_RATES],
            sta_rate_len: 0,
        };
        unsafe { rtw_rust_ioctl_max_rate_legacy_fill(adapter, &mut in_) };
        super::max_rate_legacy::calc(&in_)
    }

    pub unsafe fn disassociate_if_assoc(adapter: *mut c_void) {
        unsafe { rtw_rust_ioctl_disassociate_if_assoc(adapter) }
    }

    pub unsafe fn infra_mode_ptr(adapter: *mut c_void) -> *mut u32 {
        unsafe { rtw_rust_ioctl_infra_mode_ptr(adapter) }
    }

    pub unsafe fn join_res_ptr(adapter: *mut c_void) -> *mut i32 {
        unsafe { rtw_rust_ioctl_join_res_ptr(adapter) }
    }

    pub unsafe fn fw_state_ptr(adapter: *mut c_void) -> *mut u32 {
        unsafe { rtw_rust_ioctl_fw_state_ptr(adapter) }
    }

    pub unsafe fn clr_fwstate_mask(adapter: *mut c_void, state: c_int) {
        unsafe { rtw_rust_ioctl_clr_fwstate_mask(adapter, state) }
    }

    pub unsafe fn set_fwstate(adapter: *mut c_void, state: c_int) {
        unsafe { rtw_rust_ioctl_set_fwstate(adapter, state) }
    }

    pub unsafe fn stop_ap_mode(adapter: *mut c_void) {
        unsafe { rtw_rust_ioctl_stop_ap_mode(adapter) }
    }

    pub unsafe fn start_ap_mode(adapter: *mut c_void) {
        unsafe { rtw_rust_ioctl_start_ap_mode(adapter) }
    }

    pub unsafe fn disassoc_cmd(adapter: *mut c_void, flags: u8) {
        unsafe { rtw_rust_ioctl_disassoc_cmd(adapter, flags) }
    }

    pub unsafe fn free_assoc_resources_cmd(adapter: *mut c_void, flags: u8) {
        unsafe { rtw_rust_ioctl_free_assoc_resources_cmd(adapter, flags) }
    }

    pub unsafe fn indicate_disconnect(adapter: *mut c_void) {
        unsafe { rtw_rust_ioctl_indicate_disconnect(adapter) }
    }

    pub unsafe fn init_bcmc_stainfo(adapter: *mut c_void) {
        unsafe { rtw_rust_ioctl_init_bcmc_stainfo(adapter) }
    }
}

#[cfg(rust_ioctl_set_leaf)]
const SCAN_PASSIVE_K: i32 = 0;
#[cfg(rust_ioctl_set_leaf)]
const SCAN_ACTIVE_K: i32 = 1;
#[cfg(rust_ioctl_set_leaf)]
const WIFI_FREQUENCY_BAND_2GHZ_K: u8 = 2;

#[cfg(rust_ioctl_set_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_scan_mode(adapter: *mut c_void, scan_mode: i32) -> i32 {
    if scan_mode != SCAN_ACTIVE_K && scan_mode != SCAN_PASSIVE_K {
        return _FAIL;
    }
    let ptr = unsafe { kernel::scan_mode_ptr(adapter) };
    unsafe {
        *ptr = scan_mode;
    }
    _SUCCESS
}

#[cfg(rust_ioctl_set_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_band(adapter: *mut c_void, band: u8) -> i32 {
    if band > WIFI_FREQUENCY_BAND_2GHZ_K {
        return _FAIL;
    }
    let ptr = unsafe { kernel::setband_ptr(adapter) };
    unsafe {
        *ptr = band as u32;
    }
    _SUCCESS
}

#[cfg(rust_ioctl_set_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_channel_plan(adapter: *mut c_void, channel_plan: u8) -> i32 {
    let regsty = unsafe { kernel::regsty_ptr(adapter) };
    if unsafe { kernel::regd_from_os(regsty) } {
        return _SUCCESS;
    }
    unsafe { kernel::set_chplan_cmd(adapter, RTW_CMDF_WAIT_ACK, channel_plan, 1) as i32 }
}

#[cfg(rust_ioctl_set_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_country(adapter: *mut c_void, country_code: *const u8) -> i32 {
    if !unsafe { kernel::set_country_kbuild_enabled() } {
        return _SUCCESS;
    }
    let regsty = unsafe { kernel::regsty_ptr(adapter) };
    if unsafe { kernel::regd_from_os(regsty) } {
        return _SUCCESS;
    }
    unsafe { kernel::set_country_cmd(adapter, RTW_CMDF_WAIT_ACK, country_code, 1) as i32 }
}

#[cfg(rust_ioctl_set_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_802_11_disassociate(adapter: *mut c_void) -> u8 {
    let mut irqL: c_ulong = 0;
    let lock = unsafe { kernel::mlme_lock_ptr(adapter) };
    unsafe {
        kernel::enter_critical_bh(lock, &mut irqL);
        kernel::disassociate_if_assoc(adapter);
        kernel::exit_critical_bh(lock, &mut irqL);
    }
    _TRUE
}

#[cfg(rust_ioctl_set_leaf)]
const NDIS802_11_IBSS_K: u32 = 0;
#[cfg(rust_ioctl_set_leaf)]
const NDIS802_11_INFRASTRUCTURE_K: u32 = 1;
#[cfg(rust_ioctl_set_leaf)]
const NDIS802_11_AP_MODE_K: u32 = 4;
#[cfg(rust_ioctl_set_leaf)]
const WIFI_ASOC_STATE_K: u32 = 0x0000_0001;
#[cfg(rust_ioctl_set_leaf)]
const WIFI_ADHOC_MASTER_STATE_K: u32 = 0x0000_0040;
#[cfg(rust_ioctl_set_leaf)]
const WIFI_NULL_STATE_K: u32 = 0;
#[cfg(rust_ioctl_set_leaf)]
const WIFI_ADHOC_STATE_K: u32 = 0x0000_0020;
#[cfg(rust_ioctl_set_leaf)]
const WIFI_STATION_STATE_K: u32 = 0x0000_0008;
#[cfg(rust_ioctl_set_leaf)]
const WIFI_AP_STATE_K: u32 = 0x0000_0010;
#[cfg(config_rtw_mesh)]
#[cfg(rust_ioctl_set_leaf)]
const NDIS802_11_MESH_K: u32 = 6;
#[cfg(config_wifi_monitor)]
#[cfg(rust_ioctl_set_leaf)]
const NDIS802_11_MONITOR_K: u32 = 5;
#[cfg(config_wifi_monitor)]
#[cfg(rust_ioctl_set_leaf)]
const WIFI_MONITOR_STATE_K: c_int = 0x8000_0000_u32 as c_int;
#[cfg(config_rtw_mesh)]
#[cfg(rust_ioctl_set_leaf)]
const WIFI_MESH_STATE_K: c_int = 0x0000_0200;

#[cfg(rust_ioctl_set_leaf)]
fn kernel_chk_fw(fw_state: u32, bit: u32) -> bool {
    (fw_state & bit) != 0
}

#[cfg(rust_ioctl_set_leaf)]
unsafe fn kernel_set_infra_mode(adapter: *mut c_void, networktype: u32, flags: u8) -> u8 {
    let old_ptr = unsafe { kernel::infra_mode_ptr(adapter) };
    if old_ptr.is_null() {
        return _FALSE;
    }
    let pold_state = unsafe { *old_ptr };
    if pold_state == networktype {
        return _TRUE;
    }
    let mut ap2sta_mode = false;
    let mut ret = _TRUE;
    if pold_state == NDIS802_11_AP_MODE_K {
        let join_res = unsafe { kernel::join_res_ptr(adapter) };
        if !join_res.is_null() {
            unsafe { *join_res = -1 };
        }
        ap2sta_mode = true;
        unsafe { kernel::stop_ap_mode(adapter) };
    }
    #[cfg(config_rtw_mesh)]
    if pold_state == NDIS802_11_MESH_K {
        let join_res = unsafe { kernel::join_res_ptr(adapter) };
        if !join_res.is_null() {
            unsafe { *join_res = -1 };
        }
        ap2sta_mode = true;
        unsafe { kernel::stop_ap_mode(adapter) };
    }

    let mut irqL: c_ulong = 0;
    let lock = unsafe { kernel::mlme_lock_ptr(adapter) };
    let fw_ptr = unsafe { kernel::fw_state_ptr(adapter) };
    unsafe { kernel::enter_critical_bh(lock, &mut irqL) };
    let fw_state = unsafe { *fw_ptr };
    let is_linked = kernel_chk_fw(fw_state, WIFI_ASOC_STATE_K);
    let is_adhoc_master = kernel_chk_fw(fw_state, WIFI_ADHOC_MASTER_STATE_K);

    if flags != 0 {
        unsafe { kernel::exit_critical_bh(lock, &mut irqL) };
    }

    if is_linked || pold_state == NDIS802_11_IBSS_K {
        unsafe { kernel::disassoc_cmd(adapter, flags) };
    }
    if is_linked || is_adhoc_master {
        unsafe { kernel::free_assoc_resources_cmd(adapter, flags) };
    }
    if (pold_state == NDIS802_11_INFRASTRUCTURE_K || pold_state == NDIS802_11_IBSS_K) && is_linked {
        unsafe { kernel::indicate_disconnect(adapter) };
    }

    if flags != 0 {
        unsafe { kernel::enter_critical_bh(lock, &mut irqL) };
    }

    unsafe { *old_ptr = networktype };
    unsafe { kernel::clr_fwstate_mask(adapter, !0) };

    match networktype {
        NDIS802_11_IBSS_K => unsafe { kernel::set_fwstate(adapter, WIFI_ADHOC_STATE_K as c_int) },
        NDIS802_11_INFRASTRUCTURE_K => {
            unsafe { kernel::set_fwstate(adapter, WIFI_STATION_STATE_K as c_int) };
            if ap2sta_mode {
                unsafe { kernel::init_bcmc_stainfo(adapter) };
            }
        }
        NDIS802_11_AP_MODE_K => {
            unsafe { kernel::set_fwstate(adapter, WIFI_AP_STATE_K as c_int) };
            unsafe { kernel::start_ap_mode(adapter) };
        }
        2 | 3 => {}
        #[cfg(config_rtw_mesh)]
        NDIS802_11_MESH_K => {
            unsafe { kernel::set_fwstate(adapter, WIFI_MESH_STATE_K) };
            unsafe { kernel::start_ap_mode(adapter) };
        }
        #[cfg(config_wifi_monitor)]
        NDIS802_11_MONITOR_K => unsafe { kernel::set_fwstate(adapter, WIFI_MONITOR_STATE_K) },
        _ => ret = _FALSE,
    }

    unsafe { kernel::exit_critical_bh(lock, &mut irqL) };
    ret
}

#[cfg(rust_ioctl_set_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_802_11_infrastructure_mode(
    adapter: *mut c_void,
    networktype: u32,
    flags: u8,
) -> u8 {
    unsafe { kernel_set_infra_mode(adapter, networktype, flags) }
}

#[cfg(rust_ioctl_set_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_802_11_bssid_list_scan(
    adapter: *mut c_void,
    pparm: *mut c_void,
) -> u8 {
    let mut irqL: c_ulong = 0;
    let lock = unsafe { kernel::mlme_lock_ptr(adapter) };
    unsafe {
        kernel::enter_critical_bh(lock, &mut irqL);
        let res = kernel::sitesurvey_cmd(adapter, pparm);
        kernel::exit_critical_bh(lock, &mut irqL);
        res
    }
}

#[cfg(rust_ioctl_set_leaf)]
const _SUCCESS: i32 = 1;
#[cfg(rust_ioctl_set_leaf)]
const _FAIL: i32 = 0;
#[cfg(rust_ioctl_set_leaf)]
const RTW_CMDF_WAIT_ACK: i32 = 2;

#[cfg(rust_ioctl_set_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_get_cur_max_rate_legacy_kernel(adapter: *mut c_void) -> u16 {
    unsafe { kernel::max_rate_legacy_kernel(adapter) }
}

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;

#[cfg(host_ioctl_validate_test)]
#[repr(C)]
pub struct Ndis80211Ssid {
    pub SsidLength: u32,
    pub Ssid: [u8; 32],
}

#[cfg(host_ioctl_validate_test)]
fn is_zero_mac_addr(addr: &[u8; 6]) -> bool {
    addr.iter().all(|&b| b == 0)
}

#[cfg(host_ioctl_validate_test)]
fn is_broadcast_mac_addr(addr: &[u8; 6]) -> bool {
    addr.iter().all(|&b| b == 0xff)
}

#[cfg(host_ioctl_validate_test)]
fn is_multicast_mac_addr(addr: &[u8; 6]) -> bool {
    (addr[0] & 0x01) == 0x01 && addr[0] != 0xff
}

#[cfg(host_ioctl_validate_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_validate_bssid_rust(bssid: *mut u8) -> u8 {
    if bssid.is_null() {
        return _FALSE;
    }
    let mac: &[u8; 6] = &*(bssid as *const [u8; 6]);
    if is_zero_mac_addr(mac) || is_broadcast_mac_addr(mac) || is_multicast_mac_addr(mac) {
        _FALSE
    } else {
        _TRUE
    }
}

#[cfg(host_ioctl_validate_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_validate_ssid_rust(ssid: *mut Ndis80211Ssid) -> u8 {
    if ssid.is_null() {
        return _FALSE;
    }
    let ssid = &*ssid;
    if ssid.SsidLength > 32 {
        _FALSE
    } else {
        _TRUE
    }
}

#[cfg(host_ioctl_scan_channel_test)]
const _SUCCESS: i32 = 1;
#[cfg(host_ioctl_scan_channel_test)]
const _FAIL: i32 = 0;
#[cfg(host_ioctl_scan_channel_test)]
const SCAN_PASSIVE: i32 = 0;
#[cfg(host_ioctl_scan_channel_test)]
const SCAN_ACTIVE: i32 = 1;
#[cfg(host_ioctl_scan_channel_test)]
const WIFI_FREQUENCY_BAND_2GHZ: u8 = 2;
#[cfg(host_ioctl_scan_channel_test)]
const DOT11_AUTH_ALGRTHM_8021X: u32 = 2;
#[cfg(host_ioctl_scan_channel_test)]
const _WEP40_: u32 = 0x01;
#[cfg(host_ioctl_scan_channel_test)]
const _WEP104_: u32 = 0x05;
#[cfg(host_ioctl_scan_channel_test)]
const _NO_PRIVACY_: u32 = 0x00;

#[cfg(host_ioctl_scan_channel_test)]
#[repr(C)]
pub struct MockMlmePriv {
    pub scan_mode: i32,
}

#[cfg(host_ioctl_scan_channel_test)]
#[repr(C)]
pub struct MockAdapter {
    pub mlmepriv: MockMlmePriv,
    pub setband: u8,
}

#[cfg(host_ioctl_scan_channel_test)]
#[repr(C)]
pub struct MockSecurityPriv {
    pub ndisauthtype: u32,
    pub dot11AuthAlgrthm: u32,
}

#[cfg(host_ioctl_scan_channel_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_scan_mode_rust(adapter: *mut MockAdapter, scan_mode: i32) -> i32 {
    if adapter.is_null() {
        return _FAIL;
    }
    let a = &mut *adapter;
    if scan_mode != SCAN_ACTIVE && scan_mode != SCAN_PASSIVE {
        return _FAIL;
    }
    a.mlmepriv.scan_mode = scan_mode;
    _SUCCESS
}

#[cfg(host_ioctl_scan_channel_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_band_rust(adapter: *mut MockAdapter, band: u8) -> i32 {
    if adapter.is_null() {
        return _FAIL;
    }
    let a = &mut *adapter;
    if band > WIFI_FREQUENCY_BAND_2GHZ {
        return _FAIL;
    }
    a.setband = band;
    _SUCCESS
}

#[cfg(host_ioctl_scan_channel_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_add_wep_privacy_rust(
    key_index: u32,
    key_length: u32,
    privacy_out: *mut u32,
) -> u8 {
    let keyid = key_index & 0x3fff_ffff;
    if keyid >= 4 {
        return _FALSE;
    }
    let privacy = match key_length {
        5 => _WEP40_,
        13 => _WEP104_,
        _ => _NO_PRIVACY_,
    };
    if !privacy_out.is_null() {
        *privacy_out = privacy;
    }
    _TRUE
}

#[cfg(host_ioctl_scan_channel_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_auth_mode_map_rust(
    sec: *mut MockSecurityPriv,
    authmode: u32,
    dot11_out: *mut u32,
) -> u8 {
    if sec.is_null() {
        return _FALSE;
    }
    let s = &mut *sec;
    s.ndisauthtype = authmode;
    if s.ndisauthtype > 3 {
        s.dot11AuthAlgrthm = DOT11_AUTH_ALGRTHM_8021X;
    }
    if !dot11_out.is_null() {
        *dot11_out = s.dot11AuthAlgrthm;
    }
    _TRUE
}

#[cfg(host_ioctl_connect_test)]
const WIFI_ASOC_STATE_K: u32 = 0x0000_0001;
#[cfg(host_ioctl_connect_test)]
const WIFI_UNDER_LINKING_K: u32 = 0x0000_0080;
#[cfg(host_ioctl_connect_test)]
const WIFI_UNDER_SURVEY_K: u32 = 0x0000_0800;

#[cfg(host_ioctl_connect_test)]
#[repr(C)]
#[derive(Copy, Clone)]
pub struct HostNdis80211Ssid {
    pub SsidLength: u32,
    pub Ssid: [u8; 32],
}

#[cfg(host_ioctl_connect_test)]
#[repr(C)]
pub struct HostMlmePriv {
    pub fw_state: u32,
    pub to_join: u8,
    pub assoc_by_bssid: u8,
    pub assoc_ch: u16,
    pub assoc_bssid: [u8; 6],
    pub assoc_ssid: HostNdis80211Ssid,
}

#[cfg(host_ioctl_connect_test)]
#[repr(C)]
pub struct HostAdapter {
    pub hw_init_done: u8,
    pub tkip_fail: u8,
    pub do_join_ret: u8,
    pub mlmepriv: HostMlmePriv,
}

#[cfg(host_ioctl_connect_test)]
static mut HOST_IOCTL_CONNECT_DISASSOC_CALLS: u32 = 0;
#[cfg(host_ioctl_connect_test)]
static mut HOST_IOCTL_CONNECT_JOIN_CALLS: u32 = 0;

#[cfg(host_ioctl_connect_test)]
#[no_mangle]
pub extern "C" fn ioctl_connect_test_reset_counters() {
    unsafe {
        HOST_IOCTL_CONNECT_DISASSOC_CALLS = 0;
        HOST_IOCTL_CONNECT_JOIN_CALLS = 0;
    }
}

#[cfg(host_ioctl_connect_test)]
#[no_mangle]
pub extern "C" fn ioctl_connect_test_disassoc_calls() -> u32 {
    unsafe { HOST_IOCTL_CONNECT_DISASSOC_CALLS }
}

#[cfg(host_ioctl_connect_test)]
#[no_mangle]
pub extern "C" fn ioctl_connect_test_join_calls() -> u32 {
    unsafe { HOST_IOCTL_CONNECT_JOIN_CALLS }
}

#[cfg(host_ioctl_connect_test)]
fn host_validate_bssid(bssid: &[u8; 6]) -> bool {
    let all_zero = bssid.iter().all(|&b| b == 0);
    let all_ff = bssid.iter().all(|&b| b == 0xff);
    if all_zero || all_ff {
        return false;
    }
    !((bssid[0] & 0x01) != 0 && bssid[0] != 0xff)
}

#[cfg(host_ioctl_connect_test)]
fn host_validate_ssid(ssid: &HostNdis80211Ssid) -> bool {
    ssid.SsidLength <= 32
}

#[cfg(host_ioctl_connect_test)]
fn host_chk_fw(mlme: &HostMlmePriv, st: i32) -> bool {
    st == 0 && mlme.fw_state == 0 || (mlme.fw_state & st as u32) != 0
}

#[cfg(host_ioctl_connect_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_802_11_disassociate_rust(p: *mut HostAdapter) -> u8 {
    if p.is_null() {
        return _FALSE;
    }
    let padapter = &mut *p;
    if host_chk_fw(&padapter.mlmepriv, WIFI_ASOC_STATE_K as i32) {
        unsafe {
            HOST_IOCTL_CONNECT_DISASSOC_CALLS += 1;
        }
    }
    _TRUE
}

#[cfg(host_ioctl_connect_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_802_11_connect_rust(
    p: *mut HostAdapter,
    bssid: *mut u8,
    ssid: *mut HostNdis80211Ssid,
    ch: u16,
) -> u8 {
    if p.is_null() {
        return _FAIL as u8;
    }
    let padapter = &mut *p;
    let mlme = &mut padapter.mlmepriv;

    let mut sv = true;
    let mut bv = true;
    if ssid.is_null() || !host_validate_ssid(&*ssid) {
        sv = false;
    }
    if bssid.is_null() || !host_validate_bssid(&*(bssid as *const [u8; 6])) {
        bv = false;
    }
    if !sv && !bv {
        return _FAIL as u8;
    }
    if padapter.hw_init_done == 0 {
        return _FAIL as u8;
    }

    if host_chk_fw(mlme, WIFI_UNDER_SURVEY_K as i32) {
        // handle_tkip_countermeasure — no-op in host oracle
    } else if host_chk_fw(mlme, WIFI_UNDER_LINKING_K as i32) {
        return _SUCCESS as u8;
    }

    if padapter.tkip_fail != 0 {
        return _FAIL as u8;
    }

    if !ssid.is_null() && sv {
        mlme.assoc_ssid = *ssid;
    } else {
        mlme.assoc_ssid = HostNdis80211Ssid {
            SsidLength: 0,
            Ssid: [0; 32],
        };
    }

    if !bssid.is_null() && bv {
        mlme.assoc_bssid
            .copy_from_slice(&*(bssid as *const [u8; 6]));
        mlme.assoc_by_bssid = _TRUE;
    } else {
        mlme.assoc_by_bssid = _FALSE;
    }
    mlme.assoc_ch = ch;

    if host_chk_fw(mlme, WIFI_UNDER_SURVEY_K as i32) {
        mlme.to_join = _TRUE;
        return _SUCCESS as u8;
    }

    unsafe {
        HOST_IOCTL_CONNECT_JOIN_CALLS += 1;
    }
    if padapter.do_join_ret != 0 {
        _SUCCESS as u8
    } else {
        _FAIL as u8
    }
}

#[cfg(host_ioctl_connect_test)]
fn host_is_bad_set_bssid(bssid: &[u8; 6]) -> bool {
    let all_zero = bssid.iter().all(|&b| b == 0);
    let all_ff = bssid.iter().all(|&b| b == 0xff);
    all_zero || all_ff
}

#[cfg(host_ioctl_connect_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_802_11_bssid_rust(p: *mut HostAdapter, bssid: *mut u8) -> u8 {
    if p.is_null() || bssid.is_null() {
        return _FAIL as u8;
    }
    let padapter = &mut *p;
    let mlme = &mut padapter.mlmepriv;
    let b = &*(bssid as *const [u8; 6]);

    if host_is_bad_set_bssid(b) {
        return _FAIL as u8;
    }

    if host_chk_fw(mlme, WIFI_UNDER_SURVEY_K as i32) {
        // handle_tkip_countermeasure
    } else if host_chk_fw(mlme, WIFI_UNDER_LINKING_K as i32) {
        return _SUCCESS as u8;
    }

    if padapter.tkip_fail != 0 {
        return _FAIL as u8;
    }

    mlme.assoc_ssid = HostNdis80211Ssid {
        SsidLength: 0,
        Ssid: [0; 32],
    };
    mlme.assoc_bssid.copy_from_slice(b);
    mlme.assoc_ch = 0;
    mlme.assoc_by_bssid = _TRUE;

    if host_chk_fw(mlme, WIFI_UNDER_SURVEY_K as i32) {
        mlme.to_join = _TRUE;
        return _SUCCESS as u8;
    }

    unsafe {
        HOST_IOCTL_CONNECT_JOIN_CALLS += 1;
    }
    if padapter.do_join_ret != 0 {
        _SUCCESS as u8
    } else {
        _FAIL as u8
    }
}

#[cfg(host_ioctl_connect_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_802_11_ssid_rust(
    p: *mut HostAdapter,
    ssid: *mut HostNdis80211Ssid,
) -> u8 {
    if p.is_null() {
        return _FAIL as u8;
    }
    let padapter = &mut *p;
    let mlme = &mut padapter.mlmepriv;

    if padapter.hw_init_done == 0 {
        return _FAIL as u8;
    }

    if host_chk_fw(mlme, WIFI_UNDER_SURVEY_K as i32) {
        // handle_tkip_countermeasure
    } else if host_chk_fw(mlme, WIFI_UNDER_LINKING_K as i32) {
        return _SUCCESS as u8;
    }

    if padapter.tkip_fail != 0 {
        return _FAIL as u8;
    }

    if ssid.is_null() || !host_validate_ssid(&*ssid) {
        return _FAIL as u8;
    }

    mlme.assoc_ssid = *ssid;
    mlme.assoc_ch = 0;
    mlme.assoc_by_bssid = _FALSE;

    if host_chk_fw(mlme, WIFI_UNDER_SURVEY_K as i32) {
        mlme.to_join = _TRUE;
        return _SUCCESS as u8;
    }

    unsafe {
        HOST_IOCTL_CONNECT_JOIN_CALLS += 1;
    }
    if padapter.do_join_ret != 0 {
        _SUCCESS as u8
    } else {
        _FAIL as u8
    }
}

#[cfg(host_ioctl_connect_test)]
const _SUCCESS: i32 = 1;
#[cfg(host_ioctl_connect_test)]
const _FAIL: i32 = 0;

#[cfg(host_ioctl_regd_test)]
const REGD_SRC_OS_K: u8 = 1;

#[cfg(host_ioctl_regd_test)]
#[repr(C)]
pub struct MockRegsty {
    pub regd_src: u8,
}

#[cfg(host_ioctl_regd_test)]
fn regd_from_os(reg: &MockRegsty, cfg: i32) -> bool {
    cfg != 0 && reg.regd_src == REGD_SRC_OS_K
}

#[cfg(host_ioctl_regd_test)]
#[no_mangle]
pub unsafe extern "C" fn ioctl_chplan_leaf_rust(
    reg: *mut MockRegsty,
    regd_cfg: i32,
    _chplan: u8,
    cmd_ret: i32,
    cmd_invoked: *mut i32,
) -> i32 {
    if reg.is_null() {
        return 0;
    }
    if !cmd_invoked.is_null() {
        *cmd_invoked = 0;
    }
    let reg = &*reg;
    if !regd_from_os(reg, regd_cfg) {
        if !cmd_invoked.is_null() {
            *cmd_invoked = 1;
        }
        return cmd_ret;
    }
    1
}

#[cfg(host_ioctl_regd_test)]
#[no_mangle]
pub unsafe extern "C" fn ioctl_country_leaf_rust(
    reg: *mut MockRegsty,
    regd_cfg: i32,
    country_enabled: i32,
    cmd_ret: i32,
    cmd_invoked: *mut i32,
) -> i32 {
    if !cmd_invoked.is_null() {
        *cmd_invoked = 0;
    }
    if country_enabled == 0 {
        return 1;
    }
    if reg.is_null() {
        return 0;
    }
    let reg = &*reg;
    if !regd_from_os(reg, regd_cfg) {
        if !cmd_invoked.is_null() {
            *cmd_invoked = 1;
        }
        return cmd_ret;
    }
    1
}

#[cfg(host_ioctl_bssid_scan_test)]
#[repr(C)]
pub struct HostSitesurveyParm {
    pub marker: i32,
}

#[cfg(host_ioctl_bssid_scan_test)]
#[repr(C)]
pub struct HostBssidScanMlme {
    pub lock_depth: i32,
}

#[cfg(host_ioctl_bssid_scan_test)]
#[repr(C)]
pub struct HostBssidScanAdapter {
    pub mlmepriv: HostBssidScanMlme,
    pub ss_cmd_ret: u8,
    pub ss_cmd_calls: i32,
    pub last_parm: *mut HostSitesurveyParm,
}

#[cfg(host_ioctl_infra_mode_test)]
const INFRA_NDIS_IBSS: u32 = 0;
#[cfg(host_ioctl_infra_mode_test)]
const INFRA_NDIS_INFRA: u32 = 1;
#[cfg(host_ioctl_infra_mode_test)]
const INFRA_NDIS_AP: u32 = 4;
#[cfg(host_ioctl_infra_mode_test)]
const INFRA_WIFI_ASOC: u32 = 0x0000_0001;
#[cfg(host_ioctl_infra_mode_test)]
const INFRA_WIFI_ADHOC_MASTER: u32 = 0x0000_0040;
#[cfg(host_ioctl_infra_mode_test)]
const INFRA_WIFI_ADHOC: u32 = 0x0000_0020;
#[cfg(host_ioctl_infra_mode_test)]
const INFRA_WIFI_STA: u32 = 0x0000_0008;
#[cfg(host_ioctl_infra_mode_test)]
const INFRA_WIFI_AP: u32 = 0x0000_0010;

#[cfg(host_ioctl_infra_mode_test)]
static mut HOST_INFRA_STOP_AP: u32 = 0;
#[cfg(host_ioctl_infra_mode_test)]
static mut HOST_INFRA_START_AP: u32 = 0;
#[cfg(host_ioctl_infra_mode_test)]
static mut HOST_INFRA_DISASSOC: u32 = 0;
#[cfg(host_ioctl_infra_mode_test)]
static mut HOST_INFRA_FREE_RES: u32 = 0;
#[cfg(host_ioctl_infra_mode_test)]
static mut HOST_INFRA_DISCONNECT: u32 = 0;
#[cfg(host_ioctl_infra_mode_test)]
static mut HOST_INFRA_INIT_BCMC: u32 = 0;

#[cfg(host_ioctl_infra_mode_test)]
#[no_mangle]
pub extern "C" fn ioctl_infra_mode_test_reset_counters() {
    unsafe {
        HOST_INFRA_STOP_AP = 0;
        HOST_INFRA_START_AP = 0;
        HOST_INFRA_DISASSOC = 0;
        HOST_INFRA_FREE_RES = 0;
        HOST_INFRA_DISCONNECT = 0;
        HOST_INFRA_INIT_BCMC = 0;
    }
}

#[cfg(host_ioctl_infra_mode_test)]
#[no_mangle]
pub extern "C" fn ioctl_infra_mode_test_stop_ap_calls() -> u32 {
    unsafe { HOST_INFRA_STOP_AP }
}
#[cfg(host_ioctl_infra_mode_test)]
#[no_mangle]
pub extern "C" fn ioctl_infra_mode_test_start_ap_calls() -> u32 {
    unsafe { HOST_INFRA_START_AP }
}
#[cfg(host_ioctl_infra_mode_test)]
#[no_mangle]
pub extern "C" fn ioctl_infra_mode_test_disassoc_calls() -> u32 {
    unsafe { HOST_INFRA_DISASSOC }
}
#[cfg(host_ioctl_infra_mode_test)]
#[no_mangle]
pub extern "C" fn ioctl_infra_mode_test_free_res_calls() -> u32 {
    unsafe { HOST_INFRA_FREE_RES }
}
#[cfg(host_ioctl_infra_mode_test)]
#[no_mangle]
pub extern "C" fn ioctl_infra_mode_test_disconnect_calls() -> u32 {
    unsafe { HOST_INFRA_DISCONNECT }
}
#[cfg(host_ioctl_infra_mode_test)]
#[no_mangle]
pub extern "C" fn ioctl_infra_mode_test_init_bcmc_calls() -> u32 {
    unsafe { HOST_INFRA_INIT_BCMC }
}

#[cfg(host_ioctl_infra_mode_test)]
#[repr(C)]
pub struct HostInfraNetwork {
    pub infrastructure_mode: u32,
}

#[cfg(host_ioctl_infra_mode_test)]
#[repr(C)]
pub struct HostInfraCurNetwork {
    pub network: HostInfraNetwork,
    pub join_res: i32,
}

#[cfg(host_ioctl_infra_mode_test)]
#[repr(C)]
pub struct HostInfraMlme {
    pub fw_state: u32,
    pub lock_depth: i32,
    pub cur_network: HostInfraCurNetwork,
}

#[cfg(host_ioctl_infra_mode_test)]
#[repr(C)]
pub struct HostInfraAdapter {
    pub mlmepriv: HostInfraMlme,
}

#[cfg(host_ioctl_infra_mode_test)]
fn host_infra_chk(fw: u32, bit: u32) -> bool {
    (fw & bit) != 0
}

#[cfg(host_ioctl_infra_mode_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_802_11_infrastructure_mode_rust(
    padapter: *mut HostInfraAdapter,
    networktype: u32,
    flags: u8,
) -> u8 {
    if padapter.is_null() {
        return _FALSE;
    }
    let a = &mut *padapter;
    let pold = a.mlmepriv.cur_network.network.infrastructure_mode;
    if pold == networktype {
        return _TRUE;
    }
    let mut ap2sta = false;
    let mut ret = _TRUE;
    if pold == INFRA_NDIS_AP {
        a.mlmepriv.cur_network.join_res = -1;
        ap2sta = true;
        unsafe { HOST_INFRA_STOP_AP += 1 };
    }

    a.mlmepriv.lock_depth += 1;
    let is_linked = host_infra_chk(a.mlmepriv.fw_state, INFRA_WIFI_ASOC);
    let is_adhoc_master = host_infra_chk(a.mlmepriv.fw_state, INFRA_WIFI_ADHOC_MASTER);
    if flags != 0 {
        a.mlmepriv.lock_depth -= 1;
    }

    if is_linked || pold == INFRA_NDIS_IBSS {
        unsafe { HOST_INFRA_DISASSOC += 1 };
    }
    if is_linked || is_adhoc_master {
        unsafe { HOST_INFRA_FREE_RES += 1 };
    }
    if (pold == INFRA_NDIS_INFRA || pold == INFRA_NDIS_IBSS) && is_linked {
        unsafe { HOST_INFRA_DISCONNECT += 1 };
    }

    if flags != 0 {
        a.mlmepriv.lock_depth += 1;
    }

    a.mlmepriv.cur_network.network.infrastructure_mode = networktype;
    a.mlmepriv.fw_state = 0;

    match networktype {
        INFRA_NDIS_IBSS => a.mlmepriv.fw_state |= INFRA_WIFI_ADHOC,
        INFRA_NDIS_INFRA => {
            a.mlmepriv.fw_state |= INFRA_WIFI_STA;
            if ap2sta {
                unsafe { HOST_INFRA_INIT_BCMC += 1 };
            }
        }
        INFRA_NDIS_AP => {
            a.mlmepriv.fw_state |= INFRA_WIFI_AP;
            unsafe { HOST_INFRA_START_AP += 1 };
        }
        2 | 3 => {}
        _ => ret = _FALSE,
    }

    a.mlmepriv.lock_depth -= 1;
    ret
}

#[cfg(host_ioctl_bssid_scan_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_802_11_bssid_list_scan_rust(
    padapter: *mut HostBssidScanAdapter,
    pparm: *mut HostSitesurveyParm,
) -> u8 {
    if padapter.is_null() {
        return 0;
    }
    let a = &mut *padapter;
    a.mlmepriv.lock_depth += 1;
    a.ss_cmd_calls += 1;
    a.last_parm = pparm;
    let res = a.ss_cmd_ret;
    a.mlmepriv.lock_depth -= 1;
    res
}

#[cfg(host_ioctl_max_rate_test)]
const WIFI_ASOC_STATE_MR: u32 = 1 << 0;
#[cfg(host_ioctl_max_rate_test)]
const WIFI_ADHOC_MASTER_STATE_MR: u32 = 1 << 1;

#[cfg(host_ioctl_max_rate_test)]
fn host_mr_chk_fw(fw_state: u32, bit: u32) -> bool {
    (fw_state & bit) != 0
}

#[cfg(host_ioctl_max_rate_test)]
fn host_mr_calc_legacy(
    has_sta: i32,
    sta_mode: i32,
    ap_rates: &[u8; max_rate_legacy::NUM_RATES],
    sta_rates: &[u8],
    sta_rate_len: i32,
) -> u16 {
    use max_rate_legacy::MaxRateLegacyIn;
    let mut sta_arr = [0u8; max_rate_legacy::NUM_RATES];
    let n = sta_rate_len.max(0) as usize;
    for (i, b) in sta_rates.iter().take(n).enumerate() {
        sta_arr[i] = *b;
    }
    let in_ = MaxRateLegacyIn {
        fw_state: 0,
        has_sta: if has_sta != 0 { 1 } else { 0 },
        sta_mode: if sta_mode != 0 { 1 } else { 0 },
        ap_rates: *ap_rates,
        sta_rates: sta_arr,
        sta_rate_len: sta_rate_len.max(0) as u8,
    };
    max_rate_legacy::calc(&in_)
}

#[cfg(host_ioctl_max_rate_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_get_cur_max_rate_legacy_rust(
    fw_state: u32,
    has_sta: i32,
    sta_mode: i32,
    ap_rates: *const u8,
    sta_rates: *const u8,
    sta_rate_len: i32,
) -> u16 {
    if ap_rates.is_null() {
        return 0;
    }
    if !host_mr_chk_fw(fw_state, WIFI_ASOC_STATE_MR)
        && !host_mr_chk_fw(fw_state, WIFI_ADHOC_MASTER_STATE_MR)
    {
        return 0;
    }
    if has_sta == 0 {
        return 0;
    }
    let ap_arr: [u8; max_rate_legacy::NUM_RATES] = {
        let s = core::slice::from_raw_parts(ap_rates, max_rate_legacy::NUM_RATES);
        let mut a = [0u8; max_rate_legacy::NUM_RATES];
        a.copy_from_slice(s);
        a
    };
    let sta = if sta_rates.is_null() {
        &[] as &[u8]
    } else {
        core::slice::from_raw_parts(sta_rates, sta_rate_len.max(0) as usize)
    };
    host_mr_calc_legacy(has_sta, sta_mode, &ap_arr, sta, sta_rate_len)
}

#[cfg(host_ioctl_do_join_test)]
const DO_JOIN_WIFI_ADHOC_STATE: u32 = 0x0000_0020;
#[cfg(host_ioctl_do_join_test)]
const DO_JOIN_WIFI_ADHOC_MASTER_STATE: u32 = 0x0000_0040;
#[cfg(host_ioctl_do_join_test)]
const DO_JOIN_WIFI_UNDER_LINKING: u32 = 0x0000_0080;
#[cfg(host_ioctl_do_join_test)]
const DO_JOIN_SS_DENY_BUSY_TRAFFIC: u8 = 12;
#[cfg(host_ioctl_do_join_test)]
const DO_JOIN_SS_ALLOW: u8 = 13;
#[cfg(host_ioctl_do_join_test)]
const DO_JOIN_MAX_JOIN_TIMEOUT: u32 = 6500;

#[cfg(host_ioctl_do_join_test)]
#[repr(C)]
pub struct HostDoJoinSsid {
    pub SsidLength: u32,
    pub Ssid: [u8; 32],
}

#[cfg(host_ioctl_do_join_test)]
#[repr(C)]
pub struct HostDoJoinLinkDetect {
    pub bBusyTraffic: u8,
}

#[cfg(host_ioctl_do_join_test)]
#[repr(C)]
pub struct HostDoJoinMlme {
    pub fw_state: u32,
    pub to_join: u8,
    pub assoc_ch: u16,
    pub assoc_ssid: HostDoJoinSsid,
    pub join_res: i8,
    pub scanned_lock_depth: i32,
    pub queue_empty: u8,
    pub LinkDetectInfo: HostDoJoinLinkDetect,
    pub assoc_timer_ms: u32,
}

#[cfg(host_ioctl_do_join_test)]
#[repr(C)]
pub struct HostDoJoinAdapter {
    pub mlmepriv: HostDoJoinMlme,
    pub to_roam: i8,
    pub ssc_chk: u8,
    pub sitesurvey_ret: u8,
    pub select_ret: i8,
    pub create_ibss_ret: u8,
    pub ss_calls: u32,
    pub select_calls: u32,
    pub create_ibss_calls: u32,
}

#[cfg(host_ioctl_do_join_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_do_join_rust(p: *mut HostDoJoinAdapter) -> u8 {
    if p.is_null() {
        return _FALSE;
    }
    let a = &mut *p;
    let m = &mut a.mlmepriv;

    m.scanned_lock_depth += 1;
    m.join_res = -2;
    m.fw_state |= DO_JOIN_WIFI_UNDER_LINKING;
    m.to_join = _TRUE;

    if m.queue_empty != 0 {
        m.scanned_lock_depth -= 1;
        m.fw_state &= !DO_JOIN_WIFI_UNDER_LINKING;
        if m.LinkDetectInfo.bBusyTraffic == 0 || a.to_roam > 0 {
            if a.ssc_chk == DO_JOIN_SS_ALLOW || a.ssc_chk == DO_JOIN_SS_DENY_BUSY_TRAFFIC {
                a.ss_calls += 1;
                let ret = a.sitesurvey_ret;
                if ret != _TRUE {
                    m.to_join = _FALSE;
                }
                return ret;
            }
            m.to_join = _FALSE;
            return _FALSE;
        }
        m.to_join = _FALSE;
        return _FALSE;
    }

    m.scanned_lock_depth -= 1;
    a.select_calls += 1;
    if a.select_ret == 1 {
        m.to_join = _FALSE;
        m.assoc_timer_ms = DO_JOIN_MAX_JOIN_TIMEOUT;
        return _TRUE;
    }

    if (m.fw_state & DO_JOIN_WIFI_ADHOC_STATE) != 0 {
        m.fw_state = DO_JOIN_WIFI_ADHOC_MASTER_STATE;
        a.create_ibss_calls += 1;
        if a.create_ibss_ret != _TRUE {
            return _FALSE;
        }
        m.to_join = _FALSE;
        return _TRUE;
    }

    m.fw_state &= !DO_JOIN_WIFI_UNDER_LINKING;
    if m.LinkDetectInfo.bBusyTraffic == 0 || a.to_roam > 0 {
        if a.ssc_chk == DO_JOIN_SS_ALLOW || a.ssc_chk == DO_JOIN_SS_DENY_BUSY_TRAFFIC {
            a.ss_calls += 1;
            let ret = a.sitesurvey_ret;
            if ret != _TRUE {
                m.to_join = _FALSE;
            }
            return ret;
        }
        m.to_join = _FALSE;
        return _FALSE;
    }
    m.to_join = _FALSE;
    _FALSE
}

#[cfg(any(host_ioctl_acs_test, all(rust_ioctl_set_leaf, config_rtw_acs)))]
mod acs_sitesurvey {
    pub const SCAN_PASSIVE: i32 = 0;
    pub const CHANNEL_WIDTH_20: u8 = 0;
    pub const CHAN_PASSIVE_SCAN: u32 = 2;

    pub fn is_2g_ch(ch: u8) -> bool {
        ch >= 1 && ch <= 14
    }

    #[cfg(ieee80211_band_5ghz)]
    pub fn is_5g_ch(ch: u8) -> bool {
        ch >= 36 && ch <= 177
    }

    #[cfg(host_ioctl_acs_test)]
    const HOST_ACS_2G: [u8; 3] = [1, 6, 11];
    #[cfg(all(host_ioctl_acs_test, ieee80211_band_5ghz))]
    const HOST_ACS_5G: [u8; 3] = [36, 40, 44];

    #[cfg(host_ioctl_acs_test)]
    fn host_center_2g_num(_bw: u8) -> u8 {
        HOST_ACS_2G.len() as u8
    }

    #[cfg(host_ioctl_acs_test)]
    fn host_center_2g(_bw: u8, id: u8) -> u8 {
        HOST_ACS_2G.get(id as usize).copied().unwrap_or(0)
    }

    #[cfg(all(host_ioctl_acs_test, ieee80211_band_5ghz))]
    fn host_center_5g_num(_bw: u8) -> u8 {
        HOST_ACS_5G.len() as u8
    }

    #[cfg(all(host_ioctl_acs_test, ieee80211_band_5ghz))]
    fn host_center_5g(_bw: u8, id: u8) -> u8 {
        HOST_ACS_5G.get(id as usize).copied().unwrap_or(0)
    }

    #[cfg(host_ioctl_acs_test)]
    pub fn host_add_band(
        union_ok_uch: u8,
        ch_sel_same: bool,
        band_is_2g: bool,
        center_num: fn(u8) -> u8,
        center_at: fn(u8, u8) -> u8,
        parm: &mut HostAcsParmFill,
    ) {
        if ch_sel_same {
            if is_2g_ch(union_ok_uch) && !band_is_2g {
                return;
            }
            #[cfg(ieee80211_band_5ghz)]
            if is_5g_ch(union_ok_uch) && band_is_2g {
                return;
            }
        }
        let ch_num = center_num(CHANNEL_WIDTH_20);
        for i in 0..ch_num {
            if parm.ch_num as usize >= parm.ch_cap {
                break;
            }
            let idx = parm.ch_num as usize;
            parm.ch_hw[idx] = center_at(CHANNEL_WIDTH_20, i);
            parm.ch_flags[idx] = CHAN_PASSIVE_SCAN;
            parm.ch_num += 1;
        }
    }

    #[cfg(host_ioctl_acs_test)]
    pub struct HostAcsParmFill {
        pub scan_mode: i32,
        pub ch_num: u8,
        pub bw: u8,
        pub acs: i32,
        pub ch_hw: [u8; 8],
        pub ch_flags: [u32; 8],
        pub ch_cap: usize,
    }

    #[cfg(host_ioctl_acs_test)]
    pub fn host_fill(uch: u8, ch_sel_same: bool, out: &mut HostAcsParmFill) {
        out.scan_mode = SCAN_PASSIVE;
        out.bw = CHANNEL_WIDTH_20;
        out.acs = 1;
        out.ch_num = 0;
        host_add_band(
            uch,
            ch_sel_same,
            true,
            host_center_2g_num,
            host_center_2g,
            out,
        );
        #[cfg(ieee80211_band_5ghz)]
        host_add_band(
            uch,
            ch_sel_same,
            false,
            host_center_5g_num,
            host_center_5g,
            out,
        );
    }

    #[cfg(all(rust_ioctl_set_leaf, config_rtw_acs))]
    const RTW_CHANNEL_SCAN_AMOUNT_K: usize = 51;

    #[cfg(all(rust_ioctl_set_leaf, config_rtw_acs))]
    #[repr(C)]
    pub struct KernelIeeeChannel {
        pub hw_value: u16,
        pub flags: u32,
    }

    #[cfg(all(rust_ioctl_set_leaf, config_rtw_acs))]
    #[repr(C)]
    pub struct KernelNdisSsid {
        pub ssid_length: u32,
        pub ssid: [u8; 32],
    }

    #[cfg(all(rust_ioctl_set_leaf, config_rtw_acs))]
    use core::ffi::c_int;

    #[cfg(all(rust_ioctl_set_leaf, config_rtw_acs))]
    #[repr(C)]
    pub struct KernelSitesurveyParm {
        pub scan_mode: c_int,
        pub ssid_num: u8,
        pub ch_num: u8,
        pub ssid: [KernelNdisSsid; 9],
        pub ch: [KernelIeeeChannel; RTW_CHANNEL_SCAN_AMOUNT_K],
        pub token: u32,
        pub duration: u16,
        pub igi: u8,
        pub bw: u8,
        pub acs: u8,
        pub reason: u8,
    }

    #[cfg(all(rust_ioctl_set_leaf, config_rtw_acs))]
    pub fn kernel_add_band(
        uch: u8,
        ch_sel_same: bool,
        band_is_2g: bool,
        center_num: unsafe extern "C" fn(u8) -> u8,
        center_at: unsafe extern "C" fn(u8, u8) -> u8,
        parm: &mut KernelSitesurveyParm,
    ) {
        if ch_sel_same {
            if is_2g_ch(uch) && !band_is_2g {
                return;
            }
            #[cfg(ieee80211_band_5ghz)]
            if is_5g_ch(uch) && band_is_2g {
                return;
            }
        }
        let ch_num = unsafe { center_num(CHANNEL_WIDTH_20) };
        for i in 0..ch_num {
            if parm.ch_num as usize >= RTW_CHANNEL_SCAN_AMOUNT_K {
                break;
            }
            let idx = parm.ch_num as usize;
            parm.ch[idx].hw_value = unsafe { center_at(CHANNEL_WIDTH_20, i) } as u16;
            parm.ch[idx].flags = CHAN_PASSIVE_SCAN;
            parm.ch_num += 1;
        }
    }

    #[cfg(all(rust_ioctl_set_leaf, config_rtw_acs))]
    pub unsafe fn kernel_fill(uch: u8, ch_sel_same: bool, parm: &mut KernelSitesurveyParm) {
        extern "C" {
            fn center_chs_2g_num(bw: u8) -> u8;
            fn center_chs_2g(bw: u8, id: u8) -> u8;
            #[cfg(ieee80211_band_5ghz)]
            fn center_chs_5g_num(bw: u8) -> u8;
            #[cfg(ieee80211_band_5ghz)]
            fn center_chs_5g(bw: u8, id: u8) -> u8;
        }
        parm.scan_mode = SCAN_PASSIVE;
        parm.bw = CHANNEL_WIDTH_20;
        parm.acs = 1;
        parm.ch_num = 0;
        kernel_add_band(
            uch,
            ch_sel_same,
            true,
            center_chs_2g_num,
            center_chs_2g,
            parm,
        );
        #[cfg(ieee80211_band_5ghz)]
        kernel_add_band(
            uch,
            ch_sel_same,
            false,
            center_chs_5g_num,
            center_chs_5g,
            parm,
        );
    }
}

#[cfg(all(rust_ioctl_set_leaf, config_rtw_acs))]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_acs_sitesurvey(adapter: *mut c_void) -> u8 {
    extern "C" {
        fn rtw_rust_ioctl_acs_ch_union(adapter: *mut c_void, uch: *mut u8) -> c_int;
        fn rtw_rust_ioctl_acs_ch_sel_same_band(adapter: *mut c_void) -> u8;
    }
    let mut uch = 0u8;
    if unsafe { rtw_rust_ioctl_acs_ch_union(adapter, &mut uch) } == 0 {
        return _FALSE;
    }
    let ch_sel = unsafe { rtw_rust_ioctl_acs_ch_sel_same_band(adapter) } != 0;
    let mut parm: acs_sitesurvey::KernelSitesurveyParm = unsafe { core::mem::zeroed() };
    unsafe { acs_sitesurvey::kernel_fill(uch, ch_sel, &mut parm) };
    unsafe { rtw_set_802_11_bssid_list_scan(adapter, &mut parm as *mut _ as *mut c_void) }
}

#[cfg(host_ioctl_acs_test)]
#[repr(C)]
pub struct HostAcsChannel {
    pub hw_value: u8,
    pub flags: u32,
}

#[cfg(host_ioctl_acs_test)]
#[repr(C)]
pub struct HostAcsSitesurveyParm {
    pub scan_mode: i32,
    pub ch_num: u8,
    pub bw: u8,
    pub acs: i32,
    pub ch: [HostAcsChannel; 8],
}

#[cfg(host_ioctl_acs_test)]
#[repr(C)]
pub struct HostAcsAdapter {
    pub union_ok: u8,
    pub uch: u8,
    pub ch_sel_within_same_band: u8,
    pub scan_ret: u8,
    pub scan_calls: i32,
    pub last_parm: HostAcsSitesurveyParm,
}

#[cfg(host_ioctl_acs_test)]
#[no_mangle]
pub unsafe extern "C" fn rtw_set_acs_sitesurvey_rust(a: *mut HostAcsAdapter) -> u8 {
    if a.is_null() {
        return 0;
    }
    let ad = &mut *a;
    if ad.union_ok == 0 {
        return 0;
    }
    let mut fill = acs_sitesurvey::HostAcsParmFill {
        scan_mode: 0,
        ch_num: 0,
        bw: 0,
        acs: 0,
        ch_hw: [0; 8],
        ch_flags: [0; 8],
        ch_cap: 8,
    };
    acs_sitesurvey::host_fill(ad.uch, ad.ch_sel_within_same_band != 0, &mut fill);
    ad.scan_calls += 1;
    ad.last_parm.scan_mode = fill.scan_mode;
    ad.last_parm.ch_num = fill.ch_num;
    ad.last_parm.bw = fill.bw;
    ad.last_parm.acs = fill.acs;
    for i in 0..fill.ch_num as usize {
        ad.last_parm.ch[i].hw_value = fill.ch_hw[i];
        ad.last_parm.ch[i].flags = fill.ch_flags[i];
    }
    ad.scan_ret
}
