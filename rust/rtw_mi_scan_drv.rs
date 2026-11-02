// SPDX-License-Identifier: GPL-2.0
//! W3-199 mi scan abort + drv thread control (host L2 + kernel leaf).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_snake_case,
    unreachable_pub
)]

const _TRUE: u8 = 1;
const _SUCCESS: u32 = 1;
const _FAIL: u32 = 0;

#[cfg(host_mi_scan_drv_test)]
#[repr(C)]
pub struct ScanDrvDvobj {
    pub iface_nums: u8,
    pub padapters: [*mut ScanDrvAdapter; 4],
}

#[cfg(host_mi_scan_drv_test)]
#[repr(C)]
pub struct ScanDrvAdapter {
    pub iface_id: u8,
    pub adapter_up: u8,
    pub start_fail: u8,
    pub dvobj: *mut ScanDrvDvobj,
}

#[cfg(host_mi_scan_drv_test)]
type ScanDrvOp = unsafe extern "C" fn(*mut ScanDrvAdapter, *mut core::ffi::c_void) -> u8;

#[cfg(host_mi_scan_drv_test)]
extern "C" {
    pub fn rtw_scan_abort(adapter: *mut ScanDrvAdapter);
    pub fn rtw_scan_abort_no_wait(adapter: *mut ScanDrvAdapter);
    pub fn rtw_start_drv_threads(adapter: *mut ScanDrvAdapter) -> u32;
    pub fn rtw_stop_drv_threads(adapter: *mut ScanDrvAdapter);
}

#[cfg(host_mi_scan_drv_test)]
unsafe fn mi_process_host(
    pad: *mut ScanDrvAdapter,
    exclude_self: i32,
    data: *mut core::ffi::c_void,
    op: ScanDrvOp,
) -> u8 {
    let dv = &*(*pad).dvobj;
    let mut ret = 0u8;
    for i in 0..dv.iface_nums as usize {
        let p = dv.padapters[i];
        if p.is_null() {
            continue;
        }
        let iface = &*p;
        if iface.adapter_up == 0 {
            continue;
        }
        if exclude_self != 0 && std::ptr::eq(p, pad) {
            continue;
        }
        if op(p, data) == _TRUE {
            ret += 1;
        }
    }
    ret
}

#[cfg(host_mi_scan_drv_test)]
unsafe extern "C" fn op_scan_abort_host(
    a: *mut ScanDrvAdapter,
    data: *mut core::ffi::c_void,
) -> u8 {
    let bwait = *(data as *const u8);
    if bwait != 0 {
        rtw_scan_abort(a);
    } else {
        rtw_scan_abort_no_wait(a);
    }
    _TRUE
}

#[cfg(host_mi_scan_drv_test)]
unsafe fn start_drv_host(adapter: *mut ScanDrvAdapter, exclude_self: bool) -> u32 {
    let dv = &*(*adapter).dvobj;
    let mut status = _SUCCESS;
    for i in 0..dv.iface_nums as usize {
        let p = dv.padapters[i];
        if p.is_null() {
            continue;
        }
        if exclude_self && std::ptr::eq(p, adapter) {
            continue;
        }
        if rtw_start_drv_threads(p) == _FAIL {
            status = _FAIL;
            break;
        }
    }
    status
}

#[cfg(host_mi_scan_drv_test)]
unsafe fn stop_drv_host(adapter: *mut ScanDrvAdapter, exclude_self: bool) {
    let dv = &*(*adapter).dvobj;
    for i in 0..dv.iface_nums as usize {
        let p = dv.padapters[i];
        if p.is_null() {
            continue;
        }
        if exclude_self && std::ptr::eq(p, adapter) {
            continue;
        }
        rtw_stop_drv_threads(p);
    }
}

#[cfg(host_mi_scan_drv_test)]
#[no_mangle]
pub unsafe extern "C" fn mi_rust_call_scan_drv(
    pad: *mut ScanDrvAdapter,
    fn_id: i32,
    bwait: i32,
) -> u32 {
    let bw = if bwait != 0 { 1u8 } else { 0u8 };
    match fn_id {
        0 => {
            let mut in_data = bw;
            mi_process_host(
                pad,
                0,
                &mut in_data as *mut u8 as *mut core::ffi::c_void,
                op_scan_abort_host,
            ) as u32
        }
        1 => {
            let mut in_data = bw;
            mi_process_host(
                pad,
                1,
                &mut in_data as *mut u8 as *mut core::ffi::c_void,
                op_scan_abort_host,
            ) as u32
        }
        2 => start_drv_host(pad, false),
        3 => start_drv_host(pad, true),
        4 => {
            stop_drv_host(pad, false);
            0
        }
        5 => {
            stop_drv_host(pad, true);
            0
        }
        _ => 0xff,
    }
}

#[cfg(rust_mi_scan_drv_leaf)]
mod kernel {
    use core::ffi::c_void;

