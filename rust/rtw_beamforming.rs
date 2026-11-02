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
pub struct BfHostSoundingInfo {
    pub su_sounding_list: [U8; 2],
    pub mu_sounding_list: [U8; 6],
    pub state: U8,
    pub su_bfee_curidx: U8,
    pub candidate_mu_bfee_cnt: U8,
    pub min_sounding_period: U16,
    pub sound_remain_cnt_per_period: U8,
}

#[cfg(host_bf_entry_packet_test)]
#[repr(C)]
pub struct BfHostInfo {
    pub bfee: [BfHostBfee; 8],
    pub bfer: [BfHostBfer; 3],
    pub bEnableSUTxBFWorkAround: U8,
    pub TargetCSIInfo: BfHostCsi,
    pub TargetSUBFee: *mut BfHostBfee,
    pub beamforming_cap: U8,
    pub beamforming_state: U8,
    pub sounding_sequence: U8,
    pub beamformee_su_cnt: U8,
    pub beamformer_su_cnt: U8,
    pub beamformee_su_reg_maping: U32,
    pub beamformer_su_reg_maping: U32,
    pub beamformee_mu_cnt: U8,
    pub beamformer_mu_cnt: U8,
    pub beamformee_mu_reg_maping: U32,
    pub first_mu_bfee_index: U8,
    pub mu_bfer_curidx: U8,
    pub cur_csi_rpt_rate: U8,
    pub sounding_info: BfHostSoundingInfo,
    pub SetHalBFEnterOnDemandCnt: U8,
    pub SetHalBFLeaveOnDemandCnt: U8,
    pub SetHalSoundownOnDemandCnt: U8,
    pub sounding_running: i8,
    pub timer_inits: U8,
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
    pub mac_addr: [U8; 6],
    pub bssid: [U8; 6],
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
const BF_HOST_MAX_BFER: usize = 3;

#[cfg(host_bf_entry_packet_test)]
unsafe fn cap_by_macid(a: BfHostPadpt, macid: U8) -> U32 {
    if a.is_null() {
        return 0;
    }
    let info = &(*a).hal.beamforming_info;
    for bfee in info.bfee.iter().take(BF_HOST_MAX_BFER) {
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
const BF_HOST_CAT_VHT: U8 = 21;
#[cfg(host_bf_entry_packet_test)]
const BF_HOST_CAT_HT: U8 = 7;
#[cfg(host_bf_entry_packet_test)]
const BF_HOST_ACT_VHT_BF: U8 = 0;
#[cfg(host_bf_entry_packet_test)]
const BF_HOST_ACT_HT_BF: U8 = 6;

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
    if cat == BF_HOST_CAT_VHT && act == BF_HOST_ACT_VHT_BF {
        let mimo = pframe.add(26);
        nc = *mimo & 0x7;
        nr = (*mimo & 0x38) >> 3;
        ch_w = (*mimo & 0xC0) >> 6;
        ng = *mimo.add(1) & 0x3;
        code_book = (*mimo.add(1) & 0x4) >> 2;
        info.TargetCSIInfo.bVHT = 1;
    } else if cat == BF_HOST_CAT_HT && act == BF_HOST_ACT_HT_BF {
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
const BF_HOST_OFDM24: U8 = 8;

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
    info.cur_csi_rpt_rate = BF_HOST_OFDM24;
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

// --- W3-197: legacy update_attrib_txbf_info (host L2 + kernel export) ---

#[cfg(host_bf_update_txbf_attrib_test)]
use std::mem::offset_of;

#[cfg(host_bf_update_txbf_attrib_test)]
#[repr(C)]
struct BfUpdateTxbfStaCmn {
    p_aid: U16,
    g_id: U8,
}

#[cfg(host_bf_update_txbf_attrib_test)]
#[repr(C)]
struct BfUpdateTxbfSta {
    used: U8,
    mac_addr: [U8; 6],
    _pad: U8,
    cmn: BfUpdateTxbfStaCmn,
}

#[cfg(host_bf_update_txbf_attrib_test)]
#[repr(C)]
struct BfUpdateTxbfStaPriv {
    stas: [BfUpdateTxbfSta; 4],
}

#[cfg(host_bf_update_txbf_attrib_test)]
#[repr(C)]
struct BfUpdateTxbfPktAttrib {
    txbf_p_aid: U16,
    txbf_g_id: U16,
}

#[cfg(host_bf_update_txbf_attrib_test)]
#[repr(C)]
struct BfUpdateTxbfAdpt {
    stapriv: BfUpdateTxbfStaPriv,
}

#[cfg(host_bf_update_txbf_attrib_test)]
type BfUpdateTxbfPadpt = *mut BfUpdateTxbfAdpt;

#[cfg(host_bf_update_txbf_attrib_test)]
#[no_mangle]
pub unsafe extern "C" fn update_attrib_txbf_info(
    padapter: BfUpdateTxbfPadpt,
    pattrib: *mut BfUpdateTxbfPktAttrib,
    psta: *mut BfUpdateTxbfSta,
) {
    let _ = padapter;
    if psta.is_null() {
        return;
    }
    if pattrib.is_null() {
        return;
    }
    let sta = &*psta;
    let att = &mut *pattrib;
    att.txbf_g_id = sta.cmn.g_id as U16;
    att.txbf_p_aid = sta.cmn.p_aid;
}

#[cfg(host_bf_update_txbf_attrib_test)]
const _: () = {
    assert!(core::mem::size_of::<BfUpdateTxbfPktAttrib>() == 4);
    assert!(offset_of!(BfUpdateTxbfSta, cmn) == 8);
};

#[cfg(all(
    config_beamforming,
    not(beamforming_version_2),
    not(host_bf_update_txbf_attrib_test)
))]
extern "C" {
    fn rtw_rust_update_attrib_txbf_info_impl(
        padapter: *mut core::ffi::c_void,
        pattrib: *mut core::ffi::c_void,
        psta: *mut core::ffi::c_void,
    );
}

#[cfg(all(
    config_beamforming,
    not(beamforming_version_2),
    not(host_bf_update_txbf_attrib_test)
))]
#[no_mangle]
pub extern "C" fn update_attrib_txbf_info(
    padapter: *mut core::ffi::c_void,
    pattrib: *mut core::ffi::c_void,
    psta: *mut core::ffi::c_void,
) {
    unsafe { rtw_rust_update_attrib_txbf_info_impl(padapter, pattrib, psta) };
}

