#[cfg(host_hal_dm_dump_sta_traffic_test)]
use std::os::raw::{c_char, c_void};

#[cfg(not(host_hal_dm_dump_sta_traffic_test))]
use core::ffi::{c_char, c_void};

type U8 = u8;
type I8 = i8;
type U32 = u32;

#[repr(C)]
pub struct HostHalDmDumpStaTrafficFixture {
    pub mac_addr: [U8; 6],
    pub mac_id: U8,
    pub associated_to_ap: U8,
    pub bcn_cnt: U8,
    pub bcn_dtim: U8,
    pub stub_tx_rate: U8,
    pub stub_tx_sgi: U8,
    pub curr_rx_rate: U8,
    pub curr_rx_rate_bmc: U8,
    pub rssi: I8,
    pub tx_tp_kbits: U32,
    pub rx_tp_kbits: U32,
    pub smooth_tx_tp_kbits: U32,
    pub smooth_rx_tp_kbits: U32,
}

#[repr(C)]
struct RtwHalDmStaTrafficFields {
    mac_addr: [U8; 6],
    mac_id: U8,
    rssi: I8,
    curr_rx_rate: U8,
    curr_rx_rate_bmc: U8,
    tx_tp_kbits: U32,
    rx_tp_kbits: U32,
    smooth_tx_tp_kbits: U32,
    smooth_rx_tp_kbits: U32,
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

fn push_u8_dec(buf: &mut [u8], pos: &mut usize, n: U8) -> bool {
    if n >= 100 {
        if !push_byte(buf, pos, b'0' + n / 100) {
            return false;
        }
    }
    if n >= 10 {
        if !push_byte(buf, pos, b'0' + (n / 10) % 10) {
            return false;
        }
    }
    push_byte(buf, pos, b'0' + n % 10)
}

fn push_i8_dec(buf: &mut [u8], pos: &mut usize, n: I8) -> bool {
    if n < 0 {
        if !push_byte(buf, pos, b'-') {
            return false;
        }
        return push_u8_dec(buf, pos, (-(n as i32)) as U8);
    }
    push_u8_dec(buf, pos, n as U8)
}

fn push_u32_dec(buf: &mut [u8], pos: &mut usize, n: U32) -> bool {
    let mut tmp = [0u8; 10];
    let mut len = 0usize;
    let mut v = n;

    if v == 0 {
        return push_byte(buf, pos, b'0');
    }
    while v > 0 {
        tmp[len] = b'0' + (v % 10) as u8;
        len += 1;
        v /= 10;
    }
    for i in (0..len).rev() {
        if !push_byte(buf, pos, tmp[i]) {
            return false;
        }
    }
    true
}

fn push_u8_hex2(buf: &mut [u8], pos: &mut usize, n: U8) -> bool {
    const HEX: &[u8; 16] = b"0123456789abcdef";
    push_byte(buf, pos, HEX[(n >> 4) as usize]) && push_byte(buf, pos, HEX[(n & 0xf) as usize])
}

#[cfg(host_hal_dm_dump_sta_traffic_test)]
fn hdata_rate(rate: U8) -> &'static [u8] {
    extern "C" {
        fn rtw_rust_hal_com_hdata_rate(hw_rate: U8) -> *const c_char;
    }
    c_str_bytes(unsafe { rtw_rust_hal_com_hdata_rate(rate) })
}

#[cfg(not(host_hal_dm_dump_sta_traffic_test))]
fn hdata_rate(rate: U8) -> &'static [u8] {
    extern "C" {
        fn rtw_rust_hal_dm_hdata_rate(rate: U8) -> *const c_char;
    }
    c_str_bytes(unsafe { rtw_rust_hal_dm_hdata_rate(rate) })
}

fn c_str_bytes(ptr: *const c_char) -> &'static [u8] {
    if ptr.is_null() {
        return b"";
    }
    unsafe {
        let mut len = 0usize;
        while *ptr.add(len) != 0 {
            len += 1;
        }
        core::slice::from_raw_parts(ptr as *const u8, len)
    }
}

fn fields_to_fixture(f: &RtwHalDmStaTrafficFields) -> HostHalDmDumpStaTrafficFixture {
    HostHalDmDumpStaTrafficFixture {
        mac_addr: f.mac_addr,
        mac_id: f.mac_id,
        associated_to_ap: 0,
        bcn_cnt: 0,
        bcn_dtim: 0,
        stub_tx_rate: 0,
        stub_tx_sgi: 0,
        curr_rx_rate: f.curr_rx_rate,
        curr_rx_rate_bmc: f.curr_rx_rate_bmc,
        rssi: f.rssi,
        tx_tp_kbits: f.tx_tp_kbits,
        rx_tp_kbits: f.rx_tp_kbits,
        smooth_tx_tp_kbits: f.smooth_tx_tp_kbits,
        smooth_rx_tp_kbits: f.smooth_rx_tp_kbits,
    }
}

