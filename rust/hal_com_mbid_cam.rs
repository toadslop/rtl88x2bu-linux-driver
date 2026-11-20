// SPDX-License-Identifier: GPL-2.0
// CONFIG_MBSSID_CAM init/search (W4-119) + alloc/assign/clean (W4-120).

use core::ffi::{c_char, c_ulong, c_void};

type U8 = u8;

const INVALID_CAM_ID: U8 = 0xFF;
const TOTAL_MBID_CAM_NUM: u8 = 8;
const ETH_ALEN: u32 = 6;
const TRUE: i32 = 1;
const C_FALSE: U8 = 0;
const C_TRUE: U8 = 1;

#[cfg(not(host_hal_com_mbid_cam_test))]
extern "C" {
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_adapter_iface_id(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_mbid_cam_ctl(dvobj: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_spinlock_init(lock: *mut c_void);
    fn rtw_rust_hal_com_spinlock_free(lock: *mut c_void);
    fn rtw_rust_hal_com_enter_critical_bh(lock: *mut c_void, irqL: *mut c_void);
    fn rtw_rust_hal_com_exit_critical_bh(lock: *mut c_void, irqL: *mut c_void);
    fn rtw_rust_hal_com_mbid_cam_cache_zero(dvobj: *mut c_void);
    fn rtw_rust_hal_com_mbid_ctl_bitmap(ctl: *mut c_void) -> U8;
    fn rtw_rust_hal_com_mbid_ctl_set_bitmap(ctl: *mut c_void, bitmap: U8);
    fn rtw_rust_hal_com_mbid_ctl_bitmap_set_bit(ctl: *mut c_void, idx: U8);
    fn rtw_rust_hal_com_mbid_ctl_bitmap_clr_bit(ctl: *mut c_void, idx: U8);
    fn rtw_rust_hal_com_mbid_ctl_bitmap_test(ctl: *mut c_void, idx: U8) -> U8;
    fn rtw_rust_hal_com_mbid_ctl_lock(ctl: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_mbid_ctl_entry_num(ctl: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_atomic_read(v: *mut c_void) -> i32;
    fn rtw_rust_hal_com_atomic_set(v: *mut c_void, x: i32);
    fn rtw_rust_hal_com_atomic_inc(v: *mut c_void);
    fn rtw_rust_hal_com_atomic_dec(v: *mut c_void);
    fn rtw_rust_hal_com_mbid_cache_iface_id(dvobj: *mut c_void, idx: U8) -> U8;
    fn rtw_rust_hal_com_mbid_cache_mac(dvobj: *mut c_void, idx: U8) -> *mut U8;
    fn rtw_rust_hal_com_mbid_log_mac_search(func: *const c_char, mac_addr: *mut U8, cam_id: U8);
    fn rtw_rust_hal_com_mbid_log_iface_search(
        func: *const c_char,
        iface_id: U8,
        mac_addr: *const U8,
        cam_id: U8,
    );
    fn rtw_rust_hal_com_mbid_log_alloc_over_limit(adapter: *mut c_void, entry_num: U8);
    fn rtw_rust_hal_com_mbid_log_alloc_result(func: *const c_char, mac_addr: *mut U8, cam_id: U8);
    fn rtw_rust_hal_com_mbid_log_invalid_camid(adapter: *mut c_void, camid: U8);
    fn rtw_rust_hal_com_mbid_log_assign_result(
        func: *const c_char,
        mac_addr: *mut U8,
        camid: U8,
        success: U8,
    );
    fn rtw_rust_hal_com_mbid_log_clean(func: *const c_char, camid: U8);
    fn rtw_rust_hal_com_mbid_cache_init(adapter: *mut c_void, dvobj: *mut c_void, idx: U8, mac: *mut U8);
    fn rtw_rust_hal_com_mbid_cache_clr(dvobj: *mut c_void, idx: U8);
    fn rtw_rust_hal_com_warn_on(condition: i32);
    fn _rtw_memcmp(dst: *const c_void, src: *const c_void, sz: u32) -> i32;
}

#[cfg(host_hal_com_mbid_cam_test)]
extern "C" {
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_adapter_iface_id(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_mbid_cam_ctl(dvobj: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_spinlock_init(lock: *mut c_void);
    fn rtw_rust_hal_com_spinlock_free(lock: *mut c_void);
    fn rtw_rust_hal_com_enter_critical_bh(lock: *mut c_void, irqL: *mut c_void);
    fn rtw_rust_hal_com_exit_critical_bh(lock: *mut c_void, irqL: *mut c_void);
    fn rtw_rust_hal_com_mbid_cam_cache_zero(dvobj: *mut c_void);
    fn rtw_rust_hal_com_mbid_ctl_bitmap(ctl: *mut c_void) -> U8;
    fn rtw_rust_hal_com_mbid_ctl_set_bitmap(ctl: *mut c_void, bitmap: U8);
    fn rtw_rust_hal_com_mbid_ctl_bitmap_set_bit(ctl: *mut c_void, idx: U8);
    fn rtw_rust_hal_com_mbid_ctl_bitmap_clr_bit(ctl: *mut c_void, idx: U8);
    fn rtw_rust_hal_com_mbid_ctl_bitmap_test(ctl: *mut c_void, idx: U8) -> U8;
    fn rtw_rust_hal_com_mbid_ctl_lock(ctl: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_mbid_ctl_entry_num(ctl: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_atomic_read(v: *mut c_void) -> i32;
    fn rtw_rust_hal_com_atomic_set(v: *mut c_void, x: i32);
    fn rtw_rust_hal_com_atomic_inc(v: *mut c_void);
    fn rtw_rust_hal_com_atomic_dec(v: *mut c_void);
    fn rtw_rust_hal_com_mbid_cache_iface_id(dvobj: *mut c_void, idx: U8) -> U8;
    fn rtw_rust_hal_com_mbid_cache_mac(dvobj: *mut c_void, idx: U8) -> *mut U8;
    fn rtw_rust_hal_com_mbid_log_mac_search(func: *const c_char, mac_addr: *mut U8, cam_id: U8);
    fn rtw_rust_hal_com_mbid_log_iface_search(
        func: *const c_char,
        iface_id: U8,
        mac_addr: *const U8,
        cam_id: U8,
    );
    fn rtw_rust_hal_com_mbid_log_alloc_over_limit(adapter: *mut c_void, entry_num: U8);
    fn rtw_rust_hal_com_mbid_log_alloc_result(func: *const c_char, mac_addr: *mut U8, cam_id: U8);
    fn rtw_rust_hal_com_mbid_log_invalid_camid(adapter: *mut c_void, camid: U8);
    fn rtw_rust_hal_com_mbid_log_assign_result(
        func: *const c_char,
        mac_addr: *mut U8,
        camid: U8,
        success: U8,
    );
    fn rtw_rust_hal_com_mbid_log_clean(func: *const c_char, camid: U8);
    fn rtw_rust_hal_com_mbid_cache_init(adapter: *mut c_void, dvobj: *mut c_void, idx: U8, mac: *mut U8);
    fn rtw_rust_hal_com_mbid_cache_clr(dvobj: *mut c_void, idx: U8);
    fn rtw_rust_hal_com_warn_on(condition: i32);
    fn _rtw_memcmp(dst: *const c_void, src: *const c_void, sz: u32) -> i32;
}

static MBID_MAC_SEARCH: &[u8] = b"_rtw_mbid_cam_search_by_macaddr\0";
static MBID_IFACE_SEARCH: &[u8] = b"_rtw_mbid_cam_search_by_ifaceid\0";
static FN_CAMID_ALLOC: &[u8] = b"rtw_mbid_camid_alloc\0";
static FN_CAM_ASSIGN: &[u8] = b"rtw_mbid_cam_assign\0";
static FN_CAMID_CLEAN: &[u8] = b"rtw_mbid_camid_clean\0";

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

#[no_mangle]
pub extern "C" fn rtw_mbid_camid_alloc(adapter: *mut c_void, mac_addr: *mut U8) -> U8 {
    if adapter.is_null() {
        return INVALID_CAM_ID;
    }
    let dvobj = unsafe { dvobj_of(adapter) };
    let ctl = unsafe { ctl_of_dvobj(dvobj) };
    if ctl.is_null() {
        return INVALID_CAM_ID;
    }
    if unsafe { rtw_mbid_cam_search_by_macaddr(adapter, mac_addr) } != INVALID_CAM_ID {
        return INVALID_CAM_ID;
    }
    let entry_num =
        unsafe { rtw_rust_hal_com_atomic_read(rtw_rust_hal_com_mbid_ctl_entry_num(ctl)) as U8 };
    if entry_num >= TOTAL_MBID_CAM_NUM {
        unsafe {
            rtw_rust_hal_com_mbid_log_alloc_over_limit(adapter, entry_num);
            rtw_rust_hal_com_warn_on(1);
        }
    }

    let mut cam_id = INVALID_CAM_ID;
    unsafe {
        with_mbid_lock(ctl, || {
            for i in 0..TOTAL_MBID_CAM_NUM {
                if rtw_rust_hal_com_mbid_ctl_bitmap_test(ctl, i) == 0 {
                    rtw_rust_hal_com_mbid_ctl_bitmap_set_bit(ctl, i);
                    cam_id = i;
                    break;
                }
            }
            if cam_id != INVALID_CAM_ID && !mac_addr.is_null() {
                rtw_rust_hal_com_mbid_cache_init(adapter, dvobj, cam_id, mac_addr);
            }
        });
    }
    if cam_id != INVALID_CAM_ID {
        unsafe { rtw_rust_hal_com_atomic_inc(rtw_rust_hal_com_mbid_ctl_entry_num(ctl)) };
    }
    unsafe {
        rtw_rust_hal_com_mbid_log_alloc_result(FN_CAMID_ALLOC.as_ptr().cast(), mac_addr, cam_id);
    }
    cam_id
}

#[no_mangle]
pub extern "C" fn rtw_mbid_cam_info_change(adapter: *mut c_void, mac_addr: *mut U8) -> U8 {
    if adapter.is_null() {
        return INVALID_CAM_ID;
    }
    let dvobj = unsafe { dvobj_of(adapter) };
    let ctl = unsafe { ctl_of_dvobj(dvobj) };
    if ctl.is_null() {
        return INVALID_CAM_ID;
    }
    let iface_id = unsafe { rtw_rust_hal_com_adapter_iface_id(adapter) };
    let mut entry_id = INVALID_CAM_ID;
    unsafe {
        with_mbid_lock(ctl, || {
            entry_id = search_by_iface(adapter, iface_id);
            if entry_id != INVALID_CAM_ID {
                rtw_rust_hal_com_mbid_cache_init(adapter, dvobj, entry_id, mac_addr);
            }
        });
    }
    entry_id
}

#[no_mangle]
pub extern "C" fn rtw_mbid_cam_assign(adapter: *mut c_void, mac_addr: *mut U8, camid: U8) -> U8 {
    if adapter.is_null() {
        return C_FALSE;
    }
    if camid >= TOTAL_MBID_CAM_NUM || camid == INVALID_CAM_ID {
        unsafe {
            rtw_rust_hal_com_mbid_log_invalid_camid(adapter, camid);
            rtw_rust_hal_com_warn_on(1);
        }
    }
    if unsafe { rtw_mbid_cam_search_by_macaddr(adapter, mac_addr) } != INVALID_CAM_ID {
        return C_FALSE;
    }
    let dvobj = unsafe { dvobj_of(adapter) };
    let ctl = unsafe { ctl_of_dvobj(dvobj) };
    if ctl.is_null() {
        return C_FALSE;
    }
    let mut ret = C_FALSE;
    unsafe {
        with_mbid_lock(ctl, || {
            if rtw_rust_hal_com_mbid_ctl_bitmap_test(ctl, camid) == 0 && !mac_addr.is_null() {
                rtw_rust_hal_com_mbid_ctl_bitmap_set_bit(ctl, camid);
                if camid < TOTAL_MBID_CAM_NUM {
                    rtw_rust_hal_com_mbid_cache_init(adapter, dvobj, camid, mac_addr);
                }
                ret = C_TRUE;
            }
        });
    }
    if ret == C_TRUE {
        unsafe { rtw_rust_hal_com_atomic_inc(rtw_rust_hal_com_mbid_ctl_entry_num(ctl)) };
    }
    unsafe {
        rtw_rust_hal_com_mbid_log_assign_result(
            FN_CAM_ASSIGN.as_ptr().cast(),
            mac_addr,
            camid,
            ret,
        );
    }
    ret
}

#[no_mangle]
pub extern "C" fn rtw_mbid_camid_clean(adapter: *mut c_void, mbss_camid: U8) {
    if adapter.is_null() {
        return;
    }
    if mbss_camid >= TOTAL_MBID_CAM_NUM || mbss_camid == INVALID_CAM_ID {
        unsafe {
            rtw_rust_hal_com_mbid_log_invalid_camid(adapter, mbss_camid);
            rtw_rust_hal_com_warn_on(1);
        }
    }
    let dvobj = unsafe { dvobj_of(adapter) };
    let ctl = unsafe { ctl_of_dvobj(dvobj) };
    if ctl.is_null() {
        return;
    }
    unsafe {
        with_mbid_lock(ctl, || {
            if mbss_camid < TOTAL_MBID_CAM_NUM {
                rtw_rust_hal_com_mbid_cache_clr(dvobj, mbss_camid);
            }
            rtw_rust_hal_com_mbid_ctl_bitmap_clr_bit(ctl, mbss_camid);
        });
        rtw_rust_hal_com_atomic_dec(rtw_rust_hal_com_mbid_ctl_entry_num(ctl));
    }
    unsafe {
        rtw_rust_hal_com_mbid_log_clean(FN_CAMID_CLEAN.as_ptr().cast(), mbss_camid);
    }
}