// SPDX-License-Identifier: GPL-2.0
//! W3-128 beamforming entry lookup + packet leaf — host L2 oracle.

#![allow(dead_code, improper_ctypes, missing_docs, non_snake_case)]

#[cfg(host_bf_entry_packet_test)]
use std::ffi::c_void;
#[cfg(host_bf_entry_packet_test)]
use std::mem::offset_of;

type U8 = u8;
type U16 = u16;
type U32 = u32;

#[cfg(host_bf_entry_packet_test)]
#[repr(C)]
#[derive(Copy, Clone)]
pub enum BfHostCap {
    None = 0,
    BfeeVhtSu = 0x8,
}

#[cfg(host_bf_entry_packet_test)]
#[repr(C)]
pub struct BfHostBfee {
    pub used: U8,
    pub mac_id: U16,
    pub mac_addr: [U8; 6],
    pub cap: BfHostCap,
}

#[cfg(host_bf_entry_packet_test)]
#[repr(C)]
pub struct BfHostBfer {
    pub used: U8,
    pub mac_addr: [U8; 6],
}

#[cfg(host_bf_entry_packet_test)]
#[repr(C)]
pub struct BfHostCsi {
    pub Nc: U8,
    pub Nr: U8,
    pub Ng: U8,
    pub CodeBook: U8,
    pub ChnlWidth: U8,
    pub bVHT: U8,
}

#[cfg(host_bf_entry_packet_test)]
#[repr(C)]
pub struct BfHostInfo {
    pub bfee: [BfHostBfee; 8],
    pub bfer: [BfHostBfer; 3],
    pub bEnableSUTxBFWorkAround: U8,
    pub TargetCSIInfo: BfHostCsi,
    pub TargetSUBFee: *mut BfHostBfee,
}

#[cfg(host_bf_entry_packet_test)]
#[repr(C)]
pub struct BfHostHal {
    pub beamforming_info: BfHostInfo,
}

#[cfg(host_bf_entry_packet_test)]
#[repr(C)]
pub struct BfHostMlme {
    pub pad: U8,
}

#[cfg(host_bf_entry_packet_test)]
#[repr(C)]
pub struct BfHostAdpt {
    pub mlmepriv: BfHostMlme,
    pub hal: BfHostHal,
}

#[cfg(host_bf_entry_packet_test)]
#[repr(C)]
pub struct BfHostRx {
    pub len: U32,
    pub data: [U8; 256],
}

#[cfg(host_bf_entry_packet_test)]
#[repr(C)]
pub struct BfHostRecvFrame {
    pub hdr: BfHostRx,
}

#[cfg(host_bf_entry_packet_test)]
type BfHostPadpt = *mut BfHostAdpt;

#[cfg(host_bf_entry_packet_test)]
fn mac_eq(a: &[U8; 6], b: &[U8; 6]) -> bool {
    a == b
}

#[cfg(host_bf_entry_packet_test)]
unsafe fn bfer_by_addr(a: BfHostPadpt, ra: *mut U8) -> *mut BfHostBfer {
    if a.is_null() || ra.is_null() {
        return std::ptr::null_mut();
    }
    let info = &mut (*a).hal.beamforming_info;
    let ra = std::slice::from_raw_parts(ra, 6);
    for bfer in &mut info.bfer {
        if bfer.used == 0 {
            continue;
        }
        let mut m = [0u8; 6];
        m.copy_from_slice(ra);
        if mac_eq(&bfer.mac_addr, &m) {
            return bfer as *mut BfHostBfer;
        }
    }
    std::ptr::null_mut()
}

#[cfg(host_bf_entry_packet_test)]
unsafe fn bfee_by_addr(a: BfHostPadpt, ra: *mut U8) -> *mut BfHostBfee {
    if a.is_null() || ra.is_null() {
        return std::ptr::null_mut();
    }
    let info = &mut (*a).hal.beamforming_info;
    let ra = std::slice::from_raw_parts(ra, 6);
    for bfee in &mut info.bfee {
        if bfee.used == 0 {
            continue;
        }
        let mut m = [0u8; 6];
        m.copy_from_slice(ra);
        if mac_eq(&bfee.mac_addr, &m) {
            return bfee as *mut BfHostBfee;
        }
    }
    std::ptr::null_mut()
}

#[cfg(host_bf_entry_packet_test)]
unsafe fn cap_by_macid(a: BfHostPadpt, macid: U8) -> U32 {
    if a.is_null() {
        return 0;
    }
    let info = &(*a).hal.beamforming_info;
    for bfee in &info.bfee {
        if bfee.used == 0 {
            continue;
        }
        if bfee.mac_id == macid as U16 {
            return bfee.cap as u32;
        }
    }
    0
}

#[cfg(host_bf_entry_packet_test)]
#[no_mangle]
pub unsafe extern "C" fn o_cap_by_macid(mlme: *mut c_void, macid: U8) -> U32 {
    let adpt = (mlme as *mut U8).sub(offset_of!(BfHostAdpt, mlmepriv)) as BfHostPadpt;
    cap_by_macid(adpt, macid)
}

