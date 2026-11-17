// W4-95 — `GetHalODMVar` from hal/hal_dm.c

use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;
type U32 = u32;

const HAL_ODM_INITIAL_GAIN: i32 = 4;
const PHYDM_INFO_CURR_IGI: U32 = 20;

#[cfg(CONFIG_ANTENNA_DIVERSITY)]
const HAL_ODM_ANTDIV_SELECT: i32 = 7;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    #[cfg(not(host_hal_dm_get_odm_var_test))]
    fn rtw_phydm_get_cur_igi(adapter: *mut c_void) -> U8;
    #[cfg(host_hal_dm_get_odm_var_test)]
    fn phydm_cmn_info_query(dm: *mut c_void, info_type: U32) -> U32;
    #[cfg(CONFIG_ANTENNA_DIVERSITY)]
    fn rtw_rust_hal_dm_fat_rx_idle_ant(phydm: *mut DmStruct) -> U8;
}

#[no_mangle]
pub extern "C" fn GetHalODMVar(
    adapter: *mut c_void,
    e_variable: i32,
    p_value1: *mut c_void,
    p_value2: *mut c_void,
) {
    let _ = p_value2;
    match e_variable {
        #[cfg(CONFIG_ANTENNA_DIVERSITY)]
        x if x == HAL_ODM_ANTDIV_SELECT => unsafe {
            let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
            *(p_value1 as *mut U8) = rtw_rust_hal_dm_fat_rx_idle_ant(phydm);
        },
        x if x == HAL_ODM_INITIAL_GAIN => unsafe {
            #[cfg(host_hal_dm_get_odm_var_test)]
            {
                let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
                *(p_value1 as *mut U8) = phydm_cmn_info_query(
                    phydm as *mut c_void,
                    PHYDM_INFO_CURR_IGI,
                ) as U8;
            }
            #[cfg(not(host_hal_dm_get_odm_var_test))]
            {
                *(p_value1 as *mut U8) = rtw_phydm_get_cur_igi(adapter);
            }
        }
        _ => {}
    }
}