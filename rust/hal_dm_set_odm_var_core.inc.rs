// W4-94 — `SetHalODMVar` core cases from `hal/hal_dm.c`.

use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;
type U32 = u32;
type U64 = u64;
type Boolean = u8;

const HAL_ODM_STA_INFO: U32 = 0;
const HAL_ODM_P2P_STATE: U32 = 1;
const HAL_ODM_WIFI_DISPLAY_STATE: U32 = 2;
const HAL_ODM_REGULATION: U32 = 3;
const HAL_ODM_INITIAL_GAIN: U32 = 4;

#[cfg(not(host_hal_dm_set_odm_var_core_test))]
mod set_odm_var_rx {
    include!("hal_dm_set_odm_var_rx.inc.rs");
}
#[cfg(not(host_hal_dm_set_odm_var_core_test))]
use set_odm_var_rx::set_odm_var_rx_cases;

const STA_DM_CTRL_ACTIVE: U8 = 1;

/* Match enum odm_cmninfo in hal/phydm/phydm.h (ODM_CMNINFO_PLATFORM = 0). */
const ODM_CMNINFO_WIFI_DIRECT: U32 = 86;
const ODM_CMNINFO_WIFI_DISPLAY: U32 = 87;

const PHYDM_ADAPINFO_DOMAIN_CODE_2G: U32 = 4;
const PHYDM_ADAPINFO_DOMAIN_CODE_5G: U32 = 5;

const PHYDM_PAUSE: U8 = 1;
const PHYDM_RESUME: U8 = 3;
const PHYDM_PAUSE_LEVEL_0: U8 = 0;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn rtw_rust_hal_dm_sta_mac_id(psta: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_sta_set_dm_ctrl(psta: *mut c_void, dm_ctrl: U8);
    fn rtw_rust_hal_dm_sta_cmn_ptr(psta: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_dm_log_set_sta(mac_id: U8);
    fn rtw_rust_hal_dm_log_clean_sta(mac_id: U8);
    fn phydm_cmn_sta_info_hook(dm: *mut DmStruct, mac_id: U8, sta: *mut c_void);
    fn odm_cmn_info_update(dm: *mut DmStruct, cmn_info: U32, value: U64);
    fn phydm_adaptivity_info_update(dm_void: *mut c_void, cmn_info: U32, value: U64);
    fn odm_pause_dig(dm_void: *mut c_void, pause_type: U8, pause_level: U8, igi: U8);
    fn rtw_rust_hal_dm_set_odm_var_rest(
        adapter: *mut c_void,
        e_variable: U32,
        p_value1: *mut c_void,
        b_set: Boolean,
    );
}

#[no_mangle]
pub extern "C" fn SetHalODMVar(
    adapter: *mut c_void,
    e_variable: U32,
    p_value1: *mut c_void,
    b_set: Boolean,
) {
    // SAFETY: Kernel callers pass a valid `_adapter *`; PHYDM helpers use the
    // derived `dm_struct *`. For STA_INFO and INITIAL_GAIN, `p_value1` must point
    // at a `sta_info` or `u8` respectively (null returns early — safer than C).
    unsafe {
        let podmpriv = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        match e_variable {
            HAL_ODM_STA_INFO => {
                if p_value1.is_null() {
                    return;
                }
                let psta = p_value1;
                let mac_id = rtw_rust_hal_dm_sta_mac_id(psta);
                if b_set != 0 {
                    rtw_rust_hal_dm_log_set_sta(mac_id);
                    rtw_rust_hal_dm_sta_set_dm_ctrl(psta, STA_DM_CTRL_ACTIVE);
                    phydm_cmn_sta_info_hook(
                        podmpriv,
                        mac_id,
                        rtw_rust_hal_dm_sta_cmn_ptr(psta),
                    );
                } else {
                    rtw_rust_hal_dm_log_clean_sta(mac_id);
                    rtw_rust_hal_dm_sta_set_dm_ctrl(psta, 0);
                    phydm_cmn_sta_info_hook(podmpriv, mac_id, core::ptr::null_mut());
                }
            }
            HAL_ODM_P2P_STATE => {
                odm_cmn_info_update(podmpriv, ODM_CMNINFO_WIFI_DIRECT, b_set as U64);
            }
            HAL_ODM_WIFI_DISPLAY_STATE => {
                odm_cmn_info_update(podmpriv, ODM_CMNINFO_WIFI_DISPLAY, b_set as U64);
            }
            HAL_ODM_REGULATION => {
                phydm_adaptivity_info_update(
                    podmpriv as *mut c_void,
                    PHYDM_ADAPINFO_DOMAIN_CODE_2G,
                    0,
                );
                phydm_adaptivity_info_update(
                    podmpriv as *mut c_void,
                    PHYDM_ADAPINFO_DOMAIN_CODE_5G,
                    0,
                );
            }
            HAL_ODM_INITIAL_GAIN => {
                if p_value1.is_null() {
                    return;
                }
                let rx_gain = *(p_value1 as *const U8);
                if rx_gain == 0xff {
                    odm_pause_dig(
                        podmpriv as *mut c_void,
                        PHYDM_RESUME,
                        PHYDM_PAUSE_LEVEL_0,
                        rx_gain,
                    );
                } else {
                    odm_pause_dig(
                        podmpriv as *mut c_void,
                        PHYDM_PAUSE,
                        PHYDM_PAUSE_LEVEL_0,
                        rx_gain,
                    );
                }
            }
            5 | 6 => {
                #[cfg(not(host_hal_dm_set_odm_var_core_test))]
                {
                    set_odm_var_rx_cases(adapter, e_variable, p_value1);
                }
                #[cfg(host_hal_dm_set_odm_var_core_test)]
                {
                    rtw_rust_hal_dm_set_odm_var_rest(adapter, e_variable, p_value1, b_set);
                }
            }
            _ => rtw_rust_hal_dm_set_odm_var_rest(adapter, e_variable, p_value1, b_set),
        }
    }
}