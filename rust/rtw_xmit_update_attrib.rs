// SPDX-License-Identifier: GPL-2.0
//! Host L2 oracle for `update_attrib_vcs_info` / `update_attrib_phy_info` (W3-86).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

use std::os::raw::c_void;

type U8 = u8;
type U16 = u16;
type U32 = u32;

const NONE_VCS: U8 = 0;
const RTS_CTS: U8 = 1;
const CTS_TO_SELF: U8 = 2;
const DISABLE_VCS: U8 = 0;
const ENABLE_VCS: U8 = 1;
const AUTO_VCS: U8 = 2;
const WIRELESS_11_24N: U8 = 1 << 3;
const WIRELESS_11_5N: U8 = 1 << 4;
const HT_IOT_PEER_ATHEROS: U8 = 5;
const CHANNEL_WIDTH_20: U8 = 0;
const CHANNEL_WIDTH_40: U8 = 1;
const CHANNEL_WIDTH_80: U8 = 2;
const DRIVER_AMPDU_SPACING_DEFAULT: U8 = 0xFF;
const _AES_: U8 = 0x04;
const _TRUE: U8 = 1;
const _FALSE: U8 = 0;

#[inline]
fn is_supported_ht(net_type: U8) -> bool {
    (net_type & (WIRELESS_11_24N | WIRELESS_11_5N)) != 0
}

#[inline]
fn rtw_min_u8(a: U8, b: U8) -> U8 {
    if a > b {
        b
    } else {
        a
    }
}

fn validate_vcs(vrtl_carrier_sense: U8, vcs_type: U8, mode: U8) -> U8 {
    match vrtl_carrier_sense {
        DISABLE_VCS => NONE_VCS,
        ENABLE_VCS => vcs_type,
        AUTO_VCS => mode,
        _ => NONE_VCS,
    }
}

fn update_attrib_vcs_info_inner(
    cur_wireless_mode: U8,
    cur_bwmode: U8,
    assoc_ap_vendor: U8,
    ht_protection: U8,
    wifi_spec: U8,
    rts_thresh: U16,
    frag_len: U32,
    dot11_privacy: U8,
    vrtl_carrier_sense: U8,
    vcs_type: U8,
    driver_vcs_en: U8,
    driver_vcs_type: U8,
    is_hw_8812: bool,
    nr_frags: U8,
    last_txcmdsz: U32,
    rtsen: U8,
    cts2self: U8,
    ht_en: U8,
    ampdu_en: U8,
) -> U8 {
    let sz = if nr_frags != 1 {
        frag_len
    } else {
        last_txcmdsz
    };

    let mut vcs_mode = if cur_wireless_mode < WIRELESS_11_24N || wifi_spec != 0 {
        if sz > rts_thresh as U32 {
            RTS_CTS
        } else if rtsen != 0 {
            RTS_CTS
        } else if cts2self != 0 {
            CTS_TO_SELF
        } else {
            NONE_VCS
        }
    } else {
        let mode = 'ht: {
            if assoc_ap_vendor == HT_IOT_PEER_ATHEROS && ampdu_en == _TRUE && dot11_privacy == _AES_
            {
                break 'ht CTS_TO_SELF;
            }
            if rtsen != 0 || cts2self != 0 {
                break 'ht if rtsen != 0 { RTS_CTS } else { CTS_TO_SELF };
            }
            if ht_en != 0 {
                let ht_op = ht_protection;
                if (cur_bwmode != 0 && (ht_op == 2 || ht_op == 3))
                    || (cur_bwmode == 0 && ht_op == 3)
                {
                    break 'ht RTS_CTS;
                }
            }
            if sz > rts_thresh as U32 {
                break 'ht RTS_CTS;
            }
            if ampdu_en == _TRUE && !is_hw_8812 {
                break 'ht RTS_CTS;
            }
            NONE_VCS
        };
        mode
    };

    vcs_mode = validate_vcs(vrtl_carrier_sense, vcs_type, vcs_mode);
    if driver_vcs_en == 1 {
        vcs_mode = driver_vcs_type;
    }
    vcs_mode
}

#[repr(C)]
struct RaInfo {
    rate_id: U8,
}

#[repr(C)]
struct StaCmnInfo {
    ra_info: RaInfo,
    ldpc_en: U8,
    stbc_en: U8,
    bw_mode: U8,
}

#[repr(C)]
struct HtPriv {
    ht_option: U8,
    ch_offset: U8,
    ampdu_enable: U8,
    agg_enable_bitmap: U8,
    rx_ampdu_min_spacing: U8,
    tx_amsdu_enable: U8,
    sgi_20m: U8,
    sgi_40m: U8,
}

#[repr(C)]
struct VhtPriv {
    vht_option: U8,
    sgi_80m: U8,
}

#[repr(C)]
struct StaInfo {
    rtsen: U8,
    cts2self: U8,
    cmn: StaCmnInfo,
    htpriv: HtPriv,
    vhtpriv: VhtPriv,
}

#[repr(C)]
struct RegistryPriv {
    wifi_spec: U8,
    rts_thresh: U16,
    vrtl_carrier_sense: U8,
    vcs_type: U8,
    ht_enable: U8,
    wireless_mode: U8,
}

#[repr(C)]
struct SecurityPriv {
    dot11PrivacyAlgrthm: U8,
}

#[repr(C)]
struct MlmeExtInfo {
    assoc_ap_vendor: U8,
    ht_protection: U8,
}

#[repr(C)]
struct MlmeExtPriv {
    cur_wireless_mode: U8,
    cur_bwmode: U8,
    mlmext_info: MlmeExtInfo,
}

