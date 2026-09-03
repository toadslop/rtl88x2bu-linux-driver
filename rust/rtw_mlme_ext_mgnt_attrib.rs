// SPDX-License-Identifier: GPL-2.0
//! W3-68 mgnt frame attrib builders — host L2 oracle and kernel port.

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    private_interfaces,
    unused_imports,
    missing_docs
)]

#[cfg(host_mlme_ext_mgnt_attrib_test)]
use std::os::raw::{c_int, c_void};

#[cfg(all(not(host_mlme_ext_mgnt_attrib_test), rust_mlme_ext_mgnt_attrib))]
use core::ffi::{c_int, c_void};

type U8 = u8;
type U16 = u16;
type U32 = u32;

const _TRUE: c_int = 1;
const _FALSE: c_int = 0;

const RTW_DEFAULT_MGMT_MACID: U8 = 1;
const QSLT_MGNT: U8 = 0x12;
const QSLT_VO: U8 = 0x7;
const _NO_PRIVACY_: U8 = 0;

const P2P_ROLE_CLIENT: U32 = 2;
const P2P_PS_NONE: U32 = 0;

const WIRELESS_11B: U8 = 1 << 0;
const WIRELESS_11G: U8 = 1 << 1;

const IEEE80211_CCK_RATE_1MB: U8 = 0x02;
const MGN_MCS7: U8 = 0x87;
const MGN_VHT1SS_MCS9: U8 = 0x9f;

const RATEID_IDX_VHT_1SS: U8 = 10;
const RATEID_IDX_VHT_2SS: U8 = 9;
const RATEID_IDX_VHT_3SS: U8 = 13;
const RATEID_IDX_BGN_40M_1SS: U8 = 1;

const RF_1T1R: U8 = 0;
const RF_2T2R: U8 = 1;
const RF_2T4R: U8 = 2;
const RF_3T3R: U8 = 3;

const CHANNEL_WIDTH_20: U8 = 0;
const HAL_PRIME_CHNL_OFFSET_DONT_CARE: U8 = 0;

const WIFI_ACTION: U8 = 0xd0;
const WIFI_DISASSOC: U8 = 0xa0;
const WIFI_DEAUTH: U8 = 0xc0;
const WIFI_PROBERSP: U8 = 0x50;

const ACT_PUBLIC_FTM_REQ: U8 = 14;
const ACT_PUBLIC_FTM: U8 = 15;

const WIFI_ADHOC_STATE: U32 = 0x0000_0004;
const TXDESC_OFFSET: usize = 48;

#[repr(C)]
pub struct StaInfo {
    pub cmn_mac_id: U8,
    #[cfg(host_mlme_ext_mgnt_attrib_test)]
    pub bf_g_id: U8,
    #[cfg(host_mlme_ext_mgnt_attrib_test)]
    pub bf_p_aid: U16,
}

#[cfg(host_mlme_ext_mgnt_attrib_test)]
mod layout {
    use super::*;

    #[repr(C)]
    pub struct StaPriv {
        pub fixture_sta: *mut StaInfo,
    }

    #[repr(C)]
    pub struct WlanBssidEx {
        pub mac_address: [U8; 6],
    }

    #[repr(C)]
    pub struct MlmeExtInfo {
        pub network: WlanBssidEx,
    }

    #[repr(C)]
    pub struct MlmeExtPriv {
        pub mgnt_seq: U16,
        pub tx_rate: U8,
        pub mlmext_info: MlmeExtInfo,
    }

    #[repr(C)]
    pub struct XmitPriv {
        pub hw_ssn_seq_no: U8,
    }

    #[repr(C)]
    pub struct HalDataType {
        pub rf_type: U8,
    }

    #[repr(C)]
    pub struct HostFixture {
        pub mlme_state: U32,
        pub buddy_asoc: U8,
    }

    #[repr(C)]
    pub struct WifiDirectInfo {
        pub role: U32,
        pub p2p_ps_mode: U32,
        pub p2p_ps_state: U32,
    }

    #[repr(C)]
    pub struct Adapter {
        pub mlmeextpriv: MlmeExtPriv,
        pub xmitpriv: XmitPriv,
        pub stapriv: StaPriv,
        pub hal_data: HalDataType,
        pub host_fixture: HostFixture,
        pub wdinfo: WifiDirectInfo,
    }

