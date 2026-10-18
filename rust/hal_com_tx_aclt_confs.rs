// dump_tx_aclt_confs formatter (W4-40).

const TX_ACLT_CONF_MSG_LEN: usize = 32;
const TX_ACLT_CONF_NUM: usize = 3;

const TX_ACLT_CONF_NAMES: [&str; TX_ACLT_CONF_NUM] = ["DEFAULT", "AP_M2U", "MESH"];

#[repr(C)]
#[derive(Copy, Clone)]
pub struct HostHalComTxAcltConfRow {
    pub en: u8,
    pub vo_vi: u32,
    pub be_bk: u32,
}

#[repr(C)]
pub struct HostHalComTxAcltConfsFixture {
    pub tx_aclt_unit_factor: u8,
    _pad: [u8; 3],
    pub confs: [HostHalComTxAcltConfRow; TX_ACLT_CONF_NUM],
}

fn push_str(buf: &mut [u8], pos: &mut usize, s: &str) -> bool {
    for &b in s.as_bytes() {
        if *pos >= buf.len() {
            return false;
        }
        buf[*pos] = b;
        *pos += 1;
    }
    true
}

fn push_u32(buf: &mut [u8], pos: &mut usize, n: u32) -> bool {
    let mut t = [0u8; 10];
    let mut l = 0usize;
    let mut v = n;
    if v == 0 {
        return push_str(buf, pos, "0");
    }
    while v > 0 {
        t[l] = b'0' + (v % 10) as u8;
        l += 1;
        v /= 10;
    }
    while l > 0 {
        l -= 1;
        if *pos >= buf.len() {
            return false;
        }
        buf[*pos] = t[l];
        *pos += 1;
    }
    true
}

fn push_u32_width(buf: &mut [u8], pos: &mut usize, n: u32, width: usize) -> bool {
    let start = *pos;
    if !push_u32(buf, pos, n) {
        return false;
    }
    let len = *pos - start;
    if len >= width {
        return true;
    }
    let pad = width - len;
    if *pos + pad > buf.len() {
        return false;
    }
    for i in 0..len {
        let src = start + len - 1 - i;
        let dst = start + pad + len - 1 - i;
        buf[dst] = buf[src];
    }
    for i in 0..pad {
        buf[start + i] = b' ';
    }
    *pos = start + width;
    true
}

fn push_hex_x(buf: &mut [u8], pos: &mut usize, n: u32) -> bool {
    const HEX: &[u8; 16] = b"0123456789abcdef";
    if !push_str(buf, pos, "0x") {
        return false;
    }
    if n == 0 {
        return push_str(buf, pos, "0");
    }
    let mut digits = [0u8; 8];
    let mut l = 0usize;
    let mut v = n;
    while v > 0 {
        digits[l] = HEX[(v & 0xf) as usize];
        l += 1;
        v >>= 4;
    }
    while l > 0 {
        l -= 1;
        if *pos >= buf.len() {
            return false;
        }
        buf[*pos] = digits[l];
        *pos += 1;
    }
    true
}

fn push_field_7s(buf: &mut [u8], pos: &mut usize, s: &str) -> bool {
    let pad = 7usize.saturating_sub(s.len());
    for _ in 0..pad {
        if *pos >= buf.len() {
            return false;
        }
        buf[*pos] = b' ';
        *pos += 1;
    }
    push_str(buf, pos, s)
}

fn push_hex_u16(buf: &mut [u8], pos: &mut usize, n: u16) -> bool {
    const HEX: &[u8; 16] = b"0123456789abcdef";
    if *pos + 4 > buf.len() {
        return false;
    }
    for shift in [12, 8, 4, 0] {
        buf[*pos] = HEX[((n >> shift) & 0xf) as usize];
        *pos += 1;
    }
    true
}

fn format_reg_columns(vo_vi: u32, be_bk: u32, buf: &mut [u8]) -> usize {
    let mut pos = 0usize;
    if vo_vi != 0 {
        if !push_str(buf, &mut pos, "     0x") || !push_hex_u16(buf, &mut pos, vo_vi as u16) {
            return pos;
        }
    } else if !push_str(buf, &mut pos, "        N/A") {
        return pos;
    }
    if pos >= TX_ACLT_CONF_MSG_LEN - 1 {
        return pos;
    }
    if be_bk != 0 {
        if !push_str(buf, &mut pos, "     0x") || !push_hex_u16(buf, &mut pos, be_bk as u16) {
            return pos;
        }
    } else if !push_str(buf, &mut pos, "        N/A") {
        return pos;
    }
    pos
}

