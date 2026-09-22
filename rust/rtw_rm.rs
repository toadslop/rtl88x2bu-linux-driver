// SPDX-License-Identifier: GPL-2.0
//! RM parse helpers — Rust port part 1 (W3-111 PR5).

#![allow(non_camel_case_types, non_snake_case, improper_ctypes)]
#![cfg(any(host_rm_parse_test, rtw_80211k))]

use std::ffi::c_int;

const _SUCCESS: c_int = 1;
const RM_CH_LOAD_CAP_EN: c_int = 8;
const RM_NOISE_HISTO_CAP_EN: c_int = 9;
const RM_BCN_MEAS_REP_COND_CAP_EN: c_int = 7;

#[repr(C)]
pub struct OptRepInfo {
    pub cond: u8,
    pub threshold: u8,
}
#[repr(C)]
pub struct Ndis80211Ssid {
    pub SsidLength: u32,
    pub Ssid: [u8; 32],
}
#[repr(C)]
pub struct RtOperatingClass {
    pub global_op_class: i32,
    pub Len: i32,
    pub Channel: [u8; 11],
}
#[repr(C)]
pub struct BcnReqOpt {
    pub opt_id: [u8; 16],
    pub opt_id_num: u8,
    pub rep_detail: u8,
    pub ssid: Ndis80211Ssid,
    pub rep_cond: OptRepInfo,
    pub ap_ch_rpt_num: u8,
    pub ap_ch_rpt: [*mut RtOperatingClass; 12],
    pub req_start: *mut u8,
    pub req_len: u8,
}
#[repr(C)]
pub struct MeasReqOpt {
    pub rep_cond: OptRepInfo,
}
const RM_MEAS_OPT_BYTES: usize = 176;
#[repr(C)]
pub union RmMeasOpt {
    pub bytes: [u8; RM_MEAS_OPT_BYTES],
}
#[repr(C)]
pub struct RmMeasReq {
    pub m_type: u8,
    pub m_mode: u8,
    pub op_class: u8,
    pub ch_num: u8,
    pub rand_intvl: u16,
    pub meas_dur: u16,
    pub bssid: [u8; 6],
    pub _pad_bssid: [u8; 2],
    pub opt_s_elem_len: i32,
    pub _pad_opt: [u8; 4],
    pub opt: RmMeasOpt,
}
#[repr(C)]
pub struct RmObj {
    pub q: RmMeasReq,
}

#[repr(C)]
pub struct WlanNetwork {
    _pad: u8,
}

unsafe extern "C" {
    fn rm_en_cap_chk_and_set(prm: *mut RmObj, en: c_int) -> c_int;
    fn rm_get_bcn_rcpi(prm: *mut RmObj, pnetwork: *mut WlanNetwork) -> u8;
    fn rm_get_bcn_rsni(prm: *mut RmObj, pnetwork: *mut WlanNetwork) -> u8;
}

fn bcn_opt(prm: &mut RmObj) -> &mut BcnReqOpt {
    unsafe { &mut *(&mut prm.q.opt.bytes as *mut u8 as *mut BcnReqOpt) }
}
fn clm_opt(prm: &mut RmObj) -> &mut MeasReqOpt {
    unsafe { &mut *(&mut prm.q.opt.bytes as *mut u8 as *mut MeasReqOpt) }
}