#[cfg(host_bf_entry_packet_test)]
#[no_mangle]
pub unsafe extern "C" fn o_bfer_by_addr(a: BfHostPadpt, ra: *mut U8) -> *mut BfHostBfer {
    bfer_by_addr(a, ra)
}

#[cfg(host_bf_entry_packet_test)]
#[no_mangle]
pub unsafe extern "C" fn o_bfee_by_addr(a: BfHostPadpt, ra: *mut U8) -> *mut BfHostBfee {
    bfee_by_addr(a, ra)
}

#[cfg(host_bf_entry_packet_test)]
const BF_HOST_SUCCESS: U32 = 1;
#[cfg(host_bf_entry_packet_test)]
const BF_HOST_FAIL: U32 = 0;

#[cfg(host_bf_entry_packet_test)]
extern "C" {
    fn bf_host_cmd(a: BfHostPadpt, ty: i32, p: *mut U8, sz: i32, enq: U8);
}

#[cfg(host_bf_entry_packet_test)]
#[no_mangle]
pub unsafe extern "C" fn o_ndpa(_a: BfHostPadpt, _f: *mut BfHostRecvFrame) {}

#[cfg(host_bf_entry_packet_test)]
#[no_mangle]
pub unsafe extern "C" fn o_report(adapter: BfHostPadpt, rf: *mut BfHostRecvFrame) -> U32 {
    if adapter.is_null() || rf.is_null() {
        return BF_HOST_FAIL;
    }
    let info = &mut (*adapter).hal.beamforming_info;
    let pframe = (*rf).hdr.data.as_mut_ptr();
    let bfee = bfee_by_addr(adapter, pframe.add(10));
    if bfee.is_null() {
        return BF_HOST_FAIL;
    }
    let body = pframe.add(24);
    let (cat, act) = (*body, *body.add(1));
    let mut nc = 0u8;
    let mut nr = 0u8;
    let mut ch_w = 0u8;
    let mut ng = 0u8;
    let mut code_book = 0u8;
    if cat == 21 && act == 0 {
        let mimo = pframe.add(26);
        nc = *mimo & 0x7;
        nr = (*mimo & 0x38) >> 3;
        ch_w = (*mimo & 0xC0) >> 6;
        ng = *mimo.add(1) & 0x3;
        code_book = (*mimo.add(1) & 0x4) >> 2;
        info.TargetCSIInfo.bVHT = 1;
    } else if cat == 0 && act == 6 {
        let mimo = pframe.add(26);
        nc = *mimo & 0x3;
        nr = (*mimo & 0xC) >> 2;
        ch_w = (*mimo & 0x10) >> 4;
        ng = (*mimo & 0x60) >> 5;
        code_book = (*mimo.add(1) & 0x6) >> 1;
        info.TargetCSIInfo.bVHT = 0;
    }
    if info.bEnableSUTxBFWorkAround != 0 && info.TargetSUBFee == bfee {
        let csi = &mut info.TargetCSIInfo;
        if csi.Nc != nc
            || csi.Nr != nr
            || csi.ChnlWidth != ch_w
            || csi.Ng != ng
            || csi.CodeBook != code_book
        {
            csi.Nc = nc;
            csi.Nr = nr;
            csi.ChnlWidth = ch_w;
            csi.Ng = ng;
            csi.CodeBook = code_book;
            bf_host_cmd(adapter, 7, csi as *mut BfHostCsi as *mut U8, 6, 1);
        }
    }
    BF_HOST_SUCCESS
}

#[cfg(all(host_bf_entry_packet_test, host_bf_init_cmd_test))]
const BF_HOST_IDX_NONE: U8 = 0xFF;

#[cfg(all(host_bf_entry_packet_test, host_bf_init_cmd_test))]
unsafe fn sounding_init(s: &mut BfHostSoundingInfo) {
    s.su_sounding_list = [BF_HOST_IDX_NONE; 2];
    s.mu_sounding_list = [BF_HOST_IDX_NONE; 6];
    s.state = 0;
    s.su_bfee_curidx = BF_HOST_IDX_NONE;
    s.candidate_mu_bfee_cnt = 0;
    s.min_sounding_period = 0;
    s.sound_remain_cnt_per_period = 0;
}

#[cfg(all(host_bf_entry_packet_test, host_bf_init_cmd_test))]
extern "C" {
    fn bf_host_beamforming_enter(a: BfHostPadpt, p: *mut U8);
    fn bf_host_beamforming_leave(a: BfHostPadpt, p: *mut U8);
    fn bf_host_beamforming_reset(a: BfHostPadpt);
    fn bf_host_sounding_handler(a: BfHostPadpt);
    fn bf_host_beamforming_sounding_down(a: BfHostPadpt, macid: U8);
    fn bf_host_hal_set_gid(a: BfHostPadpt, p: *mut U8);
    fn bf_host_hal_set_csi(a: BfHostPadpt, p: *mut U8);
}

