// SPDX-License-Identifier: GPL-2.0
//! HALMAC ↔ driver enum/struct mappers from `hal/hal_halmac.c` (W4-10, W4-11, W4-12, W4-21, W4-22, W4-23, W4-24, W4-26, W4-27).

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
const HW_STATE_MONITOR: U8 = 0x04;

const HALMAC_PORTID0: u32 = 0;
const HALMAC_PORTID1: u32 = 1;
const HALMAC_PORTID2: u32 = 2;
const HALMAC_PORTID3: u32 = 3;
const HALMAC_PORTID4: u32 = 4;
const HALMAC_PORTID_NUM: u32 = 5;

const HALMAC_NETWORK_NO_LINK: u32 = 0;
const HALMAC_NETWORK_ADHOC: u32 = 1;
const HALMAC_NETWORK_INFRASTRUCTURE: u32 = 2;
const HALMAC_NETWORK_AP: u32 = 3;
const HALMAC_NETWORK_UNDEFINE: u32 = 0x7F;

type U16 = u16;

const HALMAC_RSVD_PG_NUM8: u32 = 0;
const HALMAC_RSVD_PG_NUM16: u32 = 1;
const HALMAC_RSVD_PG_NUM24: u32 = 2;
const HALMAC_RSVD_PG_NUM32: u32 = 3;
const HALMAC_RSVD_PG_NUM64: u32 = 4;
const HALMAC_RSVD_PG_NUM128: u32 = 5;
const HALMAC_RSVD_PG_NUM256: u32 = 6;

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

fn hw_port_drv2halmac_impl(hwport: u32) -> u32 {
    match hwport {
        0 => HALMAC_PORTID0,
        1 => HALMAC_PORTID1,
        2 => HALMAC_PORTID2,
        3 => HALMAC_PORTID3,
        4 => HALMAC_PORTID4,
        _ => HALMAC_PORTID_NUM,
    }
}

