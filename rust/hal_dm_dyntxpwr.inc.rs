// W4-100 — host L2 staticlib wrapper for dynamic TX power desc helpers.

use core::ffi::c_void;

type DmStruct = u8;

unsafe extern "C" {
    fn rtw_rust_hal_dm_phydm_adapter(p_dm: *mut DmStruct) -> *mut c_void;
    fn rtw_rust_hal_dm_get_chip_type(adapter: *mut c_void) -> u8;
    fn rtw_rust_hal_dm_set_tx_desc_txpwr_ofset_8822b(desc: *mut u8, value: u8);
    fn rtw_rust_hal_dm_log_dyntxpwr_unsupported();
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn rtw_rust_hal_dm_odm_set_dyntxpwr(dm: *mut c_void, desc: *mut u8, mac_id: u8);
}

include!("hal_dm_dyntxpwr_body.inc.rs");