// SPDX-License-Identifier: GPL-2.0
//! I/O rest helpers — Rust port of `core/rtw_io_rest.c` slices (W3-18).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_io_rest_test)]
use std::os::raw::{c_int, c_void};

#[cfg(not(host_io_rest_test))]
use core::ffi::{c_int, c_void};

const _TRUE: c_int = 1;
const _FALSE: c_int = 0;
const MAX_CONTINUAL_IO_ERR: c_int = 4;
const RTW_IO_SNIFF_TYPE_RANGE: u8 = 0;
const RTW_IO_SNIFF_TYPE_VALUE: u8 = 1;

#[cfg(host_io_rest_test)]
const MAX_CHIP_TYPE: u8 = 255;
#[cfg(host_io_rest_test)]
const MAX_RF_PATH: u8 = 4;
#[cfg(host_io_rest_test)]
const RTW_USB: u8 = 2;

#[repr(C)]
#[derive(Copy, Clone)]
union SniffUnion {
    end_addr: u32,
    vm: SniffValueMatch,
}

#[repr(C)]
#[derive(Copy, Clone)]
struct SniffValueMatch {
    mask: u32,
    val: u32,
    equal: bool,
}

#[repr(C)]
struct RtwIoSniffEnt {
    chip: u8,
    hci: u8,
    addr: u32,
    ent_type: u8,
    u: SniffUnion,
    trace: bool,
    tag: *const u8,
    assert_protsel: Option<AssertProtselFn>,
}

#[repr(C)]
struct RfSniffEnt {
    path: u8,
    reg: u16,
    mask: u32,
}

type AssertProtselFn = unsafe extern "C" fn(*mut c_void, u32, u8) -> bool;

#[cfg(host_io_rest_test)]
#[repr(C)]
pub struct HostDvobjPriv {
    pub chip_type: u8,
    pub interface_type: u8,
    pub continual_io_error: c_int,
}

#[cfg(host_io_rest_test)]
#[repr(C)]
pub struct HostIoAdapter {
    pub dvobj: *mut HostDvobjPriv,
}

#[cfg(not(host_io_rest_test))]
mod kernel {
    use super::*;

    extern "C" {
        fn rtw_rust_dvobj_continual_io_error(dvobj: *mut c_void) -> *mut c_int;
        fn rtw_rust_atomic_inc_return(v: *mut c_int) -> c_int;
        fn rtw_rust_atomic_set(v: *mut c_int, val: c_int);
        fn rtw_rust_get_chip_type(adapter: *mut c_void) -> u8;
        fn rtw_rust_get_intf_type(adapter: *mut c_void) -> u8;
        fn rtw_rust_io_warn_on(condition: c_int);
        fn rtw_rust_io_dbg_tag(tag: *const u8);
        fn rtw_rust_io_continual_io_error_log(dvobj: *mut c_void, value: c_int, max: c_int);
        fn rtw_rust_io_sniff_assert_protsel(
            f: Option<AssertProtselFn>,
            adapter: *mut c_void,
            addr: u32,
            len: u8,
        ) -> bool;
        fn rtw_rust_io_max_chip_type() -> u8;
        fn rtw_rust_io_max_rf_path() -> u8;

        static read_sniff: RtwIoSniffEnt;
        static read_sniff_num: c_int;
        static write_sniff: RtwIoSniffEnt;
        static write_sniff_num: c_int;
        static rf_read_sniff_ranges: RfSniffEnt;
        static rf_read_sniff_num: c_int;
        static rf_write_sniff_ranges: RfSniffEnt;
        static rf_write_sniff_num: c_int;
    }

    pub(super) fn max_chip_type() -> u8 {
        unsafe { rtw_rust_io_max_chip_type() }
    }

    pub(super) fn max_rf_path() -> u8 {
        unsafe { rtw_rust_io_max_rf_path() }
    }

    pub(super) fn chip_type(adapter: *mut c_void) -> u8 {
        unsafe { rtw_rust_get_chip_type(adapter) }
    }

    pub(super) fn intf_type(adapter: *mut c_void) -> u8 {
        unsafe { rtw_rust_get_intf_type(adapter) }
    }

    pub(super) fn warn_on(condition: bool) {
        unsafe { rtw_rust_io_warn_on(condition as c_int) };
    }

