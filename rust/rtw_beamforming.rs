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
        if csi.Nc != nc || csi.Nr != nr || csi.ChnlWidth != ch_w || csi.Ng != ng || csi.CodeBook != code_book {
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
