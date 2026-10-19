// dump_hal_spec formatter (W4-41).

pub const BAND_CAP_BIT_NUM: usize = 2;
pub const BW_CAP_BIT_NUM: usize = 7;
pub const PROTO_CAP_BIT_NUM: usize = 4;
pub const WL_FUNC_BIT_NUM: usize = 4;

#[repr(C)]
#[derive(Clone, Copy)]
pub struct HostHalComHalSpecFixture {
    pub macid_num: u8,
    pub macid_cap: u8,
    pub sec_cap: u8,
    pub sec_cam_ent_num: u8,
    pub rfpath_num_2g: u8,
    pub rfpath_num_5g: u8,
    pub rf_reg_path_num: u8,
    pub rf_reg_path_avail_num: u8,
    pub rf_reg_trx_path_bmp: u8,
    pub max_tx_cnt: u8,
    pub tx_nss_num: u8,
    pub rx_nss_num: u8,
    pub band_cap: u8,
    pub bw_cap: u8,
    pub proto_cap: u8,
    pub txgi_max: u8,
    pub txgi_pdbm: u8,
    pub wl_func: u8,
    pub tx_aclt_unit_factor: u8,
    pub rx_tsf_filter: u8,
    pub pg_txpwr_saddr: u8,
    pub pg_txgi_diff_factor: u8,
}

static BAND_CAP_STR: [&str; BAND_CAP_BIT_NUM] = ["2G", "5G"];
static BW_CAP_STR: [&str; BW_CAP_BIT_NUM] = ["5M", "10M", "20M", "40M", "80M", "160M", "80_80M"];
static PROTO_CAP_STR: [&str; PROTO_CAP_BIT_NUM] = ["b", "g", "n", "ac"];
static WL_FUNC_STR: [&str; WL_FUNC_BIT_NUM] = ["P2P", "MIRACAST", "TDLS", "FTM"];

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

fn push_u8(buf: &mut [u8], pos: &mut usize, n: u8) -> bool {
    push_u32(buf, pos, n as u32)
}

