// SPDX-License-Identifier: GPL-2.0
//! W3-174 — `traffic_check_for_leave_lps_by_tp` from `core/rtw_pwrctrl.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe
)]

#[cfg(host_pwrctrl_lps_tp_test)]
use std::ffi::c_void;

#[cfg(not(host_pwrctrl_lps_tp_test))]
use core::ffi::c_void;

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;
const PS_MODE_ACTIVE: u8 = 0;
const LPS_CTRL_TX_TRAFFIC_LEAVE: u8 = 7;
const LPS_CTRL_RX_TRAFFIC_LEAVE: u8 = 8;

type Padapter = *mut c_void;
type Psta = *mut c_void;

#[cfg(host_pwrctrl_lps_tp_test)]
#[repr(C)]
struct StainfoStats {
    tx_bytes: u64,
    acc_tx_bytes: u64,
    rx_bytes: u64,
    acc_rx_bytes: u64,
}

#[cfg(host_pwrctrl_lps_tp_test)]
#[repr(C)]
struct StaInfo {
    sta_stats: StainfoStats,
}

#[cfg(host_pwrctrl_lps_tp_test)]
#[repr(C)]
struct RtLinkDetect {
    NumTxOkInPeriod: u32,
    NumRxUnicastOkInPeriod: u32,
}

#[cfg(host_pwrctrl_lps_tp_test)]
#[repr(C)]
struct PwrctrlPriv {
    bLeisurePs: u8,
    pwr_mode: u8,
    lps_tx_tp_th: u16,
    lps_rx_tp_th: u16,
    lps_chk_cnt_th: i32,
    lps_chk_cnt: i32,
    lps_tx_pkts: u32,
    lps_rx_pkts: u32,
}

#[cfg(host_pwrctrl_lps_tp_test)]
#[repr(C)]
struct MlmePriv {
    LinkDetectInfo: RtLinkDetect,
}

#[cfg(host_pwrctrl_lps_tp_test)]
#[repr(C)]
struct HostAdapter {
    pwrctrlpriv: PwrctrlPriv,
    mlmepriv: MlmePriv,
}

#[cfg(host_pwrctrl_lps_tp_test)]
#[repr(C)]
pub struct HostLpsTpTrace {
    pub lps_ctrl_calls: u32,
    pub last_lps_ctrl_type: u8,
    pub bt_control_lps: u8,
}

#[cfg(host_pwrctrl_lps_tp_test)]
mod host_trace {
    use super::*;

    static mut G_TRACE: HostLpsTpTrace = HostLpsTpTrace {
        lps_ctrl_calls: 0,
        last_lps_ctrl_type: 0,
        bt_control_lps: 0,
    };
    static mut G_BT_CONTROL_LPS: u8 = _FALSE;

    pub fn reset() {
        unsafe {
            G_TRACE = HostLpsTpTrace {
                lps_ctrl_calls: 0,
                last_lps_ctrl_type: 0,
                bt_control_lps: 0,
            };
            G_BT_CONTROL_LPS = _FALSE;
        }
    }

    pub fn set_bt_control_lps(v: u8) {
        unsafe {
            G_BT_CONTROL_LPS = v;
        }
    }

    fn bt_control_lps() -> u8 {
        unsafe { G_BT_CONTROL_LPS }
    }

    pub fn get_trace() -> *mut HostLpsTpTrace {
        unsafe { &mut G_TRACE }
    }

    pub fn record_lps_ctrl(lps_ctrl_type: u8) {
        unsafe {
            G_TRACE.lps_ctrl_calls += 1;
            G_TRACE.last_lps_ctrl_type = lps_ctrl_type;
        }
    }

    pub fn leisure_may_leave(pwr: &PwrctrlPriv) -> bool {
        pwr.bLeisurePs == _TRUE && pwr.pwr_mode != PS_MODE_ACTIVE && bt_control_lps() == _FALSE
    }
}