fn format_tp_section(
    buf: &mut [u8],
    pos: &mut usize,
    label: &[u8],
    tx_kbits: U32,
    rx_kbits: U32,
    smooth: bool,
    smooth_tx: U32,
    smooth_rx: U32,
) -> bool {
    let (tx_tp, rx_tp) = if smooth {
        (smooth_tx, smooth_rx)
    } else {
        (tx_kbits, rx_kbits)
    };
    let tx_mb = tx_tp >> 10;
    let rx_mb = rx_tp >> 10;
    let bi_mb = tx_mb + rx_mb;

    if !push_bytes(buf, pos, label) {
        return false;
    }
    if tx_mb != 0 {
        if !push_bytes(buf, pos, b"Tx : ")
            || !push_u32_dec(buf, pos, tx_mb)
            || !push_bytes(buf, pos, b"(Mbps) ")
        {
            return false;
        }
    } else if !push_bytes(buf, pos, b"Tx : ")
        || !push_u32_dec(buf, pos, tx_tp)
        || !push_bytes(buf, pos, b"(Kbps) ")
    {
        return false;
    }
    if rx_mb != 0 {
        if !push_bytes(buf, pos, b"Rx : ")
            || !push_u32_dec(buf, pos, rx_mb)
            || !push_bytes(buf, pos, b"(Mbps) ")
        {
            return false;
        }
    } else if !push_bytes(buf, pos, b"Rx : ")
        || !push_u32_dec(buf, pos, rx_tp)
        || !push_bytes(buf, pos, b"(Kbps) ")
    {
        return false;
    }
    if bi_mb != 0 {
        return push_bytes(buf, pos, b"Total : ")
            && push_u32_dec(buf, pos, bi_mb)
            && push_bytes(buf, pos, b"(Mbps)\n");
    }
    push_bytes(buf, pos, b"Total : ")
        && push_u32_dec(buf, pos, tx_tp + rx_tp)
        && push_bytes(buf, pos, b"(Kbps)\n")
}

fn format_dump_sta_traffic(
    fx: &HostHalDmDumpStaTrafficFixture,
    curr_tx_rate: U8,
    curr_tx_sgi: U8,
    buf: &mut [u8],
) -> i32 {
    let mut pos = 0usize;

    if !push_byte(buf, &mut pos, b'\n')
        || !push_bytes(buf, &mut pos, b"====== mac_id : ")
        || !push_u8_dec(buf, &mut pos, fx.mac_id)
        || !push_bytes(buf, &mut pos, b" [")
    {
        return -1;
    }
    for i in 0..6 {
        if i > 0 && !push_byte(buf, &mut pos, b':') {
            return -1;
        }
        if !push_u8_hex2(buf, &mut pos, fx.mac_addr[i]) {
            return -1;
        }
    }
    if !push_bytes(buf, &mut pos, b"] ======\n") {
        return -1;
    }
    if fx.associated_to_ap != 0 {
        if !push_bytes(buf, &mut pos, b"BCN counts : ")
            || !push_u8_dec(buf, &mut pos, fx.bcn_cnt / 2)
            || !push_bytes(buf, &mut pos, b" (per-1 second), DTIM Period:")
            || !push_u8_dec(buf, &mut pos, fx.bcn_dtim)
            || !push_byte(buf, &mut pos, b'\n')
        {
            return -1;
        }
    }
    if !push_bytes(buf, &mut pos, b"tx_rate : ")
        || !push_bytes(buf, &mut pos, hdata_rate(curr_tx_rate))
        || !push_byte(buf, &mut pos, b'(')
        || !push_bytes(buf, &mut pos, if curr_tx_sgi != 0 { b"S" } else { b"L" })
        || !push_bytes(buf, &mut pos, b")  rx_rate : ")
        || !push_bytes(buf, &mut pos, hdata_rate(fx.curr_rx_rate & 0x7f))
        || !push_bytes(buf, &mut pos, b", rx_rate_bmc : ")
        || !push_bytes(buf, &mut pos, hdata_rate(fx.curr_rx_rate_bmc & 0x7f))
        || !push_bytes(buf, &mut pos, b", rssi : ")
        || !push_i8_dec(buf, &mut pos, fx.rssi)
        || !push_bytes(buf, &mut pos, b" %\n")
    {
        return -1;
    }
    if !format_tp_section(
        buf,
        &mut pos,
        b"RTW: [TP] ",
        fx.tx_tp_kbits,
        fx.rx_tp_kbits,
        false,
        fx.smooth_tx_tp_kbits,
        fx.smooth_rx_tp_kbits,
    ) || !format_tp_section(
        buf,
        &mut pos,
        b"RTW: [Smooth TP] ",
        fx.tx_tp_kbits,
        fx.rx_tp_kbits,
        true,
        fx.smooth_tx_tp_kbits,
        fx.smooth_rx_tp_kbits,
    ) {
        return -1;
    }
    pos as i32
}