#[cfg(all(host_bf_entry_packet_test, host_bf_init_cmd_test))]
#[no_mangle]
pub unsafe extern "C" fn o_bf_init(adapter: BfHostPadpt) {
    if adapter.is_null() {
        return;
    }
    let info = &mut (*adapter).hal.beamforming_info;
    info.beamforming_cap = 0;
    info.beamforming_state = 0;
    info.sounding_sequence = 0;
    info.beamformee_su_cnt = 0;
    info.beamformer_su_cnt = 0;
    info.beamformee_su_reg_maping = 0;
    info.beamformer_su_reg_maping = 0;
    info.beamformee_mu_cnt = 0;
    info.beamformer_mu_cnt = 0;
    info.beamformee_mu_reg_maping = 0;
    info.first_mu_bfee_index = BF_HOST_IDX_NONE;
    info.mu_bfer_curidx = BF_HOST_IDX_NONE;
    info.cur_csi_rpt_rate = 0;
    sounding_init(&mut info.sounding_info);
    info.timer_inits = 2;
    info.SetHalBFEnterOnDemandCnt = 0;
    info.SetHalBFLeaveOnDemandCnt = 0;
    info.SetHalSoundownOnDemandCnt = 0;
    info.bEnableSUTxBFWorkAround = 1;
    info.TargetSUBFee = std::ptr::null_mut();
    info.sounding_running = 0;
}

#[cfg(all(host_bf_entry_packet_test, host_bf_init_cmd_test))]
#[no_mangle]
pub unsafe extern "C" fn o_bf_cmd_hdl(adapter: BfHostPadpt, ty: U8, pbuf: *mut U8) {
    if adapter.is_null() {
        return;
    }
    match ty {
        0 => bf_host_beamforming_enter(adapter, pbuf),
        1 => {
            if pbuf.is_null() {
                bf_host_beamforming_reset(adapter);
            } else {
                bf_host_beamforming_leave(adapter, pbuf);
            }
        }
        2 => bf_host_sounding_handler(adapter),
        3 => {
            let macid = if pbuf.is_null() { 0 } else { *pbuf };
            bf_host_beamforming_sounding_down(adapter, macid);
        }
        6 => bf_host_hal_set_gid(adapter, pbuf),
        7 => bf_host_hal_set_csi(adapter, pbuf),
        _ => {}
    }
}

#[cfg(all(host_bf_entry_packet_test, host_bf_gid_test))]
#[repr(C)]
pub struct BfHostGidXmitTr {
    pub ok: U8,
    pub ra: [U8; 6],
    pub gid: [U8; 8],
    pub position: [U8; 16],
    pub frame: [U8; 64],
    pub pktlen: U16,
}

#[cfg(all(host_bf_entry_packet_test, host_bf_gid_test))]
extern "C" {
    static mut bf_host_gid_xmit_tr: BfHostGidXmitTr;
    fn bf_host_bfer_set_gid(a: BfHostPadpt, ta: *mut U8, gid: *mut U8, pos: *mut U8);
}

#[cfg(all(host_bf_entry_packet_test, host_bf_gid_test))]
#[no_mangle]
pub unsafe extern "C" fn o_bf_send_vht_gid_mgnt(
    adapter: BfHostPadpt,
    ra: *mut U8,
    gid: *mut U8,
    position: *mut U8,
) -> U8 {
    if adapter.is_null() || ra.is_null() || gid.is_null() || position.is_null() {
        return 0;
    }
    let tr_ptr = core::ptr::addr_of_mut!(bf_host_gid_xmit_tr);
    core::ptr::write_bytes(
        tr_ptr as *mut u8,
        0,
        core::mem::size_of::<BfHostGidXmitTr>(),
    );
    let tr = &mut *tr_ptr;
    let ra_s = std::slice::from_raw_parts(ra, 6);
    tr.ra[..6].copy_from_slice(ra_s);
    tr.gid[..8].copy_from_slice(std::slice::from_raw_parts(gid, 8));
    tr.position[..16].copy_from_slice(std::slice::from_raw_parts(position, 16));
    let mlmepriv = &(*adapter).mlmepriv;
    tr.frame[4..10].copy_from_slice(ra_s);
    tr.frame[10..16].copy_from_slice(&mlmepriv.mac_addr);
    tr.frame[16..22].copy_from_slice(&mlmepriv.bssid);
    tr.frame[24] = 21;
    tr.frame[25] = 1;
    tr.frame[26..34].copy_from_slice(&tr.gid);
    tr.frame[34..50].copy_from_slice(&tr.position);
    tr.pktlen = 54;
    tr.ok = 1;
    1
}

#[cfg(all(host_bf_entry_packet_test, host_bf_gid_test))]
#[no_mangle]
pub unsafe extern "C" fn o_bf_get_vht_gid_mgnt(adapter: BfHostPadpt, rf: *mut BfHostRecvFrame) {
    if adapter.is_null() || rf.is_null() {
        return;
    }
    let pframe = (*rf).hdr.data.as_mut_ptr();
    let mut ta = [0u8; 6];
    ta.copy_from_slice(std::slice::from_raw_parts(pframe.add(10), 6));
    ta[0] &= 0xFE;
    bf_host_bfer_set_gid(adapter, ta.as_mut_ptr(), pframe.add(26), pframe.add(34));
}
