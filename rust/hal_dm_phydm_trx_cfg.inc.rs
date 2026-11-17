// W4-108 — `rtw_phydm_trx_cfg` from hal/hal_dm.c (CONFIG_CTRL_TXSS_BY_TP).

use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;
type U32 = u32;

const _FALSE: U8 = 0;

// enum bb_path — matches include/cmn_info/rtw_sta_info.h.
const BB_PATH_A: U32 = 0x0000_0001;
const BB_PATH_AB: U32 = 0x0000_0003;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn rtw_hal_get_trx_path(
        dvobj: *mut c_void,
        chip_type: *mut c_void,
        txpath: *mut U32,
        rxpath: *mut U32,
    );
    fn phydm_api_trx_mode(
        dm_void: *mut c_void,
        tx_path: U32,
        rx_path: U32,
        tx_path_ctrl: U32,
    ) -> U8;
    fn rtw_rust_hal_dm_err_phydm_trx_cfg();
}

#[cfg(any(config_ctrl_txss_by_tp, host_hal_dm_phydm_trx_cfg_test))]
#[no_mangle]
pub extern "C" fn rtw_phydm_trx_cfg(adapter: *mut c_void, tx_1ss: U8) {
    unsafe {
        let mut txpath = BB_PATH_AB;
        let mut rxpath = BB_PATH_AB;
        let txpath_1ss = BB_PATH_A;
        let dvobj = rtw_rust_hal_dm_adapter_to_dvobj(adapter);

        rtw_hal_get_trx_path(dvobj, core::ptr::null_mut(), &mut txpath, &mut rxpath);
        if tx_1ss != 0 {
            txpath = BB_PATH_A;
        }
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        if phydm_api_trx_mode(phydm.cast(), txpath, rxpath, txpath_1ss) == _FALSE {
            rtw_rust_hal_dm_err_phydm_trx_cfg();
        }
    }
}