// SPDX-License-Identifier: GPL-2.0
//! HALMAC ↔ driver enum/struct mappers from `hal/hal_halmac.c` (W4-10).

#![allow(
    dead_code,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe
)]

type U8 = u8;

const HW_STATE_NOLINK: U8 = 0x00;
const HW_STATE_ADHOC: U8 = 0x01;
const HW_STATE_STATION: U8 = 0x02;
const HW_STATE_AP: U8 = 0x03;

const HALMAC_NETWORK_NO_LINK: u32 = 0;
const HALMAC_NETWORK_ADHOC: u32 = 1;
const HALMAC_NETWORK_INFRASTRUCTURE: u32 = 2;
const HALMAC_NETWORK_AP: u32 = 3;
const HALMAC_NETWORK_UNDEFINE: u32 = 0x7F;

#[repr(C)]
pub struct HalmacBcnCtrl {
    pub dis_rx_bssid_fit: U8,
    pub en_txbcn_rpt: U8,
    pub dis_tsf_udt: U8,
    pub en_bcn: U8,
    pub en_rxbcn_rpt: U8,
    pub en_p2p_ctwin: U8,
    pub en_p2p_bcn_area: U8,
}

/// Matches `struct rtw_halmac_bcn_ctrl` in `hal/hal_halmac.h` (seven `u8 : 1` in one byte).
#[repr(C)]
pub struct RtwHalmacBcnCtrl {
    pub bits: U8,
}

impl RtwHalmacBcnCtrl {
    fn rx_bssid_fit(&self) -> U8 {
        self.bits & 1
    }
    fn set_rx_bssid_fit(&mut self, v: U8) {
        self.bits = (self.bits & !1) | (v & 1);
    }
    fn txbcn_rpt(&self) -> U8 {
        (self.bits >> 1) & 1
    }
    fn set_txbcn_rpt(&mut self, v: U8) {
        self.bits = (self.bits & !2) | ((v & 1) << 1);
    }
    fn tsf_update(&self) -> U8 {
        (self.bits >> 2) & 1
    }
    fn set_tsf_update(&mut self, v: U8) {
        self.bits = (self.bits & !4) | ((v & 1) << 2);
    }
    fn enable_bcn(&self) -> U8 {
        (self.bits >> 3) & 1
    }
    fn set_enable_bcn(&mut self, v: U8) {
        self.bits = (self.bits & !8) | ((v & 1) << 3);
    }
    fn rxbcn_rpt(&self) -> U8 {
        (self.bits >> 4) & 1
    }
    fn set_rxbcn_rpt(&mut self, v: U8) {
        self.bits = (self.bits & !16) | ((v & 1) << 4);
    }
    fn p2p_ctwin(&self) -> U8 {
        (self.bits >> 5) & 1
    }
    fn set_p2p_ctwin(&mut self, v: U8) {
        self.bits = (self.bits & !32) | ((v & 1) << 5);
    }
    fn p2p_bcn_area(&self) -> U8 {
        (self.bits >> 6) & 1
    }
    fn set_p2p_bcn_area(&mut self, v: U8) {
        self.bits = (self.bits & !64) | ((v & 1) << 6);
    }
}

fn network_type_halmac2drv_impl(network: u32) -> U8 {
    match network {
        HALMAC_NETWORK_NO_LINK | HALMAC_NETWORK_UNDEFINE => HW_STATE_NOLINK,
        HALMAC_NETWORK_ADHOC => HW_STATE_ADHOC,
        HALMAC_NETWORK_INFRASTRUCTURE => HW_STATE_STATION,
        HALMAC_NETWORK_AP => HW_STATE_AP,
        _ => HW_STATE_NOLINK,
    }
}

