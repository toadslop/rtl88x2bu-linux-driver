// SPDX-License-Identifier: GPL-2.0
//! W3-88 join_cmd_hdl — host L2 oracle.

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    private_interfaces,
    missing_docs
)]

use std::os::raw::{c_int, c_void};

type U8 = u8;
type U32 = u32;
const _TRUE: c_int = 1;
const _FALSE: c_int = 0;
const _FAIL: c_int = 0;
const MAX_IE_SZ: usize = 768;

const H2C_SUCCESS: U8 = 0x00;
const H2C_PARAMETERS_ERROR: U8 = 0x04;
const WIFI_FW_NULL_STATE: U32 = 0;
const WIFI_FW_STATION_STATE: U32 = 0x02;
const WIFI_FW_ASSOC_SUCCESS: U32 = 0x00004000;
const WIFI_STATION_STATE: U32 = 0x00000001;
const _HW_STATE_STATION_: U8 = 0x02;
const _FIXED_IE_LENGTH_: U32 = 12;
const _VENDOR_SPECIFIC_IE_: U8 = 221;
const _HT_CAPABILITY_IE_: U8 = 45;
const _HT_EXTRA_INFO_IE_: U8 = 61;
const EID_VHTCapability: U8 = 191;
const WLAN_REASON_DEAUTH_LEAVING: u16 = 3;
const WLAN_STATUS_UNSPECIFIED_FAILURE: u16 = 1;
const MLME_STA_CONNECTING: c_int = 1;
const MLME_ADHOC_STARTED: c_int = 3;

const HW_VAR_BSSID: c_int = 0;
const HW_VAR_MLME_DISCONNECT: c_int = 1;
const HW_VAR_MLME_JOIN: c_int = 2;
const HW_VAR_DO_IQK: c_int = 3;

const WLAN_BSSID_IE_LENGTH_OFF: usize = 136;
const WLAN_BSSID_IES_OFF: usize = 140;

#[repr(C)]
struct MlmeExtInfo {
    state: U32,
    bcn_interval: u16,
    wmm_enable: U8,
    erp_enable: U8,
    ht_enable: U8,
    ht_caps_enable: U8,
    ht_info_enable: U8,
    agg_enable_bitmap: U8,
    candidate_tid_bitmap: U8,
    bwmode_updated: U8,
    vht_enable: U8,
    network: [U8; MAX_IE_SZ + 256],
}

#[repr(C)]
struct Timer {
    cancelled: c_int,
}

#[repr(C)]
struct MlmeExtPriv {
    mlmext_info: MlmeExtInfo,
    link_timer: Timer,
    cur_channel: U8,
    cur_bwmode: U8,
    cur_ch_offset: U8,
}

#[repr(C)]
struct MlmePriv {
    fw_state: U32,
}

#[repr(C)]
struct Adapter {
    mlmeextpriv: MlmeExtPriv,
    mlmepriv: MlmePriv,
}

#[no_mangle]
pub extern "C" fn _rtw_memcpy(dest: *mut c_void, src: *const c_void, n: usize) -> *mut c_void {
    if !dest.is_null() && !src.is_null() && n > 0 {
        unsafe {
            std::ptr::copy_nonoverlapping(src as *const u8, dest as *mut u8, n);
        }
    }
    dest
}

#[no_mangle]
pub extern "C" fn _rtw_memcmp(a: *const c_void, b: *const c_void, n: usize) -> c_int {
    if a.is_null() || b.is_null() {
        return _FALSE;
    }
    unsafe {
        if std::slice::from_raw_parts(a as *const u8, n)
            == std::slice::from_raw_parts(b as *const u8, n)
        {
            _TRUE
        } else {
            _FALSE
        }
    }
}

extern "C" {
    static mut WMM_OUI: [U8; 4];
    fn issue_deauth_ex(a: *mut Adapter, addr: *mut U8, reason: u16, try_cnt: U8, try_ms: c_int);
    fn flush_all_cam_entry(a: *mut Adapter);
    fn _cancel_timer_ex(t: *mut Timer);
    fn Set_MSR(a: *mut Adapter, ty: U8);
    fn rtw_hal_set_hwreg(a: *mut Adapter, id: c_int, val: *mut U8);
    fn rtw_hal_rcr_set_chk_bssid(a: *mut Adapter, mode: c_int);
    fn rtw_joinbss_reset(a: *mut Adapter);
    fn report_join_res(a: *mut Adapter, aid_res: c_int, status: u16) -> U32;
    fn get_beacon_interval(bss: *mut U8) -> u16;
    fn WMM_param_handler(a: *mut Adapter, pie: *mut U8) -> c_int;
    fn rtw_bss_get_chbw(bss: *mut U8, ch: *mut U8, bw: *mut U8, offset: *mut U8, ht: U8, vht: U8);
    fn rtw_adjust_chbw(a: *mut Adapter, ch: U8, bw: *mut U8, offset: *mut U8);
    fn rtw_chk_start_clnt_join(a: *mut Adapter, ch: *mut U8, bw: *mut U8, offset: *mut U8)
        -> c_int;
    fn rtw_btcoex_connect_notify(a: *mut Adapter, join_type: U8);
    fn set_channel_bwmode(a: *mut Adapter, ch: U8, offset: U8, bw: U8);
    fn start_clnt_join(a: *mut Adapter);
}

fn mlme_is_sta(adapter: *mut Adapter) -> bool {
    unsafe { ((*adapter).mlmepriv.fw_state & WIFI_STATION_STATE) != 0 }
}

