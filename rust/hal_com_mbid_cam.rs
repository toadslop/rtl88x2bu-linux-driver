// SPDX-License-Identifier: GPL-2.0
// CONFIG_MBSSID_CAM init/search (W4-119) + alloc/assign/clean (W4-120) + HW push (W4-126).

use core::ffi::{c_char, c_ulong, c_void};

type U8 = u8;
type U32 = u32;

const REG_MBID_NUM: U32 = 0x0552;
const MBIDCAM_ADDR_MASK: U32 = 0x1F;
const MBIDCAM_ADDR_SHIFT: U32 = 24;
const RCR_ENMBID: U32 = 1 << 24;

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
    fn rtw_rust_hal_com_mbid_log_set_macaddr(
        func: *const c_char,
        adapter: *mut c_void,
        entry_id: U8,
        mac_addr: *mut U8,
    );
    fn rtw_rust_hal_com_mbid_cache_init(adapter: *mut c_void, dvobj: *mut c_void, idx: U8, mac: *mut U8);
    fn rtw_rust_hal_com_mbid_cache_clr(dvobj: *mut c_void, idx: U8);
    fn rtw_rust_hal_com_warn_on(condition: i32);
    fn _rtw_memcmp(dst: *const c_void, src: *const c_void, sz: u32) -> i32;
    fn rtw_rust_hal_com_print_line(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_hal_com_dvobj_padapter(dvobj: *mut c_void, iface_id: U8) -> *mut c_void;
    fn rtw_rust_hal_com_mlme_is_sta(iface: *mut c_void) -> U8;
    fn rtw_rust_hal_com_mlme_is_ap(iface: *mut c_void) -> U8;
    fn rtw_rust_hal_com_mlme_is_mesh(iface: *mut c_void) -> U8;
    fn rtw_rust_hal_com_read8(adapter: *mut c_void, addr: U32) -> U8;
    fn rtw_rust_hal_com_write8(adapter: *mut c_void, addr: U32, val: U8);
    fn rtw_rust_hal_com_set_hwreg(adapter: *mut c_void, variable: U32, val: *mut U8);
    fn rtw_rust_hal_com_hw_var_mbssid_cam_write() -> i32;
    fn rtw_hal_rcr_add(adapter: *mut c_void, add: U32) -> U8;
    fn rtw_rust_hal_com_mbid_log_mi_set_cam(func: *const c_char, cam_id: U8, mac: *mut U8);
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
    fn rtw_rust_hal_com_mbid_log_set_macaddr(
        func: *const c_char,
        adapter: *mut c_void,
        entry_id: U8,
        mac_addr: *mut U8,
    );
    fn rtw_rust_hal_com_mbid_cache_init(adapter: *mut c_void, dvobj: *mut c_void, idx: U8, mac: *mut U8);
    fn rtw_rust_hal_com_mbid_cache_clr(dvobj: *mut c_void, idx: U8);
    fn rtw_rust_hal_com_warn_on(condition: i32);
    fn _rtw_memcmp(dst: *const c_void, src: *const c_void, sz: u32) -> i32;
    fn rtw_rust_hal_com_print_line(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_hal_com_dvobj_padapter(dvobj: *mut c_void, iface_id: U8) -> *mut c_void;
    fn rtw_rust_hal_com_mlme_is_sta(iface: *mut c_void) -> U8;
    fn rtw_rust_hal_com_mlme_is_ap(iface: *mut c_void) -> U8;
    fn rtw_rust_hal_com_mlme_is_mesh(iface: *mut c_void) -> U8;
    fn rtw_rust_hal_com_read8(adapter: *mut c_void, addr: U32) -> U8;
    fn rtw_rust_hal_com_write8(adapter: *mut c_void, addr: U32, val: U8);
    fn rtw_rust_hal_com_set_hwreg(adapter: *mut c_void, variable: U32, val: *mut U8);
    fn rtw_rust_hal_com_hw_var_mbssid_cam_write() -> i32;
    fn rtw_hal_rcr_add(adapter: *mut c_void, add: U32) -> U8;
    fn rtw_rust_hal_com_mbid_log_mi_set_cam(func: *const c_char, cam_id: U8, mac: *mut U8);
}

static MBID_MAC_SEARCH: &[u8] = b"_rtw_mbid_cam_search_by_macaddr\0";
static FN_MI_SET_MBID_CAM: &[u8] = b"rtw_mi_set_mbid_cam\0";
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

fn mbid_print(sel: *mut c_void, line: &[u8]) {
    let mut buf = [0i8; 128];
    let n = core::cmp::min(line.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = line[i] as i8;
    }
    unsafe {
        rtw_rust_hal_com_print_line(sel, buf.as_ptr());
    }
}

fn mbid_print_u8(sel: *mut c_void, mut n: U8) {
    let mut tmp = [0u8; 3];
    let mut len = 0usize;
    if n == 0 {
        mbid_print(sel, b"0");
        return;
    }
    while n > 0 {
        tmp[len] = b'0' + (n % 10);
        len += 1;
        n /= 10;
    }
    let mut line = [0u8; 4];
    for i in 0..len {
        line[i] = tmp[len - 1 - i];
    }
    mbid_print(sel, &line[..len]);
}

fn mbid_print_hex32(sel: *mut c_void, n: U8) {
    const HEX: &[u8; 16] = b"0123456789abcdef";
    let v = n as u32;
    let mut line = [0u8; 8];
    for (i, shift) in (0..=28).rev().step_by(4).enumerate() {
        line[i] = HEX[((v >> shift) & 0xf) as usize];
    }
    mbid_print(sel, &line);
}

fn mbid_print_hex8(sel: *mut c_void, n: U8) {
    const HEX: &[u8; 16] = b"0123456789abcdef";
    let line = [HEX[(n >> 4) as usize], HEX[(n & 0xf) as usize]];
    mbid_print(sel, &line);
}

fn mbid_print_cstr(sel: *mut c_void, p: *const c_char) {
    if p.is_null() {
        return;
    }
    unsafe {
        let mut cur = p;
        while *cur != 0 {
            let byte = [*cur as u8];
            mbid_print(sel, &byte);
            cur = cur.add(1);
        }
    }
}

#[cfg(any(config_mbssid_cam, host_hal_com_mbid_cam_test))]
#[no_mangle]
pub extern "C" fn rtw_mbid_cam_cache_dump(
    sel: *mut c_void,
    fun_name: *const c_char,
    adapter: *mut c_void,
) -> i32 {
    if adapter.is_null() {
        return 0;
    }
    let dvobj = unsafe { dvobj_of(adapter) };
    let ctl = unsafe { ctl_of_dvobj(dvobj) };

    mbid_print(sel, b"== MBSSID CAM DUMP (");
    mbid_print_cstr(sel, fun_name);
    mbid_print(sel, b")==\n");

    unsafe {
        let entry_num =
            rtw_rust_hal_com_atomic_read(rtw_rust_hal_com_mbid_ctl_entry_num(ctl)) as U8;
        let max_cam_id = rtw_get_max_mbid_cam_id(adapter);
        with_mbid_lock(ctl, || {
            let bitmap = rtw_rust_hal_com_mbid_ctl_bitmap(ctl);
            mbid_print(sel, b"Entry numbers:");
            mbid_print_u8(sel, entry_num);
            mbid_print(sel, b", max_camid:");
            mbid_print_u8(sel, max_cam_id);
            mbid_print(sel, b", bitmap:0x");
            mbid_print_hex32(sel, bitmap);
            mbid_print(sel, b"\n");
            for i in 0..TOTAL_MBID_CAM_NUM {
                mbid_print(sel, b"CAM_ID = ");
                mbid_print_u8(sel, i);
                mbid_print(sel, b"\t");
                if bitmap & (1u8.wrapping_shl(i as u32)) != 0 {
                    let iface_id = rtw_rust_hal_com_mbid_cache_iface_id(dvobj, i);
                    let mac = rtw_rust_hal_com_mbid_cache_mac(dvobj, i);
                    mbid_print(sel, b"IF_ID:");
                    mbid_print_u8(sel, iface_id);
                    mbid_print(sel, b"\tMAC Addr:");
                    if !mac.is_null() {
                        for b in 0..ETH_ALEN as usize {
                            if b > 0 {
                                mbid_print(sel, b":");
                            }
                            mbid_print_hex8(sel, *mac.add(b));
                        }
                    }
                    mbid_print(sel, b"\t");
                    let iface = rtw_rust_hal_com_dvobj_padapter(dvobj, iface_id);
                    if !iface.is_null() {
                        let role: &[u8] = if rtw_rust_hal_com_mlme_is_sta(iface) != 0 {
                            b"ROLE:STA\n"
                        } else if rtw_rust_hal_com_mlme_is_ap(iface) != 0 {
                            b"ROLE:AP\n"
                        } else if rtw_rust_hal_com_mlme_is_mesh(iface) != 0 {
                            b"ROLE:MESH\n"
                        } else {
                            b"ROLE:NONE\n"
                        };
                        mbid_print(sel, role);
                    }
                } else {
                    mbid_print(sel, b"N/A\n");
                }
            }
        });
    }
    0
}

#[cfg(any(config_mbssid_cam, host_hal_com_mbid_cam_test))]
fn write_mbssid_cam_inner(adapter: *mut c_void, cam_addr: U8, mac: *mut U8) {
    if adapter.is_null() || mac.is_null() {
        return;
    }
    let mut cam_val: [U32; 2] = [0, 0];
    unsafe {
        let m0 = *mac;
        let m1 = *mac.add(1);
        let m2 = *mac.add(2);
        let m3 = *mac.add(3);
        let m4 = *mac.add(4);
        let m5 = *mac.add(5);
        cam_val[0] = (u32::from(m3) << 24)
            | (u32::from(m2) << 16)
            | (u32::from(m1) << 8)
            | u32::from(m0);
        cam_val[1] = ((u32::from(cam_addr) & MBIDCAM_ADDR_MASK) << MBIDCAM_ADDR_SHIFT)
            | (u32::from(m5) << 8)
            | u32::from(m4);
        let hw_var = rtw_rust_hal_com_hw_var_mbssid_cam_write() as U32;
        rtw_rust_hal_com_set_hwreg(adapter, hw_var, cam_val.as_mut_ptr().cast());
    }
}

#[cfg(any(config_mbssid_cam, host_hal_com_mbid_cam_test))]
#[no_mangle]
pub extern "C" fn write_mbssid_cam(adapter: *mut c_void, cam_addr: U8, mac: *mut U8) {
    write_mbssid_cam_inner(adapter, cam_addr, mac);
}

#[cfg(any(config_mbssid_cam, host_hal_com_mbid_cam_test))]
#[no_mangle]
pub extern "C" fn rtw_ap_set_mbid_num(adapter: *mut c_void, ap_num: U8) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let cur = rtw_rust_hal_com_read8(adapter, REG_MBID_NUM);
        let val = (cur & 0xF8) | ap_num.wrapping_sub(1) & 0x07;
        rtw_rust_hal_com_write8(adapter, REG_MBID_NUM, val);
    }
}

