// SPDX-License-Identifier: GPL-2.0
//! HAL USB HCI — `usb_set_intf_ops`, vendor wvalue fixup, and sync reads from `hal/hal_hci/hal_usb.c`
//! (W4-25, W4-28, W4-33).

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

type U16 = u16;

/// WLANON PAGE0_REG wvalue offset for RTL8710B USB vendor requests.
#[no_mangle]
pub extern "C" fn usb_vendor_wvalue_fixup(wvalue: U16) -> U16 {
    #[cfg(any(CONFIG_RTL8710B, host_hal_usb_wvalue_test, host_hal_usb_read_test))]
    if wvalue < 0x0100 {
        return wvalue | 0x8000;
    }
    wvalue
}

#[cfg(host_hal_usb_read_test)]
mod sync_read {
    pub type Hdl = *mut u8;

    unsafe extern "C" {
        fn usbctrl_vendorreq(
            pintfhdl: Hdl,
            request: u8,
            value: u16,
            index: u16,
            pdata: *mut u8,
            len: u16,
            requesttype: u8,
        ) -> i32;
    }

    unsafe fn usb_sync_vendor_read(pintfhdl: Hdl, addr: u32, len: u16, data: *mut u8) {
        let request: u8 = 0x05;
        let requesttype: u8 = 0x01;
        let index: u16 = 0;
        let wvalue = super::usb_vendor_wvalue_fixup((addr & 0x0000ffff) as u16);
        unsafe {
            usbctrl_vendorreq(pintfhdl, request, wvalue, index, data, len, requesttype);
        }
    }

    #[no_mangle]
    pub extern "C" fn usb_read8(pintfhdl: Hdl, addr: u32) -> u8 {
        let mut data: u8 = 0;
        unsafe {
            usb_sync_vendor_read(pintfhdl, addr, 1, &mut data as *mut u8);
        }
        data
    }

    #[no_mangle]
    pub extern "C" fn usb_read16(pintfhdl: Hdl, addr: u32) -> u16 {
        let mut data: u16 = 0;
        unsafe {
            usb_sync_vendor_read(
                pintfhdl,
                addr,
                2,
                &mut data as *mut u16 as *mut u8,
            );
        }
        data
    }

    #[no_mangle]
    pub extern "C" fn usb_read32(pintfhdl: Hdl, addr: u32) -> u32 {
        let mut data: u32 = 0;
        unsafe {
            usb_sync_vendor_read(
                pintfhdl,
                addr,
                4,
                &mut data as *mut u32 as *mut u8,
            );
        }
        data
    }
}

#[cfg(not(any(host_hal_usb_wvalue_test, host_hal_usb_read_test)))]
mod kernel {
    use core::ffi::c_void;

    pub type Hdl = *mut c_void;

    unsafe extern "C" {
        fn usbctrl_vendorreq(
            pintfhdl: Hdl,
            request: u8,
            value: u16,
            index: u16,
            pdata: *mut c_void,
            len: u16,
            requesttype: u8,
        ) -> i32;
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

    unsafe fn usb_sync_vendor_read(pintfhdl: Hdl, addr: u32, len: u16, data: *mut c_void) {
        let request: u8 = 0x05;
        let requesttype: u8 = 0x01;
        let index: u16 = 0;
        let wvalue = super::usb_vendor_wvalue_fixup((addr & 0x0000ffff) as u16);
        unsafe {
            usbctrl_vendorreq(pintfhdl, request, wvalue, index, data, len, requesttype);
        }
    }

    #[no_mangle]
    pub extern "C" fn usb_read8(pintfhdl: Hdl, addr: u32) -> u8 {
        let mut data: u8 = 0;
        unsafe {
            usb_sync_vendor_read(pintfhdl, addr, 1, &mut data as *mut u8 as *mut c_void);
        }
        data
    }

    #[no_mangle]
    pub extern "C" fn usb_read16(pintfhdl: Hdl, addr: u32) -> u16 {
        let mut data: u16 = 0;
        unsafe {
            usb_sync_vendor_read(pintfhdl, addr, 2, &mut data as *mut u16 as *mut c_void);
        }
        data
    }

    #[no_mangle]
    pub extern "C" fn usb_read32(pintfhdl: Hdl, addr: u32) -> u32 {
        let mut data: u32 = 0;
        unsafe {
            usb_sync_vendor_read(pintfhdl, addr, 4, &mut data as *mut u32 as *mut c_void);
        }
        data
    }

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
}