fn network_type_drv2halmac_impl(hw_type: U8) -> u32 {
    match hw_type {
        HW_STATE_NOLINK | HW_STATE_MONITOR => HALMAC_NETWORK_NO_LINK,
        HW_STATE_ADHOC => HALMAC_NETWORK_ADHOC,
        HW_STATE_STATION => HALMAC_NETWORK_INFRASTRUCTURE,
        HW_STATE_AP => HALMAC_NETWORK_AP,
        _ => HALMAC_NETWORK_UNDEFINE,
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

fn rsvd_page_num_drv2halmac_impl(num: U16) -> u32 {
    if num <= 8 {
        return HALMAC_RSVD_PG_NUM8;
    }
    if num <= 16 {
        return HALMAC_RSVD_PG_NUM16;
    }
    if num <= 24 {
        return HALMAC_RSVD_PG_NUM24;
    }
    if num <= 32 {
        return HALMAC_RSVD_PG_NUM32;
    }
    if num <= 64 {
        return HALMAC_RSVD_PG_NUM64;
    }
    if num <= 128 {
        return HALMAC_RSVD_PG_NUM128;
    }
    HALMAC_RSVD_PG_NUM256
}

fn rsvd_page_num_halmac2drv_impl(rsvd_page_number: u32) -> U16 {
    match rsvd_page_number {
        HALMAC_RSVD_PG_NUM8 => 8,
        HALMAC_RSVD_PG_NUM16 => 16,
        HALMAC_RSVD_PG_NUM24 => 24,
        HALMAC_RSVD_PG_NUM32 => 32,
        HALMAC_RSVD_PG_NUM64 => 64,
        HALMAC_RSVD_PG_NUM128 => 128,
        HALMAC_RSVD_PG_NUM256 => 256,
        _ => 0,
    }
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

const RF_1T1R: u32 = 0;
const RF_1T2R: u32 = 1;
const RF_2T2R: u32 = 2;
const RF_2T3R: u32 = 3;
const RF_2T4R: u32 = 4;
const RF_3T3R: u32 = 5;
const RF_3T4R: u32 = 6;
const RF_4T4R: u32 = 7;
const RF_TYPE_MAX: u32 = 16;

const HALMAC_RF_1T2R: u32 = 0;
const HALMAC_RF_2T4R: u32 = 1;
const HALMAC_RF_2T2R: u32 = 2;
const HALMAC_RF_2T3R: u32 = 3;
const HALMAC_RF_1T1R: u32 = 4;
const HALMAC_RF_2T2R_GREEN: u32 = 5;
const HALMAC_RF_3T3R: u32 = 6;
const HALMAC_RF_3T4R: u32 = 7;
const HALMAC_RF_4T4R: u32 = 8;
const HALMAC_RF_MAX_TYPE: u32 = 0xF;

static RF_DRV_TO_HALMAC: [u32; 8] = [
    HALMAC_RF_1T1R,
    HALMAC_RF_1T2R,
    HALMAC_RF_2T2R,
    HALMAC_RF_2T3R,
    HALMAC_RF_2T4R,
    HALMAC_RF_3T3R,
    HALMAC_RF_3T4R,
    HALMAC_RF_4T4R,
];

fn rf_type_drv2halmac_impl(rf_drv: u32) -> u32 {
    if rf_drv < 8 {
        return RF_DRV_TO_HALMAC[rf_drv as usize];
    }
    HALMAC_RF_MAX_TYPE
}

fn rf_type_halmac2drv_impl(rf_mac: u32) -> u32 {
    match rf_mac {
        HALMAC_RF_1T2R => RF_1T2R,
        HALMAC_RF_2T4R => RF_2T4R,
        HALMAC_RF_2T2R | HALMAC_RF_2T2R_GREEN => RF_2T2R,
        HALMAC_RF_2T3R => RF_2T3R,
        HALMAC_RF_1T1R => RF_1T1R,
        HALMAC_RF_3T3R => RF_3T3R,
        HALMAC_RF_3T4R => RF_3T4R,
        HALMAC_RF_4T4R => RF_4T4R,
        _ => RF_TYPE_MAX,
    }
}

const HAL_FIFO_SEL_TX: u32 = 0;
const HAL_FIFO_SEL_RX: u32 = 1;
const HAL_FIFO_SEL_RSVD_PAGE: u32 = 2;
const HAL_FIFO_SEL_REPORT: u32 = 3;
const HAL_FIFO_SEL_LLT: u32 = 4;
const HAL_FIFO_SEL_RXBUF_FW: u32 = 5;

fn fifo_sel_drv2halmac_impl(fifo_sel: U8) -> u32 {
    match fifo_sel {
        0 => HAL_FIFO_SEL_TX,
        1 => HAL_FIFO_SEL_RX,
        2 => HAL_FIFO_SEL_RSVD_PAGE,
        3 => HAL_FIFO_SEL_REPORT,
        4 => HAL_FIFO_SEL_LLT,
        5 => HAL_FIFO_SEL_RXBUF_FW,
        _ => HAL_FIFO_SEL_RSVD_PAGE,
    }
}

const RTW_USB_SPEED_2: u32 = 2;
const RTW_USB_SPEED_3: u32 = 3;
const HALMAC_USB_MODE_U2: u32 = 1;
const HALMAC_USB_MODE_U3: u32 = 2;

#[cfg(any(host_hal_halmac_enum_test, CONFIG_USB_HCI))]
fn usb_mode_drv2halmac_impl(usb_mode: u32) -> u32 {
    match usb_mode {
        RTW_USB_SPEED_2 => HALMAC_USB_MODE_U2,
        RTW_USB_SPEED_3 => HALMAC_USB_MODE_U3,
        _ => HALMAC_USB_MODE_U2,
    }
}

#[cfg(not(host_hal_halmac_enum_test))]
#[no_mangle]
pub extern "C" fn _hw_port_drv2halmac(hwport: u32) -> u32 {
    hw_port_drv2halmac_impl(hwport)
}

#[cfg(not(host_hal_halmac_enum_test))]
#[no_mangle]
pub extern "C" fn _network_type_drv2halmac(hw_type: U8) -> u32 {
    network_type_drv2halmac_impl(hw_type)
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
pub extern "C" fn _rsvd_page_num_drv2halmac(num: U16) -> u32 {
    rsvd_page_num_drv2halmac_impl(num)
}

#[cfg(not(host_hal_halmac_enum_test))]
#[no_mangle]
pub extern "C" fn _rsvd_page_num_halmac2drv(rsvd_page_number: u32) -> U16 {
    rsvd_page_num_halmac2drv_impl(rsvd_page_number)
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

#[cfg(not(host_hal_halmac_enum_test))]
#[no_mangle]
pub extern "C" fn _rf_type_drv2halmac(rf_drv: u32) -> u32 {
    rf_type_drv2halmac_impl(rf_drv)
}

#[cfg(not(host_hal_halmac_enum_test))]
#[no_mangle]
pub extern "C" fn _rf_type_halmac2drv(rf_mac: u32) -> u32 {
    rf_type_halmac2drv_impl(rf_mac)
}

#[cfg(not(host_hal_halmac_enum_test))]
#[no_mangle]
pub extern "C" fn _fifo_sel_drv2halmac(fifo_sel: U8) -> u32 {
    fifo_sel_drv2halmac_impl(fifo_sel)
}

#[cfg(all(not(host_hal_halmac_enum_test), CONFIG_USB_HCI))]
#[no_mangle]
pub extern "C" fn _usb_mode_drv2halmac(usb_mode: u32) -> u32 {
    usb_mode_drv2halmac_impl(usb_mode)
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn hw_port_drv2halmac(hwport: u32) -> u32 {
    hw_port_drv2halmac_impl(hwport)
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn network_type_drv2halmac(hw_type: U8) -> u32 {
    network_type_drv2halmac_impl(hw_type)
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

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn rsvd_page_num_drv2halmac(num: U16) -> u32 {
    rsvd_page_num_drv2halmac_impl(num)
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn rsvd_page_num_halmac2drv(rsvd_page_number: u32) -> U16 {
    rsvd_page_num_halmac2drv_impl(rsvd_page_number)
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn rf_type_drv2halmac(rf_drv: u32) -> u32 {
    rf_type_drv2halmac_impl(rf_drv)
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn rf_type_halmac2drv(rf_mac: u32) -> u32 {
    rf_type_halmac2drv_impl(rf_mac)
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn fifo_sel_drv2halmac(fifo_sel: U8) -> u32 {
    fifo_sel_drv2halmac_impl(fifo_sel)
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn usb_mode_drv2halmac(usb_mode: u32) -> u32 {
    usb_mode_drv2halmac_impl(usb_mode)
}

const FW_EMEM: u32 = 0;
const FW_IMEM: u32 = 1;
const FW_DMEM: u32 = 2;

const HALMAC_DLFW_MEM_EMEM: u32 = 0x00;
const HALMAC_DLFW_MEM_EMEM_RSVD_PG: u32 = 0x01;
const HALMAC_DLFW_MEM_UNDEFINE: u32 = 0x7F;

const HALMAC_WLLED_MODE_TRX: u32 = 0;
const HALMAC_WLLED_MODE_TX: u32 = 1;
const HALMAC_WLLED_MODE_RX: u32 = 2;
const HALMAC_WLLED_MODE_SW_CTRL: u32 = 3;

fn bitshift(bitmask: u32) -> u32 {
    for i in 0..=31 {
        if (bitmask >> i) & 1 == 1 {
            return i;
        }
    }
    32
}

fn fw_mem_drv2halmac_impl(mem: u32, tx_stop: U8) -> u32 {
    match mem {
        FW_EMEM => {
            if tx_stop == 0 {
                HALMAC_DLFW_MEM_EMEM_RSVD_PG
            } else {
                HALMAC_DLFW_MEM_EMEM
            }
        }
        FW_IMEM | FW_DMEM => HALMAC_DLFW_MEM_UNDEFINE,
        _ => HALMAC_DLFW_MEM_UNDEFINE,
    }
}

fn led_mode_drv2halmac_impl(drv_mode: U8) -> u32 {
    match drv_mode {
        1 => HALMAC_WLLED_MODE_TX,
        2 => HALMAC_WLLED_MODE_RX,
        3 => HALMAC_WLLED_MODE_SW_CTRL,
        _ => HALMAC_WLLED_MODE_TRX,
    }
}

fn phy_parameter_val_drv2halmac_impl(val: u32, msk_en: U8, msk: u32) -> u32 {
    if msk_en == 0 {
        return val;
    }
    val << bitshift(msk)
}

const HALMAC_PARAMETER_CMD_MAC_W8: u32 = 0x4;
const HALMAC_PARAMETER_CMD_MAC_W16: u32 = 0x5;
const HALMAC_PARAMETER_CMD_MAC_W32: u32 = 0x6;
const HALMAC_PARAMETER_CMD_RF_W: u32 = 0x7;
const HALMAC_PARAMETER_CMD_BB_W8: u32 = 0x8;
const HALMAC_PARAMETER_CMD_BB_W16: u32 = 0x9;
const HALMAC_PARAMETER_CMD_BB_W32: u32 = 0xA;
const HALMAC_PARAMETER_CMD_DELAY_US: u32 = 0x10;
const HALMAC_PARAMETER_CMD_DELAY_MS: u32 = 0x11;
const HALMAC_PARAMETER_CMD_END: u32 = 0xFF;

#[repr(C)]
#[derive(Copy, Clone)]
pub struct RtwPhyParaMac {
    pub value: u32,
    pub msk: u32,
    pub offset: u16,
    pub msk_en: U8,
    pub size: U8,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct RtwPhyParaBb {
    pub value: u32,
    pub msk: u32,
    pub offset: u16,
    pub msk_en: U8,
    pub size: U8,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct RtwPhyParaRf {
    pub value: u32,
    pub msk: u32,
    pub offset: U8,
    pub msk_en: U8,
    pub path: U8,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct RtwPhyParaDelay {
    pub unit: U8,
    pub value: u16,
}

#[repr(C)]
pub union RtwPhyParaData {
    pub mac: RtwPhyParaMac,
    pub bb: RtwPhyParaBb,
    pub rf: RtwPhyParaRf,
    pub delay: RtwPhyParaDelay,
}

#[repr(C)]
pub struct RtwPhyParameter {
    pub cmd: U8,
    pub data: RtwPhyParaData,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct HalmacMacRegW {
    pub value: u32,
    pub msk: u32,
    pub offset: u16,
    pub msk_en: U8,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct HalmacBbRegW {
    pub value: u32,
    pub msk: u32,
    pub offset: u16,
    pub msk_en: U8,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct HalmacRfRegW {
    pub value: u32,
    pub msk: u32,
    pub offset: U8,
    pub msk_en: U8,
    pub rf_path: U8,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct HalmacDelayTime {
    pub rsvd1: u32,
    pub rsvd2: u32,
    pub delay_time: u16,
    pub rsvd3: U8,
}

#[repr(C)]
pub union HalmacParameterContent {
    pub MAC_REG_W: HalmacMacRegW,
    pub BB_REG_W: HalmacBbRegW,
    pub RF_REG_W: HalmacRfRegW,
    pub DELAY_TIME: HalmacDelayTime,
}

#[repr(C)]
pub struct HalmacPhyParameterInfo {
    pub cmd_id: u32,
    pub content: HalmacParameterContent,
}

fn phy_parameter_drv2halmac_impl(
    para: *const RtwPhyParameter,
    info: *mut HalmacPhyParameterInfo,
) -> i32 {
    if para.is_null() || info.is_null() {
        return -1;
    }
    let para = unsafe { &*para };
    let info = unsafe { &mut *info };
    unsafe {
        core::ptr::write_bytes(
            info as *mut HalmacPhyParameterInfo as *mut u8,
            0,
            core::mem::size_of::<HalmacPhyParameterInfo>(),
        );
    }

    match para.cmd {
        0 => {
            let mac = unsafe { para.data.mac };
            info.cmd_id = match mac.size {
                1 => HALMAC_PARAMETER_CMD_MAC_W8,
                2 => HALMAC_PARAMETER_CMD_MAC_W16,
                _ => HALMAC_PARAMETER_CMD_MAC_W32,
            };
            info.content.MAC_REG_W.value =
                phy_parameter_val_drv2halmac_impl(mac.value, mac.msk_en, mac.msk);
            info.content.MAC_REG_W.msk = mac.msk;
            info.content.MAC_REG_W.offset = mac.offset;
            info.content.MAC_REG_W.msk_en = mac.msk_en;
        }
        1 => {
            let bb = unsafe { para.data.bb };
            info.cmd_id = match bb.size {
                1 => HALMAC_PARAMETER_CMD_BB_W8,
                2 => HALMAC_PARAMETER_CMD_BB_W16,
                _ => HALMAC_PARAMETER_CMD_BB_W32,
            };
            info.content.BB_REG_W.value =
                phy_parameter_val_drv2halmac_impl(bb.value, bb.msk_en, bb.msk);
            info.content.BB_REG_W.msk = bb.msk;
            info.content.BB_REG_W.offset = bb.offset;
            info.content.BB_REG_W.msk_en = bb.msk_en;
        }
        2 => {
            let rf = unsafe { para.data.rf };
            info.cmd_id = HALMAC_PARAMETER_CMD_RF_W;
            info.content.RF_REG_W.value =
                phy_parameter_val_drv2halmac_impl(rf.value, rf.msk_en, rf.msk);
            info.content.RF_REG_W.msk = rf.msk;
            info.content.RF_REG_W.offset = rf.offset;
            info.content.RF_REG_W.msk_en = rf.msk_en;
            info.content.RF_REG_W.rf_path = rf.path;
        }
        3 => {
            let delay = unsafe { para.data.delay };
            info.cmd_id = if delay.unit == 0 {
                HALMAC_PARAMETER_CMD_DELAY_US
            } else {
                HALMAC_PARAMETER_CMD_DELAY_MS
            };
            info.content.DELAY_TIME.delay_time = delay.value;
        }
        0xFF => {
            info.cmd_id = HALMAC_PARAMETER_CMD_END;
        }
        _ => return -1,
    }
    0
}

#[cfg(not(host_hal_halmac_enum_test))]
#[no_mangle]
pub extern "C" fn _fw_mem_drv2halmac(mem: u32, tx_stop: U8) -> u32 {
    fw_mem_drv2halmac_impl(mem, tx_stop)
}

#[cfg(not(host_hal_halmac_enum_test))]
#[no_mangle]
pub extern "C" fn _led_mode_drv2halmac(drv_mode: U8) -> u32 {
    led_mode_drv2halmac_impl(drv_mode)
}

#[cfg(not(host_hal_halmac_enum_test))]
#[no_mangle]
pub extern "C" fn _phy_parameter_val_drv2halmac(val: u32, msk_en: U8, msk: u32) -> u32 {
    phy_parameter_val_drv2halmac_impl(val, msk_en, msk)
}

#[cfg(not(host_hal_halmac_enum_test))]
#[no_mangle]
pub extern "C" fn _phy_parameter_drv2halmac(
    para: *mut RtwPhyParameter,
    info: *mut HalmacPhyParameterInfo,
) -> i32 {
    phy_parameter_drv2halmac_impl(para, info)
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn fw_mem_drv2halmac(mem: u32, tx_stop: U8) -> u32 {
    fw_mem_drv2halmac_impl(mem, tx_stop)
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn led_mode_drv2halmac(drv_mode: U8) -> u32 {
    led_mode_drv2halmac_impl(drv_mode)
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn phy_parameter_val_drv2halmac(val: u32, msk_en: U8, msk: u32) -> u32 {
    phy_parameter_val_drv2halmac_impl(val, msk_en, msk)
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn phy_parameter_drv2halmac(
    para: *const RtwPhyParameter,
    info: *mut HalmacPhyParameterInfo,
) -> i32 {
    phy_parameter_drv2halmac_impl(para, info)
}

const HALMAC_RX_FIFO_EXPANDING_MODE_DISABLE: u32 = 0;
const HALMAC_RX_FIFO_EXPANDING_MODE_1_BLOCK: u32 = 1;
const HALMAC_RX_FIFO_EXPANDING_MODE_2_BLOCK: u32 = 2;
const HALMAC_RX_FIFO_EXPANDING_MODE_3_BLOCK: u32 = 3;

fn trx_share_mode_drv2halmac_impl(trx_share_mode: U8) -> u32 {
    match trx_share_mode {
        0 => HALMAC_RX_FIFO_EXPANDING_MODE_DISABLE,
        1 => HALMAC_RX_FIFO_EXPANDING_MODE_1_BLOCK,
        2 => HALMAC_RX_FIFO_EXPANDING_MODE_2_BLOCK,
        3 => HALMAC_RX_FIFO_EXPANDING_MODE_3_BLOCK,
        _ => HALMAC_RX_FIFO_EXPANDING_MODE_DISABLE,
    }
}

fn format_trx_share_dump(trx_share_mode: U8, out: &mut [u8]) -> i32 {
    let mode = trx_share_mode_drv2halmac_impl(trx_share_mode);
    let label: &[u8] = match mode {
        HALMAC_RX_FIFO_EXPANDING_MODE_1_BLOCK => b"RX_FIFO_EXPANDING_MODE_1",
        HALMAC_RX_FIFO_EXPANDING_MODE_2_BLOCK => b"RX_FIFO_EXPANDING_MODE_2",
        HALMAC_RX_FIFO_EXPANDING_MODE_3_BLOCK => b"RX_FIFO_EXPANDING_MODE_3",
        _ => b"DISABLE",
    };
    let prefix = b"TRx share mode : ";
    let mut pos = 0usize;

    if out.is_empty() {
        return -1;
    }
    for &b in prefix {
        if pos >= out.len() {
            return -1;
        }
        out[pos] = b;
        pos += 1;
    }
    for &b in label {
        if pos >= out.len() {
            return -1;
        }
        out[pos] = b;
        pos += 1;
    }
    if pos >= out.len() {
        return -1;
    }
    out[pos] = b'\n';
    pos += 1;
    pos as i32
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn trx_share_mode_drv2halmac(trx_share_mode: U8) -> u32 {
    trx_share_mode_drv2halmac_impl(trx_share_mode)
}

#[cfg(host_hal_halmac_enum_test)]
#[no_mangle]
pub extern "C" fn dump_trx_share_mode_format(
    trx_share_mode: U8,
    out: *mut u8,
    buflen: usize,
) -> i32 {
    if out.is_null() || buflen == 0 {
        return -1;
    }
    let buf = unsafe { core::slice::from_raw_parts_mut(out, buflen) };
    let n = format_trx_share_dump(trx_share_mode, buf);
    if n > 0 && (n as usize) < buflen {
        buf[n as usize] = 0;
    }
    n
}

#[cfg(all(not(host_hal_halmac_enum_test), CONFIG_SUPPORT_TRX_SHARED))]
mod trx_share_kernel {
    use super::*;
    use core::ffi::{c_char, c_void};

    extern "C" {
        fn rtw_rust_hal_halmac_print_sel(sel: *mut c_void, line: *const c_char);
        fn rtw_rust_hal_halmac_trx_share_mode(adapter: *mut c_void, out: *mut U8);
    }

    fn print_line(sel: *mut c_void, line: &[u8]) {
        let mut buf = [0i8; 128];
        let n = core::cmp::min(line.len(), buf.len().saturating_sub(1));
        for i in 0..n {
            buf[i] = line[i] as i8;
        }
        unsafe {
            rtw_rust_hal_halmac_print_sel(sel, buf.as_ptr());
        }
    }

    #[no_mangle]
    pub extern "C" fn _trx_share_mode_drv2halmac(trx_share_mode: U8) -> u32 {
        trx_share_mode_drv2halmac_impl(trx_share_mode)
    }

    #[no_mangle]
    pub extern "C" fn dump_trx_share_mode(sel: *mut c_void, adapter: *mut c_void) {
        let mut trx_share_mode = 0u8;
        unsafe {
            rtw_rust_hal_halmac_trx_share_mode(adapter, &mut trx_share_mode);
        }
        let mut buf = [0u8; 128];
        let n = format_trx_share_dump(trx_share_mode, &mut buf);
        if n > 0 {
            print_line(sel, &buf[..n as usize]);
        }
    }
}

#[repr(C)]
struct HalmacVer {
    major_ver: U8,
    prototype_ver: U8,
    minor_ver: U8,
}

fn append_byte(buf: &mut [u8], pos: &mut usize, b: u8) {
    if *pos < buf.len() {
        buf[*pos] = b;
        *pos += 1;
    }
}

fn append_u8(buf: &mut [u8], pos: &mut usize, n: u8) {
    let mut digits = [0u8; 3];
    let mut len = 0usize;
    let mut v = n;
    loop {
        digits[len] = b'0' + (v % 10);
        len += 1;
        v /= 10;
        if v == 0 {
            break;
        }
    }
    while len > 0 {
        len -= 1;
        append_byte(buf, pos, digits[len]);
    }
}

fn append_u8_2(buf: &mut [u8], pos: &mut usize, n: u8) {
    if n < 10 {
        append_byte(buf, pos, b'0');
    }
    append_u8(buf, pos, n);
}

fn snprintf_halmac_version(major: U8, prototype: U8, minor: U8, buf: &mut [u8]) {
    let mut pos = 0usize;
    append_byte(buf, &mut pos, b'V');
    append_u8(buf, &mut pos, major);
    append_byte(buf, &mut pos, b'_');
    append_u8_2(buf, &mut pos, prototype);
    append_byte(buf, &mut pos, b'_');
    append_u8_2(buf, &mut pos, minor);
    if !buf.is_empty() {
        buf[core::cmp::min(pos, buf.len() - 1)] = 0;
    }
}

#[cfg(any(not(host_hal_halmac_enum_test), host_hal_halmac_version_test))]
extern "C" {
    fn halmac_get_version(version: *mut HalmacVer) -> u32;
}

#[cfg(any(not(host_hal_halmac_enum_test), host_hal_halmac_version_test))]
#[no_mangle]
pub extern "C" fn rtw_halmac_get_version(str: *mut u8, len: u32) {
    if str.is_null() || len == 0 {
        return;
    }
    let mut ver = HalmacVer {
        major_ver: 0,
        prototype_ver: 0,
        minor_ver: 0,
    };
    if unsafe { halmac_get_version(&mut ver) } != 0 {
        return;
    }
    let buf = unsafe { core::slice::from_raw_parts_mut(str, len as usize) };
    snprintf_halmac_version(ver.major_ver, ver.prototype_ver, ver.minor_ver, buf);
}