#[cfg(not(host_pwrctrl_lps_tp_test))]
extern "C" {
    fn rtw_rust_lps_tp_sta_tx_bytes(sta: Psta) -> u64;
    fn rtw_rust_lps_tp_sta_acc_tx_bytes(sta: Psta) -> u64;
    fn rtw_rust_lps_tp_sta_rx_bytes(sta: Psta) -> u64;
    fn rtw_rust_lps_tp_sta_acc_rx_bytes(sta: Psta) -> u64;
    fn rtw_rust_lps_tp_num_tx_ok(padapter: Padapter) -> u32;
    fn rtw_rust_lps_tp_num_rx_unicast_ok(padapter: Padapter) -> u32;
    fn rtw_rust_lps_tp_get_u16(padapter: Padapter, field: u32) -> u16;
    fn rtw_rust_lps_tp_get_u8(padapter: Padapter, field: u32) -> u8;
    fn rtw_rust_lps_tp_get_u32(padapter: Padapter, field: u32) -> u32;
    fn rtw_rust_lps_tp_get_i32(padapter: Padapter, field: u32) -> i32;
    fn rtw_rust_lps_tp_set_lps_chk_cnt(padapter: Padapter, v: i32);
    fn rtw_rust_lps_tp_bt_control_lps(padapter: Padapter) -> u8;
    fn rtw_rust_lps_tp_lps_ctrl_wk_cmd(padapter: Padapter, lps_ctrl_type: u8, flags: u8);
}

fn tp_threshold_kbytes(mbps_th: u16) -> u32 {
    (mbps_th as u32) * 1024 / 8 * 2
}

#[cfg(not(host_pwrctrl_lps_tp_test))]
fn leisure_may_leave_kernel(padapter: Padapter) -> bool {
    let leisure = unsafe { rtw_rust_lps_tp_get_u8(padapter, 0) } == _TRUE;
    let pwr_mode = unsafe { rtw_rust_lps_tp_get_u8(padapter, 1) };
    let bt_ok = unsafe { rtw_rust_lps_tp_bt_control_lps(padapter) } == _FALSE;
    leisure && pwr_mode != PS_MODE_ACTIVE && bt_ok
}

#[no_mangle]
pub extern "C" fn traffic_check_for_leave_lps_by_tp(padapter: Padapter, tx: u8, sta: Psta) {
    if padapter.is_null() || sta.is_null() {
        return;
    }

    let mut leave_lps = false;

    if tx != 0 {
        let (cur_acc_tx_bytes, tx_tp_th, num_tx_ok, lps_tx_pkts) = read_tx_inputs(padapter, sta);
        let tx_tp_kbyte = (cur_acc_tx_bytes >> 10) as u32;
        if tx_tp_kbyte >= tx_tp_th || num_tx_ok >= lps_tx_pkts {
            leave_lps = may_leave(padapter, sta);
        }
    } else {
        let (cur_acc_rx_bytes, rx_tp_th, num_rx_ok, lps_rx_pkts) = read_rx_inputs(padapter, sta);
        let rx_tp_kbyte = (cur_acc_rx_bytes >> 10) as u32;
        if rx_tp_kbyte >= rx_tp_th || num_rx_ok >= lps_rx_pkts {
            leave_lps = may_leave(padapter, sta);
        }
    }

    if !leave_lps {
        return;
    }

    let lps_chk_cnt_th = read_lps_chk_cnt_th(padapter);
    set_lps_chk_cnt(padapter, lps_chk_cnt_th);
    let cmd = if tx != 0 {
        LPS_CTRL_TX_TRAFFIC_LEAVE
    } else {
        LPS_CTRL_RX_TRAFFIC_LEAVE
    };
    issue_lps_ctrl(padapter, cmd);
}

fn may_leave(padapter: Padapter, _sta: Psta) -> bool {
    #[cfg(host_pwrctrl_lps_tp_test)]
    {
        let adapter = padapter as *mut HostAdapter;
        let pwr = unsafe { &(*adapter).pwrctrlpriv };
        host_trace::leisure_may_leave(pwr)
    }
    #[cfg(not(host_pwrctrl_lps_tp_test))]
    {
        leisure_may_leave_kernel(padapter)
    }
}

