// SPDX-License-Identifier: GPL-2.0
//! W3-71 sitesurvey_pick_ch_behavior — host L2 oracle and kernel port.

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    unreachable_pub,
    missing_docs
)]

#[cfg(host_mlme_ext_pick_ch_test)]
use std::os::raw::{c_int, c_ulong};

type U8 = u8;
type U32 = u32;
type Systime = c_ulong;
type Adapter = *mut core::ffi::c_void;

const _TRUE: c_int = 1;
const _FALSE: c_int = 0;

const SCAN_PASSIVE: c_int = 0;
const SCAN_ACTIVE: c_int = 1;
const SCAN_PROCESS: U8 = 4;
const SCAN_BACKING_OP: U8 = 5;
const SCAN_TO_P2P_LISTEN: U8 = 10;
const SCAN_COMPLETE: U8 = 12;

const RTW_IEEE80211_CHAN_PASSIVE_SCAN: U32 = 1 << 1;
const RTW_CHF_NO_IR: U8 = 1 << 0;
const SS_BACKOP_EN: U8 = 1 << 0;
const SS_BACKOP_EN_NL: U8 = 1 << 1;
const RTW_CHANNEL_SCAN_AMOUNT: usize = 8;
const MAX_CHANNEL_NUM: usize = 59;

#[repr(C)]
pub struct Ieee80211Channel {
    pub hw_value: u16,
    pub flags: U32,
}

#[repr(C)]
pub struct RtChannelInfo {
    pub channel_num: U8,
    pub flags: U8,
    pub _pad0: [U8; 6],
    pub non_ocp_end_time: Systime,
    pub hidden_bss_cnt: U8,
    pub _pad1: [U8; 7],
    pub os_chan: *mut core::ffi::c_void,
}

#[repr(C)]
pub struct SsRes {
    pub scan_ch_ms: u16,
    pub scan_timeout_ms: U32,
    pub duration: u16,
    pub channel_idx: c_int,
    pub force_ssid_scan: U8,
    pub ssid_num: U8,
    pub ch_num: U8,
    pub ch: [Ieee80211Channel; RTW_CHANNEL_SCAN_AMOUNT],
    pub backop_flags_sta: U8,
    pub backop_flags_ap: U8,
    pub backop_flags_mesh: U8,
    pub backop_flags: U8,
    pub scan_cnt: U8,
    pub scan_cnt_max: U8,
    pub backop_ms: u16,
}

#[repr(C)]
pub struct RegistryPriv {
    pub wireless_mode: U32,
}

#[repr(C)]
pub struct OpChInfo {
    pub scan_op_ch_only: U8,
    pub operation_ch: [U8; RTW_CHANNEL_SCAN_AMOUNT],
}

#[repr(C)]
pub struct WifiDirectInfo {
    pub rx_invitereq_info: OpChInfo,
    pub p2p_info: OpChInfo,
    pub social_chan: [U8; RTW_CHANNEL_SCAN_AMOUNT],
    pub find_phase_state_exchange_cnt: U8,
}

#[repr(C)]
pub struct MlmeExtPriv {
    pub last_scan_time: Systime,
    pub scan_abort: U8,
    pub sitesurvey_res: SsRes,
}

#[repr(C)]
pub struct RfCtl {
    pub channel_set: [RtChannelInfo; MAX_CHANNEL_NUM],
    pub dfs_slave_with_rd: U8,
}

#[repr(C)]
pub struct HostAdapter {
    pub registrypriv: RegistryPriv,
    pub mlmeextpriv: MlmeExtPriv,
    pub rfctl: RfCtl,
    pub wdinfo: WifiDirectInfo,
}

extern "C" {
    fn host_p2p_social_get() -> U8;
    fn host_p2p_needed_get() -> U8;
    fn host_scan_current_time() -> Systime;
    fn rtw_scan_backop_decision(a: Adapter) -> U8;
    fn rtw_chset_search_ch(ch_set: *mut RtChannelInfo, ch: U32) -> c_int;
    fn rtw_rfctl_dfs_domain_unknown(rfctl: *mut RfCtl) -> U8;
}

fn p2p_social(_wd: &WifiDirectInfo) -> bool {
    unsafe { host_p2p_social_get() != 0 }
}

fn p2p_needed(_wd: &WifiDirectInfo) -> bool {
    unsafe { host_p2p_needed_get() != 0 }
}

fn ch_is_non_ocp(info: &RtChannelInfo) -> bool {
    unsafe { info.non_ocp_end_time > host_scan_current_time() }
}

fn is_dfs_slave_with_rd(rfctl: &RfCtl) -> bool {
    rfctl.dfs_slave_with_rd != 0
}

fn scan_abort_hdl(adapter: &mut HostAdapter) {
    let ss = &mut adapter.mlmeextpriv.sitesurvey_res;
    if adapter.mlmeextpriv.scan_abort == _TRUE as U8 {
        ss.channel_idx = ss.ch_num as c_int;
    }
}