fn push_u32(buf: &mut [u8], pos: &mut usize, n: u32) -> bool {
    if n == 0 {
        return push_str(buf, pos, "0");
    }
    let mut t = [0u8; 10];
    let mut l = 0usize;
    let mut v = n;
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

fn push_hex8_digits(buf: &mut [u8], pos: &mut usize, n: u8) -> bool {
    const HEX: &[u8; 16] = b"0123456789abcdef";
    let hi = (n >> 4) & 0xf;
    let lo = n & 0xf;
    if *pos + 2 > buf.len() {
        return false;
    }
    buf[*pos] = HEX[hi as usize];
    *pos += 1;
    buf[*pos] = HEX[lo as usize];
    *pos += 1;
    true
}

fn push_hex32_digits(buf: &mut [u8], pos: &mut usize, n: u8) -> bool {
    const HEX: &[u8; 16] = b"0123456789ABCDEF";
    if n == 0 {
        return push_str(buf, pos, "0");
    }
    let mut digits = [0u8; 2];
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

fn push_u8_line(buf: &mut [u8], pos: &mut usize, label: &str, val: u8) -> bool {
    push_str(buf, pos, label) && push_u8(buf, pos, val) && push_str(buf, pos, "\n")
}

fn push_hex8_line(buf: &mut [u8], pos: &mut usize, label: &str, val: u8) -> bool {
    push_str(buf, pos, label) && push_hex8_digits(buf, pos, val) && push_str(buf, pos, "\n")
}

fn push_hex32_line(buf: &mut [u8], pos: &mut usize, label: &str, val: u8) -> bool {
    push_str(buf, pos, label) && push_hex32_digits(buf, pos, val) && push_str(buf, pos, "\n")
}

fn append_cap_line(buf: &mut [u8], pos: &mut usize, label: &str, cap: u8, table: &[&str]) -> bool {
    if !push_str(buf, pos, label) {
        return false;
    }
    for i in 0..table.len() {
        if (cap >> i) & 1 != 0 {
            if !push_str(buf, pos, table[i]) || !push_str(buf, pos, " ") {
                return false;
            }
        }
    }
    push_str(buf, pos, "\n")
}

pub fn format_hal_spec(fx: &HostHalComHalSpecFixture, buf: &mut [u8]) -> i32 {
    let mut pos = 0usize;

    if !push_u8_line(buf, &mut pos, "macid_num:", fx.macid_num)
        || !push_u8_line(buf, &mut pos, "macid_cap:", fx.macid_cap)
        || !push_hex8_line(buf, &mut pos, "sec_cap:0x", fx.sec_cap)
        || !push_u8_line(buf, &mut pos, "sec_cam_ent_num:", fx.sec_cam_ent_num)
        || !push_u8_line(buf, &mut pos, "rfpath_num_2g:", fx.rfpath_num_2g)
        || !push_u8_line(buf, &mut pos, "rfpath_num_5g:", fx.rfpath_num_5g)
        || !push_u8_line(buf, &mut pos, "rf_reg_path_num:", fx.rf_reg_path_num)
        || !push_u8_line(
            buf,
            &mut pos,
            "rf_reg_path_avail_num:",
            fx.rf_reg_path_avail_num,
        )
        || !push_hex8_line(
            buf,
            &mut pos,
            "rf_reg_trx_path_bmp:0x",
            fx.rf_reg_trx_path_bmp,
        )
        || !push_u8_line(buf, &mut pos, "max_tx_cnt:", fx.max_tx_cnt)
        || !push_u8_line(buf, &mut pos, "tx_nss_num:", fx.tx_nss_num)
        || !push_u8_line(buf, &mut pos, "rx_nss_num:", fx.rx_nss_num)
        || !append_cap_line(buf, &mut pos, "band_cap:", fx.band_cap, &BAND_CAP_STR)
        || !append_cap_line(buf, &mut pos, "bw_cap:", fx.bw_cap, &BW_CAP_STR)
        || !append_cap_line(buf, &mut pos, "proto_cap:", fx.proto_cap, &PROTO_CAP_STR)
        || !push_u8_line(buf, &mut pos, "txgi_max:", fx.txgi_max)
        || !push_u8_line(buf, &mut pos, "txgi_pdbm:", fx.txgi_pdbm)
        || !append_cap_line(buf, &mut pos, "wl_func:", fx.wl_func, &WL_FUNC_STR)
    {
        return -1;
    }

    #[cfg(config_tx_ac_lifetime)]
    {
        let unit_us = fx.tx_aclt_unit_factor as u32 * 32;
        if !push_str(buf, &mut pos, "tx_aclt_unit_factor:")
            || !push_u8(buf, &mut pos, fx.tx_aclt_unit_factor)
            || !push_str(buf, &mut pos, " (unit:")
            || !push_u32(buf, &mut pos, unit_us)
            || !push_str(buf, &mut pos, "us)\n")
        {
            return -1;
        }
    }

    if !push_u8_line(buf, &mut pos, "rx_tsf_filter:", fx.rx_tsf_filter)
        || !push_hex32_line(buf, &mut pos, "pg_txpwr_saddr:0x", fx.pg_txpwr_saddr)
        || !push_u8_line(
            buf,
            &mut pos,
            "pg_txgi_diff_factor:",
            fx.pg_txgi_diff_factor,
        )
    {
        return -1;
    }

    if pos < buf.len() {
        buf[pos] = 0;
    }
    pos as i32
}

#[no_mangle]
pub extern "C" fn dump_hal_spec_format(
    fx: *const HostHalComHalSpecFixture,
    out: *mut u8,
    buflen: usize,
) -> i32 {
    if fx.is_null() || out.is_null() || buflen == 0 {
        return -1;
    }
    let fx = unsafe { &*fx };
    let buf = unsafe { core::slice::from_raw_parts_mut(out, buflen) };
    format_hal_spec(fx, buf)
}