#[no_mangle]
pub extern "C" fn join_cmd_hdl(padapter: *mut Adapter, pbuf: *mut U8) -> U8 {
    if padapter.is_null() || pbuf.is_null() {
        return H2C_PARAMETERS_ERROR;
    }
    unsafe {
        let pmlmeext = &mut (*padapter).mlmeextpriv;
        let pmlmeinfo = &mut pmlmeext.mlmext_info;
        let pnetwork = pmlmeinfo.network.as_mut_ptr();
        let mut i: U32;
        let mut u_ch: U8 = 0;
        let mut u_bw: U8 = 0;
        let mut u_offset: U8 = 0;
        let mut doiqk: U8 = _FALSE as U8;
        let mut join_type: U8 = 0;

        if (pmlmeinfo.state & WIFI_FW_ASSOC_SUCCESS) != 0 {
            if (pmlmeinfo.state & WIFI_FW_STATION_STATE) != 0 {
                issue_deauth_ex(padapter, pnetwork, WLAN_REASON_DEAUTH_LEAVING, 1, 100);
            }
            pmlmeinfo.state = WIFI_FW_NULL_STATE;
            flush_all_cam_entry(padapter);
            _cancel_timer_ex(&mut pmlmeext.link_timer);
            Set_MSR(padapter, _HW_STATE_STATION_);
            rtw_hal_set_hwreg(padapter, HW_VAR_MLME_DISCONNECT, std::ptr::null_mut());
        }

        rtw_joinbss_reset(padapter);
        pmlmeinfo.erp_enable = 0;
        pmlmeinfo.wmm_enable = 0;
        pmlmeinfo.ht_enable = 0;
        pmlmeinfo.ht_caps_enable = 0;
        pmlmeinfo.ht_info_enable = 0;
        pmlmeinfo.agg_enable_bitmap = 0;
        pmlmeinfo.candidate_tid_bitmap = 0;
        pmlmeinfo.bwmode_updated = _FALSE as U8;
        pmlmeinfo.vht_enable = 0;

        _rtw_memcpy(
            pnetwork as *mut c_void,
            pbuf as *const c_void,
            WLAN_BSSID_IE_LENGTH_OFF,
        );
        let ie_length = *(pbuf.add(WLAN_BSSID_IE_LENGTH_OFF) as *const U32);

        if ie_length > MAX_IE_SZ as U32 {
            return H2C_PARAMETERS_ERROR;
        }
        if ie_length < 2 {
            report_join_res(padapter, -4, WLAN_STATUS_UNSPECIFIED_FAILURE);
            return H2C_SUCCESS;
        }
        _rtw_memcpy(
            pnetwork.add(WLAN_BSSID_IES_OFF) as *mut c_void,
            pbuf.add(WLAN_BSSID_IES_OFF) as *const c_void,
            ie_length as usize,
        );

        pmlmeinfo.bcn_interval = get_beacon_interval(pnetwork);

        i = _FIXED_IE_LENGTH_;
        while i < ie_length - 2 {
            let pie = pnetwork.add(WLAN_BSSID_IES_OFF + i as usize);
            let eid = *pie;
            let len = *pie.add(1);
            match eid {
                _VENDOR_SPECIFIC_IE_ => {
                    if _rtw_memcmp(
                        pie.add(2) as *const c_void,
                        WMM_OUI.as_ptr() as *const c_void,
                        4,
                    ) != 0
                    {
                        WMM_param_handler(padapter, pie as *mut U8);
                    }
                }
                _HT_CAPABILITY_IE_ => pmlmeinfo.ht_caps_enable = 1,
                _HT_EXTRA_INFO_IE_ => pmlmeinfo.ht_info_enable = 1,
                EID_VHTCapability => pmlmeinfo.vht_enable = 1,
                _ => {}
            }
            i += (len as U32) + 2;
        }

        rtw_bss_get_chbw(
            pnetwork,
            &mut pmlmeext.cur_channel,
            &mut pmlmeext.cur_bwmode,
            &mut pmlmeext.cur_ch_offset,
            1,
            1,
        );
        rtw_adjust_chbw(
            padapter,
            pmlmeext.cur_channel,
            &mut pmlmeext.cur_bwmode,
            &mut pmlmeext.cur_ch_offset,
        );

        if rtw_chk_start_clnt_join(padapter, &mut u_ch, &mut u_bw, &mut u_offset) == _FAIL {
            report_join_res(padapter, -4, WLAN_STATUS_UNSPECIFIED_FAILURE);
            return H2C_SUCCESS;
        }

        rtw_hal_set_hwreg(padapter, HW_VAR_BSSID, pnetwork);
        if mlme_is_sta(padapter) {
            rtw_hal_rcr_set_chk_bssid(padapter, MLME_STA_CONNECTING);
        } else {
            rtw_hal_rcr_set_chk_bssid(padapter, MLME_ADHOC_STARTED);
        }

        join_type = 0;
        rtw_hal_set_hwreg(padapter, HW_VAR_MLME_JOIN, &mut join_type);
        rtw_btcoex_connect_notify(padapter, join_type);

        doiqk = _TRUE as U8;
        rtw_hal_set_hwreg(padapter, HW_VAR_DO_IQK, &mut doiqk);
        set_channel_bwmode(padapter, u_ch, u_offset, u_bw);
        doiqk = _FALSE as U8;
        rtw_hal_set_hwreg(padapter, HW_VAR_DO_IQK, &mut doiqk);

        _cancel_timer_ex(&mut pmlmeext.link_timer);
        start_clnt_join(padapter);

        H2C_SUCCESS
    }
}
