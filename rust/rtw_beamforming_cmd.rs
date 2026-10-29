// SPDX-License-Identifier: GPL-2.0
//! W3-191 beamforming cmd/traffic leaf — host L2 oracle and kernel port.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals
)]

type U8 = u8;
type U16 = u16;
type U32 = u32;
type U64 = u64;

const BF_HOST_ETH_ALEN: usize = 6;
const BF_HOST_MAX_BFEE_ENTRY: usize = 8;
const BF_HOST_BEAMFORMING_WK_CID: U8 = 17;
const BF_HOST_CMD_END_PERIOD: U8 = 3;
const BF_HOST_CMD_ID_C2H_SND_TXBF: U8 = 0x02;
const BF_HOST_SOUNDING_STATE_NONE: U8 = 0;
const BF_HOST_SOUNDING_STATE_INIT: U8 = 1;
const BF_HOST_IDX_NONE: U8 = 0xFF;
const BF_HOST_OFDM24: U8 = 8;
const BF_HOST_SUCCESS: U8 = 1;

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostCmdEnqueueTr {
    pub count: U32,
    pub last_type: i32,
    pub last_size: i32,
    pub last_enqueue: U8,
    pub last_ec_id: U8,
    pub sync_cmd_hdl: U8,
}

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostC2hTr {
    pub cancel_timer: U32,
    pub end_period_cmd: U32,
    pub end_period_res: U8,
}

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostTrafficTr {
    pub timer_set: U32,
    pub bfee0_apply: U8,
    pub bfee0_delete: U8,
    pub bfee0_sound_cnt: U8,
}

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostBfeeEntry {
    pub used: U8,
    pub mac_id: U16,
    pub mac_addr: [U8; BF_HOST_ETH_ALEN],
    pub tx_timestamp: U32,
    pub tx_bytes: U64,
    pub bApplySounding: U8,
    pub bDeleteSounding: U8,
    pub SoundCnt: U8,
}

#[cfg(host_bf_cmd_traffic_test)]
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

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostCmdTrafficInfo {
    pub bfee_entry: [BfHostBfeeEntry; BF_HOST_MAX_BFEE_ENTRY],
    pub beamformee_su_cnt: U8,
    pub beamformee_mu_cnt: U8,
    pub sounding_info: BfHostSoundingInfo,
    pub sounding_timer_armed: U8,
}

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostBfee {
    pub used: U8,
    pub mac_id: U16,
    pub mac_addr: [U8; BF_HOST_ETH_ALEN],
    pub cap: u32,
}

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostBfer {
    pub used: U8,
    pub mac_addr: [U8; BF_HOST_ETH_ALEN],
}

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostCsi {
    pub Nc: U8,
    pub Nr: U8,
    pub Ng: U8,
    pub CodeBook: U8,
    pub ChnlWidth: U8,
    pub bVHT: U8,
}

#[cfg(host_bf_cmd_traffic_test)]
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

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostHal {
    pub beamforming_info: BfHostInfo,
}

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostMlme {
    pub pad: U8,
    pub mac_addr: [U8; BF_HOST_ETH_ALEN],
    pub bssid: [U8; BF_HOST_ETH_ALEN],
}

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostAdpt {
    pub mlmepriv: BfHostMlme,
    pub hal: BfHostHal,
}

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostStaCmn {
    pub p_aid: U16,
    pub g_id: U8,
}

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostStaStats {
    pub tx_bytes: U64,
}

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostSta {
    pub used: U8,
    pub mac_addr: [U8; BF_HOST_ETH_ALEN],
    pub cmn: BfHostStaCmn,
    pub sta_stats: BfHostStaStats,
}

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostStapriv {
    pub stas: [BfHostSta; 4],
}

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostCmdTrafficAdpt {
    pub base: BfHostAdpt,
    pub stapriv: BfHostStapriv,
    pub traffic: BfHostCmdTrafficInfo,
}

#[cfg(host_bf_cmd_traffic_test)]
#[repr(C)]
pub struct BfHostPktAttrib {
    pub txbf_p_aid: U16,
    pub txbf_g_id: U16,
}

#[cfg(host_bf_cmd_traffic_test)]
type BfHostCmdTrafficPadpt = *mut BfHostCmdTrafficAdpt;
#[cfg(host_bf_cmd_traffic_test)]
type BfHostPadpt = *mut BfHostAdpt;