// --- W3-201: txvector + STA beamform cap leaf (host L2 + kernel v2) ---

#[cfg(any(host_bf_txvector_cap_test, all(config_beamforming, beamforming_version_2)))]
mod txvector_cap {
    use super::{U8, U16};

    const WIFI_AP: u32 = 0x10;
    const WIFI_ADHOC: u32 = 0x20;
    const WIFI_ADHOC_MASTER: u32 = 0x40;
    const WIRELESS_11_24N: U8 = 1 << 3;
    const WIRELESS_11_5N: U8 = 1 << 4;
    const WIRELESS_11AC: U8 = 1 << 6;

    const HT_BFER: u16 = 1 << 0;
    const HT_BFEE: u16 = 1 << 1;
    const HT_STEER: u16 = 0x30;
    const HT_CHNL: u16 = 0xC0;
    const VHT_BFER: u16 = 1 << 0;
    const VHT_BFEE: u16 = 1 << 1;
    const VHT_MU_STA: u16 = 1 << 3;
    const VHT_MU_AP: u16 = 1 << 2;
    const VHT_STS: u16 = 0x700;
    const VHT_DIM: u16 = 0x7000;

    const BFER_HT: U8 = 0x1;
    const BFEE_HT: U8 = 0x2;
    const BFER_VHT_SU: U8 = 0x4;
    const BFEE_VHT_SU: U8 = 0x8;
    const BFER_VHT_MU: U8 = 0x10;
    const BFEE_VHT_MU: U8 = 0x20;

    #[inline]
    fn chk_fw(st: u32, mask: u32) -> bool {
        (st & mask) != 0
    }

