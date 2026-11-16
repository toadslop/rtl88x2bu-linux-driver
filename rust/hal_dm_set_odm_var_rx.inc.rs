// W4-96 — `SetHalODMVar` RX info dump cases from `hal/hal_dm.c`.

use core::ffi::{c_char, c_void};

type DmStruct = u8;
type U8 = u8;
type S8 = i8;
type U32 = u32;

const HAL_ODM_RX_INFO_DUMP: U32 = 5;
const HAL_ODM_RX_DFRAME_INFO: U32 = 6;

const FA_OFDM: U32 = 0;
const FA_CCK: U32 = 1;
const FA_TOTAL: U32 = 2;

#[repr(C)]
struct RtwHalDmDmRxFields {
    is_linked: U8,
    rx_rate: U8,
    rssi_a: S8,
    rssi_b: S8,
    rssi_c: S8,
    rssi_d: S8,
}

unsafe extern "C" {
    fn rtw_phydm_get_cur_igi(adapter: *mut c_void) -> U8;
    fn rtw_phydm_get_min_rssi(adapter: *mut c_void) -> S8;
    fn rtw_dm_get_min_rssi(adapter: *mut c_void) -> S8;
    fn rtw_phydm_get_phy_cnt(adapter: *mut c_void, cnt: U32) -> U32;
    fn rtw_rust_hal_dm_raw_print_sel(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_hal_dm_hdata_rate(rate: U8) -> *const c_char;
    fn rtw_rust_hal_dm_is_hw_8814a(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_dm_dump_rx_dframe_info(adapter: *mut c_void, sel: *mut c_void);
    fn rtw_rust_hal_dm_dump_raw_rssi_info(adapter: *mut c_void, sel: *mut c_void);
    #[cfg(host_hal_dm_set_odm_var_rx_test)]
    fn host_hdata_rate_str(rate: U8) -> *const c_char;
    #[cfg(host_hal_dm_set_odm_var_rx_test)]
    fn host_is_hardware_type_8814a(adapter: *mut c_void) -> i32;
    #[cfg(host_hal_dm_set_odm_var_rx_test)]
    fn host_hal_dm_apply_rx_fixture_to_dm(adapter: *mut c_void);
    fn rtw_rust_hal_dm_dm_rx_fields(podm: *mut c_void) -> RtwHalDmDmRxFields;
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    #[cfg(host_hal_dm_set_odm_var_rx_test)]
    fn rtw_rust_hal_dm_print_sel(sel: *mut c_void, line: *const c_char);
}

fn rate_str(rate: U8) -> *const c_char {
    #[cfg(host_hal_dm_set_odm_var_rx_test)]
    unsafe {
        return host_hdata_rate_str(rate);
    }
    #[cfg(not(host_hal_dm_set_odm_var_rx_test))]
    unsafe {
        return rtw_rust_hal_dm_hdata_rate(rate);
    }
}

fn is_8814a(adapter: *mut c_void) -> bool {
    #[cfg(host_hal_dm_set_odm_var_rx_test)]
    unsafe {
        return host_is_hardware_type_8814a(adapter) != 0;
    }
    #[cfg(not(host_hal_dm_set_odm_var_rx_test))]
    unsafe {
        return rtw_rust_hal_dm_is_hw_8814a(adapter) != 0;
    }
}

fn print_line(sel: *mut c_void, line: &[u8]) {
    let mut buf = [0i8; 384];
    let n = core::cmp::min(line.len(), buf.len().saturating_sub(1));
    for (i, &b) in line.iter().take(n).enumerate() {
        buf[i] = b as i8;
    }
    unsafe {
        #[cfg(host_hal_dm_set_odm_var_rx_test)]
        rtw_rust_hal_dm_print_sel(sel, buf.as_ptr());
        #[cfg(not(host_hal_dm_set_odm_var_rx_test))]
        rtw_rust_hal_dm_raw_print_sel(sel, buf.as_ptr());
    }
}

fn push_byte(buf: &mut [u8], pos: &mut usize, b: u8) -> bool {
    if *pos >= buf.len() {
        return false;
    }
    buf[*pos] = b;
    *pos += 1;
    true
}

fn push_bytes(buf: &mut [u8], pos: &mut usize, s: &[u8]) -> bool {
    for &b in s {
        if !push_byte(buf, pos, b) {
            return false;
        }
    }
    true
}

fn push_i32_dec(buf: &mut [u8], pos: &mut usize, n: i32) -> bool {
    let mut tmp = [0u8; 12];
    let mut len = 0usize;
    let neg = n < 0;
    let mag = n.unsigned_abs();
    if mag == 0 {
        return push_byte(buf, pos, b'0');
    }
    let mut v = mag;
    while v > 0 {
        tmp[len] = b'0' + (v % 10) as u8;
        len += 1;
        v /= 10;
    }
    if neg && !push_byte(buf, pos, b'-') {
        return false;
    }
    while len > 0 {
        len -= 1;
        if !push_byte(buf, pos, tmp[len]) {
            return false;
        }
    }
    true
}

/// C `0x%x` for a byte — no leading zero padding beyond a single `0` digit for value 0.
fn push_hex_x(buf: &mut [u8], pos: &mut usize, n: U8) -> bool {
    const HEX: &[u8; 16] = b"0123456789abcdef";
    if !push_byte(buf, pos, b'0') || !push_byte(buf, pos, b'x') {
        return false;
    }
    if n == 0 {
        return push_byte(buf, pos, b'0');
    }
    let hi = n >> 4;
    let lo = n & 0xf;
    if hi != 0 && !push_byte(buf, pos, HEX[hi as usize]) {
        return false;
    }
    push_byte(buf, pos, HEX[lo as usize])
}

pub unsafe fn set_odm_var_rx_cases(
    adapter: *mut c_void,
    e_variable: U32,
    p_value1: *mut c_void,
) {
    // SAFETY: callers pass a valid `adapter`; `adapter_to_phydm` is non-NULL for live adapters.
    unsafe {
        #[cfg(host_hal_dm_set_odm_var_rx_test)]
        host_hal_dm_apply_rx_fixture_to_dm(adapter);

        match e_variable {
            HAL_ODM_RX_INFO_DUMP => {
                let sel = p_value1;
                let podm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
                let dm = rtw_rust_hal_dm_dm_rx_fields(podm as *mut c_void);
                let cur_igi = rtw_phydm_get_cur_igi(adapter);
                let rssi_min = rtw_phydm_get_min_rssi(adapter);
                let mut out = [0u8; 384];
                let mut pos = 0usize;

                let _ = push_bytes(&mut out, &mut pos, b"============ Rx Info dump ===================\n");
                print_line(sel, &out[..pos]);
                pos = 0;
                let _ = push_bytes(&mut out, &mut pos, b"is_linked = ");
                let _ = push_i32_dec(&mut out, &mut pos, dm.is_linked as i32);
                let _ = push_bytes(&mut out, &mut pos, b", rssi_min = ");
                let _ = push_i32_dec(&mut out, &mut pos, rssi_min as i32);
                let _ = push_bytes(&mut out, &mut pos, b"(%)(");
                let _ = push_i32_dec(&mut out, &mut pos, rtw_dm_get_min_rssi(adapter) as i32);
                let _ = push_bytes(&mut out, &mut pos, b"(%)), current_igi = ");
                let _ = push_hex_x(&mut out, &mut pos, cur_igi);
                let _ = push_byte(&mut out, &mut pos, b'\n');
                print_line(sel, &out[..pos]);
                pos = 0;
                let _ = push_bytes(&mut out, &mut pos, b"cnt_cck_fail = ");
                let _ = push_i32_dec(
                    &mut out,
                    &mut pos,
                    rtw_phydm_get_phy_cnt(adapter, FA_CCK) as i32,
                );
                let _ = push_bytes(&mut out, &mut pos, b", cnt_ofdm_fail = ");
                let _ = push_i32_dec(
                    &mut out,
                    &mut pos,
                    rtw_phydm_get_phy_cnt(adapter, FA_OFDM) as i32,
                );
                let _ = push_bytes(&mut out, &mut pos, b", Total False Alarm = ");
                let _ = push_i32_dec(
                    &mut out,
                    &mut pos,
                    rtw_phydm_get_phy_cnt(adapter, FA_TOTAL) as i32,
                );
                let _ = push_byte(&mut out, &mut pos, b'\n');
                print_line(sel, &out[..pos]);
                if dm.is_linked != 0 {
                    let rate = rate_str(dm.rx_rate);
                    let rs = if rate.is_null() {
                        "<null>"
                    } else {
                        core::ffi::CStr::from_ptr(rate).to_str().unwrap_or("?")
                    };
                    pos = 0;
                    let _ = push_bytes(&mut out, &mut pos, b"rx_rate = ");
                    let _ = push_bytes(&mut out, &mut pos, rs.as_bytes());
                    print_line(sel, &out[..pos]);
                    pos = 0;
                    if is_8814a(adapter) {
                        let _ = push_bytes(&mut out, &mut pos, b" rssi_a = ");
                        let _ = push_i32_dec(&mut out, &mut pos, dm.rssi_a as i32);
                        let _ = push_bytes(&mut out, &mut pos, b"(%), rssi_b = ");
                        let _ = push_i32_dec(&mut out, &mut pos, dm.rssi_b as i32);
                        let _ = push_bytes(&mut out, &mut pos, b"(%), rssi_c = ");
                        let _ = push_i32_dec(&mut out, &mut pos, dm.rssi_c as i32);
                        let _ = push_bytes(&mut out, &mut pos, b"(%), rssi_d = ");
                        let _ = push_i32_dec(&mut out, &mut pos, dm.rssi_d as i32);
                        let _ = push_bytes(&mut out, &mut pos, b"(%)\n");
                    } else {
                        let _ = push_bytes(&mut out, &mut pos, b" rssi_a = ");
                        let _ = push_i32_dec(&mut out, &mut pos, dm.rssi_a as i32);
                        let _ = push_bytes(&mut out, &mut pos, b"(%), rssi_b = ");
                        let _ = push_i32_dec(&mut out, &mut pos, dm.rssi_b as i32);
                        let _ = push_bytes(&mut out, &mut pos, b"(%)\n");
                    }
                    print_line(sel, &out[..pos]);
                    rtw_rust_hal_dm_dump_raw_rssi_info(adapter, sel);
                }
            }
            HAL_ODM_RX_DFRAME_INFO => {
                let sel = p_value1;
                rtw_rust_hal_dm_dump_rx_dframe_info(adapter, sel);
            }
            _ => {}
        }
    }
}

#[cfg(host_hal_dm_set_odm_var_rx_test)]
#[no_mangle]
pub unsafe extern "C" fn SetHalODMVar(
    adapter: *mut c_void,
    e_variable: U32,
    p_value1: *mut c_void,
    _b_set: U8,
) {
    set_odm_var_rx_cases(adapter, e_variable, p_value1);
}