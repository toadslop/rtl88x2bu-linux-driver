// SPDX-License-Identifier: GPL-2.0
//! W3-79 `rtw_init_bcmc_stainfo` host L2 oracle (see `core/rtw_sta_mgt_free.c`).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

const _SUCCESS: u32 = 1;
const ETH_ALEN: usize = 6;
/// Host L2 `_adapter` places `stapriv` first (`tests/host/include/host_sta_mgt_types.h`).
const ADAPTER_STAPRIV_OFF: usize = 0;

#[repr(C)]
struct StaInfo {
    _data: [u8; 976],
}

extern "C" {
    fn rtw_alloc_stainfo(stapriv: *mut u8, hwaddr: *const u8) -> *mut StaInfo;
}

#[no_mangle]
pub extern "C" fn rtw_init_bcmc_stainfo(padapter: *mut u8) -> u32 {
    unsafe {
        if padapter.is_null() {
            return _SUCCESS;
        }
        let bcast: [u8; ETH_ALEN] = [0xff; ETH_ALEN];
        let stapriv = padapter.add(ADAPTER_STAPRIV_OFF);
        let _psta = rtw_alloc_stainfo(stapriv, bcast.as_ptr());
        _SUCCESS
    }
}
