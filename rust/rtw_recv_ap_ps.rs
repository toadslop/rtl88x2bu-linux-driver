// SPDX-License-Identifier: GPL-2.0
//! W3-177 AP-mode pwrbit / WMM PS data hooks from `core/rtw_recv.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(not(host_recv_ap_ps_test))]
use core::ffi::c_void;

type U8 = u8;
type U16 = u16;
type U32 = u32;

const WIFI_SLEEP_STATE: U32 = 0x0000_0004;
const _PWRMGT_: U16 = 1 << 12;

fn get_pwr_mgt(rx_data: *const U8) -> bool {
    if rx_data.is_null() {
        return false;
    }
    let fc = unsafe { *(rx_data as *const U16) };
    (fc & _PWRMGT_) != 0
}

#[cfg(host_recv_ap_ps_test)]
mod host {
    use super::*;

    #[repr(C)]
    pub struct CmnStaInfo {
        pub dm_ctrl: U16,
        pub bw_mode: U8,
        pub mac_id: U8,
        pub mac_addr: [U8; 6],
    }

    #[repr(C)]
    pub struct StaInfo {
        pub state: U32,
        pub qos_option: U32,
        pub qos_info: U8,
        pub uapsd_bk: U8,
        pub uapsd_be: U8,
        pub uapsd_vi: U8,
        pub uapsd_vo: U8,
        pub sleepq_ac_len: u32,
        pub cmn: CmnStaInfo,
    }

    #[repr(C)]
    pub struct RxPktAttrib {
        pub priority: U8,
    }

    #[repr(C)]
    pub struct RecvFrameHdr {
        pub attrib: RxPktAttrib,
        pub rx_data: *mut U8,
    }

    #[repr(C)]
    pub struct RecvFrame {
        pub hdr: RecvFrameHdr,
    }

    pub type Adapter = u8;

    extern "C" {
        fn stop_sta_xmit(adapter: *mut Adapter, sta: *mut StaInfo);
        fn wakeup_sta_to_xmit(adapter: *mut Adapter, sta: *mut StaInfo);
        fn xmit_delivery_enabled_frames(adapter: *mut Adapter, sta: *mut StaInfo);
        fn issue_qos_nulldata(
            adapter: *mut Adapter,
            da: *mut U8,
            tid: U16,
            ps: U8,
            try_cnt: i32,
            wait_ms: i32,
        ) -> i32;
    }

    pub fn process_pwrbit_data(
        padapter: *mut Adapter,
        precv_frame: *mut RecvFrame,
        psta: *mut StaInfo,
    ) {
        if padapter.is_null() || precv_frame.is_null() || psta.is_null() {
            return;
        }
        let frame = unsafe { &mut *precv_frame };
        let sta = unsafe { &mut *psta };
        let ptr = frame.hdr.rx_data;
        if get_pwr_mgt(ptr) {
            if sta.state & WIFI_SLEEP_STATE == 0 {
                unsafe { stop_sta_xmit(padapter, psta) };
            }
        } else if sta.state & WIFI_SLEEP_STATE != 0 {
            unsafe { wakeup_sta_to_xmit(padapter, psta) };
        }
    }

    pub fn process_wmmps_data(
        padapter: *mut Adapter,
        precv_frame: *mut RecvFrame,
        psta: *mut StaInfo,
    ) {
        if padapter.is_null() || precv_frame.is_null() || psta.is_null() {
            return;
        }
        let frame = unsafe { &mut *precv_frame };
        let sta = unsafe { &mut *psta };
        if sta.qos_option == 0 {
            return;
        }
        if sta.qos_info & 0xf == 0 {
            return;
        }
        if sta.state & WIFI_SLEEP_STATE == 0 {
            return;
        }
        let priority = frame.hdr.attrib.priority;
        const TRIGGER: U8 = 1 << 1;
        let wmmps_ac = match priority {
            1 | 2 => sta.uapsd_bk & TRIGGER,
            4 | 5 => sta.uapsd_vi & TRIGGER,
            6 | 7 => sta.uapsd_vo & TRIGGER,
            _ => sta.uapsd_be & TRIGGER,
        };
        if wmmps_ac != 0 {
            if sta.sleepq_ac_len > 0 {
                unsafe { xmit_delivery_enabled_frames(padapter, psta) };
            } else {
                unsafe {
                    issue_qos_nulldata(
                        padapter,
                        sta.cmn.mac_addr.as_mut_ptr(),
                        priority as U16,
                        0,
                        0,
                        0,
                    );
                }
            }
        }
    }
}

#[cfg(host_recv_ap_ps_test)]
pub use host::{Adapter, RecvFrame, StaInfo};

