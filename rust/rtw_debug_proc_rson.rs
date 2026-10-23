// SPDX-License-Identifier: GPL-2.0
// CONFIG_RTW_REPEATER_SON proc helpers (W3-155). Logic via core/rtw_debug.c + rtw_rson.c.

#[cfg(config_rtw_repeater_son)]
extern "C" {
    fn rtw_rust_debug_proc_emit_rson_data(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_rson_data(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
}

#[cfg(all(config_rtw_repeater_son, any(config_rtw_debug, config_proc_debug),))]
#[no_mangle]
pub extern "C" fn proc_get_rson_data(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_rson_data(m, adapter) };
    }
    0
}

#[cfg(all(config_rtw_repeater_son, any(config_rtw_debug, config_proc_debug),))]
#[no_mangle]
pub extern "C" fn proc_set_rson_data(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_rson_data(adapter, buffer, count) }
}