    #[repr(C)]
    pub struct PktAttrib {
        pub type_: U8,
        pub subtype: U8,
        pub bswenc: U8,
        pub dhcp_pkt: U8,
        pub ether_type: U16,
        pub seqnum: U16,
        pub hw_ssn_sel: U8,
        pub pkt_hdrlen: U16,
        pub hdrlen: U16,
        pub pktlen: U32,
        pub last_txcmdsz: U32,
        pub nr_frags: U8,
        pub encrypt: U8,
        pub bmc_camid: U8,
        pub iv_len: U8,
        pub icv_len: U8,
        pub priority: U8,
        pub ack_policy: U8,
        pub mac_id: U8,
        pub vcs_mode: U8,
        pub dst: [U8; 6],
        pub src: [U8; 6],
        pub ta: [U8; 6],
        pub ra: [U8; 6],
        pub key_idx: U8,
        pub qos_en: U8,
        pub ht_en: U8,
        pub raid: U8,
        pub bwmode: U8,
        pub ch_offset: U8,
        pub sgi: U8,
        pub mdata: U8,
        pub order: U8,
        pub rate: U8,
        pub retry_ctrl: U8,
        pub mbssid: U8,
        pub qsel: U8,
        pub psta: *mut StaInfo,
        pub txbf_p_aid: U16,
        pub txbf_g_id: U16,
        pub ps_dontq: U8,
    }

    #[repr(C)]
    pub struct XmitFrame {
        pub attrib: PktAttrib,
        pub buf_addr: *mut U8,
    }
}

#[cfg(rust_mlme_ext_mgnt_attrib)]
mod layout {
    use super::*;

    pub(crate) type Adapter = c_void;
    pub(crate) type StaPriv = c_void;

    #[repr(C)]
    pub struct PktAttrib {
        pub type_: U8,
        pub subtype: U8,
        pub bswenc: U8,
        pub dhcp_pkt: U8,
        pub ether_type: U16,
        pub seqnum: U16,
        pub hw_ssn_sel: U8,
        pub pkt_hdrlen: U16,
        pub hdrlen: U16,
        pub pktlen: U32,
        pub last_txcmdsz: U32,
        pub nr_frags: U8,
        pub encrypt: U8,
        pub bmc_camid: U8,
        pub iv_len: U8,
        pub icv_len: U8,
        pub iv: [U8; 18],
        pub icv: [U8; 16],
        pub priority: U8,
        pub ack_policy: U8,
        pub mac_id: U8,
        pub vcs_mode: U8,
        pub dst: [U8; 6],
        pub src: [U8; 6],
        pub ta: [U8; 6],
        pub ra: [U8; 6],
        pub key_idx: U8,
        pub qos_en: U8,
        pub ht_en: U8,
        pub raid: U8,
        pub bwmode: U8,
        pub ch_offset: U8,
        pub sgi: U8,
        pub ampdu_en: U8,
        pub ampdu_spacing: U8,
        pub amsdu: U8,
        pub amsdu_ampdu_en: U8,
        pub mdata: U8,
        pub pctrl: U8,
        pub triggered: U8,
        pub qsel: U8,
        pub order: U8,
        pub eosp: U8,
        pub rate: U8,
        pub intel_proxim: U8,
        pub retry_ctrl: U8,
        pub mbssid: U8,
        pub ldpc: U8,
        pub stbc: U8,
        pub trigger_frame: U8,
        pub psta: *mut StaInfo,
        pub rtsen: U8,
        pub cts2self: U8,
        pub dot11tkiptxmickey: [U8; 32],
        pub dot118021x_UncstKey: [U8; 32],
        pub key_type: U8,
        pub icmp_pkt: U8,
        pub hipriority_pkt: U8,
        pub txbf_p_aid: U16,
        pub txbf_g_id: U16,
        pub bf_pkt_type: U8,
        pub ps_dontq: U8,
    }

    #[repr(C)]
    pub struct XmitFrame {
        pub list: [U8; 16],
        pub attrib: PktAttrib,
        pub os_qid: U16,
        pub pkt: *mut c_void,
        pub frame_tag: c_int,
        pub padapter: *mut Adapter,
        pub buf_addr: *mut U8,
    }
}

use layout::{Adapter, PktAttrib, StaPriv, XmitFrame};

extern "C" {
    fn rtw_get_mgntframe_raid(adapter: *mut Adapter, network_type: U8) -> U8;
    fn rtw_get_stainfo(pstapriv: *mut StaPriv, hwaddr: *const U8) -> *mut StaInfo;
    fn rtw_action_frame_parse(
        frame: *const U8,
        frame_len: U32,
        category: *mut U8,
        action: *mut U8,
    ) -> c_int;
}

