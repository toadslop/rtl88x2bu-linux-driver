// SPDX-License-Identifier: GPL-2.0
// proc survey_info get/set (W3-156). Logic in core/rtw_debug.c.

extern "C" {
    fn rtw_rust_debug_proc_emit_survey_info(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_survey_info(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
}

#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_get_survey_info(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_survey_info(m, adapter) };
    }
    0
}

#[cfg(any(config_rtw_debug, config_proc_debug))]
#[no_mangle]
pub extern "C" fn proc_set_survey_info(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_survey_info(adapter, buffer, count) }
}