fn beacon_ctrl_halmac2drv_impl(ctrl: &HalmacBcnCtrl, drv_ctrl: &mut RtwHalmacBcnCtrl) {
    drv_ctrl.set_rx_bssid_fit(if ctrl.dis_rx_bssid_fit != 0 { 0 } else { 1 });
    drv_ctrl.set_txbcn_rpt(if ctrl.en_txbcn_rpt != 0 { 1 } else { 0 });
    drv_ctrl.set_tsf_update(if ctrl.dis_tsf_udt != 0 { 0 } else { 1 });
    drv_ctrl.set_enable_bcn(if ctrl.en_bcn != 0 { 1 } else { 0 });
    drv_ctrl.set_rxbcn_rpt(if ctrl.en_rxbcn_rpt != 0 { 1 } else { 0 });
    drv_ctrl.set_p2p_ctwin(if ctrl.en_p2p_ctwin != 0 { 1 } else { 0 });
    drv_ctrl.set_p2p_bcn_area(if ctrl.en_p2p_bcn_area != 0 { 1 } else { 0 });
}

fn beacon_ctrl_drv2halmac_impl(drv_ctrl: &RtwHalmacBcnCtrl, ctrl: &mut HalmacBcnCtrl) {
    ctrl.dis_rx_bssid_fit = if drv_ctrl.rx_bssid_fit() != 0 { 0 } else { 1 };
    ctrl.en_txbcn_rpt = if drv_ctrl.txbcn_rpt() != 0 { 1 } else { 0 };
    ctrl.dis_tsf_udt = if drv_ctrl.tsf_update() != 0 { 0 } else { 1 };
    ctrl.en_bcn = if drv_ctrl.enable_bcn() != 0 { 1 } else { 0 };
    ctrl.en_rxbcn_rpt = if drv_ctrl.rxbcn_rpt() != 0 { 1 } else { 0 };
    ctrl.en_p2p_ctwin = if drv_ctrl.p2p_ctwin() != 0 { 1 } else { 0 };
    ctrl.en_p2p_bcn_area = if drv_ctrl.p2p_bcn_area() != 0 { 1 } else { 0 };
}

#[cfg(not(host_hal_halmac_enum_test))]
#[no_mangle]
pub extern "C" fn _network_type_halmac2drv(network: u32) -> U8 {
    network_type_halmac2drv_impl(network)
}

#[cfg(not(host_hal_halmac_enum_test))]
#[no_mangle]
pub extern "C" fn _beacon_ctrl_halmac2drv(
    ctrl: *const HalmacBcnCtrl,
    drv_ctrl: *mut RtwHalmacBcnCtrl,
) {
    if ctrl.is_null() || drv_ctrl.is_null() {
        return;
    }
    unsafe {
        beacon_ctrl_halmac2drv_impl(&*ctrl, &mut *drv_ctrl);
    }
}

#[cfg(not(host_hal_halmac_enum_test))]
#[no_mangle]
pub extern "C" fn _beacon_ctrl_drv2halmac(
    drv_ctrl: *const RtwHalmacBcnCtrl,
    ctrl: *mut HalmacBcnCtrl,
) {
    if drv_ctrl.is_null() || ctrl.is_null() {
        return;
    }
    unsafe {
        beacon_ctrl_drv2halmac_impl(&*drv_ctrl, &mut *ctrl);
    }
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn network_type_halmac2drv(network: u32) -> U8 {
    network_type_halmac2drv_impl(network)
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn beacon_ctrl_halmac2drv(
    ctrl: *const HalmacBcnCtrl,
    drv_ctrl: *mut RtwHalmacBcnCtrl,
) {
    if ctrl.is_null() || drv_ctrl.is_null() {
        return;
    }
    unsafe {
        beacon_ctrl_halmac2drv_impl(&*ctrl, &mut *drv_ctrl);
    }
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn rtw_halmac_bcn_ctrl_rust_size() -> usize {
    core::mem::size_of::<RtwHalmacBcnCtrl>()
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn beacon_ctrl_drv2halmac(
    drv_ctrl: *const RtwHalmacBcnCtrl,
    ctrl: *mut HalmacBcnCtrl,
) {
    if drv_ctrl.is_null() || ctrl.is_null() {
        return;
    }
    unsafe {
        beacon_ctrl_drv2halmac_impl(&*drv_ctrl, &mut *ctrl);
    }
}
