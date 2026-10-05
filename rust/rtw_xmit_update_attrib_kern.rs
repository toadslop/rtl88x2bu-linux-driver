// SPDX-License-Identifier: GPL-2.0
//! Kernel port of `update_attrib_vcs_info` (W3-86 PR11),
//! `update_attrib_phy_info` (W3-86 PR12), and sec decision leaf (W3-86 PR16).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

use core::ffi::c_void;

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
const CHANNEL_WIDTH_20: U8 = 0;
const CHANNEL_WIDTH_40: U8 = 1;
const CHANNEL_WIDTH_80: U8 = 2;
const HT_IOT_PEER_ATHEROS: U8 = 5;
const _AES_: U8 = 0x04;
const _TRUE: U8 = 1;
const _FALSE: U8 = 0;
const _SUCCESS: i32 = 1;
const _FAIL: i32 = 0;
const _NO_PRIVACY_: U8 = 0x00;
const _WEP40_: U8 = 0x01;
const _WEP104_: U8 = 0x02;
const EAPOL_2_4: i32 = 11;
const EAPOL_4_4: i32 = 13;
const EAPOL_ETHERTYPE: U16 = 0x888e;
const WAPI_ETHERTYPE: U16 = 0x88b4;
const DOT11_AUTH_OPEN: U8 = 0;
const DOT11_AUTH_SHARED: U8 = 1;
const DOT11_AUTH_8021X: U8 = 2;
const DOT11_AUTH_AUTO: U8 = 3;
const DOT11_AUTH_WAPI: U8 = 4;

#[inline]
fn rtw_min_u8(a: U8, b: U8) -> U8 {
    if a > b {
        b
    } else {
        a
    }
}

#[repr(C)]
struct VcsIn {
    cur_wireless_mode: U8,
    cur_bwmode: U8,
    assoc_ap_vendor: U8,
    ht_protection: U8,
    wifi_spec: U8,
    rts_thresh: U16,
    dot11_privacy: U8,
    vrtl_carrier_sense: U8,
    vcs_type: U8,
    driver_vcs_en: U8,
    driver_vcs_type: U8,
    is_hw_8812: U8,
    frag_len: U32,
    nr_frags: U8,
    last_txcmdsz: U32,
    rtsen: U8,
    cts2self: U8,
    ht_en: U8,
    ampdu_en: U8,
    psta_rssi: i8,
}

fn validate_vcs(vrtl_carrier_sense: U8, vcs_type: U8, mode: U8) -> U8 {
    match vrtl_carrier_sense {
        DISABLE_VCS => NONE_VCS,
        ENABLE_VCS => vcs_type,
        AUTO_VCS => mode,
        _ => NONE_VCS,
    }
}

fn update_attrib_vcs_info_inner(vcs_in: &VcsIn) -> U8 {
    let sz = if vcs_in.nr_frags != 1 {
        vcs_in.frag_len
    } else {
        vcs_in.last_txcmdsz
    };

    let mut used_ht_branch = false;
    let mut vcs_mode = if vcs_in.cur_wireless_mode < WIRELESS_11_24N || vcs_in.wifi_spec != 0 {
        if sz > vcs_in.rts_thresh as U32 {
            RTS_CTS
        } else if vcs_in.rtsen != 0 {
            RTS_CTS
        } else if vcs_in.cts2self != 0 {
            CTS_TO_SELF
        } else {
            NONE_VCS
        }
    } else {
        used_ht_branch = true;
        let is_hw_8812 = vcs_in.is_hw_8812 != 0;
        let mode = 'ht: {
            if vcs_in.assoc_ap_vendor == HT_IOT_PEER_ATHEROS
                && vcs_in.ampdu_en == _TRUE
                && vcs_in.dot11_privacy == _AES_
            {
                break 'ht CTS_TO_SELF;
            }
            if vcs_in.rtsen != 0 || vcs_in.cts2self != 0 {
                break 'ht if vcs_in.rtsen != 0 {
                    RTS_CTS
                } else {
                    CTS_TO_SELF
                };
            }
            if vcs_in.ht_en != 0 {
                let ht_op = vcs_in.ht_protection;
                if (vcs_in.cur_bwmode != 0 && (ht_op == 2 || ht_op == 3))
                    || (vcs_in.cur_bwmode == 0 && ht_op == 3)
                {
                    break 'ht RTS_CTS;
                }
            }
            if sz > vcs_in.rts_thresh as U32 {
                break 'ht RTS_CTS;
            }
            if vcs_in.ampdu_en == _TRUE && !is_hw_8812 {
                break 'ht RTS_CTS;
            }
            NONE_VCS
        };
        mode
    };

    if used_ht_branch {
        let rssi = vcs_in.psta_rssi;
        if rssi > -128 && rssi < 18 && vcs_mode == RTS_CTS {
            vcs_mode = CTS_TO_SELF;
        }
    }

    vcs_mode = validate_vcs(vcs_in.vrtl_carrier_sense, vcs_in.vcs_type, vcs_mode);
    if vcs_in.driver_vcs_en == 1 {
        vcs_mode = vcs_in.driver_vcs_type;
    }
    vcs_mode
}

extern "C" {
    fn rtw_rust_xmit_attrib_vcs_gather(
        padapter: *mut c_void,
        pxmitframe: *mut c_void,
        out: *mut VcsIn,
    );
    fn rtw_rust_xmit_attrib_vcs_set_mode(pxmitframe: *mut c_void, mode: U8);
}