#[cfg(host_mlme_ext_mgnt_attrib_test)]
extern "C" {
    fn update_attrib_txbf_info(padapter: *mut Adapter, pattrib: *mut PktAttrib, psta: *mut StaInfo);
}

#[cfg(all(config_beamforming, not(host_mlme_ext_mgnt_attrib_test)))]
extern "C" {
    fn rtw_bf_update_attrib(padapter: *mut Adapter, pattrib: *mut PktAttrib, psta: *mut StaInfo);
}

#[cfg(rust_mlme_ext_mgnt_attrib)]
extern "C" {
    fn rtw_rust_mgnt_tx_rate(padapter: *mut Adapter) -> U8;
    fn rtw_rust_mgnt_mgnt_seq(padapter: *mut Adapter) -> U16;
    fn rtw_rust_mgnt_hw_ssn_seq_no(padapter: *mut Adapter) -> U8;
    fn rtw_rust_mgnt_hal_rf_type(padapter: *mut Adapter) -> U8;
    fn rtw_rust_mgnt_mlme_is_adhoc(padapter: *mut Adapter) -> U8;
    fn rtw_rust_mgnt_stapriv(padapter: *mut Adapter) -> *mut StaPriv;
    #[cfg(config_p2p_ps_noa_use_macid_sleep)]
    fn rtw_rust_mgnt_p2p_noa_override(padapter: *mut Adapter, mac_id: *mut U8, qsel: *mut U8)
        -> U8;
}

#[inline]
fn is_cck_rate(rate: U8) -> bool {
    rate == 0x02 || rate == 0x04 || rate == 0x0b || rate == 0x16
}

#[inline]
fn mlme_is_adhoc(padapter: *mut Adapter) -> bool {
    #[cfg(host_mlme_ext_mgnt_attrib_test)]
    {
        let adapter = unsafe { &*(padapter as *const layout::Adapter) };
        (adapter.host_fixture.mlme_state & WIFI_ADHOC_STATE) != 0
    }
    #[cfg(rust_mlme_ext_mgnt_attrib)]
    {
        unsafe { rtw_rust_mgnt_mlme_is_adhoc(padapter) != 0 }
    }
}

#[inline]
fn get_frame_sub_type(pbuf: *const U8) -> U8 {
    let fc = unsafe { core::ptr::read_unaligned(pbuf as *const U16) };
    (fc.to_le() & 0x00fc) as U8
}

#[inline]
fn get_addr1_ptr(pbuf: *const U8) -> *const U8 {
    unsafe { pbuf.add(4) }
}

#[inline]
fn get_addr2_ptr(pbuf: *const U8) -> *const U8 {
    unsafe { pbuf.add(10) }
}

#[cfg(any(config_p2p_ps_noa_use_macid_sleep, host_mlme_ext_mgnt_attrib_test))]
fn apply_p2p_noa_mac_id_qsel(padapter: *mut Adapter, attrib: &mut PktAttrib) {
    #[cfg(host_mlme_ext_mgnt_attrib_test)]
    {
        let adapter = unsafe { &*(padapter as *const layout::Adapter) };
        #[cfg(config_concurrent_mode)]
        if adapter.host_fixture.buddy_asoc == 0 {
            return;
        }
        if adapter.wdinfo.role != P2P_ROLE_CLIENT {
            return;
        }
        if adapter.wdinfo.p2p_ps_mode <= P2P_PS_NONE {
            return;
        }
        let stapriv = unsafe { &mut (*padapter.cast::<layout::Adapter>()).stapriv as *mut StaPriv };
        let sta = unsafe {
            rtw_get_stainfo(
                stapriv,
                adapter.mlmeextpriv.mlmext_info.network.mac_address.as_ptr(),
            )
        };
        if sta.is_null() {
            return;
        }
        let sta_ref = unsafe { &*sta };
        attrib.mac_id = sta_ref.cmn_mac_id;
        attrib.qsel = QSLT_VO;
    }
    #[cfg(all(
        config_p2p_ps_noa_use_macid_sleep,
        rust_mlme_ext_mgnt_attrib,
        not(host_mlme_ext_mgnt_attrib_test)
    ))]
    {
        let mut mac_id = attrib.mac_id;
        let mut qsel = attrib.qsel;
        if unsafe { rtw_rust_mgnt_p2p_noa_override(padapter, &mut mac_id, &mut qsel) } != 0 {
            attrib.mac_id = mac_id;
            attrib.qsel = qsel;
        }
    }
}

