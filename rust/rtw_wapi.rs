// SPDX-License-Identifier: GPL-2.0
//! W3-109 WAPI PN/IE/CAM leaf helpers — Rust port of `core/rtw_wapi.c`.

#![allow(dead_code, non_snake_case, non_upper_case_globals)]

use core::ffi::c_void;

type U8 = u8;
type U16 = u16;
type U32 = u32;

const _TRUE: U8 = 1;
const _FALSE: U8 = 0;
const ETH_ALEN: usize = 6;
const WAPI_CAM_ENTRY_NUM: usize = 14;
const MAX_WAPI_IE_LEN: usize = 256;

#[repr(C)]
pub struct RtwWapiCamEntry {
    pub IsUsed: U8,
    pub entry_idx: U8,
    pub keyidx: U8,
    pub PeerMacAddr: [U8; ETH_ALEN],
    pub type_: U8,
}

#[repr(C)]
pub struct RtwWapiT {
    pub wapiIE: [U8; MAX_WAPI_IE_LEN],
    pub wapiIELength: U8,
    pub bWapiPSK: U8,
    pub wapiCamEntry: [RtwWapiCamEntry; WAPI_CAM_ENTRY_NUM],
}

#[repr(C)]
pub struct Adapter {
    pub wapiInfo: RtwWapiT,
}

fn memcmp6(a: &[U8; ETH_ALEN], b: &[U8; ETH_ALEN]) -> bool {
    a == b
}

#[no_mangle]
pub extern "C" fn host_wapi_adapter_init(a: *mut Adapter) {
    if a.is_null() {
        return;
    }
    unsafe {
        core::ptr::write_bytes(a, 0, 1);
        WapiResetAllCamEntry(a);
    }
}

#[no_mangle]
pub extern "C" fn WapiSetIE(padapter: *mut Adapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        let p = &mut (*padapter).wapiInfo;
        let oui = [0x00_u8, 0x14, 0x72];
        let protocol_ver: U16 = 1;
        let akm_cnt: U16 = 1;
        let suite_cnt: U16 = 1;
        let capability: U16 = 0;
        let mut off = 0usize;

        p.wapiIELength = 0;
        core::ptr::copy_nonoverlapping(
            &protocol_ver as *const U16 as *const U8,
            p.wapiIE.as_mut_ptr().add(off),
            2,
        );
        off += 2;
        core::ptr::copy_nonoverlapping(
            &akm_cnt as *const U16 as *const U8,
            p.wapiIE.as_mut_ptr().add(off),
            2,
        );
        off += 2;
        core::ptr::copy_nonoverlapping(oui.as_ptr(), p.wapiIE.as_mut_ptr().add(off), 3);
        off += 3;
        p.wapiIE[off] = if p.bWapiPSK != 0 { 0x2 } else { 0x1 };
        off += 1;
        core::ptr::copy_nonoverlapping(
            &suite_cnt as *const U16 as *const U8,
            p.wapiIE.as_mut_ptr().add(off),
            2,
        );
        off += 2;
        core::ptr::copy_nonoverlapping(oui.as_ptr(), p.wapiIE.as_mut_ptr().add(off), 3);
        off += 3;
        p.wapiIE[off] = 0x1;
        off += 1;
        core::ptr::copy_nonoverlapping(oui.as_ptr(), p.wapiIE.as_mut_ptr().add(off), 3);
        off += 3;
        p.wapiIE[off] = 0x1;
        off += 1;
        core::ptr::copy_nonoverlapping(
            &capability as *const U16 as *const U8,
            p.wapiIE.as_mut_ptr().add(off),
            2,
        );
        off += 2;
        p.wapiIELength = off as U8;
    }
}

#[no_mangle]
pub extern "C" fn WapiComparePN(PN1: *mut U8, PN2: *mut U8) -> U32 {
    if PN1.is_null() || PN2.is_null() {
        return 1;
    }
    unsafe {
        let a = core::slice::from_raw_parts(PN1, 16);
        let b = core::slice::from_raw_parts(PN2, 16);
        if (b[15].wrapping_sub(a[15])) & 0x80 != 0 {
            return 1;
        }
        for i in (0..16).rev() {
            if a[i] == b[i] {
                continue;
            }
            return if a[i] > b[i] { 1 } else { 0 };
        }
    }
    0
}

#[no_mangle]
pub extern "C" fn WapiGetEntryForCamWrite(
    padapter: *mut Adapter,
    pMacAddr: *mut U8,
    kid: U8,
    is_msk: U8,
) -> U8 {
    if padapter.is_null() || pMacAddr.is_null() {
        return 0xff;
    }
    unsafe {
        let info = &mut (*padapter).wapiInfo;
        let mac = core::slice::from_raw_parts(pMacAddr, ETH_ALEN);
        let mac_arr: [U8; ETH_ALEN] = mac.try_into().unwrap_or([0; ETH_ALEN]);
        for i in 0..WAPI_CAM_ENTRY_NUM {
            let e = &info.wapiCamEntry[i];
            if e.IsUsed != 0
                && memcmp6(&e.PeerMacAddr, &mac_arr)
                && e.keyidx == kid
                && e.type_ == is_msk
            {
                return e.entry_idx;
            }
        }
        for i in 0..WAPI_CAM_ENTRY_NUM {
            let e = &mut info.wapiCamEntry[i];
            if e.IsUsed == 0 {
                e.IsUsed = 1;
                e.type_ = is_msk;
                e.keyidx = kid;
                e.PeerMacAddr = mac_arr;
                return e.entry_idx;
            }
        }
    }
    0xff
}

#[no_mangle]
pub extern "C" fn WapiGetEntryForCamClear(
    padapter: *mut Adapter,
    pPeerMac: *mut U8,
    keyid: U8,
    is_msk: U8,
) -> U8 {
    if padapter.is_null() || pPeerMac.is_null() {
        return 0xff;
    }
    unsafe {
        let info = &mut (*padapter).wapiInfo;
        let mac = core::slice::from_raw_parts(pPeerMac, ETH_ALEN);
        let mac_arr: [U8; ETH_ALEN] = mac.try_into().unwrap_or([0; ETH_ALEN]);
        for i in 0..WAPI_CAM_ENTRY_NUM {
            let e = &mut info.wapiCamEntry[i];
            if e.IsUsed != 0
                && memcmp6(&e.PeerMacAddr, &mac_arr)
                && e.keyidx == keyid
                && e.type_ == is_msk
            {
                e.IsUsed = 0;
                e.keyidx = 2;
                e.PeerMacAddr = [0; ETH_ALEN];
                return e.entry_idx;
            }
        }
    }
    0xff
}

#[no_mangle]
pub extern "C" fn WapiResetAllCamEntry(padapter: *mut Adapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        let info = &mut (*padapter).wapiInfo;
        for i in 0..WAPI_CAM_ENTRY_NUM {
            info.wapiCamEntry[i].PeerMacAddr = [0; ETH_ALEN];
            info.wapiCamEntry[i].IsUsed = 0;
            info.wapiCamEntry[i].keyidx = 2;
            info.wapiCamEntry[i].entry_idx = (4 + i * 2) as U8;
        }
    }
}

#[no_mangle]
pub extern "C" fn _rtw_memcmp(s1: *const c_void, s2: *const c_void, n: usize) -> i32 {
    if s1.is_null() || s2.is_null() {
        return _FALSE as i32;
    }
    unsafe {
        let a = core::slice::from_raw_parts(s1 as *const U8, n);
        let b = core::slice::from_raw_parts(s2 as *const U8, n);
        if a == b {
            _TRUE as i32
        } else {
            _FALSE as i32
        }
    }
}
