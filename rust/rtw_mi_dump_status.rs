// SPDX-License-Identifier: GPL-2.0
//! W3-198: `dump_mi_status` / `dump_dvobj_mi_status` from `core/rtw_mi.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_snake_case,
    unreachable_pub,
    unused_unsafe
)]

#[cfg(host_mi_dump_test)]
use std::os::raw::{c_char, c_int, c_void};

#[cfg(not(host_mi_dump_test))]
use core::ffi::{c_char, c_int, c_void};

#[repr(C)]
#[derive(Clone, Copy, Default)]
pub struct MiDumpSnap {
    pub sta_num: u8,
    pub lg_sta_num: u8,
    pub ld_sta_num: u8,
    pub ld_tdls_num: u8,
    pub ap_num: u8,
    pub starting_ap_num: u8,
    pub ld_ap_num: u8,
    pub adhoc_num: u8,
    pub ld_adhoc_num: u8,
    pub mesh_num: u8,
    pub ld_mesh_num: u8,
    pub p2p_device_num: u8,
    pub p2p_gc: u8,
    pub p2p_go: u8,
    pub scan_num: u8,
    pub uwps_num: u8,
    pub roch_num: u8,
    pub mgmt_tx_num: u8,
    pub union_ch: u8,
    pub union_bw: u8,
    pub union_offset: u8,
}

#[cfg(host_mi_dump_test)]
#[path = "rtw_mi_dump_status_host_types.rs"]
mod host_types;

#[cfg(rust_mi_dump_leaf)]
mod kernel {
    use core::ffi::{c_char, c_void};

    use super::MiDumpSnap;

    extern "C" {
        pub fn rtw_rust_mi_dump_snap(dvobj: *mut c_void, out: *mut MiDumpSnap);
        pub fn rtw_rust_mi_dump_print_sel(sel: *mut c_void, line: *const c_char);
        pub fn rtw_rust_mi_dump_dvobj_banner(fun_name: *const c_char);
        pub fn rtw_rust_mi_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    }
}

extern "C" {
    fn snprintf(s: *mut c_char, n: usize, fmt: *const c_char, ...) -> c_int;
}

#[cfg(host_mi_dump_test)]
fn snap_from_dvobj(dvobj: *mut c_void) -> MiDumpSnap {
    let mut s = MiDumpSnap::default();
    host_types::snap_from_host_dvobj(dvobj, &mut s);
    s
}

#[cfg(all(rust_mi_dump_leaf, not(host_mi_dump_test)))]
fn snap_from_dvobj(dvobj: *mut c_void) -> MiDumpSnap {
    let mut s = MiDumpSnap::default();
    unsafe { kernel::rtw_rust_mi_dump_snap(dvobj, &mut s) };
    s
}

fn print_sel(sel: *mut c_void, line: *const c_char) {
    #[cfg(host_mi_dump_test)]
    host_types::capture_print(sel, line);
    #[cfg(all(rust_mi_dump_leaf, not(host_mi_dump_test)))]
    unsafe {
        kernel::rtw_rust_mi_dump_print_sel(sel, line);
    }
}

fn print_fmt(sel: *mut c_void, fmt: &[u8], val: c_int) {
    let mut buf = [0i8; 96];
    unsafe {
        snprintf(
            buf.as_mut_ptr(),
            buf.len(),
            fmt.as_ptr() as *const c_char,
            val,
        );
        print_sel(sel, buf.as_ptr());
    }
}