#[no_mangle]
pub extern "C" fn update_monitor_frame_attrib(padapter: *mut Adapter, pattrib: *mut PktAttrib) {
    if padapter.is_null() || pattrib.is_null() {
        return;
    }
    let attrib = unsafe { &mut *pattrib };

    #[cfg(host_mlme_ext_mgnt_attrib_test)]
    let (tx_rate, mgnt_seq, hw_ssn_sel, rf_type) = {
        let adapter = unsafe { &mut *(padapter as *mut layout::Adapter) };
        unsafe {
            let _ = rtw_get_stainfo(&mut adapter.stapriv as *mut _, attrib.ra.as_ptr());
        }
        (
            adapter.mlmeextpriv.tx_rate,
            adapter.mlmeextpriv.mgnt_seq,
            adapter.xmitpriv.hw_ssn_seq_no,
            adapter.hal_data.rf_type,
        )
    };
    #[cfg(rust_mlme_ext_mgnt_attrib)]
    let (tx_rate, mgnt_seq, hw_ssn_sel, rf_type) = unsafe {
        (
            rtw_rust_mgnt_tx_rate(padapter),
            rtw_rust_mgnt_mgnt_seq(padapter),
            rtw_rust_mgnt_hw_ssn_seq_no(padapter),
            rtw_rust_mgnt_hal_rf_type(padapter),
        )
    };

    attrib.hdrlen = 24;
    attrib.nr_frags = 1;
    attrib.priority = 7;
    attrib.mac_id = RTW_DEFAULT_MGMT_MACID;
    attrib.qsel = QSLT_MGNT;
    attrib.pktlen = 0;

    let wireless_mode = if tx_rate == IEEE80211_CCK_RATE_1MB {
        WIRELESS_11B
    } else {
        WIRELESS_11G
    };

    attrib.raid = unsafe { rtw_get_mgntframe_raid(padapter, wireless_mode) };

    #[cfg(any(config_80211ac_vht, host_mlme_ext_mgnt_attrib_test))]
    {
        attrib.raid = match rf_type {
            RF_1T1R => RATEID_IDX_VHT_1SS,
            RF_2T2R | RF_2T4R => RATEID_IDX_VHT_2SS,
            RF_3T3R => RATEID_IDX_VHT_3SS,
            _ => RATEID_IDX_BGN_40M_1SS,
        };
        attrib.rate = MGN_VHT1SS_MCS9;
    }
    #[cfg(not(any(config_80211ac_vht, host_mlme_ext_mgnt_attrib_test)))]
    {
        attrib.rate = MGN_MCS7;
    }

    attrib.encrypt = _NO_PRIVACY_;
    attrib.bswenc = _FALSE as U8;
    attrib.qos_en = _FALSE as U8;
    attrib.ht_en = 1;
    attrib.bwmode = CHANNEL_WIDTH_20;
    attrib.ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
    attrib.sgi = _FALSE as U8;
    attrib.seqnum = mgnt_seq;
    attrib.retry_ctrl = _TRUE as U8;
    attrib.mbssid = 0;
    attrib.hw_ssn_sel = hw_ssn_sel;
}

#[cfg(any(config_rtw_mgmt_queue, host_mlme_ext_mgnt_attrib_test))]
#[no_mangle]
pub extern "C" fn update_mgntframe_subtype(padapter: *mut Adapter, pmgntframe: *mut XmitFrame) {
    if padapter.is_null() || pmgntframe.is_null() {
        return;
    }
    let mgntframe = unsafe { &mut *pmgntframe };
    let pattrib = &mut mgntframe.attrib;
    let pframe = unsafe { mgntframe.buf_addr.add(TXDESC_OFFSET) };
    let subtype = get_frame_sub_type(pframe);
    pattrib.subtype = subtype;

    let mut category: U8 = 0;
    let mut action: U8 = 0;
    unsafe {
        rtw_action_frame_parse(pframe, pattrib.pktlen, &mut category, &mut action);
    }

    pattrib.ps_dontq = if (subtype == WIFI_ACTION
        && !(action == ACT_PUBLIC_FTM_REQ || action == ACT_PUBLIC_FTM))
        || subtype == WIFI_DISASSOC
        || subtype == WIFI_DEAUTH
        || (subtype == WIFI_PROBERSP && mlme_is_adhoc(padapter))
    {
        0
    } else {
        1
    };
}

