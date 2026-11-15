// W4-92 — `rtw_phydm_get_cur_igi` and `rtw_phydm_is_iqk_in_progress` from `hal/hal_dm.c`.

use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;
type U32 = u32;

const PHYDM_INFO_CURR_IGI: U32 = 20;
const RT_IQK_SPINLOCK: U32 = 38;
const _TRUE: U8 = 1;
const _FALSE: U8 = 0;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn phydm_cmn_info_query(dm: *mut DmStruct, info_type: U32) -> U32;
    fn odm_acquire_spin_lock(dm: *mut DmStruct, lock_type: U32);
    fn odm_release_spin_lock(dm: *mut DmStruct, lock_type: U32);
    fn rtw_rust_hal_dm_err_iqk_in_progress();
    fn rtw_rust_hal_dm_phydm_iqk_in_progress_flag(dm: *mut DmStruct) -> U8;
}

#[no_mangle]
pub extern "C" fn rtw_phydm_get_cur_igi(adapter: *mut c_void) -> U8 {
    unsafe {
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        phydm_cmn_info_query(phydm, PHYDM_INFO_CURR_IGI) as U8
    }
}

#[no_mangle]
pub extern "C" fn rtw_phydm_is_iqk_in_progress(adapter: *mut c_void) -> U8 {
    let mut rts = _FALSE;
    unsafe {
        let podmpriv = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        odm_acquire_spin_lock(podmpriv, RT_IQK_SPINLOCK);
        if rtw_rust_hal_dm_phydm_iqk_in_progress_flag(podmpriv) != 0 {
            rtw_rust_hal_dm_err_iqk_in_progress();
            rts = _TRUE;
        }
        odm_release_spin_lock(podmpriv, RT_IQK_SPINLOCK);
    }
    rts
}
