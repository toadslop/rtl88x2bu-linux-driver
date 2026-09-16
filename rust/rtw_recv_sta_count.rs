// SPDX-License-Identifier: GPL-2.0
//! W3-85 PR1: host L2 `count_rx_stats` (Rust vs C).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

type U8 = u8;
type U32 = u32;
type U64 = u64;
type Systime = U32;

const ETH_ALEN: usize = 6;
const TID_NUM: usize = 16;

#[repr(C)]
pub struct StainfoStatsHost {
    pub last_rx_time: Systime,
    pub rx_data_pkts: U64,
    pub rx_data_bc_pkts: U64,
    pub rx_data_mc_pkts: U64,
    pub rx_data_qos_pkts: [U64; TID_NUM],
    pub rx_bytes: U64,
    pub rx_bc_bytes: U64,
    pub rx_mc_bytes: U64,
    pub rxratecnt: [U32; 128],
}

#[repr(C)]
pub struct StaInfoHost {
    pub sta_stats: StainfoStatsHost,
}

#[repr(C)]
pub struct LinkDetectHost {
    pub num_rx_ok_in_period: U32,
    pub num_rx_unicast_ok_in_period: U32,
}

#[repr(C)]
pub struct WlanBssidExHost {
    pub mac_address: [U8; ETH_ALEN],
}

#[repr(C)]
pub struct WlanNetworkHost {
    pub network: WlanBssidExHost,
}

#[repr(C)]
pub struct MlmePrivHost {
    pub fw_state: U32,
    pub cur_network: WlanNetworkHost,
    pub link_detect_info: LinkDetectHost,
}

#[repr(C)]
pub struct RecvPrivHost {
    pub rx_bytes: U64,
}

#[repr(C)]
pub struct StaPrivHost {
    pub _pad: U8,
}

#[repr(C)]
pub struct RfCtlHost {
    pub radar_detected: U8,
}

#[repr(C)]
pub struct RxPktAttribHost {
    pub len: u32,
    pub to_fr_ds: U8,
    pub amsdu: U8,
    pub priority: U8,
    pub data_rate: U8,
    pub dst: [U8; ETH_ALEN],
    pub src: [U8; ETH_ALEN],
    pub ta: [U8; ETH_ALEN],
    pub ra: [U8; ETH_ALEN],
    pub bssid: [U8; ETH_ALEN],
}

#[repr(C)]
pub struct RecvFrameHdrHost {
    pub len: u32,
    pub rx_data: *mut U8,
    pub rx_tail: *mut U8,
    pub attrib: RxPktAttribHost,
    pub psta: *mut StaInfoHost,
}

#[repr(C)]
pub struct RecvFrameHost {
    pub hdr: RecvFrameHdrHost,
}

#[repr(C)]
pub struct AdapterHost {
    pub mac_addr: [U8; ETH_ALEN],
    pub mlmepriv: MlmePrivHost,
    pub recvpriv: RecvPrivHost,
    pub stapriv: StaPrivHost,
    pub rfctl: RfCtlHost,
}

extern "C" {
    fn rtw_get_current_time() -> Systime;
}

fn mac_addr_is_bcst(addr: &[U8; ETH_ALEN]) -> bool {
    addr.iter().all(|&b| b == 0xff)
}

fn is_mcast(da: &[U8; ETH_ALEN]) -> bool {
    (da[0] & 0x01) != 0
}

fn is_broadcast_mac_addr(addr: &[U8; ETH_ALEN]) -> bool {
    mac_addr_is_bcst(addr)
}

#[no_mangle]
pub extern "C" fn count_rx_stats(
    adapter: *mut AdapterHost,
    rframe: *mut RecvFrameHost,
    sta: *mut StaInfoHost,
) {
    if adapter.is_null() || rframe.is_null() {
        return;
    }
    let adapter = unsafe { &mut *adapter };
    let rframe = unsafe { &mut *rframe };
    let sz = rframe.hdr.len as i32;
    adapter.recvpriv.rx_bytes += sz as u64;
    adapter.mlmepriv.link_detect_info.num_rx_ok_in_period += 1;

    let dst = rframe.hdr.attrib.dst;
    if !mac_addr_is_bcst(&dst) && !is_mcast(&dst) {
        adapter.mlmepriv.link_detect_info.num_rx_unicast_ok_in_period += 1;
    }

    let psta = if !sta.is_null() {
        sta
    } else {
        rframe.hdr.psta
    };
    if psta.is_null() {
        return;
    }
    let psta = unsafe { &mut *psta };
    let pstats = &mut psta.sta_stats;
    let ra = rframe.hdr.attrib.ra;
    let is_ra_bmc = is_mcast(&ra);

    pstats.last_rx_time = unsafe { rtw_get_current_time() };
    pstats.rx_data_pkts += 1;
    pstats.rx_bytes += sz as u64;
    if is_broadcast_mac_addr(&ra) {
        pstats.rx_data_bc_pkts += 1;
        pstats.rx_bc_bytes += sz as u64;
    } else if is_ra_bmc {
        pstats.rx_data_mc_pkts += 1;
        pstats.rx_mc_bytes += sz as u64;
    }

    if !is_ra_bmc {
        let rate = rframe.hdr.attrib.data_rate as usize;
        if rate < pstats.rxratecnt.len() {
            pstats.rxratecnt[rate] += 1;
        }
        let pri = rframe.hdr.attrib.priority as usize;
        if pri < TID_NUM {
            pstats.rx_data_qos_pkts[pri] += 1;
        }
    }
}
