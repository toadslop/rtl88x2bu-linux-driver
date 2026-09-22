// SPDX-License-Identifier: GPL-2.0
//! W3-110 FT IE update/build leaf — Rust port of `core/rtw_ft.c` (host L2).

#![allow(
    dead_code,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    clippy::too_many_arguments
)]

use std::ffi::c_void;
use std::ptr;

type U8 = u8;
type U16 = u16;
type U32 = u32;
type S32 = i32;

const _TRUE: U8 = 1;
const _FALSE: U8 = 0;
const _SUCCESS: U8 = 1;
const _FAIL: U8 = 0;
const ETH_ALEN: usize = 6;
const MAX_IE_SZ: usize = 768;
const RTW_FT_MAX_IE_SZ: usize = 256;
const RTW_FT_EN: U8 = 0x01;
const RTW_FT_OTD_EN: U8 = 0x02;
const RTW_FT_PEER_EN: U8 = 0x04;
const RTW_FT_PEER_OTD_EN: U8 = 0x08;
const RTW_FT_BTM_ROAM: U8 = 0x10;
const RTW_FT_TEST_RSSI_ROAM: U8 = 0x80;
const _MDIE_: U8 = 54;
const _FTIE_: U8 = 55;
const EID_WPA2: U8 = 48;

#[repr(C)]
pub struct FtEventHost {
    pub ies: *mut U8,
    pub ies_len: U16,
    pub ric_ies: *mut U8,
    pub ric_ies_len: U16,
}

#[repr(C)]
pub struct FtRoamInfo {
    pub mdid: U16,
    pub ft_cap: U8,
    pub ft_flags: U8,
    pub ft_updated_bcn: bool,
    pub updated_ft_ies: [U8; RTW_FT_MAX_IE_SZ],
    pub updated_ft_ies_len: U16,
    pub ft_event: FtEventHost,
}

#[repr(C)]
pub struct WlanBssidEx {
    pub MacAddress: [U8; ETH_ALEN],
    pub Rssi: S32,
    pub IELength: U32,
    pub IEs: [U8; MAX_IE_SZ],
}

#[repr(C)]
pub struct WlanNetwork {
    pub network: WlanBssidEx,
}

#[repr(C)]
pub struct MlmePriv {
    pub to_roam: S32,
    pub ft_roam: FtRoamInfo,
    pub cur_network: WlanNetwork,
    pub auth_rsp: *mut U8,
    pub auth_rsp_len: U32,
    pub assoc_bssid: [U8; ETH_ALEN],
}

#[repr(C)]
pub struct PktAttrib {
    pub pktlen: U32,
}

#[repr(C)]
pub struct Adapter {
    pub mlmepriv: MlmePriv,
}

extern "C" {
    fn _rtw_memcmp(s1: *const c_void, s2: *const c_void, n: usize) -> i32;
    fn rtw_get_ie(pbuf: *const U8, index: S32, len: *mut S32, limit: S32) -> *mut U8;
    fn rtw_set_ie(
        pbuf: *mut U8,
        index: S32,
        len: U32,
        source: *const U8,
        frlen: *mut U32,
    ) -> *mut U8;
    fn rtw_buf_update(pbuf: *mut *mut U8, size: *mut U32, src: *mut U8, len: U32);
    fn rtw_ft_report_reassoc_evt(padapter: *mut Adapter, pMacAddr: *mut U8);
}

fn ft_roam(a: *mut Adapter) -> bool {
    unsafe {
        !a.is_null()
            && (*a).mlmepriv.to_roam > 0
            && ((*a).mlmepriv.ft_roam.ft_flags & RTW_FT_PEER_EN) != 0
    }
}

fn chk_flags(a: *mut Adapter, f: U8) -> bool {
    unsafe { !a.is_null() && ((*a).mlmepriv.ft_roam.ft_flags & f) != 0 }
}

