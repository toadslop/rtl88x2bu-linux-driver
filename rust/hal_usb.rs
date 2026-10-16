// SPDX-License-Identifier: GPL-2.0
//! HAL USB HCI — `usb_set_intf_ops` and vendor wvalue fixup from `hal/hal_hci/hal_usb.c`
//! (W4-25, W4-28).

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

#[cfg(not(host_hal_usb_wvalue_test))]
use core::ffi::c_void;

#[cfg(not(host_hal_usb_wvalue_test))]
type Hdl = *mut c_void;
type U16 = u16;

/// WLANON PAGE0_REG wvalue offset for RTL8710B USB vendor requests.
#[no_mangle]
pub extern "C" fn usb_vendor_wvalue_fixup(wvalue: U16) -> U16 {
    #[cfg(any(CONFIG_RTL8710B, host_hal_usb_wvalue_test))]
    if wvalue < 0x0100 {
        return wvalue | 0x8000;
    }
    wvalue
}

#[cfg(not(host_hal_usb_wvalue_test))]
#[repr(C)]
struct IoOps {
    _read8: Option<unsafe extern "C" fn(Hdl, u32) -> u8>,
    _read16: Option<unsafe extern "C" fn(Hdl, u32) -> u16>,
    _read32: Option<unsafe extern "C" fn(Hdl, u32) -> u32>,
    _write8: Option<unsafe extern "C" fn(Hdl, u32, u8) -> i32>,
    _write16: Option<unsafe extern "C" fn(Hdl, u32, u16) -> i32>,
    _write32: Option<unsafe extern "C" fn(Hdl, u32, u32) -> i32>,
    _writeN: Option<unsafe extern "C" fn(Hdl, u32, u32, *mut u8) -> i32>,
    _write8_async: Option<unsafe extern "C" fn(Hdl, u32, u8) -> i32>,
    _write16_async: Option<unsafe extern "C" fn(Hdl, u32, u16) -> i32>,
    _write32_async: Option<unsafe extern "C" fn(Hdl, u32, u32) -> i32>,
    _read_mem: Option<unsafe extern "C" fn(Hdl, u32, u32, *mut u8)>,
    _write_mem: Option<unsafe extern "C" fn(Hdl, u32, u32, *mut u8)>,
    _sync_irp_protocol_rw: Option<unsafe extern "C" fn(*mut c_void)>,
    _read_interrupt: Option<unsafe extern "C" fn(Hdl, u32) -> u32>,
    _read_port: Option<unsafe extern "C" fn(Hdl, u32, u32, *mut u8) -> u32>,
    _write_port: Option<unsafe extern "C" fn(Hdl, u32, u32, *mut u8) -> u32>,
    _write_scsi: Option<unsafe extern "C" fn(Hdl, u32, *mut u8) -> u32>,
    _read_port_cancel: Option<unsafe extern "C" fn(Hdl)>,
    _write_port_cancel: Option<unsafe extern "C" fn(Hdl)>,
}

#[cfg(not(host_hal_usb_wvalue_test))]
unsafe extern "C" {
    fn usb_read8(pintfhdl: Hdl, addr: u32) -> u8;
    fn usb_read16(pintfhdl: Hdl, addr: u32) -> u16;
    fn usb_read32(pintfhdl: Hdl, addr: u32) -> u32;
    fn usb_read_mem(pintfhdl: Hdl, addr: u32, cnt: u32, pmem: *mut u8);
    fn usb_read_port(pintfhdl: Hdl, addr: u32, cnt: u32, pmem: *mut u8) -> u32;
    fn usb_write8(pintfhdl: Hdl, addr: u32, val: u8) -> i32;
    fn usb_write16(pintfhdl: Hdl, addr: u32, val: u16) -> i32;
    fn usb_write32(pintfhdl: Hdl, addr: u32, val: u32) -> i32;
    fn usb_writeN(pintfhdl: Hdl, addr: u32, length: u32, pdata: *mut u8) -> i32;
    fn usb_write_mem(pintfhdl: Hdl, addr: u32, cnt: u32, pmem: *mut u8);
    fn usb_write_port(pintfhdl: Hdl, addr: u32, cnt: u32, pmem: *mut u8) -> u32;
    fn usb_read_port_cancel(pintfhdl: Hdl);
    fn usb_write_port_cancel(pintfhdl: Hdl);
}

#[cfg(not(host_hal_usb_wvalue_test))]
#[no_mangle]
pub extern "C" fn usb_set_intf_ops(_padapter: *mut c_void, pops: *mut c_void) {
    if pops.is_null() {
        return;
    }
    let pops = unsafe { &mut *(pops as *mut IoOps) };
    unsafe {
        core::ptr::write_bytes(
            pops as *mut IoOps as *mut u8,
            0,
            core::mem::size_of::<IoOps>(),
        );
    }

    pops._read8 = Some(usb_read8);
    pops._read16 = Some(usb_read16);
    pops._read32 = Some(usb_read32);
    pops._read_mem = Some(usb_read_mem);
    pops._read_port = Some(usb_read_port);
    pops._write8 = Some(usb_write8);
    pops._write16 = Some(usb_write16);
    pops._write32 = Some(usb_write32);
    pops._writeN = Some(usb_writeN);
    pops._write_mem = Some(usb_write_mem);
    pops._write_port = Some(usb_write_port);
    pops._read_port_cancel = Some(usb_read_port_cancel);
    pops._write_port_cancel = Some(usb_write_port_cancel);
}