    #[inline]
    fn sup_ht(mode: U8) -> bool {
        (mode & (WIRELESS_11_24N | WIRELESS_11_5N)) != 0
    }

    #[inline]
    fn sup_vht(mode: U8) -> bool {
        (mode & WIRELESS_11AC) != 0
    }

    pub(crate) fn get_txvector(
        fw_state: u32,
        ap_mac: &[U8; 6],
        sta_aid: U16,
        sta_mac: &[U8; 6],
    ) -> (U8, U16) {
        if chk_fw(fw_state, WIFI_AP) {
            let aid = sta_aid & 0x1FF;
            let val16 = ((ap_mac[5] & 0xF0) >> 4) ^ (ap_mac[5] & 0xF);
            let paid = (aid + ((val16 as U16) << 5)) & 0x1FF;
            (63, paid)
        } else if chk_fw(fw_state, WIFI_ADHOC) || chk_fw(fw_state, WIFI_ADHOC_MASTER) {
            (63, 0)
        } else {
            let paid = ((sta_mac[5] as U16) << 1) | ((sta_mac[4] >> 7) as U16);
            (0, paid)
        }
    }

    pub(crate) fn get_sta_cap(
        fw_state: u32,
        sta_wm: U8,
        mlme_ht: u16,
        mlme_vht: u16,
        sta_ht: u16,
        sta_vht: u16,
        info_cap: &mut U8,
    ) -> (U8, U8, U8) {
        let mut sta_bf_cap = 0u8;
        let mut sounding_dim = 0u8;
        let mut comp_steering = 0u8;

        if !sup_ht(sta_wm) {
            return (sta_bf_cap, sounding_dim, comp_steering);
        }

        if chk_fw(fw_state, WIFI_AP) {
            let ht_bf_cap = sta_ht;
            if (ht_bf_cap & HT_BFEE) != 0 {
                *info_cap |= BFER_HT;
                sta_bf_cap |= BFEE_HT;
                comp_steering = ((ht_bf_cap & HT_STEER) >> 4) as U8;
            }
            if (ht_bf_cap & HT_BFER) != 0 {
                *info_cap |= BFEE_HT;
                sta_bf_cap |= BFER_HT;
                sounding_dim = ((ht_bf_cap & HT_CHNL) >> 6) as U8;
            }
        } else {
            let ht_bf_cap = mlme_ht;
            if (ht_bf_cap & HT_BFEE) != 0 {
                *info_cap |= BFEE_HT;
                sta_bf_cap |= BFER_HT;
                sounding_dim = ((ht_bf_cap & HT_CHNL) >> 6) as U8;
            }
            if (ht_bf_cap & HT_BFER) != 0 {
                *info_cap |= BFER_HT;
                sta_bf_cap |= BFEE_HT;
                comp_steering = ((ht_bf_cap & HT_STEER) >> 4) as U8;
            }
        }

        if !sup_vht(sta_wm) {
            return (sta_bf_cap, sounding_dim, comp_steering);
        }

        if chk_fw(fw_state, WIFI_AP) {
            let vht_bf_cap = sta_vht;
            if (vht_bf_cap & VHT_BFEE) != 0 {
                *info_cap |= BFER_VHT_SU;
                sta_bf_cap |= BFEE_VHT_SU;
                if (vht_bf_cap & VHT_MU_STA) != 0 {
                    *info_cap |= BFER_VHT_MU;
                    sta_bf_cap |= BFEE_VHT_MU;
                }
                comp_steering = ((vht_bf_cap & VHT_STS) >> 8) as U8;
            }
            if (vht_bf_cap & VHT_BFER) != 0 {
                *info_cap |= BFEE_VHT_SU;
                sta_bf_cap |= BFER_VHT_SU;
                sounding_dim = ((vht_bf_cap & VHT_DIM) >> 12) as U8;
            }
        } else {
            let vht_bf_cap = mlme_vht;
            if (vht_bf_cap & VHT_BFEE) != 0 {
                *info_cap |= BFEE_VHT_SU;
                sta_bf_cap |= BFER_VHT_SU;
                if (vht_bf_cap & VHT_MU_STA) != 0 {
                    *info_cap |= BFEE_VHT_MU;
                    sta_bf_cap |= BFER_VHT_MU;
                }
                sounding_dim = ((vht_bf_cap & VHT_DIM) >> 12) as U8;
            }
            if (vht_bf_cap & VHT_BFER) != 0 {
                *info_cap |= BFER_VHT_SU;
                sta_bf_cap |= BFEE_VHT_SU;
                comp_steering = ((vht_bf_cap & VHT_STS) >> 8) as U8;
            }
        }

        (sta_bf_cap, sounding_dim, comp_steering)
    }
}

