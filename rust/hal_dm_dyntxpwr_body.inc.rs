const RTL8822B: u8 = 10;

#[no_mangle]
pub unsafe extern "C" fn rtw_phydm_fill_desc_dpt(
    dm: *mut c_void,
    desc: *mut u8,
    dpt_lv: u8,
) {
    if dm.is_null() {
        return;
    }
    unsafe {
        let p_dm = dm as *mut DmStruct;
        let adapter = rtw_rust_hal_dm_phydm_adapter(p_dm);
        if adapter.is_null() {
            return;
        }
        match rtw_rust_hal_dm_get_chip_type(adapter) {
            RTL8822B => rtw_rust_hal_dm_set_tx_desc_txpwr_ofset_8822b(desc, dpt_lv),
            _ => rtw_rust_hal_dm_log_dyntxpwr_unsupported(),
        }
    }
}

#[no_mangle]
pub unsafe extern "C" fn rtw_phydm_set_dyntxpwr(
    adapter: *mut c_void,
    desc: *mut u8,
    mac_id: u8,
) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let dm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        if dm.is_null() {
            return;
        }
        rtw_rust_hal_dm_odm_set_dyntxpwr(dm as *mut c_void, desc, mac_id);
    }
}