    pub(super) fn dbg_tag(tag: *const u8) {
        unsafe { rtw_rust_io_dbg_tag(tag) };
    }

    pub(super) fn continual_io_error_log(dvobj: *mut c_void, value: c_int, max: c_int) {
        unsafe { rtw_rust_io_continual_io_error_log(dvobj, value, max) };
    }

    pub(super) fn sniff_assert_protsel(
        f: Option<AssertProtselFn>,
        adapter: *mut c_void,
        addr: u32,
        len: u8,
    ) -> bool {
        unsafe { rtw_rust_io_sniff_assert_protsel(f, adapter, addr, len) }
    }

    pub(super) fn inc_continual_io_error(dvobj: *mut c_void) -> c_int {
        let atomic = unsafe { rtw_rust_dvobj_continual_io_error(dvobj) };
        unsafe { rtw_rust_atomic_inc_return(atomic) }
    }

    pub(super) fn reset_continual_io_error(dvobj: *mut c_void) {
        let atomic = unsafe { rtw_rust_dvobj_continual_io_error(dvobj) };
        unsafe { rtw_rust_atomic_set(atomic, 0) };
    }

    pub(super) fn read_sniff_table() -> (*const RtwIoSniffEnt, c_int) {
        unsafe { (core::ptr::addr_of!(read_sniff), read_sniff_num) }
    }

    pub(super) fn write_sniff_table() -> (*const RtwIoSniffEnt, c_int) {
        unsafe { (core::ptr::addr_of!(write_sniff), write_sniff_num) }
    }

    pub(super) fn rf_read_table() -> (*const RfSniffEnt, c_int) {
        unsafe {
            (
                core::ptr::addr_of!(rf_read_sniff_ranges),
                rf_read_sniff_num,
            )
        }
    }

    pub(super) fn rf_write_table() -> (*const RfSniffEnt, c_int) {
        unsafe {
            (
                core::ptr::addr_of!(rf_write_sniff_ranges),
                rf_write_sniff_num,
            )
        }
    }
}

#[cfg(host_io_rest_test)]
mod kernel {
    use super::*;

    static HOST_READ_SNIFF: [RtwIoSniffEnt; 9] = [
        RtwIoSniffEnt {
            chip: MAX_CHIP_TYPE,
            hci: 0,
            addr: 0x522,
            ent_type: RTW_IO_SNIFF_TYPE_RANGE,
            u: SniffUnion { end_addr: 0x522 },
            trace: false,
            tag: b"read TXPAUSE\0".as_ptr(),
            assert_protsel: None,
        },
        RtwIoSniffEnt {
            chip: MAX_CHIP_TYPE,
            hci: 0,
            addr: 0x02,
            ent_type: RTW_IO_SNIFF_TYPE_VALUE,
            u: SniffUnion {
                vm: SniffValueMatch {
                    mask: 0x3,
                    val: 0xFFFF_FFFF,
                    equal: false,
                },
            },
            trace: false,
            tag: b"0x02[1:0] not all 1\0".as_ptr(),
            assert_protsel: None,
        },
        RtwIoSniffEnt {
            chip: 1,
            hci: RTW_USB,
            addr: 0x600,
            ent_type: RTW_IO_SNIFF_TYPE_RANGE,
            u: SniffUnion { end_addr: 0x600 },
            trace: false,
            tag: b"host chip1 usb read\0".as_ptr(),
            assert_protsel: None,
        },
        RtwIoSniffEnt {
            chip: MAX_CHIP_TYPE,
            hci: 0,
            addr: 0x100,
            ent_type: RTW_IO_SNIFF_TYPE_VALUE,
            u: SniffUnion {
                vm: SniffValueMatch {
                    mask: 0xFF,
                    val: 0xAB,
                    equal: true,
                },
            },
            trace: false,
            tag: b"host read value equal len1\0".as_ptr(),
            assert_protsel: None,
        },
        RtwIoSniffEnt {
            chip: MAX_CHIP_TYPE,
            hci: 0,
            addr: 0x200,
            ent_type: RTW_IO_SNIFF_TYPE_VALUE,
            u: SniffUnion {
                vm: SniffValueMatch {
                    mask: 0xFFFF,
                    val: 0x1234,
                    equal: true,
                },
            },
            trace: false,
            tag: b"host read value equal len2\0".as_ptr(),
            assert_protsel: None,
        },
        RtwIoSniffEnt {
            chip: MAX_CHIP_TYPE,
            hci: 0,
            addr: 0x300,
            ent_type: RTW_IO_SNIFF_TYPE_VALUE,
            u: SniffUnion {
                vm: SniffValueMatch {
                    mask: 0xFFFF_FFFF,
                    val: 0x1234_5678,
                    equal: true,
                },
            },
            trace: false,
            tag: b"host read value equal len4\0".as_ptr(),
            assert_protsel: None,
        },
        RtwIoSniffEnt {
            chip: MAX_CHIP_TYPE,
            hci: 0,
            addr: 0x102,
            ent_type: RTW_IO_SNIFF_TYPE_VALUE,
            u: SniffUnion {
                vm: SniffValueMatch {
                    mask: 0xFF,
                    val: 0x42,
                    equal: true,
                },
            },
            trace: false,
            tag: b"host read unaligned len4 equal\0".as_ptr(),
            assert_protsel: None,
        },
        RtwIoSniffEnt {
            chip: MAX_CHIP_TYPE,
            hci: 0,
            addr: 0x201,
            ent_type: RTW_IO_SNIFF_TYPE_VALUE,
            u: SniffUnion {
                vm: SniffValueMatch {
                    mask: 0xFF,
                    val: 0x34,
                    equal: true,
                },
            },
            trace: false,
            tag: b"host read unaligned len2 equal\0".as_ptr(),
            assert_protsel: None,
        },
        RtwIoSniffEnt {
            chip: MAX_CHIP_TYPE,
            hci: 0,
            addr: 0x500,
            ent_type: RTW_IO_SNIFF_TYPE_VALUE,
            u: SniffUnion {
                vm: SniffValueMatch {
                    mask: 0xFF_0000,
                    val: 0x42_0000,
                    equal: true,
                },
            },
            trace: false,
            tag: b"host read negative mask_shift\0".as_ptr(),
            assert_protsel: None,
        },
    ];

