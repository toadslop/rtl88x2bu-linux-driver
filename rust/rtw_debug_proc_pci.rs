// SPDX-License-Identifier: GPL-2.0
// W3-231: PCI conf space proc setters (was core/rtw_debug_rest.c, CONFIG_PCI_HCI).

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

use core::ffi::{c_char, c_int, c_long, c_uint, c_void};

const EFAULT: c_long = -14;
const PROC_SET_TMP: usize = 32;

extern "C" {
    fn rtw_rust_debug_proc_adapter_from_data(data: *mut c_void) -> *mut c_void;
    fn rtw_rust_debug_proc_read_user_line(
        buffer: *const c_void,
        count: usize,
        tmp: *mut i8,
        tmp_len: usize,
    ) -> c_long;
}

#[cfg(any(config_pci_hci, host_rtw_debug_proc_pci_test))]
extern "C" {
    fn rtw_rust_debug_proc_sscanf_x32_pair(
        tmp: *const u8,
        first: *mut c_uint,
        second: *mut c_uint,
    ) -> c_int;
    fn rtw_rust_pci_write_config_byte(
        adapter: *mut c_void,
        use_bridge: c_int,
        reg: c_uint,
        val: u8,
    );
    fn rtw_rust_debug_proc_log_pci_reg_set(adapter: *mut c_void, reg: c_uint, val: c_uint);
    fn rtw_rust_debug_log_info(line: *const c_char);
}

fn log_info(bytes: &[u8]) {
    let mut buf = [0i8; 64];
    let n = core::cmp::min(bytes.len(), buf.len().saturating_sub(1));
    buf[..n].copy_from_slice(unsafe {
        core::slice::from_raw_parts(bytes.as_ptr() as *const i8, n)
    });
    unsafe { rtw_rust_debug_log_info(buf.as_ptr()) };
}

fn proc_set_read_tmp(
    buffer: *const c_void,
    count: usize,
    tmp: &mut [i8; PROC_SET_TMP],
) -> bool {
    if count < 1 || count > PROC_SET_TMP || buffer.is_null() {
        return false;
    }
    let n = unsafe {
        rtw_rust_debug_proc_read_user_line(buffer, count, tmp.as_mut_ptr(), tmp.len())
    };
    n > 0
}

#[cfg(any(config_pci_hci, host_rtw_debug_proc_pci_test))]
fn apply_pci_conf_set(
    adapter: *mut c_void,
    buffer: *const c_void,
    count: usize,
    use_bridge: bool,
) -> c_long {
    if count < 1 {
        return EFAULT;
    }
    if count > PROC_SET_TMP {
        return EFAULT;
    }
    if adapter.is_null() {
        return EFAULT;
    }
    let mut tmp = [0i8; PROC_SET_TMP];
    if !proc_set_read_tmp(buffer, count, &mut tmp) {
        return count as c_long;
    }
    let mut reg = 0u32;
    let mut value = 0u32;
    let num = unsafe {
        rtw_rust_debug_proc_sscanf_x32_pair(
            tmp.as_ptr() as *const u8,
            &mut reg,
            &mut value,
        )
    };
    if num != 2 {
        log_info(b"invalid parameter!\n");
        return count as c_long;
    }
    if reg >= 0x1000 {
        log_info(b"invalid register!\n");
        return count as c_long;
    }
    if value > 0xff {
        log_info(b"invalid value! Only one byte\n");
        return count as c_long;
    }
    unsafe {
        rtw_rust_debug_proc_log_pci_reg_set(adapter, reg, value);
        rtw_rust_pci_write_config_byte(adapter, i32::from(use_bridge), reg, value as u8);
    }
    count as c_long
}

#[cfg(any(config_pci_hci, host_rtw_debug_proc_pci_test))]
#[cfg(any(config_rtw_debug, config_proc_debug, host_rtw_debug_proc_pci_test))]
#[no_mangle]
pub extern "C" fn proc_set_pci_bridge_conf_space(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    apply_pci_conf_set(adapter, buffer, count, true)
}

#[cfg(any(config_pci_hci, host_rtw_debug_proc_pci_test))]
#[cfg(any(config_rtw_debug, config_proc_debug, host_rtw_debug_proc_pci_test))]
#[no_mangle]
pub extern "C" fn proc_set_pci_conf_space(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    apply_pci_conf_set(adapter, buffer, count, false)
}