#[no_mangle]
pub extern "C" fn rm_parse_ch_load_s_elem(
    prm: *mut RmObj,
    pbody: *mut u8,
    req_len: c_int,
) -> c_int {
    if prm.is_null() || pbody.is_null() {
        return _SUCCESS;
    }
    let prm = unsafe { &mut *prm };
    let body = unsafe { std::slice::from_raw_parts(pbody, req_len as usize) };
    let mut p = 0usize;
    let mut len = req_len;
    prm.q.opt_s_elem_len = len;
    while len > 0 && p + 1 < body.len() {
        if body[p] == 1 {
            unsafe {
                rm_en_cap_chk_and_set(prm, RM_CH_LOAD_CAP_EN);
            }
            let c = clm_opt(prm);
            c.rep_cond.cond = body[p + 2];
            c.rep_cond.threshold = body[p + 3];
        }
        let step = body[p + 1] as i32 + 2;
        len -= step;
        p += step as usize;
    }
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn rm_parse_noise_histo_s_elem(
    prm: *mut RmObj,
    pbody: *mut u8,
    req_len: c_int,
) -> c_int {
    if prm.is_null() || pbody.is_null() {
        return _SUCCESS;
    }
    let prm = unsafe { &mut *prm };
    let body = unsafe { std::slice::from_raw_parts(pbody, req_len as usize) };
    let mut p = 0usize;
    let mut len = req_len;
    prm.q.opt_s_elem_len = len;
    while len > 0 && p + 1 < body.len() {
        if body[p] == 1 {
            unsafe {
                rm_en_cap_chk_and_set(prm, RM_NOISE_HISTO_CAP_EN);
            }
            let n = clm_opt(prm);
            n.rep_cond.cond = body[p + 2];
            n.rep_cond.threshold = body[p + 3];
        }
        let step = body[p + 1] as i32 + 2;
        len -= step;
        p += step as usize;
    }
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn rm_parse_bcn_req_s_elem(
    prm: *mut RmObj,
    pbody: *mut u8,
    req_len: c_int,
) -> c_int {
    if prm.is_null() || pbody.is_null() {
        return _SUCCESS;
    }
    let prm = unsafe { &mut *prm };
    let body = unsafe { std::slice::from_raw_parts(pbody, req_len as usize) };
    let mut p = 0usize;
    let mut len = req_len;
    prm.q.opt_s_elem_len = len;
    while len > 0 && p + 1 < body.len() && (bcn_opt(prm).opt_id_num as usize) < 16 {
        match body[p] {
            0 => {
                let b = bcn_opt(prm);
                let sl = body[p + 1] as usize;
                b.ssid.SsidLength = sl as u32;
                let n = sl.min(31);
                b.ssid.Ssid[..n].copy_from_slice(&body[p + 2..p + 2 + n]);
                b.opt_id[b.opt_id_num as usize] = 0;
                b.opt_id_num += 1;
            }
            1 => {
                unsafe {
                    rm_en_cap_chk_and_set(prm, RM_BCN_MEAS_REP_COND_CAP_EN);
                }
                let b = bcn_opt(prm);
                b.rep_cond.cond = body[p + 2];
                b.rep_cond.threshold = body[p + 3];
            }
            2 => {
                let b = bcn_opt(prm);
                b.rep_detail = body[p + 2];
                b.opt_id[b.opt_id_num as usize] = 2;
                b.opt_id_num += 1;
            }
            _ => {}
        }
        let step = body[p + 1] as i32 + 2;
        len -= step;
        p += step as usize;
    }
    _SUCCESS
}

#[no_mangle]
pub extern "C" fn rm_parse_meas_req(prm: *mut RmObj, pbody: *mut u8) -> c_int {
    if prm.is_null() || pbody.is_null() {
        return _SUCCESS;
    }
    let prm = unsafe { &mut *prm };
    let body = unsafe { std::slice::from_raw_parts(pbody, 64) };
    let req_len = body[1] as i32;
    let mut p = 5usize;
    prm.q.op_class = body[p];
    p += 1;
    prm.q.ch_num = body[p];
    p += 1;
    prm.q.rand_intvl = u16::from_le_bytes([body[p], body[p + 1]]);
    p += 2;
    prm.q.meas_dur = u16::from_le_bytes([body[p], body[p + 1]]);
    p += 2;
    if prm.q.m_type == 5 {
        prm.q.m_mode = body[p];
        p += 1;
        prm.q.bssid.copy_from_slice(&body[p..p + 6]);
        p += 6;
        bcn_opt(prm).rep_detail = 2;
    }
    if req_len - (p as i32 - 2) <= 0 {
        return _SUCCESS;
    }
    let sub = req_len - (p as i32 - 2);
    match prm.q.m_type {
        5 => rm_parse_bcn_req_s_elem(prm, unsafe { pbody.add(p) }, sub),
        3 => rm_parse_ch_load_s_elem(prm, unsafe { pbody.add(p) }, sub),
        4 => rm_parse_noise_histo_s_elem(prm, unsafe { pbody.add(p) }, sub),
        _ => _SUCCESS,
    }
}

#[no_mangle]
pub extern "C" fn rm_bcn_req_cond_mach(prm: *mut RmObj, pnetwork: *mut WlanNetwork) -> u8 {
    if prm.is_null() {
        return 0;
    }
    let bcn = bcn_opt(unsafe { &mut *prm });
    let (cond, thr) = (bcn.rep_cond.cond, bcn.rep_cond.threshold);
    match cond {
        0 => _SUCCESS as u8,
        1 => {
            let v = unsafe { rm_get_bcn_rcpi(prm, pnetwork) };
            if v > thr {
                _SUCCESS as u8
            } else {
                0
            }
        }
        2 => {
            let v = unsafe { rm_get_bcn_rcpi(prm, pnetwork) };
            if v < thr {
                _SUCCESS as u8
            } else {
                0
            }
        }
        3 => {
            let v = unsafe { rm_get_bcn_rsni(prm, pnetwork) };
            if v != 255 && v > thr {
                _SUCCESS as u8
            } else {
                0
            }
        }
        4 => {
            let v = unsafe { rm_get_bcn_rsni(prm, pnetwork) };
            if v != 255 && v < thr {
                _SUCCESS as u8
            } else {
                0
            }
        }
        _ => 0,
    }
}