    static HOST_WRITE_SNIFF: [RtwIoSniffEnt; 3] = [
        RtwIoSniffEnt {
            chip: MAX_CHIP_TYPE,
            hci: 0,
            addr: 0x522,
            ent_type: RTW_IO_SNIFF_TYPE_RANGE,
            u: SniffUnion { end_addr: 0x522 },
            trace: false,
            tag: b"write TXPAUSE\0".as_ptr(),
            assert_protsel: None,
        },
        RtwIoSniffEnt {
            chip: MAX_CHIP_TYPE,
            hci: 0,
            addr: 0x02,
            ent_type: RTW_IO_SNIFF_TYPE_VALUE,
            u: SniffUnion {
                vm: SniffValueMatch {
                    mask: 0x3,
                    val: 0xFFFF_FFFF,
                    equal: false,
                },
            },
            trace: false,
            tag: b"0x02[1:0] not all 1\0".as_ptr(),
            assert_protsel: None,
        },
        RtwIoSniffEnt {
            chip: MAX_CHIP_TYPE,
            hci: 0,
            addr: 0x400,
            ent_type: RTW_IO_SNIFF_TYPE_VALUE,
            u: SniffUnion {
                vm: SniffValueMatch {
                    mask: 0xFFFF,
                    val: 0x5600,
                    equal: true,
                },
            },
            trace: false,
            tag: b"host write value equal len2\0".as_ptr(),
            assert_protsel: None,
        },
    ];

    static HOST_RF_READ: [RfSniffEnt; 2] = [
        RfSniffEnt {
            path: 0,
            reg: 0x55,
            mask: 0xFF,
        },
        RfSniffEnt {
            path: MAX_RF_PATH,
            reg: 0x66,
            mask: 0x0F,
        },
    ];

    static HOST_RF_WRITE: [RfSniffEnt; 1] = [RfSniffEnt {
        path: 1,
        reg: 0x55,
        mask: 0xFF,
    }];

    pub(super) fn max_chip_type() -> u8 {
        MAX_CHIP_TYPE
    }

    pub(super) fn max_rf_path() -> u8 {
        MAX_RF_PATH
    }

    pub(super) fn chip_type(adapter: *mut c_void) -> u8 {
        unsafe { (*adapter.cast::<HostIoAdapter>()).dvobj.as_ref().map(|d| d.chip_type).unwrap_or(0) }
    }

