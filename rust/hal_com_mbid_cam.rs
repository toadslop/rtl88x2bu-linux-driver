// SPDX-License-Identifier: GPL-2.0
// CONFIG_MBSSID_CAM init/search helpers (W4-119).

use core::ffi::{c_char, c_ulong, c_void};

type U8 = u8;

const INVALID_CAM_ID: U8 = 0xFF;
const TOTAL_MBID_CAM_NUM: u8 = 8;
const ETH_ALEN: u32 = 6;
const TRUE: i32 = 1;

#[cfg(not(host_hal_com_mbid_cam_test))]
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
    fn rtw_rust_hal_com_mbid_log_mac_search(func: *const c_char, mac_addr: *mut U8, cam_id: U8);
    fn rtw_rust_hal_com_mbid_log_iface_search(
        func: *const c_char,
        iface_id: U8,
        mac_addr: *const U8,
        cam_id: U8,
    );
    fn _rtw_memcmp(dst: *const c_void, src: *const c_void, sz: u32) -> i32;
}

#[cfg(host_hal_com_mbid_cam_test)]
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
    fn rtw_rust_hal_com_mbid_log_mac_search(func: *const c_char, mac_addr: *mut U8, cam_id: U8);
    fn rtw_rust_hal_com_mbid_log_iface_search(
        func: *const c_char,
        iface_id: U8,
        mac_addr: *const U8,
        cam_id: U8,
    );
    fn _rtw_memcmp(dst: *const c_void, src: *const c_void, sz: u32) -> i32;
}

static MBID_MAC_SEARCH: &[u8] = b"_rtw_mbid_cam_search_by_macaddr\0";
static MBID_IFACE_SEARCH: &[u8] = b"_rtw_mbid_cam_search_by_ifaceid\0";

fn dvobj_of(adapter: *mut c_void) -> *mut c_void {
    unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) }
}

fn ctl_of_dvobj(dvobj: *mut c_void) -> *mut c_void {
    unsafe { rtw_rust_hal_com_mbid_cam_ctl(dvobj) }
}

// SAFETY: `lock` comes from mbid_cam_ctl; matches C `_enter_critical_bh` pairing.
fn with_mbid_lock(ctl: *mut c_void, f: impl FnOnce()) {
    let lock = unsafe { rtw_rust_hal_com_mbid_ctl_lock(ctl) };
    let mut irqL: c_ulong = 0;
    unsafe {
        rtw_rust_hal_com_enter_critical_bh(lock, &mut irqL as *mut _ as *mut c_void);
    }
    f();
    unsafe {
        rtw_rust_hal_com_exit_critical_bh(lock, &mut irqL as *mut _ as *mut c_void);
    }
}

fn search_by_mac(adapter: *mut c_void, mac_addr: *mut U8) -> U8 {
    let dvobj = dvobj_of(adapter);
    let mut cam_id = INVALID_CAM_ID;
    for i in 0..TOTAL_MBID_CAM_NUM {
        if !mac_addr.is_null()
            && unsafe {
                _rtw_memcmp(
                    rtw_rust_hal_com_mbid_cache_mac(dvobj, i).cast(),
                    mac_addr.cast(),
                    ETH_ALEN,
                )
            } == TRUE
        {
            cam_id = i;
            break;
        }
    }
    unsafe {
        rtw_rust_hal_com_mbid_log_mac_search(
            MBID_MAC_SEARCH.as_ptr().cast(),
            mac_addr,
            cam_id,
        );
    }
    cam_id
}

fn search_by_iface(adapter: *mut c_void, iface_id: U8) -> U8 {
    let dvobj = dvobj_of(adapter);
    let mut cam_id = INVALID_CAM_ID;
    for i in 0..TOTAL_MBID_CAM_NUM {
        if iface_id == unsafe { rtw_rust_hal_com_mbid_cache_iface_id(dvobj, i) } {
            cam_id = i;
            break;
        }
    }
    if cam_id != INVALID_CAM_ID {
        let mac = unsafe { rtw_rust_hal_com_mbid_cache_mac(dvobj, cam_id) };
        unsafe {
            rtw_rust_hal_com_mbid_log_iface_search(
                MBID_IFACE_SEARCH.as_ptr().cast(),
                iface_id,
                mac,
                cam_id,
            );
        }
    }
    cam_id
}