#[repr(C)]
struct XmitPriv {
    frag_len: U32,
}

#[repr(C)]
struct PktAttrib {
    nr_frags: U8,
    last_txcmdsz: U32,
    rtsen: U8,
    cts2self: U8,
    ht_en: U8,
    ampdu_en: U8,
    vcs_mode: U8,
    mdata: U8,
    eosp: U8,
    triggered: U8,
    ampdu_spacing: U8,
    raid: U8,
    bwmode: U8,
    sgi: U8,
    ldpc: U8,
    stbc: U8,
    ch_offset: U8,
    amsdu_ampdu_en: U8,
    priority: U8,
    retry_ctrl: U8,
}

#[repr(C)]
struct XmitFrame {
    attrib: PktAttrib,
}

#[repr(C)]
struct Adapter {
    mlmeextpriv: MlmeExtPriv,
    registrypriv: RegistryPriv,
    securitypriv: SecurityPriv,
    xmitpriv: XmitPriv,
    driver_vcs_en: U8,
    driver_vcs_type: U8,
    driver_ampdu_spacing: U8,
}

fn query_ra_short_gi(psta: &StaInfo, bw: U8) -> U8 {
    let sgi_20m = psta.htpriv.sgi_20m;
    let sgi_40m = psta.htpriv.sgi_40m;
    let sgi_80m = if psta.vhtpriv.vht_option != 0 {
        psta.vhtpriv.sgi_80m
    } else {
        _FALSE
    };
    match bw {
        CHANNEL_WIDTH_80 => sgi_80m,
        CHANNEL_WIDTH_40 => sgi_40m,
        _ => sgi_20m,
    }
}

fn rtw_get_tx_bw_mode(sta: &StaInfo) -> U8 {
    sta.cmn.bw_mode
}

fn update_attrib_phy_info_inner(adapter: &Adapter, pattrib: &mut PktAttrib, psta: &StaInfo) {
    let mlmeext = &adapter.mlmeextpriv;

    pattrib.rtsen = psta.rtsen;
    pattrib.cts2self = psta.cts2self;
    pattrib.mdata = 0;
    pattrib.eosp = 0;
    pattrib.triggered = 0;
    pattrib.ampdu_spacing = 0;

    pattrib.raid = psta.cmn.ra_info.rate_id;

    let bw = rtw_get_tx_bw_mode(psta);
    pattrib.bwmode = rtw_min_u8(bw, mlmeext.cur_bwmode);
    pattrib.sgi = query_ra_short_gi(psta, pattrib.bwmode);
    pattrib.ldpc = psta.cmn.ldpc_en;
    pattrib.stbc = psta.cmn.stbc_en;

    if adapter.registrypriv.ht_enable != 0
        && is_supported_ht(adapter.registrypriv.wireless_mode)
    {
        pattrib.ht_en = psta.htpriv.ht_option;
        pattrib.ch_offset = psta.htpriv.ch_offset;
        pattrib.ampdu_en = _FALSE;

        pattrib.ampdu_spacing = if adapter.driver_ampdu_spacing != DRIVER_AMPDU_SPACING_DEFAULT {
            adapter.driver_ampdu_spacing
        } else {
            psta.htpriv.rx_ampdu_min_spacing
        };

        if pattrib.ht_en != 0 && psta.htpriv.ampdu_enable != 0 {
            let bit = 1_u8 << pattrib.priority;
            if (psta.htpriv.agg_enable_bitmap & bit) != 0 {
                pattrib.ampdu_en = _TRUE;
                pattrib.amsdu_ampdu_en = if psta.htpriv.tx_amsdu_enable == _TRUE {
                    _TRUE
                } else {
                    _FALSE
                };
            }
        }
    }

    pattrib.retry_ctrl = _FALSE;
}

#[no_mangle]
pub extern "C" fn update_attrib_vcs_info(padapter: *mut c_void, pxmitframe: *mut c_void) {
    if padapter.is_null() || pxmitframe.is_null() {
        return;
    }
    unsafe {
        let a = padapter as *mut Adapter;
        let f = pxmitframe as *mut XmitFrame;
        let att = &mut (*f).attrib;
        att.vcs_mode = update_attrib_vcs_info_inner(
            (*a).mlmeextpriv.cur_wireless_mode,
            (*a).mlmeextpriv.cur_bwmode,
            (*a).mlmeextpriv.mlmext_info.assoc_ap_vendor,
            (*a).mlmeextpriv.mlmext_info.ht_protection,
            (*a).registrypriv.wifi_spec,
            (*a).registrypriv.rts_thresh,
            (*a).xmitpriv.frag_len,
            (*a).securitypriv.dot11PrivacyAlgrthm,
            (*a).registrypriv.vrtl_carrier_sense,
            (*a).registrypriv.vcs_type,
            (*a).driver_vcs_en,
            (*a).driver_vcs_type,
            false,
            att.nr_frags,
            att.last_txcmdsz,
            att.rtsen,
            att.cts2self,
            att.ht_en,
            att.ampdu_en,
        );
    }
}

#[no_mangle]
pub extern "C" fn update_attrib_phy_info(
    padapter: *mut c_void,
    pattrib: *mut c_void,
    psta: *mut c_void,
) {
    if padapter.is_null() || pattrib.is_null() || psta.is_null() {
        return;
    }
    unsafe {
        let a = &*(padapter as *const Adapter);
        let att = &mut *(pattrib as *mut PktAttrib);
        let sta = &*(psta as *const StaInfo);
        update_attrib_phy_info_inner(a, att, sta);
    }
}
