// SPDX-License-Identifier: GPL-2.0
//! W3-62 unassoc STA rx/add/undo helpers (included from rtw_mlme_unassoc.rs).

use super::*;

#[no_mangle]
pub extern "C" fn rtw_rx_add_unassoc_sta(
    adapter: *mut Adapter,
    stype: U8,
    addr: *mut U8,
    recv_signal_power: S8,
) {
    if adapter.is_null() || addr.is_null() {
        return;
    }
    unsafe {
        let mp = &mut (*adapter).mlmepriv;
        let head = &mut mp.unassoc_sta_queue.queue as *mut List;
        let mut list = (*head).next;
        let mut oldest: *mut UnassocStaInfo = core::ptr::null_mut();
        while list != head {
            let sta = list as *mut UnassocStaInfo;
            list = (*list).next;
            if memcmp_eq(addr, (*sta).addr.as_ptr(), ETH_ALEN) {
                if (*sta).interested != 0
                    || mp.unassoc_sta_mode_of_stype[stype as usize] >= UNASOC_STA_MODE_ALL
                {
                    (*sta).recv_signal_power = recv_signal_power;
                    (*sta).time = rtw_get_current_time();
                }
                return;
            }
            if del_unassoc_sta_chk(mp, sta) == UNASOC_STA_DEL_CHK_ALIVE {
                if oldest.is_null() {
                    oldest = sta;
                } else if rtw_time_before((*sta).time, (*oldest).time) {
                    oldest = sta;
                }
            }
        }
        if mp.unassoc_sta_mode_of_stype[stype as usize] <= UNASOC_STA_MODE_INTERESTED {
            return;
        }
        let mut sta = alloc_unassoc_sta(mp);
        if sta.is_null() {
            if !oldest.is_null() {
                del_unassoc_sta(mp, oldest);
                sta = alloc_unassoc_sta(mp);
            }
            if sta.is_null() {
                return;
            }
        }
        core::ptr::copy_nonoverlapping(addr, (*sta).addr.as_mut_ptr(), ETH_ALEN);
        (*sta).recv_signal_power = recv_signal_power;
        (*sta).time = rtw_get_current_time();
        list_insert_tail(&mut (*sta).list, &mut mp.unassoc_sta_queue.queue);
    }
}

#[no_mangle]
pub extern "C" fn rtw_add_interested_unassoc_sta(adapter: *mut Adapter, addr: *mut U8) {
    if adapter.is_null() || addr.is_null() {
        return;
    }
    unsafe {
        let mp = &mut (*adapter).mlmepriv;
        let head = &mut mp.unassoc_sta_queue.queue as *mut List;
        let mut list = (*head).next;
        let mut oldest: *mut UnassocStaInfo = core::ptr::null_mut();
        while list != head {
            let sta = list as *mut UnassocStaInfo;
            list = (*list).next;
            if memcmp_eq(addr, (*sta).addr.as_ptr(), ETH_ALEN) {
                if (*sta).interested == 0 {
                    (*sta).interested = 1;
                    mp.interested_unassoc_sta_cnt += 1;
                    if mp.interested_unassoc_sta_cnt == 1 {
                        rtw_run_in_thread_cmd(
                            adapter,
                            rtw_hal_rcr_set_chk_bssid_act_non as *mut c_void,
                            adapter,
                        );
                    }
                }
                return;
            }
            if del_unassoc_sta_chk(mp, sta) == UNASOC_STA_DEL_CHK_ALIVE {
                if oldest.is_null() {
                    oldest = sta;
                } else if rtw_time_after((*sta).time, (*oldest).time) {
                    oldest = sta;
                }
            }
        }
        let mut sta = alloc_unassoc_sta(mp);
        if sta.is_null() {
            if !oldest.is_null() {
                del_unassoc_sta(mp, oldest);
                sta = alloc_unassoc_sta(mp);
            }
            if sta.is_null() {
                return;
            }
        }
        core::ptr::copy_nonoverlapping(addr, (*sta).addr.as_mut_ptr(), ETH_ALEN);
        (*sta).interested = 1;
        (*sta).recv_signal_power = 0;
        (*sta).time =
            rtw_get_current_time().wrapping_sub(rtw_ms_to_systime(UNASSOC_STA_LIFETIME_MS));
        list_insert_tail(&mut (*sta).list, &mut mp.unassoc_sta_queue.queue);
        mp.interested_unassoc_sta_cnt += 1;
        if mp.interested_unassoc_sta_cnt == 1 {
            rtw_run_in_thread_cmd(
                adapter,
                rtw_hal_rcr_set_chk_bssid_act_non as *mut c_void,
                adapter,
            );
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_undo_interested_unassoc_sta(adapter: *mut Adapter, addr: *mut U8) {
    if adapter.is_null() || addr.is_null() {
        return;
    }
    unsafe {
        let mp = &mut (*adapter).mlmepriv;
        let head = &mut mp.unassoc_sta_queue.queue as *mut List;
        let mut list = (*head).next;
        while list != head {
            let sta = list as *mut UnassocStaInfo;
            list = (*list).next;
            if memcmp_eq(addr, (*sta).addr.as_ptr(), ETH_ALEN) {
                if (*sta).interested != 0 {
                    (*sta).interested = 0;
                    mp.interested_unassoc_sta_cnt = mp.interested_unassoc_sta_cnt.saturating_sub(1);
                    if mp.interested_unassoc_sta_cnt == 0 {
                        rtw_run_in_thread_cmd(
                            adapter,
                            rtw_hal_rcr_set_chk_bssid_act_non as *mut c_void,
                            adapter,
                        );
                    }
                }
                break;
            }
        }
    }
}
