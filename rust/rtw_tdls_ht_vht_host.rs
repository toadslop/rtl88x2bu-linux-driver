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

#[cfg(host_tdls_vht_test)]
pub mod vht {
    use std::ffi::c_void;

    const _TRUE: u8 = 1;
    const _FALSE: u8 = 0;
    const WLAN_STA_VHT: i32 = 1 << 14;
    const WIRELESS_11AC: u8 = 1 << 5;
    const CHANNEL_WIDTH_80: u8 = 2;
    const LDPC_VHT_ENABLE_TX: u8 = 1 << 0;
    const LDPC_VHT_CAP_TX: u8 = 1 << 1;
    const STBC_VHT_ENABLE_TX: u8 = 1 << 0;
    const STBC_VHT_CAP_TX: u8 = 1 << 1;
    const BEAMFORMING_VHT_BEAMFORMER_ENABLE: u16 = 1 << 0;
    const BEAMFORMING_VHT_BEAMFORMEE_ENABLE: u16 = 1 << 1;

    static mut G_HAL_TX_NSS: u8 = 2;

    #[repr(C)]
    pub struct VhtPriv {
        pub vht_cap: [u8; 12],
        pub vht_mcs_map: [u8; 2],
        pub vht_op_mode_notify: u8,
        pub vht_option: u8,
        pub ldpc_cap: u8,
        pub stbc_cap: u8,
        pub sgi_80m: u8,
        pub ampdu_len: u8,
        pub vht_highest_rate: u8,
        _pad_before_beamform: u8,
        pub beamform_cap: u16,
    }

    #[repr(C)]
    pub struct RegistryPriv {
        pub vht_enable: u8,
        pub wireless_mode: u8,
    }

    #[repr(C)]
    pub struct MlmeExtPriv {
        pub cur_bwmode: u8,
    }

    #[repr(C)]
    pub struct MlmePriv {
        pub vhtpriv: VhtPriv,
    }

    #[repr(C)]
    pub struct CountryChplan {
        pub en_11ac: u8,
    }

    #[repr(C)]
    pub struct RfCtl {
        pub country_ent: *mut c_void,
    }

    fn country_chplan_en_11ac(ent: *mut c_void) -> u8 {
        if ent.is_null() {
            return _TRUE;
        }
        unsafe { (*(ent as *const CountryChplan)).en_11ac }
    }

    fn vht_option_allowed(adapter: &Adapter) -> u8 {
        let rfctl = &adapter.rfctl;
        if adapter.registrypriv.vht_enable == 0
            || is_supported_vht(adapter.registrypriv.wireless_mode) == _FALSE
        {
            return _FALSE;
        }
        if rfctl.country_ent.is_null() || country_chplan_en_11ac(rfctl.country_ent) != 0 {
            _TRUE
        } else {
            _FALSE
        }
    }

    #[repr(C)]
    pub struct RaInfo {
        pub is_vht_enable: u8,
    }

    #[repr(C)]
    pub struct BfInfo {
        pub vht_beamform_cap: u16,
    }

    #[repr(C)]
    pub struct StaCmn {
        pub bw_mode: u8,
        pub ra_info: RaInfo,
        pub bf_info: BfInfo,
    }

    #[repr(C)]
    pub struct StaInfo {
        pub flags: i32,
        pub vhtpriv: VhtPriv,
        pub cmn: StaCmn,
    }

    #[repr(C)]
    pub struct Adapter {
        pub registrypriv: RegistryPriv,
        pub mlmepriv: MlmePriv,
        pub mlmeextpriv: MlmeExtPriv,
        _pad_before_rfctl: [u8; 5],
        pub rfctl: RfCtl,
    }

    fn le_bits(p: &[u8], off: u32, len: u32) -> u32 {
        let mut v = 0u32;
        for i in 0..len {
            let b = (off + i) / 8;
            let bit = (off + i) % 8;
            if (p[b as usize] & (1u8 << bit)) != 0 {
                v |= 1u32 << i;
            }
        }
        let mask = if len >= 32 {
            0xffffffffu32
        } else {
            0xffffffffu32 >> (32 - len)
        };
        v & mask
    }

    fn le1(p: &[u8], o: u32, l: u32) -> u8 {
        le_bits(p, o, l) as u8
    }

    fn le2(p: &[u8], o: u32, l: u32) -> u16 {
        le_bits(p, o, l) as u16
    }

    fn is_supported_vht(mode: u8) -> u8 {
        if mode & WIRELESS_11AC != 0 {
            _TRUE
        } else {
            _FALSE
        }
    }