#[cfg(host_bf_txvector_cap_test)]
use txvector_cap::*;

#[cfg(host_bf_txvector_cap_test)]
#[repr(C)]
struct BfTxvecStaHt {
    beamform_cap: U16,
}

#[cfg(host_bf_txvector_cap_test)]
#[repr(C)]
struct BfTxvecStaVht {
    beamform_cap: U16,
}

#[cfg(host_bf_txvector_cap_test)]
#[repr(C)]
struct BfTxvecStaCmn {
    mac_addr: [U8; 6],
    aid: U16,
}

#[cfg(host_bf_txvector_cap_test)]
#[repr(C)]
struct BfTxvecSta {
    wireless_mode: U8,
    cmn: BfTxvecStaCmn,
    htpriv: BfTxvecStaHt,
    vhtpriv: BfTxvecStaVht,
}

#[cfg(host_bf_txvector_cap_test)]
#[repr(C)]
struct BfTxvecMlme {
    fw_state: u32,
    mac_addr: [U8; 6],
    ht_beamform_cap: U16,
    vht_beamform_cap: U16,
}

#[cfg(host_bf_txvector_cap_test)]
#[repr(C)]
struct BfTxvecAdpt {
    mlmepriv: BfTxvecMlme,
    beamforming_cap: U8,
    _pad: [U8; 3],
    sta: BfTxvecSta,
}

#[cfg(host_bf_txvector_cap_test)]
type BfTxvecPadpt = *mut BfTxvecAdpt;

#[cfg(host_bf_txvector_cap_test)]
#[no_mangle]
pub unsafe extern "C" fn o_rtw_bf_get_txvector_parameter(
    adapter: BfTxvecPadpt,
    sta: *mut BfTxvecSta,
    g_id: *mut U8,
    p_aid: *mut U16,
) {
    if adapter.is_null() || sta.is_null() || g_id.is_null() || p_aid.is_null() {
        return;
    }
    let mlme = &(*adapter).mlmepriv;
    let st = &*sta;
    let (gid, paid) = get_txvector(
        mlme.fw_state,
        &mlme.mac_addr,
        st.cmn.aid,
        &st.cmn.mac_addr,
    );
    *g_id = gid;
    *p_aid = paid;
}

#[cfg(host_bf_txvector_cap_test)]
#[no_mangle]
pub unsafe extern "C" fn o_rtw_bf_get_sta_beamform_cap(
    adapter: BfTxvecPadpt,
    sta: *mut BfTxvecSta,
    sta_bf_cap: *mut U8,
    sounding_dim: *mut U8,
    comp_steering: *mut U8,
) {
    if adapter.is_null() || sta.is_null() {
        return;
    }
    let mlme = &(*adapter).mlmepriv;
    let st = &*sta;
    let info = &mut (*adapter).beamforming_cap;
    let (cap, dim, steer) = get_sta_cap(
        mlme.fw_state,
        st.wireless_mode,
        mlme.ht_beamform_cap,
        mlme.vht_beamform_cap,
        st.htpriv.beamform_cap,
        st.vhtpriv.beamform_cap,
        info,
    );
    if !sta_bf_cap.is_null() {
        *sta_bf_cap = cap;
    }
    if !sounding_dim.is_null() {
        *sounding_dim = dim;
    }
    if !comp_steering.is_null() {
        *comp_steering = steer;
    }
}

