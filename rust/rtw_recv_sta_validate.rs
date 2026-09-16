// SPDX-License-Identifier: GPL-2.0
//! W3-85 PR2: `rtw_sta_rx_data_validate_hdr` — host L2 oracle.

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
type U16 = u16;
type U32 = u32;
type S32 = i32;
type Systime = U32;

const _SUCCESS: i32 = 1;
const _FAIL: i32 = 0;
const RTW_RX_HANDLED: i32 = 2;

const ETH_ALEN: usize = 6;

const WIFI_ASOC_STATE: U32 = 0x0000_0001;
const WIFI_UNDER_LINKING: U32 = 0x0000_0080;
const WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA: U16 = 7;

#[repr(C)]
pub struct StainfoStatsHost {
    pub last_rx_time: Systime,
    pub rx_data_pkts: u64,
}

#[repr(C)]
pub struct StaInfoHost {
    pub sta_stats: StainfoStatsHost,
}

#[repr(C)]
pub struct StaPrivHost {
    pub _pad: U8,
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
    pub rx_bytes: u64,
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
    fn rtw_get_stainfo(stapriv: *mut StaPrivHost, hwaddr: *mut U8) -> *mut StaInfoHost;
    fn rtw_get_current_time() -> Systime;
    fn rtw_get_passing_time_ms(start: Systime) -> S32;
    fn issue_deauth(adapter: *mut AdapterHost, mac: *mut U8, reason: U16);
    fn count_rx_stats(adapter: *mut AdapterHost, rframe: *mut RecvFrameHost, sta: *mut StaInfoHost);
}

fn is_mcast(da: &[U8; ETH_ALEN]) -> bool {
    (da[0] & 0x01) != 0
}

fn get_addr1_ptr(pbuf: *mut U8) -> *mut U8 {
    unsafe { pbuf.add(4) }
}

fn get_addr2_ptr(pbuf: *mut U8) -> *mut U8 {
    unsafe { pbuf.add(10) }
}

fn get_addr3_ptr(pbuf: *mut U8) -> *mut U8 {
    unsafe { pbuf.add(16) }
}

fn get_frame_sub_type(pbuf: *mut U8) -> U16 {
    unsafe {
        let v = std::ptr::read_unaligned(pbuf as *const U16);
        v & (0x0080 | 0x0040 | 0x0020 | 0x0010 | 0x0008 | 0x0004)
    }
}

fn eth_eq(a: *const U8, b: *const U8) -> bool {
    if a.is_null() || b.is_null() {
        return false;
    }
    unsafe { std::slice::from_raw_parts(a, ETH_ALEN) == std::slice::from_raw_parts(b, ETH_ALEN) }
}

fn copy_eth(dst: &mut [U8; ETH_ALEN], src: *const U8) {
    if src.is_null() {
        return;
    }
    unsafe {
        std::ptr::copy_nonoverlapping(src, dst.as_mut_ptr(), ETH_ALEN);
    }
}

#[no_mangle]
pub extern "C" fn rtw_sta_rx_data_validate_hdr(
    adapter: *mut AdapterHost,
    rframe: *mut RecvFrameHost,
    sta: *mut *mut StaInfoHost,
) -> i32 {
    if adapter.is_null() || rframe.is_null() || sta.is_null() {
        return _FAIL;
    }
    let adapter = unsafe { &mut *adapter };
    let rframe = unsafe { &mut *rframe };
    let whdr = rframe.hdr.rx_data;
    if whdr.is_null() {
        return _FAIL;
    }
    let rattrib = &mut rframe.hdr.attrib;
    let mut a1 = [0u8; ETH_ALEN];
    unsafe {
        std::ptr::copy_nonoverlapping(get_addr1_ptr(whdr), a1.as_mut_ptr(), ETH_ALEN);
    }
    let is_ra_bmc = is_mcast(&a1);
    let mut ret = _FAIL;

    if rattrib.to_fr_ds == 0 {
        copy_eth(&mut rattrib.ra, get_addr1_ptr(whdr));
        copy_eth(&mut rattrib.ta, get_addr2_ptr(whdr));
        copy_eth(&mut rattrib.dst, get_addr1_ptr(whdr));
        copy_eth(&mut rattrib.src, get_addr2_ptr(whdr));
        copy_eth(&mut rattrib.bssid, get_addr3_ptr(whdr));
        if !eth_eq(rattrib.bssid.as_ptr(), rattrib.src.as_ptr()) {
            return ret;
        }
        unsafe {
            *sta = rtw_get_stainfo(&mut adapter.stapriv as *mut _, get_addr2_ptr(whdr));
            if !(*sta).is_null() {
                ret = _SUCCESS;
            }
        }
        return ret;
    }

    let mlme_state = adapter.mlmepriv.fw_state;
    if (mlme_state & (WIFI_ASOC_STATE | WIFI_UNDER_LINKING)) == 0 {
        if !is_ra_bmc {
            static mut SEND_ISSUE_DEAUTH_TIME: Systime = 0;
            unsafe {
                if rtw_get_passing_time_ms(SEND_ISSUE_DEAUTH_TIME) > 10000
                    || SEND_ISSUE_DEAUTH_TIME == 0
                {
                    SEND_ISSUE_DEAUTH_TIME = rtw_get_current_time();
                    issue_deauth(
                        adapter,
                        get_addr2_ptr(whdr),
                        WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA,
                    );
                }
            }
        }
        return ret;
    }

    copy_eth(&mut rattrib.ra, get_addr1_ptr(whdr));
    copy_eth(&mut rattrib.ta, get_addr2_ptr(whdr));
    match rattrib.to_fr_ds {
        2 => {
            copy_eth(&mut rattrib.dst, get_addr1_ptr(whdr));
            copy_eth(&mut rattrib.src, get_addr3_ptr(whdr));
            copy_eth(&mut rattrib.bssid, get_addr2_ptr(whdr));
        }
        3 => {
            copy_eth(&mut rattrib.dst, get_addr3_ptr(whdr));
            copy_eth(&mut rattrib.src, unsafe { whdr.add(24) });
            copy_eth(&mut rattrib.bssid, get_addr2_ptr(whdr));
        }
        _ => return RTW_RX_HANDLED,
    }

    if rattrib.amsdu == 0 && eth_eq(rattrib.src.as_ptr(), adapter.mac_addr.as_ptr()) {
        return ret;
    }

    unsafe {
        *sta = rtw_get_stainfo(&mut adapter.stapriv as *mut _, rattrib.ta.as_mut_ptr());
    }
    if unsafe { (*sta).is_null() } {
        if !is_ra_bmc && adapter.rfctl.radar_detected == 0 {
            unsafe {
                issue_deauth(
                    adapter,
                    rattrib.ta.as_mut_ptr(),
                    WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA,
                );
            }
        }
        return ret;
    }

    if (get_frame_sub_type(whdr) & (1 << 6)) != 0 {
        unsafe {
            count_rx_stats(adapter, rframe, *sta);
        }
        return RTW_RX_HANDLED;
    }

    _SUCCESS
}