    pub type Padapter = *mut c_void;

    extern "C" {
        pub fn rtw_rust_mi_iface_nums(padapter: Padapter) -> i32;
        pub fn rtw_rust_mi_iface_at(padapter: Padapter, idx: i32) -> Padapter;
        pub fn rtw_rust_mi_is_adapter_up(iface: Padapter) -> u8;
        pub fn rtw_rust_mi_scan_abort(iface: Padapter, bwait: u8);
        pub fn rtw_rust_mi_start_drv_threads(iface: Padapter) -> u32;
        pub fn rtw_rust_mi_stop_drv_threads(iface: Padapter);
    }
}

#[cfg(rust_mi_scan_drv_leaf)]
type ScanKernOp = unsafe fn(kernel::Padapter, *mut core::ffi::c_void) -> u8;

#[cfg(rust_mi_scan_drv_leaf)]
unsafe fn mi_process_kern(
    padapter: kernel::Padapter,
    exclude_self: bool,
    data: *mut core::ffi::c_void,
    op: ScanKernOp,
) -> u8 {
    let n = unsafe { kernel::rtw_rust_mi_iface_nums(padapter) };
    let mut ret = 0u8;
    for i in 0..n {
        let iface = unsafe { kernel::rtw_rust_mi_iface_at(padapter, i) };
        if iface.is_null() {
            continue;
        }
        if unsafe { kernel::rtw_rust_mi_is_adapter_up(iface) } == 0 {
            continue;
        }
        if exclude_self && core::ptr::eq(iface, padapter) {
            continue;
        }
        if unsafe { op(iface, data) } == _TRUE {
            ret += 1;
        }
    }
    ret
}

#[cfg(rust_mi_scan_drv_leaf)]
unsafe fn op_scan_abort_kern(iface: kernel::Padapter, data: *mut core::ffi::c_void) -> u8 {
    unsafe {
        let bwait = *(data as *const u8);
        kernel::rtw_rust_mi_scan_abort(iface, bwait);
    }
    _TRUE
}

#[cfg(rust_mi_scan_drv_leaf)]
unsafe fn start_drv_kern(adapter: kernel::Padapter, exclude_self: bool) -> u32 {
    let n = unsafe { kernel::rtw_rust_mi_iface_nums(adapter) };
    let mut status = _SUCCESS;
    for i in 0..n {
        let iface = unsafe { kernel::rtw_rust_mi_iface_at(adapter, i) };
        if iface.is_null() {
            continue;
        }
        if exclude_self && core::ptr::eq(iface, adapter) {
            continue;
        }
        if unsafe { kernel::rtw_rust_mi_start_drv_threads(iface) } == _FAIL {
            status = _FAIL;
            break;
        }
    }
    status
}

#[cfg(rust_mi_scan_drv_leaf)]
unsafe fn stop_drv_kern(adapter: kernel::Padapter, exclude_self: bool) {
    let n = unsafe { kernel::rtw_rust_mi_iface_nums(adapter) };
    for i in 0..n {
        let iface = unsafe { kernel::rtw_rust_mi_iface_at(adapter, i) };
        if iface.is_null() {
            continue;
        }
        if exclude_self && core::ptr::eq(iface, adapter) {
            continue;
        }
        unsafe { kernel::rtw_rust_mi_stop_drv_threads(iface) };
    }
}

#[cfg(rust_mi_scan_drv_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_mi_scan_abort(adapter: kernel::Padapter, bwait: u8) {
    let mut in_data = bwait;
    unsafe {
        mi_process_kern(
            adapter,
            false,
            &mut in_data as *mut u8 as *mut core::ffi::c_void,
            op_scan_abort_kern,
        );
    }
}

#[cfg(rust_mi_scan_drv_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_mi_buddy_scan_abort(adapter: kernel::Padapter, bwait: u8) {
    let mut in_data = bwait;
    unsafe {
        mi_process_kern(
            adapter,
            true,
            &mut in_data as *mut u8 as *mut core::ffi::c_void,
            op_scan_abort_kern,
        );
    }
}

#[cfg(rust_mi_scan_drv_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_mi_start_drv_threads(adapter: kernel::Padapter) -> u32 {
    unsafe { start_drv_kern(adapter, false) }
}

#[cfg(rust_mi_scan_drv_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_mi_buddy_start_drv_threads(adapter: kernel::Padapter) -> u32 {
    unsafe { start_drv_kern(adapter, true) }
}

#[cfg(rust_mi_scan_drv_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_mi_stop_drv_threads(adapter: kernel::Padapter) {
    unsafe { stop_drv_kern(adapter, false) }
}

#[cfg(rust_mi_scan_drv_leaf)]
#[no_mangle]
pub unsafe extern "C" fn rtw_mi_buddy_stop_drv_threads(adapter: kernel::Padapter) {
    unsafe { stop_drv_kern(adapter, true) }
}