fn dump_mi_status_body(sel: *mut c_void, s: &MiDumpSnap) {
    print_sel(
        sel,
        b"== dvobj-iface_state ==\n\0".as_ptr() as *const c_char,
    );
    #[cfg(host_mi_dump_test)]
    {
        let lines: &[(&[u8], u8)] = &[
            (b"sta_num:%d\n\0", s.sta_num),
            (b"linking_sta_num:%d\n\0", s.lg_sta_num),
            (b"linked_sta_num:%d\n\0", s.ld_sta_num),
            (b"linked_tdls_num:%d\n\0", s.ld_tdls_num),
            (b"ap_num:%d\n\0", s.ap_num),
            (b"starting_ap_num:%d\n\0", s.starting_ap_num),
            (b"linked_ap_num:%d\n\0", s.ld_ap_num),
            (b"adhoc_num:%d\n\0", s.adhoc_num),
            (b"linked_adhoc_num:%d\n\0", s.ld_adhoc_num),
            (b"mesh_num:%d\n\0", s.mesh_num),
            (b"linked_mesh_num:%d\n\0", s.ld_mesh_num),
            (b"p2p_device_num:%d\n\0", s.p2p_device_num),
            (b"p2p_gc_num:%d\n\0", s.p2p_gc),
            (b"p2p_go_num:%d\n\0", s.p2p_go),
            (b"scan_num:%d\n\0", s.scan_num),
            (b"under_wps_num:%d\n\0", s.uwps_num),
            (b"roch_num:%d\n\0", s.roch_num),
            (b"mgmt_tx_num:%d\n\0", s.mgmt_tx_num),
            (b"union_ch:%d\n\0", s.union_ch),
            (b"union_bw:%d\n\0", s.union_bw),
            (b"union_offset:%d\n\0", s.union_offset),
        ];
        for (fmt, val) in lines {
            print_fmt(sel, fmt, *val as c_int);
        }
    }
    #[cfg(all(rust_mi_dump_leaf, not(host_mi_dump_test)))]
    {
        print_fmt(sel, b"sta_num:%d\n\0", s.sta_num as c_int);
        print_fmt(sel, b"linking_sta_num:%d\n\0", s.lg_sta_num as c_int);
        print_fmt(sel, b"linked_sta_num:%d\n\0", s.ld_sta_num as c_int);
        #[cfg(config_tdls)]
        print_fmt(sel, b"linked_tdls_num:%d\n\0", s.ld_tdls_num as c_int);
        #[cfg(config_ap_mode)]
        {
            print_fmt(sel, b"ap_num:%d\n\0", s.ap_num as c_int);
            print_fmt(sel, b"starting_ap_num:%d\n\0", s.starting_ap_num as c_int);
            print_fmt(sel, b"linked_ap_num:%d\n\0", s.ld_ap_num as c_int);
        }
        print_fmt(sel, b"adhoc_num:%d\n\0", s.adhoc_num as c_int);
        print_fmt(sel, b"linked_adhoc_num:%d\n\0", s.ld_adhoc_num as c_int);
        #[cfg(config_rtw_mesh)]
        {
            print_fmt(sel, b"mesh_num:%d\n\0", s.mesh_num as c_int);
            print_fmt(sel, b"linked_mesh_num:%d\n\0", s.ld_mesh_num as c_int);
        }
        #[cfg(config_p2p)]
        {
            print_fmt(sel, b"p2p_device_num:%d\n\0", s.p2p_device_num as c_int);
            print_fmt(sel, b"p2p_gc_num:%d\n\0", s.p2p_gc as c_int);
            print_fmt(sel, b"p2p_go_num:%d\n\0", s.p2p_go as c_int);
        }
        print_fmt(sel, b"scan_num:%d\n\0", s.scan_num as c_int);
        print_fmt(sel, b"under_wps_num:%d\n\0", s.uwps_num as c_int);
        #[cfg(all(config_ioctl_cfg80211, config_p2p))]
        print_fmt(sel, b"roch_num:%d\n\0", s.roch_num as c_int);
        #[cfg(config_ioctl_cfg80211)]
        print_fmt(sel, b"mgmt_tx_num:%d\n\0", s.mgmt_tx_num as c_int);
        print_fmt(sel, b"union_ch:%d\n\0", s.union_ch as c_int);
        print_fmt(sel, b"union_bw:%d\n\0", s.union_bw as c_int);
        print_fmt(sel, b"union_offset:%d\n\0", s.union_offset as c_int);
    }
    print_sel(sel, b"================\n\n\0".as_ptr() as *const c_char);
}

#[no_mangle]
pub unsafe extern "C" fn dump_mi_status(sel: *mut c_void, dvobj: *mut c_void) {
    if dvobj.is_null() {
        return;
    }
    let s = snap_from_dvobj(dvobj);
    dump_mi_status_body(sel, &s);
}

#[no_mangle]
pub unsafe extern "C" fn dump_dvobj_mi_status(
    sel: *mut c_void,
    fun_name: *const c_char,
    adapter: *mut c_void,
) {
    let _ = fun_name;
    #[cfg(all(rust_mi_dump_leaf, not(host_mi_dump_test)))]
    unsafe {
        kernel::rtw_rust_mi_dump_dvobj_banner(fun_name);
    }

    #[cfg(host_mi_dump_test)]
    let dvobj = unsafe { host_types::adapter_dvobj(adapter) };
    #[cfg(all(rust_mi_dump_leaf, not(host_mi_dump_test)))]
    let dvobj = unsafe { kernel::rtw_rust_mi_adapter_to_dvobj(adapter) };

    unsafe {
        dump_mi_status(sel, dvobj as *mut c_void);
    }
}
