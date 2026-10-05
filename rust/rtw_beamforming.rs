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
