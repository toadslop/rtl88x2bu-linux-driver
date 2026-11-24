// SPDX-License-Identifier: GPL-2.0
// W4-130 — periodic TSF restore check (hal/hal_com_rest.c).

const MLME_ACTION_NONE: U8 = 1;
const OFFCHS_NONE: U8 = 0;

extern "C" {
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_get_current_time() -> c_ulong;
    fn rtw_rust_hal_com_get_passing_time_ms(start: c_ulong) -> u32;
    fn rtw_rust_hal_com_time_after(a: c_ulong, b: c_ulong) -> i32;
    fn rtw_rust_hal_com_ms_to_systime(ms: i32) -> c_ulong;
    fn rtw_hal_rcr_set_chk_bssid(adapter: *mut c_void, self_action: U8);
    fn rtw_rust_hal_com_tsf_periodic_etime(dvobj: *mut c_void) -> c_ulong;
    fn rtw_rust_hal_com_tsf_periodic_set_etime(dvobj: *mut c_void, etime: c_ulong);
    fn rtw_rust_hal_com_tsf_rf_offch_state(dvobj: *mut c_void) -> U8;
    fn rtw_rust_hal_com_tsf_iface_nums(dvobj: *mut c_void) -> i32;
    fn rtw_rust_hal_com_tsf_iface_at(dvobj: *mut c_void, i: i32) -> *mut c_void;
    fn rtw_rust_hal_com_tsf_ext_required(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_tsf_ext_pause_stime(adapter: *mut c_void) -> c_ulong;
    fn rtw_rust_hal_com_tsf_ext_pause_factor(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_tsf_ext_restore_factor(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_tsf_ext_bcn_interval(adapter: *mut c_void) -> u16;
    fn rtw_rust_hal_com_tsf_periodic_arm_timer(dvobj: *mut c_void, ms: u32);
    fn rtw_rust_hal_com_dvobj_primary(dvobj: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_com_dev_surprise(dvobj: *mut c_void) -> i32;
    fn rtw_rust_hal_com_dev_stopped(dvobj: *mut c_void) -> i32;
    fn rtw_rust_hal_com_periodic_tsf_end_cmd(adapter: *mut c_void) -> U8;
}

#[no_mangle]
pub extern "C" fn rtw_hal_periodic_tsf_update_chk(adapter: *mut c_void) {
    #[cfg(not(config_mi_with_mbssid_cam))]
    unsafe {
        let dvobj = rtw_rust_hal_com_adapter_to_dvobj(adapter);
        if dvobj.is_null() {
            return;
        }

        let etime = rtw_rust_hal_com_tsf_periodic_etime(dvobj);
        if etime != 0 {
            if rtw_rust_hal_com_time_after(rtw_rust_hal_com_get_current_time(), etime) != 0 {
                rtw_rust_hal_com_tsf_periodic_set_etime(dvobj, 0);
                rtw_hal_rcr_set_chk_bssid(adapter, MLME_ACTION_NONE);
            }
            return;
        }

        if rtw_rust_hal_com_tsf_rf_offch_state(dvobj) != OFFCHS_NONE {
            return;
        }

        let iface_nums = rtw_rust_hal_com_tsf_iface_nums(dvobj);
        let mut restore_ms: u32 = 0;
        let mut i = 0;
        while i < iface_nums {
            let iface = rtw_rust_hal_com_tsf_iface_at(dvobj, i);
            i += 1;
            if iface.is_null() {
                continue;
            }
            if rtw_rust_hal_com_tsf_ext_required(iface) == 0 {
                continue;
            }
            let pause_stime = rtw_rust_hal_com_tsf_ext_pause_stime(iface);
            if pause_stime == 0 {
                continue;
            }
            let bcn = u32::from(rtw_rust_hal_com_tsf_ext_bcn_interval(iface));
            let pause_factor = u32::from(rtw_rust_hal_com_tsf_ext_pause_factor(iface));
            let elapsed = rtw_rust_hal_com_get_passing_time_ms(pause_stime);
            let threshold = bcn * pause_factor;
            let past = if elapsed <= i32::MAX as u32 && threshold <= i32::MAX as u32 {
                (elapsed as i32) > (threshold as i32)
            } else {
                elapsed > threshold
            };
            if !past {
                continue;
            }
            let cand = bcn * u32::from(rtw_rust_hal_com_tsf_ext_restore_factor(iface));
            if restore_ms < cand {
                restore_ms = cand;
            }
        }

        if restore_ms == 0 {
            return;
        }

        let mut new_etime = rtw_rust_hal_com_get_current_time().wrapping_add(
            rtw_rust_hal_com_ms_to_systime(restore_ms as i32),
        );
        if new_etime == 0 {
            new_etime = 1;
        }
        rtw_rust_hal_com_tsf_periodic_set_etime(dvobj, new_etime);
        rtw_hal_rcr_set_chk_bssid(adapter, MLME_ACTION_NONE);
        rtw_rust_hal_com_tsf_periodic_arm_timer(dvobj, restore_ms);
    }
    #[cfg(config_mi_with_mbssid_cam)]
    let _ = adapter;
}

#[no_mangle]
pub extern "C" fn rtw_hal_periodic_tsf_update_end_timer_hdl(ctx: *mut c_void) {
    if ctx.is_null() {
        return;
    }
    unsafe {
        if rtw_rust_hal_com_dev_surprise(ctx) != 0 || rtw_rust_hal_com_dev_stopped(ctx) != 0 {
            return;
        }
        let adapter = rtw_rust_hal_com_dvobj_primary(ctx);
        if adapter.is_null() {
            return;
        }
        rtw_rust_hal_com_periodic_tsf_end_cmd(adapter);
    }
}