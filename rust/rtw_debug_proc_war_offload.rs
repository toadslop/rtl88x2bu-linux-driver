// SPDX-License-Identifier: GPL-2.0
// CONFIG_WAR_OFFLOAD proc helpers (W3-152). Logic in core/rtw_debug.c.

#[cfg(config_war_offload)]
extern "C" {
    fn rtw_rust_debug_proc_emit_war_offload_enable(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_rust_debug_proc_set_war_offload_enable(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_set_war_offload_ipv4_addr(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
    fn rtw_rust_debug_proc_set_war_offload_ipv6_addr(
        adapter: *mut c_void,
        buffer: *const c_void,
        count: usize,
    ) -> c_long;
}

#[cfg(all(config_war_offload, any(config_rtw_debug, config_proc_debug)))]
#[no_mangle]
pub extern "C" fn proc_get_war_offload_enable(m: *mut c_void, _v: *mut c_void) -> c_int {
    let adapter = adapter_from_seq(m);
    if !adapter.is_null() {
        unsafe { rtw_rust_debug_proc_emit_war_offload_enable(m, adapter) };
    }
    0
}

#[cfg(all(config_war_offload, any(config_rtw_debug, config_proc_debug)))]
#[no_mangle]
pub extern "C" fn proc_set_war_offload_enable(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_war_offload_enable(adapter, buffer, count) }
}

#[cfg(all(config_war_offload, any(config_rtw_debug, config_proc_debug)))]
#[no_mangle]
pub extern "C" fn proc_set_war_offload_ipv4_addr(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_war_offload_ipv4_addr(adapter, buffer, count) }
}

#[cfg(all(config_war_offload, any(config_rtw_debug, config_proc_debug)))]
#[no_mangle]
pub extern "C" fn proc_set_war_offload_ipv6_addr(
    _file: *mut c_void,
    buffer: *const c_void,
    count: usize,
    _pos: *mut c_long,
    data: *mut c_void,
) -> c_long {
    let adapter = unsafe { rtw_rust_debug_proc_adapter_from_data(data) };
    unsafe { rtw_rust_debug_proc_set_war_offload_ipv6_addr(adapter, buffer, count) }
}