#[cfg(host_bf_cmd_traffic_test)]
extern "C" {
    static mut bf_host_cmd_enqueue_tr: BfHostCmdEnqueueTr;
    static mut bf_host_c2h_tr: BfHostC2hTr;
    static mut bf_host_traffic_tr: BfHostTrafficTr;
    fn bf_host_cancel_sounding_timeout_timer(a: BfHostCmdTrafficPadpt);
    fn bf_host_set_sounding_timer(a: BfHostCmdTrafficPadpt, ms: U32);
    fn bf_host_get_stainfo(a: BfHostCmdTrafficPadpt, mac: *mut U8) -> *mut BfHostSta;
    fn bf_host_phydm_get_beamforming_sounding_info(tp: *mut U16, num: U8, tx_rate: *mut U8) -> U8;
    fn bf_host_get_current_time() -> U32;
    fn bf_host_get_time_interval_ms(a: U32, b: U32) -> U32;
    fn bf_host_get_current_tx_rate(a: BfHostCmdTrafficPadpt, sta: *mut BfHostSta) -> U8;
    fn bf_host_beamforming_enter(a: BfHostPadpt, p: *mut U8);
    fn bf_host_beamforming_leave(a: BfHostPadpt, p: *mut U8);
    fn bf_host_beamforming_reset(a: BfHostPadpt);
    fn bf_host_sounding_handler(a: BfHostPadpt);
    fn bf_host_beamforming_sounding_down(a: BfHostPadpt, macid: U8);
    fn bf_host_hal_set_gid(a: BfHostPadpt, p: *mut U8);
    fn bf_host_hal_set_csi(a: BfHostPadpt, p: *mut U8);
}

#[cfg(host_bf_cmd_traffic_test)]
unsafe fn c2h_snd_result(buf: *mut U8) -> U8 {
    if buf.is_null() {
        return 0;
    }
    if (*buf & 1) != 0 {
        1
    } else {
        0
    }
}

#[cfg(host_bf_cmd_traffic_test)]
unsafe fn to_mbps(bytes: U64, secs: U32) -> U64 {
    let div = if secs == 0 { 1 } else { secs as U64 };
    (bytes >> 17) / div
}

#[cfg(host_bf_cmd_traffic_test)]
#[no_mangle]
pub unsafe extern "C" fn o_bf_cmd(
    adapter: BfHostCmdTrafficPadpt,
    ty: i32,
    pbuf: *mut U8,
    size: i32,
    enqueue: U8,
) -> U8 {
    if enqueue == 0 {
        let base = &mut (*adapter).base as *mut BfHostAdpt;
        match ty as U8 {
            0 => bf_host_beamforming_enter(base, pbuf),
            1 => {
                if pbuf.is_null() {
                    bf_host_beamforming_reset(base);
                } else {
                    bf_host_beamforming_leave(base, pbuf);
                }
            }
            2 => bf_host_sounding_handler(base),
            3 => {
                let macid = if pbuf.is_null() { 0 } else { *pbuf };
                bf_host_beamforming_sounding_down(base, macid);
            }
            6 => bf_host_hal_set_gid(base, pbuf),
            7 => bf_host_hal_set_csi(base, pbuf),
            _ => {}
        }
        bf_host_cmd_enqueue_tr.sync_cmd_hdl += 1;
        return BF_HOST_SUCCESS;
    }
    bf_host_cmd_enqueue_tr.count += 1;
    bf_host_cmd_enqueue_tr.last_type = ty;
    bf_host_cmd_enqueue_tr.last_size = size;
    bf_host_cmd_enqueue_tr.last_enqueue = enqueue;
    bf_host_cmd_enqueue_tr.last_ec_id = BF_HOST_BEAMFORMING_WK_CID;
    let _ = pbuf;
    BF_HOST_SUCCESS
}

#[cfg(host_bf_cmd_traffic_test)]
#[no_mangle]
pub unsafe extern "C" fn o_bf_update_attrib(
    adapter: BfHostCmdTrafficPadpt,
    attrib: *mut BfHostPktAttrib,
    sta: *mut BfHostSta,
) {
    let _ = adapter;
    if sta.is_null() || attrib.is_null() {
        return;
    }
    (*attrib).txbf_g_id = (*sta).cmn.g_id as U16;
    (*attrib).txbf_p_aid = (*sta).cmn.p_aid;
}