#[no_mangle]
pub extern "C" fn update_attrib_vcs_info(padapter: *mut c_void, pxmitframe: *mut c_void) {
    if padapter.is_null() || pxmitframe.is_null() {
        return;
    }
    let mut vcs_in: VcsIn = unsafe { core::mem::zeroed() };
    unsafe { rtw_rust_xmit_attrib_vcs_gather(padapter, pxmitframe, &mut vcs_in) };
    let vcs = update_attrib_vcs_info_inner(&vcs_in);
    unsafe { rtw_rust_xmit_attrib_vcs_set_mode(pxmitframe, vcs) };
}

#[repr(C)]
pub struct PhyBaseIn {
    cur_bwmode: U8,
    sta_tx_bw: U8,
    rtsen: U8,
    cts2self: U8,
    raid: U8,
    ldpc: U8,
    stbc: U8,
    sgi_20m: U8,
    sgi_40m: U8,
    sgi_80m: U8,
    vht_option: U8,
}

#[repr(C)]
pub struct PhyBaseOut {
    rtsen: U8,
    cts2self: U8,
    raid: U8,
    bwmode: U8,
    sgi: U8,
    ldpc: U8,
    stbc: U8,
}

fn query_ra_short_gi_base(base: &PhyBaseIn, bw: U8) -> U8 {
    let sgi_80m = if base.vht_option != 0 {
        base.sgi_80m
    } else {
        _FALSE
    };
    match bw {
        CHANNEL_WIDTH_80 => sgi_80m,
        CHANNEL_WIDTH_40 => base.sgi_40m,
        _ => base.sgi_20m,
    }
}

#[no_mangle]
pub extern "C" fn update_attrib_phy_info_base(phy_in: *const PhyBaseIn, phy_out: *mut PhyBaseOut) {
    if phy_in.is_null() || phy_out.is_null() {
        return;
    }
    let base = unsafe { &*phy_in };
    let bwmode = rtw_min_u8(base.sta_tx_bw, base.cur_bwmode);
    unsafe {
        *phy_out = PhyBaseOut {
            rtsen: base.rtsen,
            cts2self: base.cts2self,
            raid: base.raid,
            bwmode,
            sgi: query_ra_short_gi_base(base, bwmode),
            ldpc: base.ldpc,
            stbc: base.stbc,
        };
    }
}

#[repr(C)]
pub struct SecGather {
    ieee8021x_blocked: U8,
    passing_ms: U32,
    eapol_type: i32,
    ether_type: U16,
    wifi_mp_state: U8,
    bmcast: U8,
    dot11_auth_algrthm: U8,
    dot11_privacy_algrthm: U8,
    dot118021x_grp_privacy: U8,
    sta_dot118021x_privacy: U8,
    dot11_privacy_key_index: U8,
    dot118021x_grp_keyid: U8,
    direct_link: U8,
}

#[repr(C)]
pub struct SecDecision {
    res: i32,
    encrypt: U8,
    key_idx: U8,
}

fn get_encry_algo(g: &SecGather) -> U8 {
    match g.dot11_auth_algrthm {
        DOT11_AUTH_OPEN | DOT11_AUTH_SHARED | DOT11_AUTH_AUTO | DOT11_AUTH_WAPI => {
            g.dot11_privacy_algrthm
        }
        DOT11_AUTH_8021X => {
            if g.bmcast != 0 {
                g.dot118021x_grp_privacy
            } else {
                g.sta_dot118021x_privacy
            }
        }
        _ => 0,
    }
}

fn update_attrib_sec_info_decide_inner(g: &SecGather) -> SecDecision {
    let (res, mut encrypt, key_idx) = if g.ieee8021x_blocked != 0
        || ((g.eapol_type == EAPOL_2_4 || g.eapol_type == EAPOL_4_4) && g.passing_ms <= 100)
    {
        let res = if g.ether_type != EAPOL_ETHERTYPE && g.wifi_mp_state == 0 {
            _FAIL
        } else {
            _SUCCESS
        };
        (res, 0u8, 0u8)
    } else {
        let mut encrypt = get_encry_algo(g);
        #[cfg(CONFIG_WAPI_SUPPORT)]
        if g.ether_type == WAPI_ETHERTYPE {
            encrypt = _NO_PRIVACY_;
        }
        let key_idx = match g.dot11_auth_algrthm {
            DOT11_AUTH_OPEN | DOT11_AUTH_SHARED | DOT11_AUTH_AUTO => g.dot11_privacy_key_index,
            DOT11_AUTH_8021X => {
                if g.bmcast != 0 {
                    g.dot118021x_grp_keyid
                } else {
                    0
                }
            }
            _ => 0,
        };
        if (encrypt == _WEP40_ || encrypt == _WEP104_) && g.ether_type == EAPOL_ETHERTYPE {
            encrypt = _NO_PRIVACY_;
        }
        (_SUCCESS, encrypt, key_idx)
    };

    if g.direct_link != 0 && encrypt > 0 {
        encrypt = _AES_;
    }

    SecDecision {
        res,
        encrypt,
        key_idx,
    }
}

#[no_mangle]
pub extern "C" fn update_attrib_sec_info_decide_rust(
    gather: *const SecGather,
    out: *mut SecDecision,
) {
    if gather.is_null() || out.is_null() {
        return;
    }
    let g = unsafe { &*gather };
    let decision = update_attrib_sec_info_decide_inner(g);
    unsafe {
        *out = decision;
    }
}
