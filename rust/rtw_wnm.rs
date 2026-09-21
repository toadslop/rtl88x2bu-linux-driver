// SPDX-License-Identifier: GPL-2.0
#![allow(dead_code, non_snake_case, non_upper_case_globals)]

use std::os::raw::{c_int, c_uint, c_ulong};

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;
const ETH_ALEN: usize = 6;
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
pub struct Adapter {
    pub _pad: u8,
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