#[cfg(host_bf_cmd_traffic_test)]
#[no_mangle]
pub unsafe extern "C" fn o_bf_c2h_handler(
    adapter: BfHostCmdTrafficPadpt,
    id: U8,
    buf: *mut U8,
    buf_len: U8,
) {
    let _ = buf_len;
    if id != BF_HOST_CMD_ID_C2H_SND_TXBF {
        return;
    }
    bf_host_cancel_sounding_timeout_timer(adapter);
    let mut res = c2h_snd_result(buf);
    bf_host_c2h_tr.cancel_timer += 1;
    bf_host_c2h_tr.end_period_res = res;
    o_bf_cmd(adapter, BF_HOST_CMD_END_PERIOD as i32, &mut res as *mut U8, 1, 1);
    bf_host_c2h_tr.end_period_cmd += 1;
}

#[cfg(host_bf_cmd_traffic_test)]
#[no_mangle]
pub unsafe extern "C" fn o_bf_update_traffic(adapter: BfHostCmdTrafficPadpt) {
    if adapter.is_null() {
        return;
    }
    let info = &mut (*adapter).traffic;
    let bfee_cnt = info.beamformee_su_cnt + info.beamformee_mu_cnt;
    if bfee_cnt == 0 {
        return;
    }
    let mut tp = [0u16; BF_HOST_MAX_BFEE_ENTRY];
    let mut tx_rate = [0u8; BF_HOST_MAX_BFEE_ENTRY];
    let mut set_timer = false;

    for i in 0..BF_HOST_MAX_BFEE_ENTRY {
        let bfee = &mut info.bfee_entry[i];
        if bfee.used == 0 {
            continue;
        }
        let sta = bf_host_get_stainfo(adapter, bfee.mac_addr.as_mut_ptr());
        if sta.is_null() {
            continue;
        }
        let last_timestamp = bfee.tx_timestamp;
        let last_bytes = bfee.tx_bytes;
        bfee.tx_timestamp = bf_host_get_current_time();
        bfee.tx_bytes = (*sta).sta_stats.tx_bytes;
        if last_timestamp != 0 {
            let tx_bytes = if bfee.tx_bytes >= last_bytes {
                bfee.tx_bytes - last_bytes
            } else {
                bfee.tx_bytes + !last_bytes
            };
            let mut time = bf_host_get_time_interval_ms(last_timestamp, bfee.tx_timestamp);
            if time > 1000 {
                time /= 1000;
            } else {
                time = 1;
            }
            tp[i] = to_mbps(tx_bytes, time) as U16;
            tx_rate[i] = bf_host_get_current_tx_rate(adapter, sta);
        }
    }

    let sounding_idx =
        bf_host_phydm_get_beamforming_sounding_info(tp.as_mut_ptr(), BF_HOST_MAX_BFEE_ENTRY as U8, tx_rate.as_mut_ptr());

    for i in 0..BF_HOST_MAX_BFEE_ENTRY {
        let bfee = &mut info.bfee_entry[i];
        if bfee.used == 0 {
            continue;
        }
        if (sounding_idx & (1u8 << i)) != 0 {
            if bfee.bApplySounding == 0 {
                bfee.bApplySounding = 1;
                bfee.SoundCnt = 0;
                set_timer = true;
            }
        } else if bfee.bApplySounding != 0 {
            bfee.bApplySounding = 0;
            bfee.bDeleteSounding = 1;
            bfee.SoundCnt = 0;
            set_timer = true;
        }
    }

    if set_timer && info.sounding_info.state == BF_HOST_SOUNDING_STATE_NONE {
        info.sounding_info.state = BF_HOST_SOUNDING_STATE_INIT;
        bf_host_set_sounding_timer(adapter, 0);
        bf_host_traffic_tr.timer_set += 1;
    }

    if info.bfee_entry[0].used != 0 {
        bf_host_traffic_tr.bfee0_apply = info.bfee_entry[0].bApplySounding;
        bf_host_traffic_tr.bfee0_delete = info.bfee_entry[0].bDeleteSounding;
        bf_host_traffic_tr.bfee0_sound_cnt = info.bfee_entry[0].SoundCnt;
    }
}

#[cfg(host_bf_cmd_traffic_test)]
const _: () = {
    assert!(core::mem::size_of::<BfHostCmdTrafficAdpt>() == 616);
    assert!(core::mem::offset_of!(BfHostCmdTrafficAdpt, traffic) == 336);
};
