// SPDX-License-Identifier: GPL-2.0
//! W3-202 mi timer cancel + intf lifecycle (host L2 + kernel leaf).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_snake_case,
    unreachable_pub
)]

const _TRUE: u8 = 1;

#[cfg(host_mi_timer_intf_test)]
#[repr(C)]
pub struct TimerIntfDvobj {
    pub iface_nums: u8,
    pub padapters: [*mut TimerIntfAdapter; 4],
}

#[cfg(host_mi_timer_intf_test)]
#[repr(C)]
pub struct TimerIntfAdapter {
    pub iface_id: u8,
    pub adapter_up: u8,
    pub dvobj: *mut TimerIntfDvobj,
}

#[cfg(host_mi_timer_intf_test)]
type TimerIntfOp = unsafe extern "C" fn(*mut TimerIntfAdapter, *mut core::ffi::c_void) -> u8;

#[cfg(host_mi_timer_intf_test)]
extern "C" {
    pub fn rtw_cancel_all_timer(adapter: *mut TimerIntfAdapter);
    pub fn rtw_reset_drv_sw(adapter: *mut TimerIntfAdapter);
    pub fn rtw_intf_start(adapter: *mut TimerIntfAdapter);
    pub fn rtw_intf_stop(adapter: *mut TimerIntfAdapter);
}