#[cfg(any(config_mbssid_cam, host_hal_com_mbid_cam_test))]
#[no_mangle]
pub extern "C" fn rtw_mbid_cam_init(dvobj: *mut c_void) {
    let ctl = unsafe { ctl_of_dvobj(dvobj) };
    unsafe {
        rtw_rust_hal_com_spinlock_init(rtw_rust_hal_com_mbid_ctl_lock(ctl));
        rtw_rust_hal_com_mbid_ctl_set_bitmap(ctl, 0);
        rtw_rust_hal_com_atomic_set(rtw_rust_hal_com_mbid_ctl_entry_num(ctl), 0);
        rtw_rust_hal_com_mbid_cam_cache_zero(dvobj);
    }
}

#[cfg(any(config_mbssid_cam, host_hal_com_mbid_cam_test))]
#[no_mangle]
pub extern "C" fn rtw_mbid_cam_deinit(dvobj: *mut c_void) {
    let ctl = unsafe { ctl_of_dvobj(dvobj) };
    unsafe { rtw_rust_hal_com_spinlock_free(rtw_rust_hal_com_mbid_ctl_lock(ctl)) };
}

#[cfg(any(config_mbssid_cam, host_hal_com_mbid_cam_test))]
#[no_mangle]
pub extern "C" fn rtw_mbid_cam_reset(adapter: *mut c_void) {
    let dvobj = unsafe { dvobj_of(adapter) };
    let ctl = unsafe { ctl_of_dvobj(dvobj) };
    unsafe {
        with_mbid_lock(ctl, || {
            rtw_rust_hal_com_mbid_ctl_set_bitmap(ctl, 0);
            rtw_rust_hal_com_mbid_cam_cache_zero(dvobj);
        });
        rtw_rust_hal_com_atomic_set(rtw_rust_hal_com_mbid_ctl_entry_num(ctl), 0);
    }
}

#[cfg(any(config_mbssid_cam, host_hal_com_mbid_cam_test))]
#[no_mangle]
pub extern "C" fn rtw_mbid_cam_search_by_macaddr(adapter: *mut c_void, mac_addr: *mut U8) -> U8 {
    let dvobj = unsafe { dvobj_of(adapter) };
    let ctl = unsafe { ctl_of_dvobj(dvobj) };
    let mut cam_id = INVALID_CAM_ID;
    unsafe {
        with_mbid_lock(ctl, || {
            cam_id = search_by_mac(adapter, mac_addr);
        });
    }
    cam_id
}

#[cfg(any(config_mbssid_cam, host_hal_com_mbid_cam_test))]
#[no_mangle]
pub extern "C" fn rtw_mbid_cam_search_by_ifaceid(adapter: *mut c_void, iface_id: U8) -> U8 {
    let dvobj = unsafe { dvobj_of(adapter) };
    let ctl = unsafe { ctl_of_dvobj(dvobj) };
    let mut cam_id = INVALID_CAM_ID;
    unsafe {
        with_mbid_lock(ctl, || {
            cam_id = search_by_iface(adapter, iface_id);
        });
    }
    cam_id
}

#[cfg(any(config_mbssid_cam, host_hal_com_mbid_cam_test))]
#[no_mangle]
pub extern "C" fn rtw_get_max_mbid_cam_id(adapter: *mut c_void) -> U8 {
    let dvobj = unsafe { dvobj_of(adapter) };
    let ctl = unsafe { ctl_of_dvobj(dvobj) };
    let mut cam_id = INVALID_CAM_ID;
    unsafe {
        with_mbid_lock(ctl, || {
            let bitmap = rtw_rust_hal_com_mbid_ctl_bitmap(ctl);
            let mut i = TOTAL_MBID_CAM_NUM as i32 - 1;
            while i >= 0 {
                let bit = 1u8.wrapping_shl(i as u32);
                if bitmap & bit != 0 {
                    cam_id = i as U8;
                    break;
                }
                i -= 1;
            }
        });
    }
    cam_id
}

#[cfg(any(config_mbssid_cam, host_hal_com_mbid_cam_test))]
#[no_mangle]
pub extern "C" fn rtw_get_mbid_cam_entry_num(adapter: *mut c_void) -> U8 {
    let dvobj = unsafe { dvobj_of(adapter) };
    let ctl = unsafe { ctl_of_dvobj(dvobj) };
    unsafe { rtw_rust_hal_com_atomic_read(rtw_rust_hal_com_mbid_ctl_entry_num(ctl)) as U8 }
}