    pub(super) fn intf_type(adapter: *mut c_void) -> u8 {
        unsafe {
            (*adapter.cast::<HostIoAdapter>())
                .dvobj
                .as_ref()
                .map(|d| d.interface_type)
                .unwrap_or(0)
        }
    }

    pub(super) fn warn_on(_condition: bool) {}

    pub(super) fn dbg_tag(_tag: *const u8) {}

    pub(super) fn continual_io_error_log(_dvobj: *mut c_void, _value: c_int, _max: c_int) {}

    pub(super) fn sniff_assert_protsel(
        _f: Option<AssertProtselFn>,
        _adapter: *mut c_void,
        _addr: u32,
        _len: u8,
    ) -> bool {
        false
    }

    pub(super) fn inc_continual_io_error(dvobj: *mut c_void) -> c_int {
        let dvobj = unsafe { &mut *dvobj.cast::<HostDvobjPriv>() };
        dvobj.continual_io_error += 1;
        dvobj.continual_io_error
    }

    pub(super) fn reset_continual_io_error(dvobj: *mut c_void) {
        let dvobj = unsafe { &mut *dvobj.cast::<HostDvobjPriv>() };
        dvobj.continual_io_error = 0;
    }

    pub(super) fn read_sniff_table() -> (*const RtwIoSniffEnt, c_int) {
        (HOST_READ_SNIFF.as_ptr(), HOST_READ_SNIFF.len() as c_int)
    }

    pub(super) fn write_sniff_table() -> (*const RtwIoSniffEnt, c_int) {
        (HOST_WRITE_SNIFF.as_ptr(), HOST_WRITE_SNIFF.len() as c_int)
    }

    pub(super) fn rf_read_table() -> (*const RfSniffEnt, c_int) {
        (HOST_RF_READ.as_ptr(), HOST_RF_READ.len() as c_int)
    }

    pub(super) fn rf_write_table() -> (*const RfSniffEnt, c_int) {
        (HOST_RF_WRITE.as_ptr(), HOST_RF_WRITE.len() as c_int)
    }
}

fn bitshift(bitmask: u32) -> u32 {
    let mut i = 0u32;
    while i <= 31 {
        if (bitmask >> i) & 0x1 == 1 {
            break;
        }
        i += 1;
    }
    i
}

fn match_io_sniff_ranges(
    adapter: *mut c_void,
    sniff: &RtwIoSniffEnt,
    addr: u32,
    len: u8,
) -> bool {
    if addr > unsafe { sniff.u.end_addr } {
        return false;
    }
    if let Some(f) = sniff.assert_protsel {
        if kernel::sniff_assert_protsel(Some(f), adapter, addr, len) {
            return false;
        }
    }
    true
}

fn match_io_sniff_value(sniff: &RtwIoSniffEnt, addr: u32, _len: u8, val: u32) -> bool {
    let vm = unsafe { sniff.u.vm };
    let mut sniff_len = 4u8;
    while sniff_len > 0 && (vm.mask & (0xFF << ((sniff_len - 1) * 8))) == 0 {
        sniff_len -= 1;
    }
    if sniff_len == 0 || sniff.addr + sniff_len as u32 <= addr {
        return false;
    }

    let mask_shift = ((sniff.addr as i32) - (addr as i32)) * 8;
    let value_shift = mask_shift + bitshift(vm.mask) as i32;
    let mask = if mask_shift > 0 {
        vm.mask << mask_shift
    } else if mask_shift < 0 {
        vm.mask >> -mask_shift
    } else {
        vm.mask
    };
    let value = if value_shift > 0 {
        vm.val << value_shift
    } else if value_shift < 0 {
        vm.val >> -value_shift
    } else {
        vm.val
    };

    (vm.equal && (mask & val) == (mask & value)) || (!vm.equal && (mask & val) != (mask & value))
}

