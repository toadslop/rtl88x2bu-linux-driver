// W4-88 — Init_ODM_ComInfo tail from ODM_CMNINFO_CHNL through early init.

use core::ffi::c_void;

type DmStruct = u8;
type U32 = u32;

const ODM_CMNINFO_CHNL: U32 = 58;
const ODM_CMNINFO_SCAN: U32 = 64;
const ODM_CMNINFO_POWER_SAVING: U32 = 65;
const ODM_CMNINFO_TX_TP: U32 = 33;
const ODM_CMNINFO_RX_TP: U32 = 34;
const ODM_CMNINFO_ANT_TEST: U32 = 70;
const ODM_CMNINFO_FCS_MODE: U32 = 73;
const HALRF_CMNINFO_MANUAL_RF_SUPPORTABILITY: u8 = 4;

#[repr(C)]
pub struct RtwHalDmInitOdmTailHooks {
    pub current_channel: *mut u8,
    pub b_scan_in_process: *mut u8,
    pub bpower_saving: *mut u8,
    pub cur_tx_tp: *mut U32,
    pub cur_rx_tp: *mut U32,
    pub antenna_test: *mut u8,
    pub multi_ch_switch_mode: *mut u8,
    pub halrf_ability: *mut U32,
}

unsafe extern "C" {
    fn odm_cmn_info_hook(dm: *mut DmStruct, cmn_info: U32, ptr: *mut u8);
    fn halrf_cmn_info_hook(dm: *mut DmStruct, id: u8, ptr: *mut u8);
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn rtw_rust_hal_dm_init_odm_tail_fill_hooks(
        adapter: *mut c_void,
        out: *mut RtwHalDmInitOdmTailHooks,
    );
    fn rtw_rust_hal_dm_init_odm_tail_optional_hooks(adapter: *mut c_void, phydm: *mut DmStruct);
    fn rtw_rust_hal_dm_init_odm_tail_sta_hooks(phydm: *mut DmStruct);
    fn rtw_rust_hal_dm_init_odm_tail_finish(phydm: *mut DmStruct);
}

#[no_mangle]
pub extern "C" fn rtw_rust_hal_dm_init_odm_cominfo_tail(adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        if phydm.is_null() {
            return;
        }
        let mut hooks = RtwHalDmInitOdmTailHooks {
            current_channel: core::ptr::null_mut(),
            b_scan_in_process: core::ptr::null_mut(),
            bpower_saving: core::ptr::null_mut(),
            cur_tx_tp: core::ptr::null_mut(),
            cur_rx_tp: core::ptr::null_mut(),
            antenna_test: core::ptr::null_mut(),
            multi_ch_switch_mode: core::ptr::null_mut(),
            halrf_ability: core::ptr::null_mut(),
        };
        rtw_rust_hal_dm_init_odm_tail_fill_hooks(adapter, &mut hooks);
        odm_cmn_info_hook(phydm, ODM_CMNINFO_CHNL, hooks.current_channel);
        odm_cmn_info_hook(phydm, ODM_CMNINFO_SCAN, hooks.b_scan_in_process);
        odm_cmn_info_hook(phydm, ODM_CMNINFO_POWER_SAVING, hooks.bpower_saving);
        odm_cmn_info_hook(phydm, ODM_CMNINFO_TX_TP, hooks.cur_tx_tp as *mut u8);
        odm_cmn_info_hook(phydm, ODM_CMNINFO_RX_TP, hooks.cur_rx_tp as *mut u8);
        odm_cmn_info_hook(phydm, ODM_CMNINFO_ANT_TEST, hooks.antenna_test);
        odm_cmn_info_hook(phydm, ODM_CMNINFO_FCS_MODE, hooks.multi_ch_switch_mode);
        halrf_cmn_info_hook(
            phydm,
            HALRF_CMNINFO_MANUAL_RF_SUPPORTABILITY,
            hooks.halrf_ability as *mut u8,
        );
        rtw_rust_hal_dm_init_odm_tail_optional_hooks(adapter, phydm);
        rtw_rust_hal_dm_init_odm_tail_sta_hooks(phydm);
        rtw_rust_hal_dm_init_odm_tail_finish(phydm);
    }
}