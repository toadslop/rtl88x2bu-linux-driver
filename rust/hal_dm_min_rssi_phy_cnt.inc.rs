use core::ffi::c_void;

type DmStruct = u8;
type MacidCtl = u8;
type StaInfo = u8;
type S8 = i8;
type U8 = u8;
type U32 = u32;
type Bool = u8;

const MACID_NUM_SW_LIMIT: i32 = 32;
const PHYDM_INFO_FA_OFDM: U32 = 0;
const PHYDM_INFO_FA_CCK: U32 = 1;
const PHYDM_INFO_FA_TOTAL: U32 = 2;
const PHYDM_INFO_CCA_OFDM: U32 = 3;
const PHYDM_INFO_CCA_CCK: U32 = 4;
const PHYDM_INFO_CCA_ALL: U32 = 5;
const PHYDM_INFO_CRC32_OK_VHT: U32 = 6;
const PHYDM_INFO_CRC32_OK_HT: U32 = 7;
const PHYDM_INFO_CRC32_OK_LEGACY: U32 = 8;
const PHYDM_INFO_CRC32_OK_CCK: U32 = 9;
const PHYDM_INFO_CRC32_ERROR_VHT: U32 = 10;
const PHYDM_INFO_CRC32_ERROR_HT: U32 = 11;
const PHYDM_INFO_CRC32_ERROR_LEGACY: U32 = 12;
const PHYDM_INFO_CRC32_ERROR_CCK: U32 = 13;
const PHYDM_INFO_EDCCA_FLAG: U32 = 14;
const PHYDM_INFO_RSSI_MIN: U32 = 21;

const FA_OFDM: U32 = 0;
const FA_CCK: U32 = 1;
const FA_TOTAL: U32 = 2;
const CCA_OFDM: U32 = 3;
const CCA_CCK: U32 = 4;
const CCA_ALL: U32 = 5;
const CRC32_OK_VHT: U32 = 6;
const CRC32_OK_HT: U32 = 7;
const CRC32_OK_LEGACY: U32 = 8;
const CRC32_OK_CCK: U32 = 9;
const CRC32_ERROR_VHT: U32 = 10;
const CRC32_ERROR_HT: U32 = 11;
const CRC32_ERROR_LEGACY: U32 = 12;
const CRC32_ERROR_CCK: U32 = 13;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn phydm_cmn_info_query(dm: *mut DmStruct, info_type: U32) -> U32;
    fn rtw_rust_hal_dm_macidctl(adapter: *mut c_void) -> *mut MacidCtl;
    fn rtw_rust_hal_dm_macid_sta(ctl: *mut MacidCtl, i: i32) -> *mut StaInfo;
    fn rtw_rust_hal_dm_macid_h2c_opmode(ctl: *mut MacidCtl, i: i32) -> U8;
    fn rtw_rust_hal_dm_sta_rssi(sta: *mut StaInfo) -> S8;
    fn rtw_rust_hal_dm_is_broadcast_sta(sta: *mut StaInfo) -> Bool;
}

#[no_mangle]
pub extern "C" fn rtw_dm_get_min_rssi(adapter: *mut c_void) -> S8 {
    let mut min_rssi: S8 = 127;
    unsafe {
        let macid_ctl = rtw_rust_hal_dm_macidctl(adapter);
        for i in 0..MACID_NUM_SW_LIMIT {
            let sta = rtw_rust_hal_dm_macid_sta(macid_ctl, i);
            if sta.is_null()
                || rtw_rust_hal_dm_macid_h2c_opmode(macid_ctl, i) == 0
                || rtw_rust_hal_dm_is_broadcast_sta(sta) != 0
            {
                continue;
            }
            let rssi = rtw_rust_hal_dm_sta_rssi(sta);
            if rssi >= 0 && min_rssi > rssi {
                min_rssi = rssi;
            }
        }
    }
    if min_rssi == 127 {
        0
    } else {
        min_rssi
    }
}

#[no_mangle]
pub extern "C" fn rtw_phydm_get_min_rssi(adapter: *mut c_void) -> S8 {
    unsafe {
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        phydm_cmn_info_query(phydm, PHYDM_INFO_RSSI_MIN) as S8
    }
}

#[no_mangle]
pub extern "C" fn rtw_phydm_get_edcca_flag(adapter: *mut c_void) -> Bool {
    unsafe {
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        phydm_cmn_info_query(phydm, PHYDM_INFO_EDCCA_FLAG) as Bool
    }
}

#[no_mangle]
pub extern "C" fn rtw_phydm_get_phy_cnt(adapter: *mut c_void, cnt: U32) -> U32 {
    let info = match cnt {
        x if x == FA_OFDM => PHYDM_INFO_FA_OFDM,
        x if x == FA_CCK => PHYDM_INFO_FA_CCK,
        x if x == FA_TOTAL => PHYDM_INFO_FA_TOTAL,
        x if x == CCA_OFDM => PHYDM_INFO_CCA_OFDM,
        x if x == CCA_CCK => PHYDM_INFO_CCA_CCK,
        x if x == CCA_ALL => PHYDM_INFO_CCA_ALL,
        x if x == CRC32_OK_VHT => PHYDM_INFO_CRC32_OK_VHT,
        x if x == CRC32_OK_HT => PHYDM_INFO_CRC32_OK_HT,
        x if x == CRC32_OK_LEGACY => PHYDM_INFO_CRC32_OK_LEGACY,
        x if x == CRC32_OK_CCK => PHYDM_INFO_CRC32_OK_CCK,
        x if x == CRC32_ERROR_VHT => PHYDM_INFO_CRC32_ERROR_VHT,
        x if x == CRC32_ERROR_HT => PHYDM_INFO_CRC32_ERROR_HT,
        x if x == CRC32_ERROR_LEGACY => PHYDM_INFO_CRC32_ERROR_LEGACY,
        x if x == CRC32_ERROR_CCK => PHYDM_INFO_CRC32_ERROR_CCK,
        _ => return 0,
    };
    unsafe {
        phydm_cmn_info_query(rtw_rust_hal_dm_adapter_to_phydm(adapter), info)
    }
}