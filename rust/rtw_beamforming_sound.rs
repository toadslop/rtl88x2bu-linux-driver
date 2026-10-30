// SPDX-License-Identifier: GPL-2.0
//! W3-192 beamforming sounding enter — host L2 oracle and kernel hook.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_snake_case,
    non_camel_case_types
)]

#[cfg(host_bf_sound_enter_test)]
use std::mem::offset_of;

type U8 = u8;
type U16 = u16;
type U32 = u32;
type U64 = u64;

const BF_HOST_ETH_ALEN: usize = 6;
const BF_HOST_WIRELESS_11_24N: U8 = 8;
const BF_HOST_BEAMFORMING_HT_BEAMFORMER_ENABLE: U16 = 1;
const BF_HOST_BEAMFORMING_HT_BEAMFORMEE_CHNL_EST_CAP: U16 = 0xC0;
const BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT: U8 = 0x2;
const BF_HOST_BEAMFORMER_CAP_HT_EXPLICIT: U8 = 0x1;
const BF_HOST_BEAMFORMEE_CAP: U8 = 0x10;
const BF_HOST_CAP_NONE: U8 = 0;
const BF_HOST_SOUNDING_STATE_NONE: U8 = 0;
const BF_HOST_SOUNDING_STATE_INIT: U8 = 1;
const BF_HOST_MAX_BEAMFORMEE_ENTRY: usize = 8;
const BF_HOST_MAX_NUM_BEAMFORMEE_SU: usize = 2;

#[cfg(host_bf_sound_enter_test)]
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

#[cfg(host_bf_sound_enter_test)]
#[repr(C)]
pub struct BfHostBfee {
    pub used: U8,
    pub mac_id: U16,
    pub mac_addr: [U8; BF_HOST_ETH_ALEN],
    pub cap: u32,
}

#[cfg(host_bf_sound_enter_test)]
#[repr(C)]
pub struct BfHostBfer {
    pub used: U8,
    pub mac_addr: [U8; BF_HOST_ETH_ALEN],
}

#[cfg(host_bf_sound_enter_test)]
#[repr(C)]
pub struct BfHostCsi {
    pub Nc: U8,
    pub Nr: U8,
    pub Ng: U8,
    pub CodeBook: U8,
    pub ChnlWidth: U8,
    pub bVHT: U8,
}

#[cfg(host_bf_sound_enter_test)]
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

#[cfg(host_bf_sound_enter_test)]
#[repr(C)]
pub struct BfHostHal {
    pub beamforming_info: BfHostInfo,
}

#[cfg(host_bf_sound_enter_test)]
#[repr(C)]
pub struct BfHostMlme {
    pub pad: U8,
    pub mac_addr: [U8; BF_HOST_ETH_ALEN],
    pub bssid: [U8; BF_HOST_ETH_ALEN],
}

#[cfg(host_bf_sound_enter_test)]
#[repr(C)]
pub struct BfHostAdpt {
    pub mlmepriv: BfHostMlme,
    pub hal: BfHostHal,
}

#[cfg(host_bf_sound_enter_test)]
#[repr(C)]
pub struct BfHostSoundStaCmn {
    pub mac_addr: [U8; BF_HOST_ETH_ALEN],
    pub aid: U16,
    pub mac_id: U16,
    pub g_id: U8,
    pub p_aid: U16,
}

#[cfg(host_bf_sound_enter_test)]
#[repr(C)]
pub struct BfHostSoundSta {
    pub used: U8,
    pub wireless_mode: U8,
    pub cmn: BfHostSoundStaCmn,
}

#[cfg(host_bf_sound_enter_test)]
#[repr(C)]
pub struct BfHostSoundStapriv {
    pub stas: [BfHostSoundSta; 4],
}

#[cfg(host_bf_sound_enter_test)]
#[repr(C)]
pub struct BfHostSoundBfeeEntry {
    pub used: U8,
    pub mac_id: U16,
    pub mac_addr: [U8; BF_HOST_ETH_ALEN],
    pub cap: U8,
    pub NumofSoundingDim: U8,
    pub su_reg_index: U8,
}

#[cfg(host_bf_sound_enter_test)]
#[repr(C)]
pub struct BfHostSoundEnterInfo {
    pub bfee_entry: [BfHostSoundBfeeEntry; BF_HOST_MAX_BEAMFORMEE_ENTRY],
    pub beamforming_cap: U8,
    pub beamformee_su_cnt: U8,
    pub beamformee_su_reg_maping: U32,
    pub sounding_info: BfHostSoundingInfo,
}

#[cfg(host_bf_sound_enter_test)]
#[repr(C)]
pub struct BfHostSoundEnterAdpt {
    pub base: BfHostAdpt,
    pub ht_beamform_cap: U16,
    pub vht_beamform_cap: U16,
    pub stapriv: BfHostSoundStapriv,
    pub sound: BfHostSoundEnterInfo,
}

#[cfg(host_bf_sound_enter_test)]
const _: () = {
    assert!(core::mem::size_of::<BfHostSoundEnterAdpt>() == 448);
    assert!(offset_of!(BfHostSoundEnterAdpt, sound) == 308);
};

