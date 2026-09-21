// SPDX-License-Identifier: GPL-2.0
#![allow(dead_code, non_snake_case, non_upper_case_globals)]

use std::os::raw::{c_int, c_schar, c_uint, c_ulong, c_void};

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;
const ETH_ALEN: usize = 6;
const RTW_MAX_NB_RPT_NUM: usize = 8;
const RTW_WLAN_ACTION_WNM_NB_RPT_ELEM: u8 = 0x34;
const WNM_BTM_CAND_PREF_SUBEID: u8 = 0x03;

static WNM_DEFAULT_VALIDITY: u32 = 6000;
static WNM_DEFAULT_DISASSOC: u32 = 5000;
const WNM_BTM_TERM_DUR_SUBEID: u8 = 0x04;
const BSS_TERMINATION_INCLUDED: u8 = 1 << 3;
const ESS_DISASSOC_IMMINENT: u8 = 1 << 4;

static mut HOST_PASSING_MS: u32 = 0;

#[repr(C)]
pub struct BtmTermDuration {
    pub id: u8,
    pub len: u8,
    pub tsf: u64,
    pub duration: u16,
}

#[repr(C)]
pub struct BtmReqHdr {
    pub dialog_token: u8,
    pub req_mode: u8,
    pub disassoc_timer: u16,
    pub validity_interval: u8,
    pub term_duration: BtmTermDuration,
}