fn read_tx_inputs(padapter: Padapter, sta: Psta) -> (u64, u32, u32, u32) {
    #[cfg(host_pwrctrl_lps_tp_test)]
    {
        let adapter = padapter as *mut HostAdapter;
        let pstats = unsafe { &mut (*(sta as *mut StaInfo)).sta_stats };
        let pwr = unsafe { &(*adapter).pwrctrlpriv };
        let cur = pstats.tx_bytes - pstats.acc_tx_bytes;
        let th = tp_threshold_kbytes(pwr.lps_tx_tp_th);
        let num_tx = unsafe { (*adapter).mlmepriv.LinkDetectInfo.NumTxOkInPeriod };
        (cur, th, num_tx, pwr.lps_tx_pkts)
    }
    #[cfg(not(host_pwrctrl_lps_tp_test))]
    {
        let tx_bytes = unsafe { rtw_rust_lps_tp_sta_tx_bytes(sta) };
        let acc = unsafe { rtw_rust_lps_tp_sta_acc_tx_bytes(sta) };
        let cur = tx_bytes - acc;
        let th = tp_threshold_kbytes(unsafe { rtw_rust_lps_tp_get_u16(padapter, 0) });
        let num_tx = unsafe { rtw_rust_lps_tp_num_tx_ok(padapter) };
        let pkts = unsafe { rtw_rust_lps_tp_get_u32(padapter, 0) };
        (cur, th, num_tx, pkts)
    }
}

fn read_rx_inputs(padapter: Padapter, sta: Psta) -> (u64, u32, u32, u32) {
    #[cfg(host_pwrctrl_lps_tp_test)]
    {
        let adapter = padapter as *mut HostAdapter;
        let pstats = unsafe { &mut (*(sta as *mut StaInfo)).sta_stats };
        let pwr = unsafe { &(*adapter).pwrctrlpriv };
        let cur = pstats.rx_bytes - pstats.acc_rx_bytes;
        let th = tp_threshold_kbytes(pwr.lps_rx_tp_th);
        let num_rx = unsafe { (*adapter).mlmepriv.LinkDetectInfo.NumRxUnicastOkInPeriod };
        (cur, th, num_rx, pwr.lps_rx_pkts)
    }
    #[cfg(not(host_pwrctrl_lps_tp_test))]
    {
        let rx_bytes = unsafe { rtw_rust_lps_tp_sta_rx_bytes(sta) };
        let acc = unsafe { rtw_rust_lps_tp_sta_acc_rx_bytes(sta) };
        let cur = rx_bytes - acc;
        let th = tp_threshold_kbytes(unsafe { rtw_rust_lps_tp_get_u16(padapter, 1) });
        let num_rx = unsafe { rtw_rust_lps_tp_num_rx_unicast_ok(padapter) };
        let pkts = unsafe { rtw_rust_lps_tp_get_u32(padapter, 1) };
        (cur, th, num_rx, pkts)
    }
}

fn read_lps_chk_cnt_th(padapter: Padapter) -> i32 {
    #[cfg(host_pwrctrl_lps_tp_test)]
    {
        let adapter = padapter as *mut HostAdapter;
        unsafe { (*adapter).pwrctrlpriv.lps_chk_cnt_th }
    }
    #[cfg(not(host_pwrctrl_lps_tp_test))]
    {
        unsafe { rtw_rust_lps_tp_get_i32(padapter, 0) }
    }
}

fn set_lps_chk_cnt(padapter: Padapter, v: i32) {
    #[cfg(host_pwrctrl_lps_tp_test)]
    {
        let adapter = padapter as *mut HostAdapter;
        unsafe {
            (*adapter).pwrctrlpriv.lps_chk_cnt = v;
        }
    }
    #[cfg(not(host_pwrctrl_lps_tp_test))]
    {
        unsafe {
            rtw_rust_lps_tp_set_lps_chk_cnt(padapter, v);
        }
    }
}

fn issue_lps_ctrl(padapter: Padapter, lps_ctrl_type: u8) {
    #[cfg(host_pwrctrl_lps_tp_test)]
    {
        host_trace::record_lps_ctrl(lps_ctrl_type);
    }
    #[cfg(not(host_pwrctrl_lps_tp_test))]
    {
        unsafe {
            rtw_rust_lps_tp_lps_ctrl_wk_cmd(padapter, lps_ctrl_type, 0);
        }
    }
}

#[cfg(host_pwrctrl_lps_tp_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_lps_tp_reset_trace() {
    host_trace::reset();
}

#[cfg(host_pwrctrl_lps_tp_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_lps_tp_set_bt_control_lps(v: u8) {
    host_trace::set_bt_control_lps(v);
}

#[cfg(host_pwrctrl_lps_tp_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_lps_tp_get_trace() -> *mut HostLpsTpTrace {
    host_trace::get_trace()
}
