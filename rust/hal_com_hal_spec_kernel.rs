// Kernel entry for dump_hal_spec (W4-41).

use super::{format_hal_spec, HostHalComHalSpecFixture};
use core::ffi::{c_char, c_void};

extern "C" {
    fn rtw_rust_hal_com_print_line(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_hal_com_copy_hal_spec(
        adapter: *mut c_void,
        out: *mut HostHalComHalSpecFixture,
    );
}

fn print_line(sel: *mut c_void, line: &[u8]) {
    let mut buf = [0i8; 128];
    let n = core::cmp::min(line.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = line[i] as i8;
    }
    unsafe {
        rtw_rust_hal_com_print_line(sel, buf.as_ptr());
    }
}

fn print_formatted(sel: *mut c_void, text: &[u8]) {
    let mut start = 0usize;
    for i in 0..text.len() {
        if text[i] == b'\n' {
            if i > start {
                print_line(sel, &text[start..i]);
            }
            print_line(sel, b"\n");
            start = i + 1;
        }
    }
    if start < text.len() {
        print_line(sel, &text[start..]);
    }
}

#[no_mangle]
pub extern "C" fn dump_hal_spec(sel: *mut c_void, adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    let mut fx = HostHalComHalSpecFixture {
        macid_num: 0,
        macid_cap: 0,
        sec_cap: 0,
        sec_cam_ent_num: 0,
        rfpath_num_2g: 0,
        rfpath_num_5g: 0,
        rf_reg_path_num: 0,
        rf_reg_path_avail_num: 0,
        rf_reg_trx_path_bmp: 0,
        max_tx_cnt: 0,
        tx_nss_num: 0,
        rx_nss_num: 0,
        band_cap: 0,
        bw_cap: 0,
        proto_cap: 0,
        txgi_max: 0,
        txgi_pdbm: 0,
        wl_func: 0,
        tx_aclt_unit_factor: 0,
        rx_tsf_filter: 0,
        pg_txpwr_saddr: 0,
        pg_txgi_diff_factor: 0,
    };
    unsafe {
        rtw_rust_hal_com_copy_hal_spec(adapter, &mut fx);
    }
    let mut buf = [0u8; 512];
    let n = format_hal_spec(&fx, &mut buf);
    if n > 0 {
        print_formatted(sel, &buf[..n as usize]);
    }
}
