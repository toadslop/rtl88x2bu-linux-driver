// SPDX-License-Identifier: GPL-2.0
//! Host L2 oracle for `update_attrib_vcs_info` (W3-86 PR4; kernel swap in PR5).

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
const HT_IOT_PEER_ATHEROS: U8 = 5;
const _AES_: U8 = 0x04;
const _TRUE: U8 = 1;

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
struct RegistryPriv {
    wifi_spec: U8,
    rts_thresh: U16,
    vrtl_carrier_sense: U8,
    vcs_type: U8,
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