#[no_mangle]
pub extern "C" fn update_mgntframe_attrib(padapter: *mut Adapter, pattrib: *mut PktAttrib) {
    if padapter.is_null() || pattrib.is_null() {
        return;
    }
    let attrib = unsafe { &mut *pattrib };

    #[cfg(host_mlme_ext_mgnt_attrib_test)]
    let (tx_rate, mgnt_seq, hw_ssn_sel) = {
        let adapter = unsafe { &*(padapter as *const layout::Adapter) };
        (
            adapter.mlmeextpriv.tx_rate,
            adapter.mlmeextpriv.mgnt_seq,
            adapter.xmitpriv.hw_ssn_seq_no,
        )
    };
    #[cfg(rust_mlme_ext_mgnt_attrib)]
    let (tx_rate, mgnt_seq, hw_ssn_sel) = unsafe {
        (
            rtw_rust_mgnt_tx_rate(padapter),
            rtw_rust_mgnt_mgnt_seq(padapter),
            rtw_rust_mgnt_hw_ssn_seq_no(padapter),
        )
    };

    attrib.hdrlen = 24;
    attrib.nr_frags = 1;
    attrib.priority = 7;
    attrib.mac_id = RTW_DEFAULT_MGMT_MACID;
    attrib.qsel = QSLT_MGNT;
    #[cfg(any(config_p2p_ps_noa_use_macid_sleep, host_mlme_ext_mgnt_attrib_test))]
    apply_p2p_noa_mac_id_qsel(padapter, attrib);
    attrib.pktlen = 0;

    let wireless_mode = if is_cck_rate(tx_rate) {
        WIRELESS_11B
    } else {
        WIRELESS_11G
    };
    attrib.raid = unsafe { rtw_get_mgntframe_raid(padapter, wireless_mode) };
    attrib.rate = tx_rate;
    attrib.encrypt = _NO_PRIVACY_;
    attrib.bswenc = _FALSE as U8;
    attrib.qos_en = _FALSE as U8;
    attrib.ht_en = _FALSE as U8;
    attrib.bwmode = CHANNEL_WIDTH_20;
    attrib.ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
    attrib.sgi = _FALSE as U8;
    attrib.seqnum = mgnt_seq;
    attrib.retry_ctrl = _TRUE as U8;
    attrib.mbssid = 0;
    attrib.hw_ssn_sel = hw_ssn_sel;
    #[cfg(any(config_rtw_mgmt_queue, host_mlme_ext_mgnt_attrib_test))]
    {
        attrib.ps_dontq = 1;
    }
}

#[no_mangle]
pub extern "C" fn update_mgntframe_attrib_addr(padapter: *mut Adapter, pmgntframe: *mut XmitFrame) {
    if padapter.is_null() || pmgntframe.is_null() {
        return;
    }
    let mgntframe = unsafe { &mut *pmgntframe };
    let pattrib = &mut mgntframe.attrib;
    let pframe = unsafe { mgntframe.buf_addr.add(TXDESC_OFFSET) };

    unsafe {
        core::ptr::copy_nonoverlapping(get_addr1_ptr(pframe), pattrib.ra.as_mut_ptr(), 6);
        core::ptr::copy_nonoverlapping(get_addr2_ptr(pframe), pattrib.ta.as_mut_ptr(), 6);
    }

    let mut sta = pattrib.psta;
    if sta.is_null() {
        #[cfg(host_mlme_ext_mgnt_attrib_test)]
        let stapriv = unsafe { &mut (*(padapter as *mut layout::Adapter)).stapriv as *mut _ };
        #[cfg(rust_mlme_ext_mgnt_attrib)]
        let stapriv = unsafe { rtw_rust_mgnt_stapriv(padapter) };
        sta = unsafe { rtw_get_stainfo(stapriv, pattrib.ra.as_ptr()) };
        pattrib.psta = sta;
    }

    #[cfg(any(config_beamforming, host_mlme_ext_mgnt_attrib_test))]
    if !sta.is_null() {
        unsafe {
            #[cfg(host_mlme_ext_mgnt_attrib_test)]
            update_attrib_txbf_info(padapter, pattrib, sta);
            #[cfg(all(config_beamforming, not(host_mlme_ext_mgnt_attrib_test)))]
            rtw_bf_update_attrib(padapter, pattrib, sta);
        }
    }
}