#[repr(C)]
pub struct BtmRptCache {
    pub dialog_token: u8,
    pub req_mode: u8,
    pub disassoc_timer: u16,
    pub validity_interval: u8,
    pub term_duration: BtmTermDuration,
    pub validity_time: u32,
    pub disassoc_time: u32,
    pub req_stime: c_ulong,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct NbRptHdr {
    pub id: u8,
    pub len: u8,
    pub bssid: [u8; ETH_ALEN],
    pub bss_info: u32,
    pub reg_class: u8,
    pub ch_num: u8,
    pub phy_type: u8,
}

#[repr(C)]
pub struct WnmBtmCant {
    pub nb_rpt: NbRptHdr,
    pub preference: u8,
}

#[repr(C)]
pub struct RoamNbInfo {
    pub nb_rpt: [NbRptHdr; RTW_MAX_NB_RPT_NUM],
    pub btm_cache: BtmRptCache,
    pub preference_en: u8,
    pub roam_target_addr: [u8; ETH_ALEN],
    pub last_nb_rpt_entries: u32,
    pub nb_rpt_is_same: u8,
    pub disassoc_waiting: c_schar,
}

#[repr(C)]
pub struct MlmePriv {
    pub nb_info: RoamNbInfo,
}

#[repr(C)]
pub struct Adapter {
    pub mlmepriv: MlmePriv,
}

fn btm_bss_term_inc(p: *const u8) -> bool {
    unsafe { *p.add(3) & BSS_TERMINATION_INCLUDED != 0 }
}

fn btm_ess_disassoc_im(p: *const u8) -> bool {
    unsafe { *p.add(3) & ESS_DISASSOC_IMMINENT != 0 }
}

#[no_mangle]
pub extern "C" fn host_wnm_set_passing_ms(ms: c_uint) {
    unsafe { HOST_PASSING_MS = ms };
}

#[no_mangle]
pub extern "C" fn rtw_get_passing_time_ms(_start: c_ulong) -> c_int {
    unsafe { HOST_PASSING_MS as c_int }
}

#[no_mangle]
pub extern "C" fn host_wnm_btm_req_hdr_parsing(pframe: *mut u8, phdr: *mut BtmReqHdr) {
    if pframe.is_null() || phdr.is_null() {
        return;
    }
    unsafe {
        let ph = &mut *phdr;
        *ph = std::mem::zeroed();
        ph.dialog_token = *pframe.add(2);
        ph.req_mode = *pframe.add(3);
        ph.disassoc_timer = *(pframe.add(4) as *const u16);
        ph.validity_interval = *pframe.add(6);
        if btm_bss_term_inc(pframe) {
            let pos = pframe.add(7);
            if *pos == WNM_BTM_TERM_DUR_SUBEID {
                ph.term_duration.id = *pos;
                ph.term_duration.len = *pos.add(1);
                std::ptr::copy_nonoverlapping(pos.add(2) as *const u8, &mut ph.term_duration.tsf as *mut u64 as *mut u8, 8);
                std::ptr::copy_nonoverlapping(pos.add(10) as *const u8, &mut ph.term_duration.duration as *mut u16 as *mut u8, 2);
            }
        }
    }
}

#[no_mangle]
pub extern "C" fn host_wnm_btm_candidates_offset_get(pframe: *mut u8) -> c_uint {
    if pframe.is_null() {
        return 0;
    }
    unsafe {
        let mut offset: u32 = 7;
        if btm_bss_term_inc(pframe) {
            offset += 12;
        }
        if btm_ess_disassoc_im(pframe) {
            offset = 1 + *pframe.add(offset as usize) as u32;
        }
        offset
    }
}

#[no_mangle]
pub extern "C" fn host_wnm_btm_candidate_validity(pcache: *mut BtmRptCache, flag: u8) -> u8 {
    if pcache.is_null() {
        return _FALSE;
    }
    unsafe {
        let c = &*pcache;
        let req = rtw_get_passing_time_ms(c.req_stime) as u32;
        let mut ok = _TRUE;
        if flag & 1 != 0 && req > c.validity_time {
            ok = _FALSE;
        }
        if flag & 2 != 0 && req > c.disassoc_time {
            ok = _FALSE;
        }
        ok
    }
}

#[no_mangle]
pub extern "C" fn host_wnm_btm_rsp_candidates_sz_get(
    _padapter: *mut Adapter,
    pframe: *mut u8,
    frame_len: c_uint,
) -> c_uint {
    if pframe.is_null() || frame_len <= 5 {
        return 0;
    }
    unsafe {
        let status = *pframe.add(3);
        if (status != 0 && status != 6) || frame_len < 23 {
            return 0;
        }
        let num = if status == 0 {
            (frame_len - 5 - ETH_ALEN as u32) / 18
        } else {
            (frame_len - 5) / 18
        };
        (std::mem::size_of::<WnmBtmCant>() as u32) * num
    }
}

#[no_mangle]
pub extern "C" fn rtw_get_ie(
    pbuf: *const u8,
    index: c_int,
    len: *mut c_int,
    limit: c_int,
) -> *mut u8 {
    if limit < 1 || pbuf.is_null() || len.is_null() {
        return std::ptr::null_mut();
    }
    unsafe {
        let mut p = pbuf;
        let mut i = 0;
        *len = 0;
        loop {
            if *p == index as u8 {
                *len = *p.add(1) as c_int;
                return p as *mut u8;
            }
            let tmp = *p.add(1) as c_int;
            p = p.add((tmp + 2) as usize);
            i += tmp + 2;
            if i >= limit {
                break;
            }
        }
    }
    std::ptr::null_mut()
}

fn rtw_memcmp(s1: *const c_void, s2: *const c_void, n: usize) -> u8 {
    if s1.is_null() || s2.is_null() {
        return _FALSE;
    }
    unsafe {
        if std::slice::from_raw_parts(s1 as *const u8, n)
            == std::slice::from_raw_parts(s2 as *const u8, n)
        {
            _TRUE
        } else {
            _FALSE
        }
    }
}

#[no_mangle]
pub extern "C" fn host_wnm_nb_elem_parsing(
    pdata: *mut u8,
    data_len: c_uint,
    from_btm: u8,
    nb_rpt_num: *mut c_uint,
    nb_rpt_is_same: *mut u8,
    pnb: *mut RoamNbInfo,
    pcandidates: *mut WnmBtmCant,
) -> u8 {
    if pdata.is_null() || pnb.is_null() || nb_rpt_num.is_null() {
        return 1;
    }
    if from_btm != 0 && pcandidates.is_null() {
        return 1;
    }
    unsafe {
        let nb = &mut *pnb;
        let mut ptr = pdata;
        let pend = pdata.add(data_len as usize);
        let mut elem_len = data_len;
        let mut subelem_len = *pdata.add(1) as u32;
        let mut entries = 0u32;
        let mut bfound = false;
        for i in 0..RTW_MAX_NB_RPT_NUM {
            if ptr.add(7) > pend || elem_len < subelem_len {
                break;
            }
            if *ptr != RTW_WLAN_ACTION_WNM_NB_RPT_ELEM {
                break;
            }
            let pie = ptr as *const NbRptHdr;
            let mut op_len: c_int = 0;
            let op = if from_btm != 0 {
                rtw_get_ie(
                    ptr.add(15),
                    WNM_BTM_CAND_PREF_SUBEID as c_int,
                    &mut op_len,
                    (subelem_len - 15) as c_int,
                )
            } else {
                std::ptr::null_mut()
            };
            ptr = ptr.add((subelem_len + 2) as usize);
            elem_len -= subelem_len + 2;
            if ptr.add(1) < pend {
                subelem_len = *ptr.add(1) as u32;
            }
            if from_btm != 0 {
                let pc = &mut *pcandidates.add(i);
                std::ptr::copy_nonoverlapping(pie, &mut pc.nb_rpt, 1);
                if !op.is_null() && op_len != 0 {
                    pc.preference = *op.add(2);
                    bfound = true;
                } else {
                    pc.preference = 0;
                }
            } else if !nb_rpt_is_same.is_null() {
                if rtw_memcmp(
                    &nb.nb_rpt[i] as *const _ as *const c_void,
                    pie as *const c_void,
                    std::mem::size_of::<NbRptHdr>(),
                ) == _FALSE
                {
                    *nb_rpt_is_same = _FALSE;
                }
                std::ptr::copy_nonoverlapping(pie, &mut nb.nb_rpt[i], 1);
            }
            entries += 1;
        }
        if from_btm != 0 {
            nb.preference_en = if bfound { _TRUE } else { _FALSE };
        }
        *nb_rpt_num = entries;
        0
    }
}

#[no_mangle]
pub extern "C" fn host_wnm_reset_btm_candidate(pnb: *mut RoamNbInfo) {
    if pnb.is_null() {
        return;
    }
    unsafe {
        let n = &mut *pnb;
        n.preference_en = _FALSE;
        n.roam_target_addr = [0; ETH_ALEN];
    }
}

#[no_mangle]
pub extern "C" fn host_wnm_reset_btm_cache(padapter: *mut Adapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        let nb_ptr = &mut (*padapter).mlmepriv.nb_info as *mut RoamNbInfo;
        if host_wnm_btm_candidate_validity(&mut (*nb_ptr).btm_cache, 1 << 0) != 0 {
            return;
        }
        host_wnm_reset_btm_candidate(nb_ptr);
        (*nb_ptr).btm_cache = std::mem::zeroed();
        (*nb_ptr).btm_cache.validity_time = WNM_DEFAULT_VALIDITY;
        (*nb_ptr).btm_cache.disassoc_time = WNM_DEFAULT_DISASSOC;
    }
}

#[no_mangle]
pub extern "C" fn host_wnm_reset_btm_state(padapter: *mut Adapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        let nb = &mut (*padapter).mlmepriv.nb_info;
        nb.last_nb_rpt_entries = 0;
        nb.nb_rpt_is_same = _TRUE;
        nb.disassoc_waiting = -1;
        nb.nb_rpt = [NbRptHdr {
            id: 0,
            len: 0,
            bssid: [0; ETH_ALEN],
            bss_info: 0,
            reg_class: 0,
            ch_num: 0,
            phy_type: 0,
        }; RTW_MAX_NB_RPT_NUM];
        host_wnm_reset_btm_cache(padapter);
    }
}