#[cfg(any(config_mbssid_cam, host_hal_com_mbid_cam_test))]
#[no_mangle]
pub extern "C" fn rtw_mbid_cam_enable(adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        rtw_hal_rcr_add(adapter, RCR_ENMBID);
    }
}

#[cfg(any(config_mbssid_cam, host_hal_com_mbid_cam_test))]
#[no_mangle]
pub extern "C" fn rtw_mi_set_mbid_cam(adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    let dvobj = dvobj_of(adapter);
    let ctl = unsafe { ctl_of_dvobj(dvobj) };
    if dvobj.is_null() || ctl.is_null() {
        return;
    }
    let bitmap = unsafe { rtw_rust_hal_com_mbid_ctl_bitmap(ctl) };
    for i in 0..TOTAL_MBID_CAM_NUM {
        if bitmap & (1u8.wrapping_shl(i as u32)) != 0 {
            let mac = unsafe { rtw_rust_hal_com_mbid_cache_mac(dvobj, i) };
            write_mbssid_cam_inner(adapter, i, mac);
            unsafe {
                rtw_rust_hal_com_mbid_log_mi_set_cam(FN_MI_SET_MBID_CAM.as_ptr().cast(), i, mac);
            }
        }
    }
    unsafe {
        rtw_hal_rcr_add(adapter, RCR_ENMBID);
    }
}