#[no_mangle]
pub extern "C" fn host_ft_info_init(pft: *mut FtRoamInfo) {
    if pft.is_null() {
        return;
    }
    unsafe {
        ptr::write_bytes(pft, 0, 1);
        (*pft).ft_flags = RTW_FT_EN;
        #[cfg(CONFIG_RTW_BTM_ROAM)]
        {
            (*pft).ft_flags |= RTW_FT_BTM_ROAM;
        }
        (*pft).ft_updated_bcn = false;
    }
}

#[no_mangle]
pub extern "C" fn host_ft_update_rsnie(
    padapter: *mut Adapter,
    bwrite: U8,
    pattrib: *mut PktAttrib,
    pframe: *mut *mut U8,
) -> U8 {
    if padapter.is_null() || pattrib.is_null() || pframe.is_null() {
        return _FAIL;
    }
    unsafe {
        let pft = &mut (*padapter).mlmepriv.ft_roam;
        let mut len: S32 = 0;
        let pie = rtw_get_ie(
            pft.updated_ft_ies.as_ptr(),
            EID_WPA2 as S32,
            &mut len,
            pft.updated_ft_ies_len as S32,
        );
        if bwrite == 0 {
            return if pie.is_null() { _FAIL } else { _SUCCESS };
        }
        if pie.is_null() {
            return _FAIL;
        }
        *pframe = rtw_set_ie(
            *pframe,
            EID_WPA2 as S32,
            len as U32,
            pie.add(2),
            &mut (*pattrib).pktlen,
        );
        _SUCCESS
    }
}

#[no_mangle]
pub extern "C" fn host_ft_update_mdie(
    padapter: *mut Adapter,
    pattrib: *mut PktAttrib,
    pframe: *mut *mut U8,
) -> U8 {
    if padapter.is_null() || pattrib.is_null() || pframe.is_null() {
        return _FAIL;
    }
    unsafe {
        let pft = &mut (*padapter).mlmepriv.ft_roam;
        let mut mdie = [0u8; 3];
        let (pie, len): (*const U8, U32) = if ft_roam(padapter) {
            let mut len: S32 = 3;
            let pie = rtw_get_ie(
                pft.updated_ft_ies.as_ptr(),
                _MDIE_ as S32,
                &mut len,
                pft.updated_ft_ies_len as S32,
            );
            if pie.is_null() {
                return _FAIL;
            }
            (pie.add(2), len as U32)
        } else {
            mdie[0] = (pft.mdid & 0xff) as U8;
            mdie[1] = (pft.mdid >> 8) as U8;
            mdie[2] = pft.ft_cap;
            (mdie.as_ptr(), 3)
        };
        *pframe = rtw_set_ie(*pframe, _MDIE_ as S32, len, pie, &mut (*pattrib).pktlen);
        _SUCCESS
    }
}

#[no_mangle]
pub extern "C" fn host_ft_update_ftie(
    padapter: *mut Adapter,
    pattrib: *mut PktAttrib,
    pframe: *mut *mut U8,
) -> U8 {
    if padapter.is_null() || pattrib.is_null() || pframe.is_null() {
        return _FAIL;
    }
    unsafe {
        let pft = &mut (*padapter).mlmepriv.ft_roam;
        let mut len: S32 = 0;
        let pie = rtw_get_ie(
            pft.updated_ft_ies.as_ptr(),
            _FTIE_ as S32,
            &mut len,
            pft.updated_ft_ies_len as S32,
        );
        if pie.is_null() {
            return _FAIL;
        }
        *pframe = rtw_set_ie(*pframe, _FTIE_ as S32, len as U32, pie.add(2), &mut (*pattrib).pktlen);
        _SUCCESS
    }
}

#[no_mangle]
pub extern "C" fn host_ft_build_auth_req_ies(
    padapter: *mut Adapter,
    pattrib: *mut PktAttrib,
    pframe: *mut *mut U8,
) {
    if padapter.is_null() || pattrib.is_null() || pframe.is_null() {
        return;
    }
    unsafe {
        if (*pframe).is_null() {
            return;
        }
    }
    if !ft_roam(padapter) {
        return;
    }
    let ftie_append = host_ft_update_rsnie(padapter, _TRUE, pattrib, pframe);
    host_ft_update_mdie(padapter, pattrib, pframe);
    if ftie_append == _SUCCESS {
        host_ft_update_ftie(padapter, pattrib, pframe);
    }
}

