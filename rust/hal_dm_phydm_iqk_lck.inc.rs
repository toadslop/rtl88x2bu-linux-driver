// W4-98 — `rtw_phydm_iqk_trigger`, `rtw_phydm_iqk_trigger_all`, `rtw_phydm_lck_trigger`.

use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;
type U32 = u32;
type U64 = u64;

const _TRUE: U8 = 1;
const _FALSE: U8 = 0;

// Ordinal values of `HALRF_CMNINFO` in hal/phydm/halrf/halrf.h (keep in sync).
const HALRF_CMNINFO_RFK_FORBIDDEN: U32 = 3;
const HALRF_CMNINFO_IQK_SEGMENT: U32 = 4;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn halrf_cmn_info_set(dm_void: *mut c_void, cmn_info: U32, value: U64);
    fn halrf_segment_iqk_trigger(dm_void: *mut c_void, clear: U8, segment: U8);
    fn halrf_iqk_trigger(dm_void: *mut c_void, is_recovery: U8);
    fn halrf_lck_trigger(dm_void: *mut c_void);
    #[cfg(any(CONFIG_RTL8822C, CONFIG_RTL8814B, CONFIG_RTL8723F))]
    fn halrf_rf_k_connect_trigger(dm_void: *mut c_void, enable: U8, segment: U32);
}

#[inline]
unsafe fn iqk_segment_path(p_dm_odm: *mut DmStruct) {
    let clear = _TRUE;
    let segment = _FALSE;
    let rfk_forbidden = _FALSE;

    unsafe {
        halrf_cmn_info_set(
            p_dm_odm.cast(),
            HALRF_CMNINFO_RFK_FORBIDDEN,
            rfk_forbidden as U64,
        );
        #[cfg(any(CONFIG_RTL8822C, CONFIG_RTL8814B, CONFIG_RTL8723F))]
        {
            const SEGMENT_FREE: U32 = 0;
            halrf_rf_k_connect_trigger(p_dm_odm.cast(), _TRUE, SEGMENT_FREE);
        }
        #[cfg(not(any(CONFIG_RTL8822C, CONFIG_RTL8814B, CONFIG_RTL8723F)))]
        {
            halrf_cmn_info_set(
                p_dm_odm.cast(),
                HALRF_CMNINFO_IQK_SEGMENT,
                segment as U64,
            );
            halrf_segment_iqk_trigger(p_dm_odm.cast(), clear, segment);
        }
    }
}

#[cfg(any(
    CONFIG_RTL8822B,
    CONFIG_RTL8821C,
    CONFIG_RTL8814B,
    CONFIG_RTL8822C,
    CONFIG_RTL8723F
))]
#[no_mangle]
pub extern "C" fn rtw_phydm_iqk_trigger(adapter: *mut c_void) {
    unsafe {
        let p_dm_odm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        iqk_segment_path(p_dm_odm);
    }
}

#[no_mangle]
pub extern "C" fn rtw_phydm_iqk_trigger_all(adapter: *mut c_void) {
    unsafe {
        let p_dm_odm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        #[cfg(any(
            CONFIG_RTL8822B,
            CONFIG_RTL8821C,
            CONFIG_RTL8814B,
            CONFIG_RTL8822C,
            CONFIG_RTL8723F
        ))]
        {
            iqk_segment_path(p_dm_odm);
        }
        #[cfg(not(any(
            CONFIG_RTL8822B,
            CONFIG_RTL8821C,
            CONFIG_RTL8814B,
            CONFIG_RTL8822C,
            CONFIG_RTL8723F
        )))]
        {
            halrf_iqk_trigger(p_dm_odm.cast(), _FALSE);
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_phydm_lck_trigger(adapter: *mut c_void) {
    unsafe {
        let p_dm_odm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        halrf_lck_trigger(p_dm_odm.cast());
    }
}