// W4-111 — `rtw_dyn_soml_*` from hal/hal_dm.c.

#[cfg(any(config_dynamic_soml, host_hal_dm_dyn_soml_test))]
use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;
type U32 = u32;

#[cfg(any(config_dynamic_soml, host_hal_dm_dyn_soml_test))]
unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn phydm_soml_bytes_acq(dm_void: *mut c_void, rate_id: U8, length: U32);
    fn phydm_adaptive_soml_para_set(
        dm_void: *mut c_void,
        train_num: U8,
        intvl: U8,
        period: U8,
        delay: U8,
    );
    fn rtw_rust_hal_dm_dyn_soml_set_hwreg(adapter: *mut c_void);
    fn rtw_rust_hal_dm_registry_dyn_soml_en(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_registry_dyn_soml_train_num(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_registry_dyn_soml_interval(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_registry_dyn_soml_period(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_registry_dyn_soml_delay(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_log_dyn_soml_config();
    fn rtw_rust_hal_dm_log_dyn_soml_para_set();
    fn rtw_rust_hal_dm_log_dyn_soml_en(u8: U8);
    fn rtw_rust_hal_dm_log_dyn_soml_en2_params(train: U8, intvl: U8, period: U8, delay: U8);
    fn rtw_rust_hal_dm_err_dyn_soml_bad_en(adapter: *mut c_void, en: U8);
}

#[cfg(any(config_dynamic_soml, host_hal_dm_dyn_soml_test))]
#[no_mangle]
pub extern "C" fn rtw_dyn_soml_byte_update(adapter: *mut c_void, data_rate: U8, size: U32) {
    unsafe {
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        phydm_soml_bytes_acq(phydm.cast(), data_rate, size);
    }
}

#[cfg(any(config_dynamic_soml, host_hal_dm_dyn_soml_test))]
#[no_mangle]
pub extern "C" fn rtw_dyn_soml_para_set(
    adapter: *mut c_void,
    train_num: U8,
    intvl: U8,
    period: U8,
    delay: U8,
) {
    unsafe {
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        phydm_adaptive_soml_para_set(phydm.cast(), train_num, intvl, period, delay);
        rtw_rust_hal_dm_log_dyn_soml_para_set();
    }
}

#[cfg(any(config_dynamic_soml, host_hal_dm_dyn_soml_test))]
#[no_mangle]
pub extern "C" fn rtw_dyn_soml_config(adapter: *mut c_void) {
    unsafe {
        let en = rtw_rust_hal_dm_registry_dyn_soml_en(adapter);

        rtw_rust_hal_dm_log_dyn_soml_config();
        if en == 1 {
            rtw_rust_hal_dm_dyn_soml_set_hwreg(adapter);
            rtw_rust_hal_dm_log_dyn_soml_en(1);
        } else if en == 2 {
            rtw_dyn_soml_para_set(
                adapter,
                rtw_rust_hal_dm_registry_dyn_soml_train_num(adapter),
                rtw_rust_hal_dm_registry_dyn_soml_interval(adapter),
                rtw_rust_hal_dm_registry_dyn_soml_period(adapter),
                rtw_rust_hal_dm_registry_dyn_soml_delay(adapter),
            );
            rtw_rust_hal_dm_log_dyn_soml_en(2);
            rtw_rust_hal_dm_log_dyn_soml_en2_params(
                rtw_rust_hal_dm_registry_dyn_soml_train_num(adapter),
                rtw_rust_hal_dm_registry_dyn_soml_interval(adapter),
                rtw_rust_hal_dm_registry_dyn_soml_period(adapter),
                rtw_rust_hal_dm_registry_dyn_soml_delay(adapter),
            );
        } else if en == 0 {
            rtw_rust_hal_dm_log_dyn_soml_en(0);
        } else {
            rtw_rust_hal_dm_err_dyn_soml_bad_en(adapter, en);
        }
    }
}