#[no_mangle]
pub extern "C" fn host_ft_build_assoc_req_ies(
    padapter: *mut Adapter,
    is_reassoc: U8,
    pattrib: *mut PktAttrib,
    pframe: *mut *mut U8,
) {
    if padapter.is_null() || pattrib.is_null() || pframe.is_null() {
        return;
    }
    if chk_flags(padapter, RTW_FT_PEER_EN) {
        host_ft_update_mdie(padapter, pattrib, pframe);
    }
    if is_reassoc == 0 || !ft_roam(padapter) {
        return;
    }
    if host_ft_update_rsnie(padapter, _FALSE, pattrib, pframe) == _SUCCESS {
        host_ft_update_ftie(padapter, pattrib, pframe);
    }
}

#[no_mangle]
pub extern "C" fn host_ft_chk_roaming_candidate(
    padapter: *mut Adapter,
    competitor: *mut WlanNetwork,
) -> U8 {
    if padapter.is_null() || competitor.is_null() {
        return _FALSE;
    }
    unsafe {
        let pft = &mut (*padapter).mlmepriv.ft_roam;
        let net = &mut (*competitor).network;
        let mut mdie_len: S32 = 0;
        let pmdie = rtw_get_ie(
            net.IEs.as_ptr().add(12),
            _MDIE_ as S32,
            &mut mdie_len,
            (net.IELength.saturating_sub(12)) as S32,
        );
        if pmdie.is_null() {
            return _FALSE;
        }
        if _rtw_memcmp(
            &pft.mdid as *const U16 as *const c_void,
            pmdie.add(2) as *const c_void,
            2,
        ) == 0
        {
            return _FALSE;
        }
        let otd = chk_flags(padapter, RTW_FT_OTD_EN)
            && ((chk_flags(padapter, RTW_FT_PEER_OTD_EN) && (*pmdie.add(4) & 0x01) == 0)
                || (!chk_flags(padapter, RTW_FT_PEER_OTD_EN) && (*pmdie.add(4) & 0x01) != 0));
        if otd {
            return _FALSE;
        }
        if chk_flags(padapter, RTW_FT_TEST_RSSI_ROAM) {
            let cur = &(*padapter).mlmepriv.cur_network.network.MacAddress;
            if _rtw_memcmp(
                cur.as_ptr() as *const c_void,
                net.MacAddress.as_ptr() as *const c_void,
                ETH_ALEN,
            ) == 0
            {
                net.Rssi += 20;
                (*padapter).mlmepriv.ft_roam.ft_flags &= !RTW_FT_TEST_RSSI_ROAM;
            }
        }
        _TRUE
    }
}

#[no_mangle]
pub extern "C" fn host_ft_update_auth_rsp_ies(padapter: *mut Adapter, pframe: *mut U8, len: U32) -> U8 {
    if padapter.is_null() || !ft_roam(padapter) || pframe.is_null() || len == 0 {
        return _FAIL;
    }
    unsafe {
        let pmlmepriv = &mut (*padapter).mlmepriv;
        let pft = &mut pmlmepriv.ft_roam;
        rtw_buf_update(
            &mut pmlmepriv.auth_rsp,
            &mut pmlmepriv.auth_rsp_len,
            pframe,
            len,
        );
        pft.ft_event.ies = pmlmepriv.auth_rsp.add(30);
        pft.ft_event.ies_len = (pmlmepriv.auth_rsp_len.saturating_sub(30)) as U16;
        pft.ft_event.ric_ies = ptr::null_mut();
        pft.ft_event.ric_ies_len = 0;
        rtw_ft_report_reassoc_evt(padapter, pmlmepriv.assoc_bssid.as_mut_ptr());
        _SUCCESS
    }
}