#[cfg(any(config_mi_with_mbssid_cam, host_hal_com_mbid_cam_test))]
static FN_SET_MACADDR_MBID: &[u8] = b"rtw_hal_set_macaddr_mbid\0";

#[cfg(any(config_mi_with_mbssid_cam, host_hal_com_mbid_cam_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_set_macaddr_mbid(adapter: *mut c_void, mac_addr: *mut U8) {
    if adapter.is_null() || mac_addr.is_null() {
        return;
    }
    let entry_id = rtw_mbid_camid_alloc(adapter, mac_addr);
    if entry_id != INVALID_CAM_ID {
        write_mbssid_cam_inner(adapter, entry_id, mac_addr);
        unsafe {
            rtw_rust_hal_com_mbid_log_set_macaddr(
                FN_SET_MACADDR_MBID.as_ptr().cast(),
                adapter,
                entry_id,
                mac_addr,
            );
        }
    }
}

#[cfg(any(config_mi_with_mbssid_cam, host_hal_com_mbid_cam_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_change_macaddr_mbid(adapter: *mut c_void, mac_addr: *mut U8) {
    if mac_addr.is_null() {
        unsafe {
            rtw_rust_hal_com_warn_on(1);
        }
        return;
    }
    let entry_id = rtw_mbid_cam_info_change(adapter, mac_addr);
    if entry_id != INVALID_CAM_ID {
        write_mbssid_cam_inner(adapter, entry_id, mac_addr);
    }
}