// W4-103 — `rtw_hal_runtime_trx_path_decision` from hal/hal_dm.c

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;

const BB_PATH_A: U32 = 0x0000_0001;
const BB_PATH_AB: U32 = 0x0000_0003;
const BB_PATH_AUTO: U32 = 0x0000_00ff;
const RF_PATH_MAX: usize = 4;
const SUCCESS: U8 = 1;
const FAIL: U8 = 0;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_hal_get_trx_path(
        dvobj: *mut c_void,
        rf_type: *mut c_void,
        txpath: *mut U32,
        rxpath: *mut U32,
    );
    fn rtw_rust_hal_dm_runtime_trx_nss_default(adapter: *mut c_void);
    fn rtw_rust_hal_dm_runtime_trx_max_tx_cnt(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_runtime_trx_path_div(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_runtime_trx_tx_npath(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_runtime_trx_get_txpath_nss0(adapter: *mut c_void) -> U32;
    fn rtw_rust_hal_dm_runtime_trx_set_txpath_nss0(
        adapter: *mut c_void,
        txpath_1ss: U32,
        txpath_num_nss0: U8,
    );
    fn rtw_rust_hal_dm_runtime_trx_set_paths(
        adapter: *mut c_void,
        txpath: U32,
        rxpath: U32,
    );
    fn rtw_rust_hal_dm_runtime_trx_dump(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_hal_dm_runtime_trx_log_empty_tx();
    fn rtw_rust_hal_dm_runtime_trx_log_empty_rx();
    fn rtw_rust_hal_dm_runtime_trx_log_invalid_max_tx_cnt(max_tx_cnt: U8);
}

fn count_path_bits(txpath_1ss: U32) -> U8 {
    let mut n: U8 = 0;
    for i in 0..RF_PATH_MAX {
        if txpath_1ss & (1u32 << i) != 0 {
            n = n.wrapping_add(1);
        }
    }
    n
}

#[cfg(any(CONFIG_RTL8192F, CONFIG_RTL8822B, CONFIG_RTL8822C))]
unsafe fn apply_8822_family_paths(adapter: *mut c_void, txpath: &mut U32) -> Option<U32> {
    // SAFETY: caller holds a valid adapter matching kernel entry expectations.
    unsafe {
    let max_tx_cnt = rtw_rust_hal_dm_runtime_trx_max_tx_cnt(adapter);
    let txpath_1ss;

    if *txpath != BB_PATH_AB {
        txpath_1ss = *txpath;
    } else {
        match max_tx_cnt {
            2 => {
                // C shims return 0 when CONFIG_RTW_TX_NPATH_EN / CONFIG_RTW_PATH_DIV are unset.
                txpath_1ss = if rtw_rust_hal_dm_runtime_trx_tx_npath(adapter) == 1 {
                    BB_PATH_AB
                } else if rtw_rust_hal_dm_runtime_trx_path_div(adapter) == 1 {
                    BB_PATH_AUTO
                } else {
                    BB_PATH_A
                };
            }
            1 => {
                if rtw_rust_hal_dm_runtime_trx_path_div(adapter) == 1 {
                    *txpath = BB_PATH_AUTO;
                    txpath_1ss = BB_PATH_AUTO;
                } else {
                    *txpath = BB_PATH_A;
                    txpath_1ss = BB_PATH_A;
                }
            }
            _ => {
                rtw_rust_hal_dm_runtime_trx_log_invalid_max_tx_cnt(max_tx_cnt);
                return None;
            }
        }
    }

    if rtw_rust_hal_dm_runtime_trx_get_txpath_nss0(adapter) != txpath_1ss {
        let num = if txpath_1ss == BB_PATH_AUTO {
            1
        } else {
            count_path_bits(txpath_1ss)
        };
        rtw_rust_hal_dm_runtime_trx_set_txpath_nss0(adapter, txpath_1ss, num);
    }
    Some(txpath_1ss)
    }
}

#[no_mangle]
pub extern "C" fn rtw_hal_runtime_trx_path_decision(adapter: *mut c_void) -> U8 {
    if adapter.is_null() {
        return FAIL;
    }

    unsafe {
        let dvobj = rtw_rust_hal_dm_adapter_to_dvobj(adapter);
        let mut txpath: U32 = 0;
        let mut rxpath: U32 = 0;

        rtw_hal_get_trx_path(dvobj, core::ptr::null_mut(), &mut txpath, &mut rxpath);
        if txpath == 0 {
            rtw_rust_hal_dm_runtime_trx_log_empty_tx();
            return FAIL;
        }
        if rxpath == 0 {
            rtw_rust_hal_dm_runtime_trx_log_empty_rx();
            return FAIL;
        }

        rtw_rust_hal_dm_runtime_trx_nss_default(adapter);

        #[cfg(any(CONFIG_RTL8192F, CONFIG_RTL8822B, CONFIG_RTL8822C))]
        if apply_8822_family_paths(adapter, &mut txpath).is_none() {
            return FAIL;
        }

        rtw_rust_hal_dm_runtime_trx_set_paths(adapter, txpath, rxpath);
        rtw_rust_hal_dm_runtime_trx_dump(core::ptr::null_mut(), adapter);
        SUCCESS
    }
}