fn print_line(sel: *mut c_void, line: &[u8]) {
    let mut buf = [0i8; 512];
    let n = core::cmp::min(line.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = line[i] as i8;
    }
    extern "C" {
        fn rtw_rust_hal_dm_print_sel(sel: *mut c_void, line: *const c_char);
    }
    unsafe {
        rtw_rust_hal_dm_print_sel(sel, buf.as_ptr());
    }
}

fn dump_lines(sel: *mut c_void, text: &[u8], len: usize) {
    let mut start = 0usize;
    for i in 0..len {
        if text[i] == b'\n' {
            print_line(sel, &text[start..=i]);
            start = i + 1;
        }
    }
    if start < len {
        print_line(sel, &text[start..len]);
    }
}

#[cfg(host_hal_dm_dump_sta_traffic_test)]
#[no_mangle]
pub extern "C" fn dump_sta_traffic_format(
    fx: *const HostHalDmDumpStaTrafficFixture,
    buf: *mut u8,
    buflen: usize,
) -> i32 {
    if fx.is_null() || buf.is_null() || buflen == 0 {
        return -1;
    }
    let fx = unsafe { &*fx };
    let out = unsafe { core::slice::from_raw_parts_mut(buf, buflen) };
    let n = format_dump_sta_traffic(fx, fx.stub_tx_rate, fx.stub_tx_sgi, out);
    if n > 0 && (n as usize) < buflen {
        out[n as usize] = 0;
    }
    n
}

#[cfg(not(host_hal_dm_dump_sta_traffic_test))]
#[no_mangle]
pub extern "C" fn dump_sta_traffic(sel: *mut c_void, adapter: *mut c_void, psta: *mut c_void) {
    if psta.is_null() {
        return;
    }
    extern "C" {
        fn rtw_rust_hal_dm_sta_traffic_fields(
            psta: *mut c_void,
            out: *mut RtwHalDmStaTrafficFields,
        );
        fn rtw_rust_hal_dm_current_tx_rate(adapter: *mut c_void, psta: *mut c_void) -> U8;
        fn rtw_rust_hal_dm_current_tx_sgi(adapter: *mut c_void, psta: *mut c_void) -> U8;
        fn rtw_rust_hal_dm_sta_adapter(psta: *mut c_void) -> *mut c_void;
        fn rtw_rust_hal_dm_is_client_associated_to_ap(adapter: *mut c_void) -> i32;
        fn rtw_rust_hal_dm_get_bcn_cnt(adapter: *mut c_void) -> U8;
        fn rtw_rust_hal_dm_get_bcn_dtim_period(adapter: *mut c_void) -> U8;
    }
    let mut fields = RtwHalDmStaTrafficFields {
        mac_addr: [0; 6],
        mac_id: 0,
        rssi: 0,
        curr_rx_rate: 0,
        curr_rx_rate_bmc: 0,
        tx_tp_kbits: 0,
        rx_tp_kbits: 0,
        smooth_tx_tp_kbits: 0,
        smooth_rx_tp_kbits: 0,
    };
    unsafe {
        rtw_rust_hal_dm_sta_traffic_fields(psta, &mut fields);
    }
    let mut fx = fields_to_fixture(&fields);
    let padapter = unsafe { rtw_rust_hal_dm_sta_adapter(psta) };
    if unsafe { rtw_rust_hal_dm_is_client_associated_to_ap(padapter) } != 0 {
        fx.associated_to_ap = 1;
        fx.bcn_cnt = unsafe { rtw_rust_hal_dm_get_bcn_cnt(padapter) };
        fx.bcn_dtim = unsafe { rtw_rust_hal_dm_get_bcn_dtim_period(padapter) };
    }
    let curr_tx_rate = unsafe { rtw_rust_hal_dm_current_tx_rate(adapter, psta) };
    let curr_tx_sgi = unsafe { rtw_rust_hal_dm_current_tx_sgi(adapter, psta) };
    let mut buf = [0u8; 1024];
    let n = format_dump_sta_traffic(&fx, curr_tx_rate, curr_tx_sgi, &mut buf);
    if n > 0 {
        dump_lines(sel, &buf[..n as usize], n as usize);
    }
}