fn match_io_sniff(
    adapter: *mut c_void,
    sniff: &RtwIoSniffEnt,
    addr: u32,
    len: u8,
    val: u32,
) -> bool {
    if sniff.chip != kernel::max_chip_type() && sniff.chip != kernel::chip_type(adapter) {
        return false;
    }
    if sniff.hci != 0 && (sniff.hci & kernel::intf_type(adapter)) == 0 {
        return false;
    }
    if sniff.addr >= addr + len as u32 {
        return false;
    }

    match sniff.ent_type {
        RTW_IO_SNIFF_TYPE_RANGE => match_io_sniff_ranges(adapter, sniff, addr, len),
        RTW_IO_SNIFF_TYPE_VALUE if len == 1 || len == 2 || len == 4 => {
            match_io_sniff_value(sniff, addr, len, val)
        }
        _ => {
            kernel::warn_on(true);
            false
        }
    }
}

fn match_sniff_table(
    adapter: *mut c_void,
    table: *const RtwIoSniffEnt,
    count: c_int,
    addr: u32,
    len: u16,
    val: u32,
) -> u32 {
    let mut trace = false;
    let mut matched = 0u32;
    if table.is_null() || count <= 0 {
        return 0;
    }
    let entries = unsafe { core::slice::from_raw_parts(table, count as usize) };
    for sniff in entries {
        if match_io_sniff(adapter, sniff, addr, len as u8, val) {
            matched += 1;
            trace |= sniff.trace;
            if !sniff.tag.is_null() {
                kernel::dbg_tag(sniff.tag);
            }
        }
    }
    kernel::warn_on(trace);
    matched
}

fn match_rf_sniff_table(
    table: *const RfSniffEnt,
    count: c_int,
    path: u8,
    addr: u32,
    mask: u32,
) -> bool {
    if table.is_null() || count <= 0 {
        return false;
    }
    let entries = unsafe { core::slice::from_raw_parts(table, count as usize) };
    for ent in entries {
        if (ent.path == kernel::max_rf_path() || ent.path == path)
            && addr == ent.reg as u32
            && (mask & ent.mask) != 0
        {
            return true;
        }
    }
    false
}

fn inc_and_chk_continual_io_error_inner(dvobj: *mut c_void) -> c_int {
    let value = kernel::inc_continual_io_error(dvobj);
    if value > MAX_CONTINUAL_IO_ERR {
        kernel::continual_io_error_log(dvobj, value, MAX_CONTINUAL_IO_ERR);
        _TRUE
    } else {
        _FALSE
    }
}

#[no_mangle]
pub extern "C" fn rtw_inc_and_chk_continual_io_error(dvobj: *mut c_void) -> c_int {
    if dvobj.is_null() {
        return _FALSE;
    }
    inc_and_chk_continual_io_error_inner(dvobj)
}

#[no_mangle]
pub extern "C" fn rtw_reset_continual_io_error(dvobj: *mut c_void) {
    if dvobj.is_null() {
        return;
    }
    kernel::reset_continual_io_error(dvobj);
}

#[cfg(any(dbg_io, host_io_rest_test))]
#[no_mangle]
pub extern "C" fn match_read_sniff(adapter: *mut c_void, addr: u32, len: u16, val: u32) -> u32 {
    let (table, count) = kernel::read_sniff_table();
    match_sniff_table(adapter, table, count, addr, len, val)
}

#[cfg(any(dbg_io, host_io_rest_test))]
#[no_mangle]
pub extern "C" fn match_write_sniff(adapter: *mut c_void, addr: u32, len: u16, val: u32) -> u32 {
    let (table, count) = kernel::write_sniff_table();
    match_sniff_table(adapter, table, count, addr, len, val)
}

#[cfg(any(dbg_io, host_io_rest_test))]
#[no_mangle]
pub extern "C" fn match_rf_read_sniff_ranges(
    _adapter: *mut c_void,
    path: u8,
    addr: u32,
    mask: u32,
) -> bool {
    let (table, count) = kernel::rf_read_table();
    match_rf_sniff_table(table, count, path, addr, mask)
}

#[cfg(any(dbg_io, host_io_rest_test))]
#[no_mangle]
pub extern "C" fn match_rf_write_sniff_ranges(
    _adapter: *mut c_void,
    path: u8,
    addr: u32,
    mask: u32,
) -> bool {
    let (table, count) = kernel::rf_write_table();
    match_rf_sniff_table(table, count, path, addr, mask)
}

#[no_mangle]
pub extern "C" fn rtw_rust_io_rest_probe() -> c_int {
    0
}

#[cfg(host_io_rest_test)]
unsafe impl Sync for RtwIoSniffEnt {}