#[cfg(host_bf_sound_enter_test)]
type BfHostSoundEnterPadpt = *mut BfHostSoundEnterAdpt;

#[cfg(host_bf_sound_enter_test)]
extern "C" {
    static mut bf_host_sound_enter_tr_hal: U32;
    static mut bf_host_sound_enter_tr_timer_ms: U32;
    fn bf_host_sound_get_stainfo(a: BfHostSoundEnterPadpt, mac: *mut U8) -> *mut BfHostSoundSta;
}

#[cfg(host_bf_sound_enter_test)]
unsafe fn sound_get_sta_beamform_cap(
    adapter: BfHostSoundEnterPadpt,
    sta: *mut BfHostSoundSta,
    sta_bf_cap: *mut U8,
    sounding_dim: *mut U8,
) {
    let info = &mut (*adapter).sound;
    *sta_bf_cap = 0;
    *sounding_dim = 0;
    if ((*sta).wireless_mode & BF_HOST_WIRELESS_11_24N) == 0 {
        return;
    }
    let ht_bf_cap = (*adapter).ht_beamform_cap;
    if (ht_bf_cap & BF_HOST_BEAMFORMING_HT_BEAMFORMER_ENABLE) != 0 {
        info.beamforming_cap |= BF_HOST_BEAMFORMER_CAP_HT_EXPLICIT;
        *sta_bf_cap |= BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT;
        *sounding_dim = ((ht_bf_cap & BF_HOST_BEAMFORMING_HT_BEAMFORMEE_CHNL_EST_CAP) >> 6) as U8;
    }
}

#[cfg(host_bf_sound_enter_test)]
unsafe fn sound_bfee_add_entry(
    adapter: BfHostSoundEnterPadpt,
    sta: *mut BfHostSoundSta,
    bf_cap: U8,
    sounding_dim: U8,
) -> bool {
    let info = &mut (*adapter).sound;
    let mut slot = None;
    for i in 0..BF_HOST_MAX_BEAMFORMEE_ENTRY {
        if info.bfee_entry[i].used == 0 {
            slot = Some(i);
            break;
        }
    }
    let Some(i) = slot else {
        return false;
    };
    let bfee = &mut info.bfee_entry[i];
    bfee.used = 1;
    bfee.mac_id = (*sta).cmn.mac_id;
    bfee.mac_addr = (*sta).cmn.mac_addr;
    bfee.cap = bf_cap;
    bfee.NumofSoundingDim = sounding_dim;
    if (bf_cap & BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT) != 0 {
        info.beamformee_su_cnt += 1;
        for j in 0..BF_HOST_MAX_NUM_BEAMFORMEE_SU {
            if (info.beamformee_su_reg_maping & (1u32 << j)) == 0 {
                info.beamformee_su_reg_maping |= 1u32 << j;
                bfee.su_reg_index = j as U8;
                break;
            }
        }
    }
    true
}

#[cfg(host_bf_sound_enter_test)]
#[no_mangle]
pub unsafe extern "C" fn o_beamforming_enter(adapter: BfHostSoundEnterPadpt, p: *mut U8) {
    if adapter.is_null() || p.is_null() {
        return;
    }
    let sta_copy = p as *mut BfHostSoundSta;
    let sta = bf_host_sound_get_stainfo(adapter, (*sta_copy).cmn.mac_addr.as_mut_ptr());
    if sta.is_null() || ((*sta).wireless_mode & BF_HOST_WIRELESS_11_24N) == 0 {
        return;
    }
    if (*adapter).ht_beamform_cap == 0 && (*adapter).vht_beamform_cap == 0 {
        return;
    }
    let mut sta_bf_cap = 0u8;
    let mut sounding_dim = 0u8;
    sound_get_sta_beamform_cap(adapter, sta, &mut sta_bf_cap, &mut sounding_dim);
    if sta_bf_cap == BF_HOST_CAP_NONE {
        return;
    }
    if (sta_bf_cap & BF_HOST_BEAMFORMEE_CAP_HT_EXPLICIT) != 0 {
        sta_bf_cap |= BF_HOST_BEAMFORMEE_CAP;
    }
    if !sound_bfee_add_entry(adapter, sta, sta_bf_cap, sounding_dim) {
        return;
    }
    bf_host_sound_enter_tr_hal += 1;
    let info = &mut (*adapter).sound;
    if info.beamformee_su_cnt != 0 && info.sounding_info.state == BF_HOST_SOUNDING_STATE_NONE {
        info.sounding_info.state = BF_HOST_SOUNDING_STATE_INIT;
        bf_host_sound_enter_tr_timer_ms = 2000;
    }
}

#[cfg(not(host_bf_sound_enter_test))]
extern "C" {
    fn rtw_rust_beamforming_enter_impl(adapter: *mut core::ffi::c_void, p: *mut core::ffi::c_void);
}

#[cfg(not(host_bf_sound_enter_test))]
#[no_mangle]
pub extern "C" fn rtw_rust_beamforming_enter(
    adapter: *mut core::ffi::c_void,
    p: *mut core::ffi::c_void,
) {
    unsafe { rtw_rust_beamforming_enter_impl(adapter, p) };
}