fn pick_ch_impl(adapter: &mut HostAdapter, ch_out: &mut U8, type_out: &mut c_int) -> U8 {
    scan_abort_hdl(adapter);

    let mut scan_ch: U8 = 0;
    let mut scan_type = SCAN_PASSIVE;
    let mut backop_flags: U8 = 0;
    let adapter_ptr = adapter as *mut HostAdapter;

    let (p2p_op, p2p_social_path) = {
        let wdinfo = &adapter.wdinfo;
        (
            wdinfo.rx_invitereq_info.scan_op_ch_only != 0 || wdinfo.p2p_info.scan_op_ch_only != 0,
            p2p_social(wdinfo),
        )
    };

    if p2p_op {
        let ss = &adapter.mlmeextpriv.sitesurvey_res;
        let wdinfo = &adapter.wdinfo;
        let idx = ss.channel_idx as usize;
        scan_ch = if wdinfo.rx_invitereq_info.scan_op_ch_only != 0 {
            wdinfo.rx_invitereq_info.operation_ch[idx]
        } else {
            wdinfo.p2p_info.operation_ch[idx]
        };
        scan_type = SCAN_ACTIVE;
    } else if p2p_social_path {
        let ss = &adapter.mlmeextpriv.sitesurvey_res;
        let rfctl = &adapter.rfctl;
        let wdinfo = &adapter.wdinfo;
        let idx = ss.channel_idx as usize;
        scan_ch = wdinfo.social_chan[idx];
        let ch_set_idx =
            unsafe { rtw_chset_search_ch(rfctl.channel_set.as_ptr() as *mut _, scan_ch as U32) };
        scan_type = if ch_set_idx >= 0
            && (rfctl.channel_set[ch_set_idx as usize].flags & RTW_CHF_NO_IR) != 0
        {
            SCAN_PASSIVE
        } else {
            SCAN_ACTIVE
        };
    } else {
        backop_flags = unsafe { rtw_scan_backop_decision(adapter_ptr as Adapter) };
        let ss = &mut adapter.mlmeextpriv.sitesurvey_res;
        let rfctl = &adapter.rfctl;

        if !(backop_flags != 0 && ss.scan_cnt >= ss.scan_cnt_max) {
            if ss.channel_idx != 0
                && ss.force_ssid_scan == 0
                && ss.ssid_num != 0
                && (ss.ch[(ss.channel_idx - 1) as usize].flags & RTW_IEEE80211_CHAN_PASSIVE_SCAN)
                    != 0
            {
                let prev = ss.ch[(ss.channel_idx - 1) as usize].hw_value as U32;
                let ch_set_idx =
                    unsafe { rtw_chset_search_ch(rfctl.channel_set.as_ptr() as *mut _, prev) };
                if ch_set_idx != -1
                    && rfctl.channel_set[ch_set_idx as usize].hidden_bss_cnt != 0
                    && (!is_dfs_slave_with_rd(rfctl)
                        || unsafe { rtw_rfctl_dfs_domain_unknown(rfctl as *const _ as *mut _) }
                            != 0
                        || !ch_is_non_ocp(&rfctl.channel_set[ch_set_idx as usize]))
                {
                    ss.channel_idx -= 1;
                    ss.force_ssid_scan = 1;
                }
            } else {
                ss.force_ssid_scan = 0;
            }
        }

        if ss.channel_idx < ss.ch_num as c_int {
            let ch = &ss.ch[ss.channel_idx as usize];
            scan_ch = ch.hw_value as U8;
            scan_type = if (ch.flags & RTW_IEEE80211_CHAN_PASSIVE_SCAN) != 0 {
                SCAN_PASSIVE
            } else {
                SCAN_ACTIVE
            };
        }
    }

    let ss = &mut adapter.mlmeextpriv.sitesurvey_res;
    let wdinfo = &adapter.wdinfo;
    let next_state = if scan_ch != 0 {
        let mut state = SCAN_PROCESS;
        if backop_flags != 0 {
            if ss.scan_cnt < ss.scan_cnt_max {
                ss.scan_cnt += 1;
            } else {
                ss.backop_flags = backop_flags;
                state = SCAN_BACKING_OP;
            }
        }
        state
    } else if p2p_needed(wdinfo) {
        SCAN_TO_P2P_LISTEN
    } else {
        SCAN_COMPLETE
    };

    if next_state != SCAN_PROCESS {
        ss.scan_cnt = 0;
    }

    *ch_out = scan_ch;
    *type_out = scan_type;
    next_state
}

#[no_mangle]
pub extern "C" fn sitesurvey_pick_ch_behavior(
    padapter: Adapter,
    ch: *mut U8,
    scan_type: *mut c_int,
) -> U8 {
    if padapter.is_null() {
        return SCAN_COMPLETE;
    }
    let adapter = unsafe { &mut *(padapter as *mut HostAdapter) };
    let mut out_ch: U8 = 0;
    let mut out_type = SCAN_PASSIVE;
    let state = pick_ch_impl(adapter, &mut out_ch, &mut out_type);
    if !ch.is_null() {
        unsafe { *ch = out_ch };
    }
    if !scan_type.is_null() {
        unsafe { *scan_type = out_type };
    }
    state
}
