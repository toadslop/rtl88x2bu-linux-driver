// SPDX-License-Identifier: GPL-2.0
// CONFIG_MBSSID_CAM init/search helpers (W4-119).

use core::ffi::{c_ulong, c_void};

type U8 = u8;

const INVALID_CAM_ID: U8 = 0xFF;
const TOTAL_MBID_CAM_NUM: u8 = 8;
const ETH_ALEN: u32 = 6;
const TRUE: i32 = 1;

extern "C" {
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_mbid_cam_ctl(dvobj: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_spinlock_init(lock: *mut c_void);
    fn rtw_rust_hal_com_spinlock_free(lock: *mut c_void);
    fn rtw_rust_hal_com_enter_critical_bh(lock: *mut c_void, irqL: *mut c_void);
    fn rtw_rust_hal_com_exit_critical_bh(lock: *mut c_void, irqL: *mut c_void);
    fn rtw_rust_hal_com_mbid_cam_cache_zero(dvobj: *mut c_void);
    fn rtw_rust_hal_com_mbid_ctl_bitmap(ctl: *mut c_void) -> U8;
    fn rtw_rust_hal_com_mbid_ctl_set_bitmap(ctl: *mut c_void, bitmap: U8);
    fn rtw_rust_hal_com_mbid_ctl_lock(ctl: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_mbid_ctl_entry_num(ctl: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_atomic_read(v: *mut c_void) -> i32;
    fn rtw_rust_hal_com_atomic_set(v: *mut c_void, x: i32);
    fn rtw_rust_hal_com_mbid_cache_iface_id(dvobj: *mut c_void, idx: U8) -> U8;
    fn rtw_rust_hal_com_mbid_cache_mac(dvobj: *mut c_void, idx: U8) -> *mut U8;
    fn _rtw_memcmp(dst: *const c_void, src: *const c_void, sz: u32) -> i32;
}

unsafe fn dvobj_of(adapter: *mut c_void) -> *mut c_void {
    rtw_rust_hal_com_adapter_to_dvobj(adapter)
}

unsafe fn ctl_of_dvobj(dvobj: *mut c_void) -> *mut c_void {
    rtw_rust_hal_com_mbid_cam_ctl(dvobj)
}

unsafe fn with_mbid_lock(ctl: *mut c_void, f: impl FnOnce()) {
    let lock = rtw_rust_hal_com_mbid_ctl_lock(ctl);
    let mut irqL: c_ulong = 0;
    rtw_rust_hal_com_enter_critical_bh(lock, &mut irqL as *mut _ as *mut c_void);
    f();
    rtw_rust_hal_com_exit_critical_bh(lock, &mut irqL as *mut _ as *mut c_void);
}

fn search_by_mac(adapter: *mut c_void, mac_addr: *mut U8) -> U8 {
    let dvobj = unsafe { dvobj_of(adapter) };
    if dvobj.is_null() {
        return INVALID_CAM_ID;
    }
    for i in 0..TOTAL_MBID_CAM_NUM {
        let slot_mac = unsafe { rtw_rust_hal_com_mbid_cache_mac(dvobj, i) };
        if !mac_addr.is_null()
            && !slot_mac.is_null()
            && unsafe { _rtw_memcmp(slot_mac.cast(), mac_addr.cast(), ETH_ALEN) } == TRUE
        {
            return i;
        }
    }
    INVALID_CAM_ID
}

fn search_by_iface(adapter: *mut c_void, iface_id: U8) -> U8 {
    let dvobj = unsafe { dvobj_of(adapter) };
    if dvobj.is_null() {
        return INVALID_CAM_ID;
    }
    for i in 0..TOTAL_MBID_CAM_NUM {
        if iface_id == unsafe { rtw_rust_hal_com_mbid_cache_iface_id(dvobj, i) } {
            return i;
        }
    }
    INVALID_CAM_ID
}

#[no_mangle]
pub extern "C" fn rtw_mbid_cam_init(dvobj: *mut c_void) {
    if dvobj.is_null() {
        return;
    }
    let ctl = unsafe { ctl_of_dvobj(dvobj) };
    if ctl.is_null() {
        return;
    }
    unsafe {
        rtw_rust_hal_com_spinlock_init(rtw_rust_hal_com_mbid_ctl_lock(ctl));
        rtw_rust_hal_com_mbid_ctl_set_bitmap(ctl, 0);
        rtw_rust_hal_com_atomic_set(rtw_rust_hal_com_mbid_ctl_entry_num(ctl), 0);
        rtw_rust_hal_com_mbid_cam_cache_zero(dvobj);
    }
}

#[no_mangle]
pub extern "C" fn rtw_mbid_cam_deinit(dvobj: *mut c_void) {
    if dvobj.is_null() {
        return;
    }
    let ctl = unsafe { ctl_of_dvobj(dvobj) };
    if !ctl.is_null() {
        unsafe { rtw_rust_hal_com_spinlock_free(rtw_rust_hal_com_mbid_ctl_lock(ctl)) };
    }
}

#[no_mangle]
pub extern "C" fn rtw_mbid_cam_reset(adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    let dvobj = unsafe { dvobj_of(adapter) };
    let ctl = unsafe { ctl_of_dvobj(dvobj) };
    if ctl.is_null() {
        return;
    }
    unsafe {
        with_mbid_lock(ctl, || {
            rtw_rust_hal_com_mbid_ctl_set_bitmap(ctl, 0);
            rtw_rust_hal_com_mbid_cam_cache_zero(dvobj);
        });
        rtw_rust_hal_com_atomic_set(rtw_rust_hal_com_mbid_ctl_entry_num(ctl), 0);
    }
}

#[no_mangle]
pub extern "C" fn rtw_mbid_cam_search_by_macaddr(adapter: *mut c_void, mac_addr: *mut U8) -> U8 {
    let ctl = unsafe { ctl_of_dvobj(dvobj_of(adapter)) };
    if ctl.is_null() {
        return INVALID_CAM_ID;
    }
    let mut cam_id = INVALID_CAM_ID;
    unsafe { with_mbid_lock(ctl, || cam_id = search_by_mac(adapter, mac_addr)) };
    cam_id
}

#[no_mangle]
pub extern "C" fn rtw_mbid_cam_search_by_ifaceid(adapter: *mut c_void, iface_id: U8) -> U8 {
    let ctl = unsafe { ctl_of_dvobj(dvobj_of(adapter)) };
    if ctl.is_null() {
        return INVALID_CAM_ID;
    }
    let mut cam_id = INVALID_CAM_ID;
    unsafe { with_mbid_lock(ctl, || cam_id = search_by_iface(adapter, iface_id)) };
    cam_id
}

#[no_mangle]
pub extern "C" fn rtw_get_max_mbid_cam_id(adapter: *mut c_void) -> U8 {
    let ctl = unsafe { ctl_of_dvobj(dvobj_of(adapter)) };
    if ctl.is_null() {
        return INVALID_CAM_ID;
    }
    let mut cam_id = INVALID_CAM_ID;
    unsafe {
        with_mbid_lock(ctl, || {
            let bitmap = rtw_rust_hal_com_mbid_ctl_bitmap(ctl);
            let mut i = TOTAL_MBID_CAM_NUM as i32 - 1;
            while i >= 0 {
                if bitmap & (1u8 << i) != 0 {
                    cam_id = i as U8;
                    break;
                }
                i -= 1;
            }
        });
    }
    cam_id
}

#[no_mangle]
pub extern "C" fn rtw_get_mbid_cam_entry_num(adapter: *mut c_void) -> U8 {
    let ctl = unsafe { ctl_of_dvobj(dvobj_of(adapter)) };
    if ctl.is_null() {
        return 0;
    }
    unsafe { rtw_rust_hal_com_atomic_read(rtw_rust_hal_com_mbid_ctl_entry_num(ctl)) as U8 }
}