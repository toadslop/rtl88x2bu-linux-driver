// W4-113 — `init_phydm_info`, `rtw_phydm_config_trx_path`, `rtw_phydm_init` from hal/hal_dm.c.

use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;
type U32 = u32;

const ODM_CMNINFO_FW_VER: U32 = 7;
const ODM_CMNINFO_FW_SUB_VER: U32 = 8;

const SUCCESS: U8 = 1;
const FAIL: U8 = 0;
const FALSE: U8 = 0;

#[repr(i32)]
enum PhydmInitResult {
    PhyDmInitSuccess = 0,
    PhyDmInitFailBbRfRegInvalid = 1,
}

unsafe extern "C" {
    fn odm_cmn_info_init(dm: *mut DmStruct, cmn_info: U32, value: u64);
    fn odm_dm_init(dm: *mut DmStruct) -> PhydmInitResult;
    fn phydm_api_trx_mode(
        dm_void: *mut DmStruct,
        tx_path: U32,
        rx_path: U32,
        tx_path_ctrl: U32,
    ) -> U8;
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn rtw_rust_hal_dm_phydm_init_txpath(adapter: *mut c_void) -> U32;
    fn rtw_rust_hal_dm_phydm_init_rxpath(adapter: *mut c_void) -> U32;
    fn rtw_rust_hal_dm_phydm_init_txpath_1ss(adapter: *mut c_void) -> U32;
    fn rtw_rust_hal_dm_phydm_init_fw_ver(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_phydm_init_fw_sub_ver(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_phydm_init_set_result(adapter: *mut c_void, result: U8);
    fn rtw_rust_hal_dm_phydm_init_err_trx_config(
        func: *const u8,
        txpath: U32,
        rxpath: U32,
        txpath_1ss: U32,
    );
    fn rtw_rust_hal_dm_phydm_init_warn_on(cond: i32);
    #[cfg(config_customer01_smart_antenna)]
    fn phydm_pathb_q_matrix_rotate_en(dm: *mut DmStruct);
    #[cfg(CONFIG_RTL8814B)]
    fn config_phydm_trx_mode_8814b(
        dm: *mut DmStruct,
        txpath: U32,
        rxpath: U32,
    ) -> U8;
    #[cfg(CONFIG_RTL8814B)]
    fn config_phydm_trx_mode_ext_8814b(
        dm: *mut DmStruct,
        txpath: U32,
        rxpath: U32,
        t0: U32,
        t1: U32,
        t2: U32,
    ) -> U8;
    #[cfg(any(CONFIG_RTL8812A, CONFIG_RTL8192E))]
    fn phydm_tx_2path(dm: *mut DmStruct);
    #[cfg(any(CONFIG_RTL8812A, CONFIG_RTL8192E))]
    fn rtw_rust_hal_dm_phydm_init_txpath_num_nss0(adapter: *mut c_void) -> U8;
}

const BB_PATH_ABCD: U32 = 0x0000_000f;

unsafe fn init_phydm_info(adapter: *mut c_void) {
    let phydm = unsafe { rtw_rust_hal_dm_adapter_to_phydm(adapter) };
    let fw_ver = unsafe { rtw_rust_hal_dm_phydm_init_fw_ver(adapter) } as u64;
    let fw_sub = unsafe { rtw_rust_hal_dm_phydm_init_fw_sub_ver(adapter) } as u64;
    unsafe {
        odm_cmn_info_init(phydm, ODM_CMNINFO_FW_VER, fw_ver);
        odm_cmn_info_init(phydm, ODM_CMNINFO_FW_SUB_VER, fw_sub);
    }
}

#[cfg(any(CONFIG_RTL8192F, CONFIG_RTL8822B, CONFIG_RTL8822C))]
unsafe fn config_trx_path_8822_family(adapter: *mut c_void, rst: &mut U8) {
    let txpath = unsafe { rtw_rust_hal_dm_phydm_init_txpath(adapter) };
    let rxpath = unsafe { rtw_rust_hal_dm_phydm_init_rxpath(adapter) };
    let txpath_1ss = unsafe { rtw_rust_hal_dm_phydm_init_txpath_1ss(adapter) };
    let phydm = unsafe { rtw_rust_hal_dm_adapter_to_phydm(adapter) };
    if unsafe { phydm_api_trx_mode(phydm, txpath, rxpath, txpath_1ss) } == FALSE {
        unsafe {
            rtw_rust_hal_dm_phydm_init_err_trx_config(
                b"rtw_phydm_config_trx_path\0".as_ptr(),
                txpath,
                rxpath,
                txpath_1ss,
            );
            rtw_rust_hal_dm_phydm_init_warn_on(1);
        }
        *rst = FAIL;
    }
}

#[cfg(CONFIG_RTL8814B)]
unsafe fn config_trx_path_8814b(adapter: *mut c_void, rst: &mut U8) {
    let txpath = unsafe { rtw_rust_hal_dm_phydm_init_txpath(adapter) };
    let rxpath = unsafe { rtw_rust_hal_dm_phydm_init_rxpath(adapter) };
    let phydm = unsafe { rtw_rust_hal_dm_adapter_to_phydm(adapter) };
    let ok;
    if txpath == BB_PATH_ABCD && rxpath == BB_PATH_ABCD {
        ok = unsafe { config_phydm_trx_mode_8814b(phydm, txpath, rxpath) };
    } else {
        ok = unsafe {
            config_phydm_trx_mode_ext_8814b(phydm, txpath, rxpath, txpath, txpath, txpath)
        };
    }
    if ok == FALSE {
        unsafe {
            rtw_rust_hal_dm_phydm_init_err_trx_config(
                b"rtw_phydm_config_trx_path\0".as_ptr(),
                txpath,
                rxpath,
                0,
            );
            rtw_rust_hal_dm_phydm_init_warn_on(1);
        }
        *rst = FAIL;
    }
}

#[cfg(any(CONFIG_RTL8812A, CONFIG_RTL8192E))]
unsafe fn config_trx_path_legacy(adapter: *mut c_void) {
    if unsafe { rtw_rust_hal_dm_phydm_init_txpath_num_nss0(adapter) } == 2 {
        let phydm = unsafe { rtw_rust_hal_dm_adapter_to_phydm(adapter) };
        unsafe {
            phydm_tx_2path(phydm);
        }
    }
}

unsafe fn rtw_phydm_config_trx_path(adapter: *mut c_void) -> U8 {
    let mut rst = SUCCESS;
    #[cfg(any(CONFIG_RTL8192F, CONFIG_RTL8822B, CONFIG_RTL8822C))]
    unsafe {
        config_trx_path_8822_family(adapter, &mut rst);
    }
    #[cfg(CONFIG_RTL8814B)]
    unsafe {
        config_trx_path_8814b(adapter, &mut rst);
    }
    #[cfg(any(CONFIG_RTL8812A, CONFIG_RTL8192E))]
    unsafe {
        config_trx_path_legacy(adapter);
    }
    rst
}

#[no_mangle]
pub extern "C" fn rtw_phydm_init(adapter: *mut c_void) {
    let phydm = unsafe { rtw_rust_hal_dm_adapter_to_phydm(adapter) };
    unsafe {
        let _ = rtw_phydm_config_trx_path(adapter);
        init_phydm_info(adapter);
        let result = odm_dm_init(phydm);
        rtw_rust_hal_dm_phydm_init_set_result(adapter, result as u8);
    }
    #[cfg(config_customer01_smart_antenna)]
    unsafe {
        phydm_pathb_q_matrix_rotate_en(phydm);
    }
}