#[cfg(host_recv_ap_ps_test)]
#[no_mangle]
pub extern "C" fn process_pwrbit_data(
    padapter: *mut host::Adapter,
    precv_frame: *mut host::RecvFrame,
    psta: *mut host::StaInfo,
) {
    host::process_pwrbit_data(padapter, precv_frame, psta);
}

#[cfg(host_recv_ap_ps_test)]
#[no_mangle]
pub extern "C" fn process_wmmps_data(
    padapter: *mut host::Adapter,
    precv_frame: *mut host::RecvFrame,
    psta: *mut host::StaInfo,
) {
    host::process_wmmps_data(padapter, precv_frame, psta);
}

#[cfg(not(host_recv_ap_ps_test))]
mod kern {
    use super::*;

    extern "C" {
        fn rtw_rust_recv_ap_ps_rx_data(frame: *mut c_void) -> *mut U8;
        fn rtw_rust_recv_ap_ps_priority(frame: *mut c_void) -> U8;
        fn rtw_rust_sta_state(sta: *mut c_void) -> U32;
        fn rtw_rust_recv_ap_ps_uapsd_bk(sta: *mut c_void) -> U8;
        fn rtw_rust_recv_ap_ps_uapsd_be(sta: *mut c_void) -> U8;
        fn rtw_rust_recv_ap_ps_uapsd_vi(sta: *mut c_void) -> U8;
        fn rtw_rust_recv_ap_ps_uapsd_vo(sta: *mut c_void) -> U8;
        fn rtw_rust_recv_ap_ps_sleepq_ac_len(sta: *mut c_void) -> u32;
        fn rtw_rust_recv_ap_ps_mac_addr(sta: *mut c_void) -> *mut U8;
        fn rtw_rust_recv_ap_ps_wmmps_qos_ok(sta: *mut c_void) -> U8;
        fn stop_sta_xmit(padapter: *mut c_void, psta: *mut c_void);
        fn wakeup_sta_to_xmit(padapter: *mut c_void, psta: *mut c_void);
        fn xmit_delivery_enabled_frames(padapter: *mut c_void, psta: *mut c_void);
        fn issue_qos_nulldata(
            padapter: *mut c_void,
            da: *mut U8,
            tid: U16,
            ps: U8,
            try_cnt: i32,
            wait_ms: i32,
        ) -> i32;
    }

    fn do_pwrbit(padapter: *mut c_void, precv_frame: *mut c_void, psta: *mut c_void) {
        let ptr = unsafe { rtw_rust_recv_ap_ps_rx_data(precv_frame) };
        if get_pwr_mgt(ptr) {
            if unsafe { rtw_rust_sta_state(psta) } & WIFI_SLEEP_STATE == 0 {
                unsafe { stop_sta_xmit(padapter, psta) };
            }
        } else if unsafe { rtw_rust_sta_state(psta) } & WIFI_SLEEP_STATE != 0 {
            unsafe { wakeup_sta_to_xmit(padapter, psta) };
        }
    }

    fn do_wmmps(padapter: *mut c_void, precv_frame: *mut c_void, psta: *mut c_void) {
        if unsafe { rtw_rust_recv_ap_ps_wmmps_qos_ok(psta) } == 0 {
            return;
        }
        if unsafe { rtw_rust_sta_state(psta) } & WIFI_SLEEP_STATE == 0 {
            return;
        }
        let priority = unsafe { rtw_rust_recv_ap_ps_priority(precv_frame) };
        const TRIGGER: U8 = 1 << 1;
        let wmmps_ac = match priority {
            1 | 2 => (unsafe { rtw_rust_recv_ap_ps_uapsd_bk(psta) }) & TRIGGER,
            4 | 5 => (unsafe { rtw_rust_recv_ap_ps_uapsd_vi(psta) }) & TRIGGER,
            6 | 7 => (unsafe { rtw_rust_recv_ap_ps_uapsd_vo(psta) }) & TRIGGER,
            _ => (unsafe { rtw_rust_recv_ap_ps_uapsd_be(psta) }) & TRIGGER,
        };
        if wmmps_ac != 0 {
            if unsafe { rtw_rust_recv_ap_ps_sleepq_ac_len(psta) } > 0 {
                unsafe { xmit_delivery_enabled_frames(padapter, psta) };
            } else {
                let da = unsafe { rtw_rust_recv_ap_ps_mac_addr(psta) };
                unsafe {
                    issue_qos_nulldata(padapter, da, priority as U16, 0, 0, 0);
                }
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn process_pwrbit_data(
        padapter: *mut c_void,
        precv_frame: *mut c_void,
        psta: *mut c_void,
    ) {
        do_pwrbit(padapter, precv_frame, psta);
    }

    #[no_mangle]
    pub extern "C" fn process_wmmps_data(
        padapter: *mut c_void,
        precv_frame: *mut c_void,
        psta: *mut c_void,
    ) {
        do_wmmps(padapter, precv_frame, psta);
    }
}