#[cfg(all(
    config_beamforming,
    beamforming_version_2,
    not(host_bf_txvector_cap_test),
    not(host_bf_update_txbf_attrib_test),
    not(host_bf_entry_packet_test)
))]
extern "C" {
    fn rtw_bf_rust_txvec_fwstate(adapter: *mut core::ffi::c_void) -> u32;
    fn rtw_bf_rust_txvec_ap_mac(adapter: *mut core::ffi::c_void, out: *mut U8);
    fn rtw_bf_rust_txvec_mlme_ht(adapter: *mut core::ffi::c_void) -> U16;
    fn rtw_bf_rust_txvec_mlme_vht(adapter: *mut core::ffi::c_void) -> U16;
    fn rtw_bf_rust_txvec_sta_aid(sta: *mut core::ffi::c_void) -> U16;
    fn rtw_bf_rust_txvec_sta_mac(sta: *mut core::ffi::c_void, out: *mut U8);
    fn rtw_bf_rust_txvec_sta_wm(sta: *mut core::ffi::c_void) -> U8;
    fn rtw_bf_rust_txvec_sta_ht(sta: *mut core::ffi::c_void) -> U16;
    fn rtw_bf_rust_txvec_sta_vht(sta: *mut core::ffi::c_void) -> U16;
    fn rtw_bf_rust_txvec_info_cap(adapter: *mut core::ffi::c_void) -> *mut U8;
}

#[cfg(all(
    config_beamforming,
    beamforming_version_2,
    not(host_bf_txvector_cap_test),
    not(host_bf_update_txbf_attrib_test),
    not(host_bf_entry_packet_test)
))]
#[no_mangle]
pub unsafe extern "C" fn rtw_rust_get_txvector_parameter(
    adapter: *mut core::ffi::c_void,
    sta: *mut core::ffi::c_void,
    g_id: *mut U8,
    p_aid: *mut U16,
) {
    unsafe {
        if adapter.is_null() || sta.is_null() || g_id.is_null() || p_aid.is_null() {
            return;
        }
        let mut ap_mac = [0u8; 6];
        let mut sta_mac = [0u8; 6];
        rtw_bf_rust_txvec_ap_mac(adapter, ap_mac.as_mut_ptr());
        rtw_bf_rust_txvec_sta_mac(sta, sta_mac.as_mut_ptr());
        let (gid, paid) = txvector_cap::get_txvector(
            rtw_bf_rust_txvec_fwstate(adapter),
            &ap_mac,
            rtw_bf_rust_txvec_sta_aid(sta),
            &sta_mac,
        );
        *g_id = gid;
        *p_aid = paid;
    }
}

#[cfg(all(
    config_beamforming,
    beamforming_version_2,
    not(host_bf_txvector_cap_test),
    not(host_bf_update_txbf_attrib_test),
    not(host_bf_entry_packet_test)
))]
#[no_mangle]
pub unsafe extern "C" fn rtw_rust_get_sta_beamform_cap(
    adapter: *mut core::ffi::c_void,
    sta: *mut core::ffi::c_void,
    sta_bf_cap: *mut U8,
    sounding_dim: *mut U8,
    comp_steering: *mut U8,
) {
    unsafe {
        if adapter.is_null() || sta.is_null() {
            return;
        }
        let info_cap = rtw_bf_rust_txvec_info_cap(adapter);
        if info_cap.is_null() {
            return;
        }
        let (cap, dim, steer) = txvector_cap::get_sta_cap(
            rtw_bf_rust_txvec_fwstate(adapter),
            rtw_bf_rust_txvec_sta_wm(sta),
            rtw_bf_rust_txvec_mlme_ht(adapter),
            rtw_bf_rust_txvec_mlme_vht(adapter),
            rtw_bf_rust_txvec_sta_ht(sta),
            rtw_bf_rust_txvec_sta_vht(sta),
            &mut *info_cap,
        );
        if !sta_bf_cap.is_null() {
            *sta_bf_cap = cap;
        }
        if !sounding_dim.is_null() {
            *sounding_dim = dim;
        }
        if !comp_steering.is_null() {
            *comp_steering = steer;
        }
    }
}