pub fn format_tx_aclt_confs(fx: &HostHalComTxAcltConfsFixture, buf: &mut [u8]) -> i32 {
    let unit_factor = fx.tx_aclt_unit_factor;
    let unit_us = unit_factor as u32 * 32;
    let max_us = 0xffff_u32 * unit_factor as u32 * 32;
    let mut pos = 0usize;

    if !push_str(buf, &mut pos, "unit:")
        || !push_u32(buf, &mut pos, unit_us)
        || !push_str(buf, &mut pos, "us, maximum:")
        || !push_u32(buf, &mut pos, max_us)
        || !push_str(buf, &mut pos, "us\n")
    {
        return -1;
    }
    if !push_str(
        buf,
        &mut pos,
        "name    # en  vo_vi(us) be_bk(us) vo_vi(reg) be_bk(reg)\n",
    ) {
        return -1;
    }

    for i in 0..TX_ACLT_CONF_NUM {
        let mut row_msg = [0u8; TX_ACLT_CONF_MSG_LEN];
        let conf = &fx.confs[i];
        let row_len = format_reg_columns(conf.vo_vi, conf.be_bk, &mut row_msg);
        if row_len >= TX_ACLT_CONF_MSG_LEN - 1 {
            continue;
        }
        if !push_field_7s(buf, &mut pos, TX_ACLT_CONF_NAMES[i])
            || !push_str(buf, &mut pos, " ")
            || !push_u32(buf, &mut pos, i as u32)
            || !push_str(buf, &mut pos, " ")
            || !push_hex_x(buf, &mut pos, conf.en as u32)
            || !push_str(buf, &mut pos, " ")
            || !push_u32_width(buf, &mut pos, conf.vo_vi * unit_factor as u32 * 32, 9)
            || !push_str(buf, &mut pos, " ")
            || !push_u32_width(buf, &mut pos, conf.be_bk * unit_factor as u32 * 32, 9)
            || !push_str(
                buf,
                &mut pos,
                core::str::from_utf8(&row_msg[..row_len]).unwrap_or(""),
            )
            || !push_str(buf, &mut pos, "\n")
        {
            return -1;
        }
    }
    if pos < buf.len() {
        buf[pos] = 0;
    }
    pos as i32
}

#[no_mangle]
pub extern "C" fn dump_tx_aclt_confs_format(
    fx: *const HostHalComTxAcltConfsFixture,
    out: *mut u8,
    buflen: usize,
) -> i32 {
    if fx.is_null() || out.is_null() || buflen == 0 {
        return -1;
    }
    let fx = unsafe { &*fx };
    let buf = unsafe { core::slice::from_raw_parts_mut(out, buflen) };
    format_tx_aclt_confs(fx, buf)
}

#[cfg(all(config_tx_ac_lifetime, not(host_hal_com_tx_aclt_confs_test)))]
mod kernel {
    use super::{
        format_tx_aclt_confs, HostHalComTxAcltConfRow, HostHalComTxAcltConfsFixture,
        TX_ACLT_CONF_NUM,
    };
    use core::ffi::{c_char, c_void};

    #[repr(C)]
    #[derive(Copy, Clone)]
    struct TxAcltConfRow {
        en: u8,
        vo_vi: u32,
        be_bk: u32,
    }

    extern "C" {
        fn rtw_rust_hal_com_print_line(sel: *mut c_void, line: *const c_char);
        fn rtw_rust_hal_com_tx_aclt_confs_fields(
            dvobj: *mut c_void,
            unit_factor: *mut u8,
            confs: *mut TxAcltConfRow,
            conf_num: i32,
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
    pub extern "C" fn dump_tx_aclt_confs(sel: *mut c_void, dvobj: *mut c_void) {
        if dvobj.is_null() {
            return;
        }
        let mut unit_factor = 0u8;
        let mut rows = [TxAcltConfRow {
            en: 0,
            vo_vi: 0,
            be_bk: 0,
        }; TX_ACLT_CONF_NUM];
        unsafe {
            rtw_rust_hal_com_tx_aclt_confs_fields(
                dvobj,
                &mut unit_factor,
                rows.as_mut_ptr(),
                TX_ACLT_CONF_NUM as i32,
            );
        }
        let mut fx = HostHalComTxAcltConfsFixture {
            tx_aclt_unit_factor: unit_factor,
            _pad: [0; 3],
            confs: [HostHalComTxAcltConfRow {
                en: 0,
                vo_vi: 0,
                be_bk: 0,
            }; TX_ACLT_CONF_NUM],
        };
        for i in 0..TX_ACLT_CONF_NUM {
            fx.confs[i] = HostHalComTxAcltConfRow {
                en: rows[i].en,
                vo_vi: rows[i].vo_vi,
                be_bk: rows[i].be_bk,
            };
        }
        let mut buf = [0u8; 512];
        let n = format_tx_aclt_confs(&fx, &mut buf);
        if n > 0 {
            print_formatted(sel, &buf[..n as usize]);
        }
    }
}