#[cfg(host_mi_timer_intf_test)]
unsafe fn mi_process_host(
    pad: *mut TimerIntfAdapter,
    exclude_self: i32,
    data: *mut core::ffi::c_void,
    op: TimerIntfOp,
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

#[cfg(host_mi_timer_intf_test)]
unsafe fn mi_process_no_schk_host(
    pad: *mut TimerIntfAdapter,
    exclude_self: i32,
    data: *mut core::ffi::c_void,
    op: TimerIntfOp,
) -> u8 {
    let dv = &*(*pad).dvobj;
    let mut ret = 0u8;
    for i in 0..dv.iface_nums as usize {
        let p = dv.padapters[i];
        if p.is_null() {
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

#[cfg(host_mi_timer_intf_test)]
unsafe extern "C" fn op_cancel_timer_host(
    a: *mut TimerIntfAdapter,
    _data: *mut core::ffi::c_void,
) -> u8 {
    rtw_cancel_all_timer(a);
    _TRUE
}

#[cfg(host_mi_timer_intf_test)]
unsafe extern "C" fn op_reset_drv_host(
    a: *mut TimerIntfAdapter,
    _data: *mut core::ffi::c_void,
) -> u8 {
    rtw_reset_drv_sw(a);
    _TRUE
}

#[cfg(host_mi_timer_intf_test)]
unsafe extern "C" fn op_intf_start_host(
    a: *mut TimerIntfAdapter,
    _data: *mut core::ffi::c_void,
) -> u8 {
    rtw_intf_start(a);
    _TRUE
}

#[cfg(host_mi_timer_intf_test)]
unsafe extern "C" fn op_intf_stop_host(
    a: *mut TimerIntfAdapter,
    _data: *mut core::ffi::c_void,
) -> u8 {
    rtw_intf_stop(a);
    _TRUE
}

#[cfg(host_mi_timer_intf_test)]
#[no_mangle]
pub unsafe extern "C" fn mi_rust_call_timer_intf(
    pad: *mut TimerIntfAdapter,
    fn_id: i32,
) -> u8 {
    match fn_id {
        0 => mi_process_host(pad, 0, core::ptr::null_mut(), op_cancel_timer_host),
        1 => mi_process_host(pad, 1, core::ptr::null_mut(), op_cancel_timer_host),
        2 => mi_process_no_schk_host(pad, 0, core::ptr::null_mut(), op_reset_drv_host),
        3 => mi_process_no_schk_host(pad, 1, core::ptr::null_mut(), op_reset_drv_host),
        4 => mi_process_host(pad, 0, core::ptr::null_mut(), op_intf_start_host),
        5 => mi_process_host(pad, 1, core::ptr::null_mut(), op_intf_start_host),
        6 => mi_process_host(pad, 0, core::ptr::null_mut(), op_intf_stop_host),
        7 => mi_process_host(pad, 1, core::ptr::null_mut(), op_intf_stop_host),
        _ => 0xff,
    }
}

#[cfg(rust_mi_timer_intf_leaf)]
mod kernel {
    use core::ffi::c_void;

    pub type Padapter = *mut c_void;

    extern "C" {
        pub fn rtw_rust_mi_iface_nums(padapter: Padapter) -> i32;
        pub fn rtw_rust_mi_iface_at(padapter: Padapter, idx: i32) -> Padapter;
        pub fn rtw_rust_mi_is_adapter_up(iface: Padapter) -> u8;
        pub fn rtw_rust_mi_cancel_all_timer(iface: Padapter);
        pub fn rtw_rust_mi_reset_drv_sw(iface: Padapter);
        pub fn rtw_rust_mi_intf_start(iface: Padapter);
        pub fn rtw_rust_mi_intf_stop(iface: Padapter);
    }
}

#[cfg(rust_mi_timer_intf_leaf)]
type TimerKernOp = unsafe fn(kernel::Padapter, *mut core::ffi::c_void) -> u8;

#[cfg(rust_mi_timer_intf_leaf)]
unsafe fn mi_process_kern(
    padapter: kernel::Padapter,
    exclude_self: bool,
    data: *mut core::ffi::c_void,
    op: TimerKernOp,
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

#[cfg(rust_mi_timer_intf_leaf)]
unsafe fn mi_process_no_schk_kern(
    padapter: kernel::Padapter,
    exclude_self: bool,
    data: *mut core::ffi::c_void,
    op: TimerKernOp,
) -> u8 {
    let n = unsafe { kernel::rtw_rust_mi_iface_nums(padapter) };
    let mut ret = 0u8;
    for i in 0..n {
        let iface = unsafe { kernel::rtw_rust_mi_iface_at(padapter, i) };
        if iface.is_null() {
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

#[cfg(rust_mi_timer_intf_leaf)]
unsafe fn op_cancel_timer_kern(iface: kernel::Padapter, _data: *mut core::ffi::c_void) -> u8 {
    unsafe { kernel::rtw_rust_mi_cancel_all_timer(iface) };
    _TRUE
}

#[cfg(rust_mi_timer_intf_leaf)]
unsafe fn op_reset_drv_kern(iface: kernel::Padapter, _data: *mut core::ffi::c_void) -> u8 {
    unsafe { kernel::rtw_rust_mi_reset_drv_sw(iface) };
    _TRUE
}

#[cfg(rust_mi_timer_intf_leaf)]
unsafe fn op_intf_start_kern(iface: kernel::Padapter, _data: *mut core::ffi::c_void) -> u8 {
    unsafe { kernel::rtw_rust_mi_intf_start(iface) };
    _TRUE
}

#[cfg(rust_mi_timer_intf_leaf)]
unsafe fn op_intf_stop_kern(iface: kernel::Padapter, _data: *mut core::ffi::c_void) -> u8 {
    unsafe { kernel::rtw_rust_mi_intf_stop(iface) };
    _TRUE
}

#[cfg(rust_mi_timer_intf_leaf)]
macro_rules! mi_timer_intf_kern_export {
    ($fn:ident, $buddy:expr, $noschk:expr, $op:ident) => {
        #[no_mangle]
        pub unsafe extern "C" fn $fn(padapter: kernel::Padapter) {
            unsafe {
                if $noschk {
                    mi_process_no_schk_kern(
                        padapter,
                        $buddy,
                        core::ptr::null_mut(),
                        $op,
                    );
                } else {
                    mi_process_kern(padapter, $buddy, core::ptr::null_mut(), $op);
                }
            }
        }
    };
}

#[cfg(rust_mi_timer_intf_leaf)]
mi_timer_intf_kern_export!(rtw_mi_cancel_all_timer, false, false, op_cancel_timer_kern);
#[cfg(rust_mi_timer_intf_leaf)]
mi_timer_intf_kern_export!(rtw_mi_buddy_cancel_all_timer, true, false, op_cancel_timer_kern);
#[cfg(rust_mi_timer_intf_leaf)]
mi_timer_intf_kern_export!(rtw_mi_reset_drv_sw, false, true, op_reset_drv_kern);
#[cfg(rust_mi_timer_intf_leaf)]
mi_timer_intf_kern_export!(rtw_mi_buddy_reset_drv_sw, true, true, op_reset_drv_kern);
#[cfg(rust_mi_timer_intf_leaf)]
mi_timer_intf_kern_export!(rtw_mi_intf_start, false, false, op_intf_start_kern);
#[cfg(rust_mi_timer_intf_leaf)]
mi_timer_intf_kern_export!(rtw_mi_buddy_intf_start, true, false, op_intf_start_kern);
#[cfg(rust_mi_timer_intf_leaf)]
mi_timer_intf_kern_export!(rtw_mi_intf_stop, false, false, op_intf_stop_kern);
#[cfg(rust_mi_timer_intf_leaf)]
mi_timer_intf_kern_export!(rtw_mi_buddy_intf_stop, true, false, op_intf_stop_kern);
