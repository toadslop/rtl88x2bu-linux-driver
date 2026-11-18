// W4-109 — `rtw_phydm_rfe_ctrl_gpio` + `rtw_phydm_fw_iqk` from hal/hal_dm.c.

use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;

const TRUE: U8 = 1;
const FALSE: U8 = 0;

const HAL_STATUS_SUCCESS: i32 = 0;
const HAL_STATUS_FAILURE: i32 = 1;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_dm_phydm_adapter(dm: *mut DmStruct) -> *mut c_void;
    #[cfg(config_rtw_halmac)]
    fn rtw_halmac_rfe_ctrl_cfg(d: *mut c_void, gpio: U8) -> i32;
    #[cfg(config_rtw_halmac)]
    fn rtw_halmac_iqk(d: *mut c_void, clear: U8, segment: U8) -> i32;
}

#[no_mangle]
pub extern "C" fn rtw_phydm_rfe_ctrl_gpio(adapter: *mut c_void, gpio_num: U8) -> U8 {
    #[cfg(config_rtw_halmac)]
    {
        // SAFETY: dvobj pointer comes from the same adapter path as the C implementation.
        let dvobj = unsafe { rtw_rust_hal_dm_adapter_to_dvobj(adapter) };
        if unsafe { rtw_halmac_rfe_ctrl_cfg(dvobj, gpio_num) } != 0 {
            return TRUE;
        }
        return FALSE;
    }
    #[cfg(not(config_rtw_halmac))]
    {
        let _ = (adapter, gpio_num);
        FALSE
    }
}

#[no_mangle]
pub extern "C" fn rtw_phydm_fw_iqk(p_dm_odm: *mut DmStruct, clear: U8, segment: U8) -> i32 {
    #[cfg(config_rtw_halmac)]
    {
        let adapter = unsafe { rtw_rust_hal_dm_phydm_adapter(p_dm_odm) };
        let dvobj = unsafe { rtw_rust_hal_dm_adapter_to_dvobj(adapter) };
        if unsafe { rtw_halmac_iqk(dvobj, clear, segment) } == 0 {
            return HAL_STATUS_SUCCESS;
        }
        return HAL_STATUS_FAILURE;
    }
    #[cfg(not(config_rtw_halmac))]
    {
        let _ = (p_dm_odm, clear, segment);
        HAL_STATUS_FAILURE
    }
}