    fn hal_tx_nss(_a: &Adapter) -> u8 {
        unsafe { G_HAL_TX_NSS }
    }

    extern "C" {
        fn rtw_get_vht_highest_rate(pvht_mcs_map: *mut u8) -> u8;
    }

    fn vht_nss_to_mcsmap(nss: u8, target_mcs_map: &mut [u8; 2], cur_mcs_map: &[u8; 2]) {
        for i in 0..2 {
            target_mcs_map[i] = 0;
            for j in (0..8).step_by(2) {
                let cur_rate = (cur_mcs_map[i] >> j) & 3;
                let target_rate = if cur_rate == 3 || nss <= ((j / 2) + i * 4) as u8 {
                    3
                } else {
                    cur_rate
                };
                target_mcs_map[i] |= target_rate << j;
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_tdls_process_vht_cap(
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
        let vht_option = vht_option_allowed(adapter);

        sta.vhtpriv = VhtPriv {
            vht_cap: [0; 12],
            vht_mcs_map: [0; 2],
            vht_op_mode_notify: 0,
            vht_option: 0,
            ldpc_cap: 0,
            stbc_cap: 0,
            sgi_80m: 0,
            ampdu_len: 0,
            vht_highest_rate: 0,
            _pad_before_beamform: 0,
            beamform_cap: 0,
        };

        if !data.is_null() && length == 12 {
            sta.flags |= WLAN_STA_VHT;
            unsafe {
                core::ptr::copy_nonoverlapping(data, sta.vhtpriv.vht_cap.as_mut_ptr(), 12);
            }
            sta.vhtpriv.vht_op_mode_notify = CHANNEL_WIDTH_80;
        } else {
            sta.flags &= !WLAN_STA_VHT;
            return;
        }

        if (sta.flags & WLAN_STA_VHT) != 0 {
            sta.vhtpriv.vht_option = vht_option;
            if vht_option != 0 {
                sta.cmn.ra_info.is_vht_enable = _TRUE;
            }
        }

        let data = sta.vhtpriv.vht_cap;
        let pvhtpriv = &mut adapter.mlmepriv.vhtpriv;
        let mut cur_ldpc_cap = 0u8;
        if (pvhtpriv.ldpc_cap & LDPC_VHT_ENABLE_TX) != 0 && le1(&data, 4, 1) != 0 {
            cur_ldpc_cap |= LDPC_VHT_ENABLE_TX | LDPC_VHT_CAP_TX;
        }
        sta.vhtpriv.ldpc_cap = cur_ldpc_cap;

        sta.vhtpriv.sgi_80m = if le1(&data, 5, 1) != 0 && pvhtpriv.sgi_80m != 0 {
            _TRUE
        } else {
            _FALSE
        };

        let mut cur_stbc_cap = 0u8;
        if (pvhtpriv.stbc_cap & STBC_VHT_ENABLE_TX) != 0 && le1(&data[1..], 0, 3) != 0 {
            cur_stbc_cap |= STBC_VHT_ENABLE_TX | STBC_VHT_CAP_TX;
        }
        sta.vhtpriv.stbc_cap = cur_stbc_cap;

        let mut cur_beamform_cap = 0u16;
        if (pvhtpriv.beamform_cap & BEAMFORMING_VHT_BEAMFORMER_ENABLE) != 0
            && le1(&data, 12, 1) != 0
        {
            cur_beamform_cap |= BEAMFORMING_VHT_BEAMFORMEE_ENABLE;
        }
        if (pvhtpriv.beamform_cap & BEAMFORMING_VHT_BEAMFORMEE_ENABLE) != 0
            && le1(&data, 11, 1) != 0
        {
            cur_beamform_cap |= BEAMFORMING_VHT_BEAMFORMER_ENABLE;
        }
        sta.vhtpriv.beamform_cap = cur_beamform_cap;
        sta.cmn.bf_info.vht_beamform_cap = cur_beamform_cap;

        sta.vhtpriv.ampdu_len = le2(&data[2..], 7, 3) as u8;
        let cur_mcs = [data[4], data[5]];
        let mut map = [0u8; 2];
        vht_nss_to_mcsmap(hal_tx_nss(adapter), &mut map, &cur_mcs);
        sta.vhtpriv.vht_mcs_map = map;
        sta.vhtpriv.vht_highest_rate =
            unsafe { rtw_get_vht_highest_rate(sta.vhtpriv.vht_mcs_map.as_mut_ptr()) };
    }
}
