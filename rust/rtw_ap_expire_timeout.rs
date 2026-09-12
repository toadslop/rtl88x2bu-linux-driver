// SPDX-License-Identifier: GPL-2.0
//! W3-82 `expire_timeout_chk` orchestrator — Rust port of `core/rtw_ap_expire_timeout.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

use core::ffi::c_void;

type U8 = u8;
type Adapter = c_void;

const _TRUE: U8 = 1;
const _FALSE: U8 = 0;
const NUM_STA: usize = 32;
const STA_INFO_UPDATE_ALL: i32 = 0x3f;

extern "C" {
    fn rtw_ap_expire_timeout_preflight(padapter: *mut Adapter) -> U8;
    fn rtw_ap_expire_asoc_list_scan(
        padapter: *mut Adapter,
        chk_alive_list: *mut i8,
        chk_alive_num: *mut U8,
    );
    fn rtw_ap_expire_chk_alive_process(
        padapter: *mut Adapter,
        chk_alive_list: *mut i8,
        chk_alive_num: U8,
    ) -> U8;
    #[cfg(not(host_ap_expire_timeout_test))]
    fn rtw_rust_expire_clients_update(padapter: *mut Adapter, updated: U8);
    #[cfg(host_ap_expire_timeout_test)]
    fn associated_clients_update(padapter: *mut Adapter, updated: U8, flags: i32);
    #[cfg(rtw_config_rfreg18_wa)]
    fn rtw_check_restore_rf18(padapter: *mut Adapter);
}

fn expire_timeout_chk_impl(padapter: *mut Adapter) {
    unsafe {
        if rtw_ap_expire_timeout_preflight(padapter) == _FALSE {
            return;
        }
        let mut chk_alive_num: U8 = 0;
        let mut chk_alive_list = [0i8; NUM_STA];
        rtw_ap_expire_asoc_list_scan(padapter, chk_alive_list.as_mut_ptr(), &mut chk_alive_num);
        let mut updated = _FALSE;
        if chk_alive_num != 0 {
            updated |= rtw_ap_expire_chk_alive_process(
                padapter,
                chk_alive_list.as_mut_ptr(),
                chk_alive_num,
            );
        }
        #[cfg(rtw_config_rfreg18_wa)]
        rtw_check_restore_rf18(padapter);
        #[cfg(host_ap_expire_timeout_test)]
        associated_clients_update(padapter, updated, STA_INFO_UPDATE_ALL);
        #[cfg(not(host_ap_expire_timeout_test))]
        rtw_rust_expire_clients_update(padapter, updated);
    }
}

#[no_mangle]
pub extern "C" fn expire_timeout_chk(padapter: *mut Adapter) {
    expire_timeout_chk_impl(padapter);
}
