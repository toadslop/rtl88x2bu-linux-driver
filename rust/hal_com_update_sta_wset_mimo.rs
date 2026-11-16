// SPDX-License-Identifier: GPL-2.0

use core::ffi::c_void;

type U8 = u8;
type S8 = i8;
type Adapter = c_void;
type StaInfo = c_void;

const WB: U8 = 1;
const WG: U8 = 2;
const WA: U8 = 4;
const W24N: U8 = 8;
const W5N: U8 = 16;
const WAC: U8 = 64;
const S_CCK: U8 = 1;
const S_OFDM: U8 = 2;
const S_HT: U8 = 4;
const S_VHT: U8 = 8;

#[cfg(host_hal_com_update_sta_wset_mimo_test)]
#[repr(C)]
struct HostStaInfo {
    wireless_mode: U8,
    cmn_mac_id: U8,
    cmn_mimo: U8,
    cmn_wset: U8,
}

#[cfg(host_hal_com_update_sta_wset_mimo_test)]
static mut H_TX: S8 = 1;
#[cfg(host_hal_com_update_sta_wset_mimo_test)]
static mut H_RX: S8 = 1;
#[cfg(host_hal_com_update_sta_wset_mimo_test)]
static mut H_WARN: i32 = 0;

#[cfg(host_hal_com_update_sta_wset_mimo_test)]
#[no_mangle]
pub extern "C" fn host_hal_update_sta_reset_nss() {
    unsafe {
        H_TX = 1;
        H_RX = 1;
        H_WARN = 0;
    }
}

#[cfg(host_hal_com_update_sta_wset_mimo_test)]
#[no_mangle]
pub extern "C" fn host_hal_update_sta_set_nss(tx: S8, rx: S8) {
    unsafe {
        H_TX = tx;
        H_RX = rx;
    }
}

#[cfg(host_hal_com_update_sta_wset_mimo_test)]
#[no_mangle]
pub extern "C" fn host_hal_update_sta_warn_on_called() -> i32 {
    unsafe { H_WARN }
}

#[cfg(not(host_hal_com_update_sta_wset_mimo_test))]
extern "C" {
    fn rtw_rust_hal_com_sta_wireless_mode(p: *mut StaInfo) -> U8;
    fn rtw_rust_hal_com_sta_set_support_wireless_set(p: *mut StaInfo, w: U8);
    fn rtw_rust_hal_com_sta_set_mimo_type(p: *mut StaInfo, m: U8);
    fn rtw_rust_hal_com_sta_mac_id(p: *mut StaInfo) -> U8;
    fn rtw_get_sta_tx_nss(a: *mut Adapter, p: *mut StaInfo) -> S8;
    fn rtw_get_sta_rx_nss(a: *mut Adapter, p: *mut StaInfo) -> S8;
    fn rtw_rust_hal_com_warn_on(c: i32);
    fn rtw_rust_hal_com_log_sta_mimo_ss(mac: U8, tx: S8, rx: S8);
    #[cfg(config_ctrl_txss_by_tp)]
    fn rtw_ctrl_txss_update_mimo_type(a: *mut Adapter, p: *mut StaInfo);
}

fn wset_from_mode(m: U8) -> U8 {
    let mut w = 0u8;
    if m & WB != 0 {
        w |= S_CCK;
    }
    if m & (WG | WA) != 0 {
        w |= S_OFDM;
    }
    if m & (W24N | W5N) != 0 {
        w |= S_HT;
    }
    if m & WAC != 0 {
        w |= S_VHT;
    }
    w
}

fn mimo_from_nss(tx: S8, rx: S8) -> Option<U8> {
    match (tx, rx) {
        (1, 1) => Some(0),
        (1, 2) => Some(1),
        (2, 2) => Some(2),
        (2, 3) => Some(3),
        (2, 4) => Some(4),
        (3, 3) => Some(5),
        (3, 4) => Some(6),
        (4, 4) => Some(7),
        _ => None,
    }
}

#[cfg(host_hal_com_update_sta_wset_mimo_test)]
#[no_mangle]
pub extern "C" fn rtw_hal_update_sta_wset(adapter: *mut Adapter, psta: *mut StaInfo) {
    let mode = if psta.is_null() {
        0
    } else {
        unsafe { (*psta.cast::<HostStaInfo>()).wireless_mode }
    };
    let w = wset_from_mode(mode);
    if !psta.is_null() {
        unsafe {
            (*psta.cast::<HostStaInfo>()).cmn_wset = w;
        }
    }
    let _ = adapter;
}

#[cfg(not(host_hal_com_update_sta_wset_mimo_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_update_sta_wset(adapter: *mut Adapter, psta: *mut StaInfo) {
    let w = wset_from_mode(unsafe { rtw_rust_hal_com_sta_wireless_mode(psta) });
    unsafe {
        rtw_rust_hal_com_sta_set_support_wireless_set(psta, w);
    }
    let _ = adapter;
}

#[cfg(host_hal_com_update_sta_wset_mimo_test)]
#[no_mangle]
pub extern "C" fn rtw_hal_update_sta_mimo_type(adapter: *mut Adapter, psta: *mut StaInfo) {
    let (tx, rx) = unsafe { (H_TX, H_RX) };
    if let Some(m) = mimo_from_nss(tx, rx) {
        if !psta.is_null() {
            unsafe {
                (*psta.cast::<HostStaInfo>()).cmn_mimo = m;
            }
        }
    } else {
        unsafe {
            H_WARN = 1;
        }
    }
    let _ = adapter;
}

#[cfg(not(host_hal_com_update_sta_wset_mimo_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_update_sta_mimo_type(adapter: *mut Adapter, psta: *mut StaInfo) {
    let tx = unsafe { rtw_get_sta_tx_nss(adapter, psta) };
    let rx = unsafe { rtw_get_sta_rx_nss(adapter, psta) };
    if let Some(m) = mimo_from_nss(tx, rx) {
        unsafe {
            rtw_rust_hal_com_sta_set_mimo_type(psta, m);
        }
    } else {
        unsafe {
            rtw_rust_hal_com_warn_on(1);
        }
    }
    #[cfg(config_ctrl_txss_by_tp)]
    unsafe {
        rtw_ctrl_txss_update_mimo_type(adapter, psta);
    }
    let mac = unsafe { rtw_rust_hal_com_sta_mac_id(psta) };
    unsafe {
        rtw_rust_hal_com_log_sta_mimo_ss(mac, tx, rx);
    }
}