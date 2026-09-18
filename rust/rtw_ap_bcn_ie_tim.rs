// SPDX-License-Identifier: GPL-2.0
//! W3-75 `update_BCNTIM` (PR6).

use super::{bcn_malloc, bcn_mfree, net_ie_len, net_ies, AdapterPtr, NetPtr, Sint, Uint, U8};

const _BEACON_IE_OFFSET: Sint = 12;
const _FIXED_IE_LENGTH: Sint = _BEACON_IE_OFFSET;
const _SSID_IE: U8 = 0;
const _SUPPORTEDRATES_IE: U8 = 1;
const _TIM_IE: U8 = 5;
#[cfg(host_ap_bcn_ie_test)]
#[repr(C)]
struct WlanBssidEx {
    length: u32,
    mac_address: [U8; 6],
    reserved: [U8; 2],
    ssid: [U8; 32],
    mesh_id: [U8; 32],
    privacy: u32,
    rssi: Sint,
    configuration: [U8; 16],
    infrastructure_mode: u32,
    supported_rates: [U8; 16],
    phy_info: [U8; 4],
    ie_length: u32,
    ies: [U8; 256],
}

#[cfg(host_ap_bcn_ie_test)]
#[repr(C)]
struct StaPriv {
    aid_bmp_len: U8,
    tim_bitmap: [U8; 8],
}

#[cfg(host_ap_bcn_ie_test)]
#[repr(C)]
struct MlmeExtInfo {
    network: WlanBssidEx,
}

#[cfg(host_ap_bcn_ie_test)]
#[repr(C)]
struct MlmeExtPriv {
    mlmext_info: MlmeExtInfo,
}

#[cfg(host_ap_bcn_ie_test)]
#[repr(C)]
struct Adapter {
    stapriv: StaPriv,
    mlmeextpriv: MlmeExtPriv,
}

extern "C" {
    fn rtw_get_ie(pbuf: *const U8, index: Sint, len: *mut Sint, limit: Sint) -> *mut U8;
    fn rtw_set_tim_ie(
        dtim_cnt: U8,
        dtim_period: U8,
        tim_bmp: *const U8,
        tim_bmp_len: U8,
        tim_ie: *mut U8,
    ) -> U8;
}

#[cfg(not(host_ap_bcn_ie_test))]
extern "C" {
    fn rtw_rust_ap_bcn_mlme_network(adapter: AdapterPtr) -> NetPtr;
    fn rtw_rust_ap_bcn_tim_bitmap(adapter: AdapterPtr) -> *mut U8;
    fn rtw_rust_ap_bcn_aid_bmp_len(adapter: AdapterPtr) -> U8;
}

fn mlme_network(adapter: AdapterPtr) -> NetPtr {
    unsafe {
        #[cfg(host_ap_bcn_ie_test)]
        {
            &mut (*(adapter as *mut Adapter)).mlmeextpriv.mlmext_info.network as *mut WlanBssidEx
                as NetPtr
        }
        #[cfg(not(host_ap_bcn_ie_test))]
        {
            rtw_rust_ap_bcn_mlme_network(adapter)
        }
    }
}

fn tim_bitmap(adapter: AdapterPtr) -> (*mut U8, U8) {
    unsafe {
        #[cfg(host_ap_bcn_ie_test)]
        {
            let ad = &mut *(adapter as *mut Adapter);
            (ad.stapriv.tim_bitmap.as_mut_ptr(), ad.stapriv.aid_bmp_len)
        }
        #[cfg(not(host_ap_bcn_ie_test))]
        {
            (
                rtw_rust_ap_bcn_tim_bitmap(adapter),
                rtw_rust_ap_bcn_aid_bmp_len(adapter),
            )
        }
    }
}

#[no_mangle]
pub extern "C" fn update_BCNTIM(adapter: AdapterPtr) {
    if adapter.is_null() {
        return;
    }
    let net = mlme_network(adapter);
    if net.is_null() {
        return;
    }
    unsafe {
        let pie = net_ies(net);
        let ie_length = *net_ie_len(net);
        let (tim_bmp, tim_bmp_len) = tim_bitmap(adapter);
        let mut ie_len_s = 0i32;
        let p = rtw_get_ie(
            pie.add(_FIXED_IE_LENGTH as usize),
            _TIM_IE as Sint,
            &mut ie_len_s,
            (ie_length as Sint) - _FIXED_IE_LENGTH,
        );
        let mut tim_ielen = ie_len_s as Uint;
        let (mut dst_ie, remainder_ielen, premainder_ie) = if !p.is_null() && tim_ielen > 0 {
            tim_ielen += 2;
            (
                p,
                ie_length - (p.offset_from(pie) as Uint) - tim_ielen,
                p.add(tim_ielen as usize),
            )
        } else {
            let mut offset = _FIXED_IE_LENGTH as Uint;
            let mut tmp_len: Sint = 0;
            let mut q = rtw_get_ie(
                pie.add(_BEACON_IE_OFFSET as usize),
                _SSID_IE as Sint,
                &mut tmp_len,
                (ie_length as Sint) - _BEACON_IE_OFFSET,
            );
            if !q.is_null() {
                offset += (tmp_len + 2) as Uint;
            }
            q = rtw_get_ie(
                pie.add(_BEACON_IE_OFFSET as usize),
                _SUPPORTEDRATES_IE as Sint,
                &mut tmp_len,
                (ie_length as Sint) - _BEACON_IE_OFFSET,
            );
            if !q.is_null() {
                offset += (tmp_len + 2) as Uint;
            }
            offset += 3;
            (
                pie.add(offset as usize),
                ie_length - offset,
                pie.add(offset as usize),
            )
        };
        let mut pbackup = core::ptr::null_mut();
        if remainder_ielen > 0 {
            pbackup = bcn_malloc(remainder_ielen as usize);
            if !pbackup.is_null() {
                core::ptr::copy_nonoverlapping(premainder_ie, pbackup, remainder_ielen as usize);
            }
        }
        dst_ie = dst_ie.add(rtw_set_tim_ie(0, 1, tim_bmp, tim_bmp_len, dst_ie) as usize);
        if !pbackup.is_null() {
            core::ptr::copy_nonoverlapping(pbackup, dst_ie, remainder_ielen as usize);
            bcn_mfree(pbackup, remainder_ielen as usize);
        }
        *net_ie_len(net) = dst_ie.offset_from(pie) as Uint + remainder_ielen;
    }
}
