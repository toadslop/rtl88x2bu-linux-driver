// SPDX-License-Identifier: GPL-2.0
//! Host L2 TDLS HT/VHT cap helpers (oracle parity with tests/host/tdls/*_shim.c).

#[cfg(host_tdls_ht_cap_test)]
pub mod ht_cap {
    const _TRUE: u8 = 1;
    const _FALSE: u8 = 0;
    const WLAN_STA_WME: i32 = 1 << 9;
    const WLAN_STA_HT: i32 = 1 << 11;
    const WIRELESS_11_24N: u8 = 1 << 3;
    const WIRELESS_11_5N: u8 = 1 << 4;
    const CHANNEL_WIDTH_40: u8 = 1;
    const IEEE80211_HT_CAP_SUP_WIDTH: u16 = 0x0002;
    const IEEE80211_HT_CAP_SGI_20: u16 = 0x0020;
    const HT_CAP_LEN: usize = 26;

    #[repr(C)]
    pub struct HtPriv {
        pub ht_option: u8,
        pub ampdu_enable: u8,
        pub rx_ampdu_min_spacing: u8,
        pub ch_offset: u8,
        pub sgi_20m: u8,
        pub sgi_40m: u8,
        pub ht_cap: [u8; HT_CAP_LEN],
    }

    #[repr(C)]
    pub struct RegistryPriv {
        pub ht_enable: u8,
        pub wireless_mode: u8,
        pub ampu_enable: u8,
    }

    #[repr(C, packed)]
    pub struct HtCapsElement {
        pub ht_caps_info: u16,
        pub ampdu_para: u8,
        pub mcs_rate: [u8; 16],
        pub ht_ext_caps: u16,
        pub beamforming_caps: u32,
        pub asel_caps: u8,
    }

    #[repr(C)]
    pub struct MlmeExtInfo {
        pub ht_caps: HtCapsElement,
    }

    #[repr(C)]
    pub struct MlmeExtPriv {
        pub cur_bwmode: u8,
        pub cur_ch_offset: u8,
        pub mlmext_info: MlmeExtInfo,
    }

    #[repr(C)]
    pub struct MlmePriv {
        pub htpriv: HtPriv,
    }

    #[repr(C)]
    pub struct StaInfo {
        pub flags: i32,
        pub qos_option: u32,
        pub htpriv: HtPriv,
        pub bw_mode: u8,
    }

    #[repr(C)]
    pub struct Adapter {
        pub registrypriv: RegistryPriv,
        pub mlmepriv: MlmePriv,
        pub mlmeextpriv: MlmeExtPriv,
    }

    fn cpu_to_le16(x: u16) -> u16 {
        x.to_le()
    }

    fn is_supported_ht(mode: u8) -> u8 {
        if mode & (WIRELESS_11_24N | WIRELESS_11_5N) != 0 {
            _TRUE
        } else {
            _FALSE
        }
    }

    fn ht_cap_info(cap: &[u8; HT_CAP_LEN]) -> u16 {
        u16::from_le_bytes([cap[0], cap[1]])
    }

    #[no_mangle]
    pub extern "C" fn rtw_tdls_process_ht_cap(
        padapter: *mut Adapter,
        ptdls_sta: *mut StaInfo,
        data: *mut u8,
        length: u8,
    ) {
        if padapter.is_null() || ptdls_sta.is_null() {
            return;
        }
        let adapter = unsafe { &mut *padapter };
        let sta = unsafe { &mut *ptdls_sta };
        let pmlmeinfo = &mut adapter.mlmeextpriv.mlmext_info;
        let phtpriv = &mut adapter.mlmepriv.htpriv;

        sta.htpriv.ht_cap.fill(0);
        if !data.is_null() && length as usize >= HT_CAP_LEN {
            sta.flags |= WLAN_STA_HT | WLAN_STA_WME;
            unsafe {
                core::ptr::copy_nonoverlapping(data, sta.htpriv.ht_cap.as_mut_ptr(), HT_CAP_LEN);
            }
        } else {
            sta.flags &= !WLAN_STA_HT;
            return;
        }

        if adapter.registrypriv.ht_enable == _TRUE
            && is_supported_ht(adapter.registrypriv.wireless_mode) == _TRUE
        {
            sta.htpriv.ht_option = _TRUE;
            sta.qos_option = _TRUE as u32;
        } else {
            sta.htpriv.ht_option = _FALSE;
            sta.qos_option = _FALSE as u32;
            return;
        }

        if adapter.registrypriv.ampu_enable == 1 {
            sta.htpriv.ampdu_enable = _TRUE;
        }

        let data = sta.htpriv.ht_cap;
        let ap_ampdu = pmlmeinfo.ht_caps.ampdu_para;
        let max_ampdu_len = if (ap_ampdu & 0x3) > (data[2] & 0x3) {
            data[2] & 0x3
        } else {
            ap_ampdu & 0x3
        };
        let min_mpdu_spacing = if (ap_ampdu & 0x1c) > (data[2] & 0x1c) {
            ap_ampdu & 0x1c
        } else {
            data[2] & 0x1c
        };
        sta.htpriv.rx_ampdu_min_spacing = max_ampdu_len | min_mpdu_spacing;

        let cap_info = ht_cap_info(&sta.htpriv.ht_cap);
        if phtpriv.sgi_20m == _TRUE && (cap_info & cpu_to_le16(IEEE80211_HT_CAP_SGI_20)) != 0 {
            sta.htpriv.sgi_20m = _TRUE;
        }

        if (cap_info & cpu_to_le16(IEEE80211_HT_CAP_SUP_WIDTH)) != 0 {
            if adapter.mlmeextpriv.cur_bwmode >= CHANNEL_WIDTH_40 {
                sta.bw_mode = CHANNEL_WIDTH_40;
            }
            sta.htpriv.ch_offset = adapter.mlmeextpriv.cur_ch_offset;
        }
    }
}
