// SPDX-License-Identifier: GPL-2.0
//! HAL common helpers — Rust port of `hal/hal_com.c` (W4-01 rate map, W4-02 rsvd page cache, W4-03 chip info, W4-04 macaddr dump).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe
)]

#[cfg(all(
    not(host_hal_com_hw_rate_test),
    not(host_hal_com_rsvd_page_test),
    not(host_hal_com_chip_info_test)
))]
use core::ffi::{c_char, c_void};

type U8 = u8;

const DESC_RATE_NUM: usize = 0x54;
const DESC_RATE1M: U8 = 0x00;
const MGN_1M: U8 = 0x02;
const MGN_MCS32: U8 = 0x7f;
const MGN_UNKNOWN: U8 = 200;
const HT_4SS: u32 = 5;
const RATE_SECTION_NUM: u32 = 10;

static HW_RATE_TO_M_RATE: [U8; DESC_RATE_NUM] = [
    0x02, 0x04, 0x0b, 0x16, 0x0c, 0x12, 0x18, 0x24, 0x30, 0x48, 0x60, 0x6c, 0x80, 0x81, 0x82, 0x83,
    0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93,
    0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3,
    0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3,
    0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7,
];

#[repr(C)]
struct RateSectionEnt {
    tx_num: U8,
    rate_num: U8,
    rates: *mut U8,
}

#[cfg(all(
    not(host_hal_com_hw_rate_test),
    not(host_hal_com_rsvd_page_test),
    not(host_hal_com_chip_info_test)
))]
extern "C" {
    fn MRateToHwRate(rate: U8) -> U8;
    fn MGN_RATE_STR(rate: U8) -> *const c_char;
    static mut rates_by_sections: [RateSectionEnt; RATE_SECTION_NUM as usize];
    fn rtw_rust_hal_com_print_line(sel: *mut c_void, line: *const c_char);
    fn rtw_rust_hal_com_hdata_rate(hw_rate: U8) -> *const c_char;
    fn rtw_rust_hal_com_warn_invalid_hw_rate(hw_rate: U8);
}

#[cfg(all(
    not(host_hal_com_hw_rate_test),
    not(host_hal_com_rsvd_page_test),
    not(host_hal_com_chip_info_test)
))]
fn warn_invalid_hw_rate(hw_rate: U8) {
    unsafe {
        rtw_rust_hal_com_warn_invalid_hw_rate(hw_rate);
    }
}

#[cfg(any(
    host_hal_com_hw_rate_test,
    host_hal_com_rsvd_page_test,
    host_hal_com_chip_info_test
))]
fn warn_invalid_hw_rate(_hw_rate: U8) {}

#[cfg(not(any(host_hal_com_rsvd_page_test, host_hal_com_chip_info_test)))]
#[no_mangle]
pub extern "C" fn hw_rate_to_m_rate(hw_rate: U8) -> U8 {
    if (hw_rate as usize) < DESC_RATE_NUM {
        HW_RATE_TO_M_RATE[hw_rate as usize]
    } else {
        warn_invalid_hw_rate(hw_rate);
        MGN_1M
    }
}

#[cfg(config_rtw_debug)]
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

#[cfg(config_rtw_debug)]
fn print_line(sel: *mut c_void, line: &[u8]) {
    let mut buf = [0i8; 256];
    let n = core::cmp::min(line.len(), buf.len().saturating_sub(1));
    for i in 0..n {
        buf[i] = line[i] as i8;
    }
    unsafe {
        rtw_rust_hal_com_print_line(sel, buf.as_ptr());
    }
}

#[cfg(config_rtw_debug)]
fn push_decimal(mut n: u32, out: &mut [u8], pos: &mut usize) {
    if n == 0 {
        if *pos < out.len() {
            out[*pos] = b'0';
            *pos += 1;
        }
        return;
    }
    let mut digits = [0u8; 10];
    let mut nd = 0usize;
    while n > 0 {
        digits[nd] = b'0' + (n % 10) as u8;
        n /= 10;
        nd += 1;
    }
    while nd > 0 {
        nd -= 1;
        if *pos < out.len() {
            out[*pos] = digits[nd];
            *pos += 1;
        }
    }
}

#[cfg(config_rtw_debug)]
fn format_m_to_hw_line(m_rate: U8, hw_rate: U8) -> (usize, [u8; 256]) {
    let mut line = [0u8; 256];
    let mut pos = 0usize;
    let m_str = c_str_bytes(unsafe { MGN_RATE_STR(m_rate) });
    let h_str = c_str_bytes(unsafe { rtw_rust_hal_com_hdata_rate(hw_rate) });
    for &b in b"m_rate:" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in m_str {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in b"(" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    push_decimal(m_rate as u32, &mut line, &mut pos);
    for &b in b") to hw_rate:" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in h_str {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in b"(" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    push_decimal(hw_rate as u32, &mut line, &mut pos);
    if pos < line.len() {
        line[pos] = b')';
        pos += 1;
    }
    if pos < line.len() {
        line[pos] = b'\n';
        pos += 1;
    }
    (pos, line)
}

#[cfg(config_rtw_debug)]
fn format_hw_to_m_line(hw_rate: U8, m_rate: U8) -> (usize, [u8; 256]) {
    let mut line = [0u8; 256];
    let mut pos = 0usize;
    let h_str = c_str_bytes(unsafe { rtw_rust_hal_com_hdata_rate(hw_rate) });
    let m_str = c_str_bytes(unsafe { MGN_RATE_STR(m_rate) });
    for &b in b"hw_rate:" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in h_str {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in b"(" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    push_decimal(hw_rate as u32, &mut line, &mut pos);
    for &b in b") to m_rate:" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in m_str {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    for &b in b"(" {
        if pos < line.len() {
            line[pos] = b;
            pos += 1;
        }
    }
    push_decimal(m_rate as u32, &mut line, &mut pos);
    if pos < line.len() {
        line[pos] = b')';
        pos += 1;
    }
    if pos < line.len() {
        line[pos] = b'\n';
        pos += 1;
    }
    (pos, line)
}

#[cfg(config_rtw_debug)]
#[no_mangle]
pub extern "C" fn dump_hw_rate_map_test(sel: *mut c_void) {
    unsafe {
        for rs in 0..RATE_SECTION_NUM {
            let ent = &rates_by_sections[rs as usize];
            for i in 0..ent.rate_num {
                let m_rate = *ent.rates.add(i as usize);
                let hw_rate = MRateToHwRate(m_rate);
                let (len, line) = format_m_to_hw_line(m_rate, hw_rate);
                print_line(sel, &line[..len]);
            }
            if rs == HT_4SS {
                let hw_rate = MRateToHwRate(MGN_MCS32);
                let (len, line) = format_m_to_hw_line(MGN_MCS32, hw_rate);
                print_line(sel, &line[..len]);
            }
        }
        let hw_rate = MRateToHwRate(MGN_UNKNOWN);
        let (len, line) = format_m_to_hw_line(MGN_UNKNOWN, hw_rate);
        print_line(sel, &line[..len]);

        let mut i = DESC_RATE1M as i32;
        while i <= DESC_RATE_NUM as i32 {
            let m_rate = hw_rate_to_m_rate(i as U8);
            let (len, line) = format_hw_to_m_line(i as U8, m_rate);
            print_line(sel, &line[..len]);
            i += 1;
        }
    }
}

#[cfg(any(
    host_hal_com_rsvd_page_test,
    all(not(host_hal_com_hw_rate_test), not(host_hal_com_chip_info_test))
))]
#[repr(C)]
pub struct RsvdPageCache {
    pub name: *mut i8,
    pub loc: U8,
    pub page_num: U8,
    pub data: *mut U8,
    pub size: u32,
}

#[cfg(any(
    host_hal_com_rsvd_page_test,
    all(not(host_hal_com_hw_rate_test), not(host_hal_com_chip_info_test))
))]
fn page_num(len: u32, page_size: u32) -> U8 {
    let whole = len / page_size;
    let rem = len & (page_size - 1);
    (whole + if rem != 0 { 1 } else { 0 }) as U8
}

#[cfg(host_hal_com_rsvd_page_test)]
extern "C" {
    fn rtw_malloc(sz: u32) -> *mut u8;
    fn rtw_zmalloc(sz: u32) -> *mut u8;
    fn rtw_mfree(p: *mut U8, sz: u32);
    fn _rtw_memcmp(a: *const u8, b: *const u8, sz: u32) -> i32;
    fn rtw_warn_on(cond: i32);
}

#[cfg(all(
    not(host_hal_com_rsvd_page_test),
    not(host_hal_com_hw_rate_test),
    not(host_hal_com_chip_info_test)
))]
extern "C" {
    fn _rtw_malloc(sz: u32) -> *mut core::ffi::c_void;
    fn _rtw_zmalloc(sz: u32) -> *mut core::ffi::c_void;
    fn _rtw_mfree(p: *mut core::ffi::c_void, sz: u32);
    fn _rtw_memcmp(a: *const core::ffi::c_void, b: *const core::ffi::c_void, sz: u32) -> i32;
    fn rtw_rust_hal_com_warn_on(condition: i32);
}

#[cfg(any(
    host_hal_com_rsvd_page_test,
    all(not(host_hal_com_hw_rate_test), not(host_hal_com_chip_info_test))
))]
const RSVD_TRUE: i32 = 1;

#[cfg(any(
    host_hal_com_rsvd_page_test,
    all(not(host_hal_com_hw_rate_test), not(host_hal_com_chip_info_test))
))]
fn rsvd_warn_on(cond: bool) {
    if !cond {
        return;
    }
    #[cfg(host_hal_com_rsvd_page_test)]
    unsafe {
        rtw_warn_on(1);
    }
    #[cfg(not(host_hal_com_rsvd_page_test))]
    unsafe {
        rtw_rust_hal_com_warn_on(1);
    }
}

#[cfg(any(
    host_hal_com_rsvd_page_test,
    all(not(host_hal_com_hw_rate_test), not(host_hal_com_chip_info_test))
))]
fn rsvd_heap_alloc(sz: u32, zero: bool) -> *mut U8 {
    unsafe {
        #[cfg(host_hal_com_rsvd_page_test)]
        let p = if zero {
            rtw_zmalloc(sz)
        } else {
            rtw_malloc(sz)
        };
        #[cfg(not(host_hal_com_rsvd_page_test))]
        let p = if zero {
            _rtw_zmalloc(sz)
        } else {
            _rtw_malloc(sz)
        };
        p as *mut U8
    }
}

#[cfg(any(
    host_hal_com_rsvd_page_test,
    all(not(host_hal_com_hw_rate_test), not(host_hal_com_chip_info_test))
))]
fn rsvd_mfree(ptr: *mut U8, sz: u32) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        #[cfg(host_hal_com_rsvd_page_test)]
        {
            rtw_mfree(ptr, sz);
        }
        #[cfg(not(host_hal_com_rsvd_page_test))]
        {
            _rtw_mfree(ptr as *mut core::ffi::c_void, sz); // kernel shim
        }
    }
}

#[cfg(any(
    host_hal_com_rsvd_page_test,
    all(not(host_hal_com_hw_rate_test), not(host_hal_com_chip_info_test))
))]
fn rsvd_memcmp_eq(a: *const U8, b: *const U8, len: u32) -> bool {
    if len == 0 {
        return true;
    }
    if a.is_null() || b.is_null() {
        return false;
    }
    unsafe {
        #[cfg(host_hal_com_rsvd_page_test)]
        let eq = _rtw_memcmp(a, b, len) == RSVD_TRUE;
        #[cfg(not(host_hal_com_rsvd_page_test))]
        let eq = _rtw_memcmp(
            a as *const core::ffi::c_void,
            b as *const core::ffi::c_void,
            len,
        ) == RSVD_TRUE;
        eq
    }
}

#[cfg(any(
    host_hal_com_rsvd_page_test,
    all(not(host_hal_com_hw_rate_test), not(host_hal_com_chip_info_test))
))]
#[no_mangle]
pub extern "C" fn rsvd_page_cache_update_all(
    cache: *mut RsvdPageCache,
    loc: U8,
    txdesc_len: U8,
    page_size: u32,
    info: *mut U8,
    info_len: u32,
) -> u8 {
    let cache = unsafe { &mut *cache };
    let mut modified = false;
    let mut loc_mod = false;
    let mut size_mod = false;
    let mut page_num_mod = false;

    let mut eff_loc = loc;
    let page_n = if info_len != 0 {
        page_num(txdesc_len as u32 + info_len, page_size)
    } else {
        0
    };
    if info_len == 0 {
        eff_loc = 0;
    }

    if cache.loc != eff_loc {
        loc_mod = true;
    }
    if cache.size != info_len {
        size_mod = true;
    }
    if cache.page_num != page_n {
        page_num_mod = true;
    }

    if !info.is_null() && info_len != 0 {
        if !cache.data.is_null() {
            if cache.size == info_len {
                if !rsvd_memcmp_eq(cache.data, info, info_len) {
                    modified = true;
                }
            } else {
                rsvd_page_cache_free_data(cache);
            }
        }
        if cache.data.is_null() {
            let ptr = rsvd_heap_alloc(info_len, false);
            if ptr.is_null() {
                rsvd_warn_on(true);
            } else {
                cache.data = ptr;
            }
            modified = true;
        }
        if !cache.data.is_null() && modified {
            unsafe {
                core::ptr::copy_nonoverlapping(info, cache.data, info_len as usize);
            }
        }
    } else if !cache.data.is_null() && size_mod {
        rsvd_page_cache_free_data(cache);
    }

    cache.loc = eff_loc;
    cache.page_num = page_n;
    cache.size = info_len;

    (modified || loc_mod || size_mod || page_num_mod) as u8
}

#[cfg(any(
    host_hal_com_rsvd_page_test,
    all(not(host_hal_com_hw_rate_test), not(host_hal_com_chip_info_test))
))]
#[no_mangle]
pub extern "C" fn rsvd_page_cache_update_data(
    cache: *mut RsvdPageCache,
    info: *mut U8,
    info_len: u32,
) -> u8 {
    let cache = unsafe { &mut *cache };
    let mut modified = false;

    if info.is_null() || info_len == 0 {
        return 0;
    }
    if cache.loc == 0 || cache.page_num == 0 || cache.size == 0 {
        rsvd_warn_on(true);
        return 0;
    }
    if cache.size != info_len {
        rsvd_warn_on(true);
        return 0;
    }
    if cache.data.is_null() {
        let ptr = rsvd_heap_alloc(cache.size, true);
        if ptr.is_null() {
            rsvd_warn_on(true);
            return 0;
        }
        cache.data = ptr;
        modified = true;
    }
    if !rsvd_memcmp_eq(cache.data, info, cache.size) {
        unsafe {
            core::ptr::copy_nonoverlapping(info, cache.data, cache.size as usize);
        }
        modified = true;
    }
    modified as u8
}

#[cfg(any(
    host_hal_com_rsvd_page_test,
    all(not(host_hal_com_hw_rate_test), not(host_hal_com_chip_info_test))
))]
#[no_mangle]
pub extern "C" fn rsvd_page_cache_free_data(cache: *mut RsvdPageCache) {
    let cache = unsafe { &mut *cache };
    if !cache.data.is_null() {
        unsafe {
            rsvd_mfree(cache.data, cache.size);
        }
        cache.data = core::ptr::null_mut();
    }
}

#[cfg(any(
    host_hal_com_rsvd_page_test,
    all(not(host_hal_com_hw_rate_test), not(host_hal_com_chip_info_test))
))]
#[no_mangle]
pub extern "C" fn rsvd_page_cache_free(cache: *mut RsvdPageCache) {
    let cache = unsafe { &mut *cache };
    cache.loc = 0;
    cache.page_num = 0;
    rsvd_page_cache_free_data(cache);
    cache.size = 0;
}

#[cfg(any(
    host_hal_com_chip_info_test,
    all(not(host_hal_com_hw_rate_test), not(host_hal_com_rsvd_page_test))
))]
mod chip_info {
    use super::U8;

    #[repr(C)]
    pub struct HalVersion {
        pub ic_type: i32,
        pub chip_type: i32,
        pub cut_version: i32,
        pub vendor_type: i32,
        pub rf_type: i32,
        pub rom_ver: U8,
    }

    const IC_TAGS: &[(i32, &str)] = &[
        (5, "CHIP_8188E_"),
        (12, "CHIP_8188F_"),
        (18, "CHIP_8188GTV_"),
        (6, "CHIP_8812_"),
        (9, "CHIP_8192E_"),
        (7, "CHIP_8821_"),
        (8, "CHIP_8723B_"),
        (11, "CHIP_8703B_"),
        (14, "CHIP_8723D_"),
        (10, "CHIP_8814A_"),
        (13, "CHIP_8822B_"),
        (15, "CHIP_8821C_"),
        (16, "CHIP_8710B_"),
        (17, "CHIP_8192F_"),
        (19, "CHIP_8822C_"),
        (20, "CHIP_8814B_"),
        (21, "CHIP_8723F_"),
    ];

    fn push(buf: &mut [u8], pos: &mut usize, s: &str) -> bool {
        let b = s.as_bytes();
        if *pos + b.len() >= buf.len() {
            return false;
        }
        buf[*pos..*pos + b.len()].copy_from_slice(b);
        *pos += b.len();
        true
    }

    fn push_u8(buf: &mut [u8], pos: &mut usize, n: u8) -> bool {
        let mut t = [0u8; 3];
        let mut l = 0usize;
        let mut v = n;
        loop {
            t[l] = b'0' + (v % 10);
            l += 1;
            v /= 10;
            if v == 0 {
                break;
            }
        }
        while l > 0 {
            l -= 1;
            if *pos + 1 >= buf.len() {
                return false;
            }
            buf[*pos] = t[l];
            *pos += 1;
        }
        true
    }

    fn ic_tag(ic: i32) -> &'static str {
        for (id, tag) in IC_TAGS {
            if *id == ic {
                return tag;
            }
        }
        "CHIP_UNKNOWN_"
    }

    fn cut_suffix(cut: i32) -> Option<&'static str> {
        match cut {
            0 => Some("1_"),
            1 => Some("2_"),
            2 => Some("3_"),
            3 => Some("4_"),
            4 => Some("5_"),
            5 => Some("6_"),
            8 => Some("9_"),
            9 => Some("10_"),
            10 => Some("11_"),
            _ => None,
        }
    }

    fn rf_suffix(rf: i32) -> Option<&'static str> {
        match rf {
            0 => Some("1T1R_"),
            1 => Some("1T2R_"),
            2 => Some("2T2R_"),
            5 => Some("3T3R_"),
            6 => Some("3T4R_"),
            7 => Some("4T4R_"),
            _ => None,
        }
    }

    pub fn format_chip_info(v: HalVersion, buf: &mut [u8]) -> i32 {
        let mut pos = 0usize;
        if !push(buf, &mut pos, "Chip Version Info: ")
            || !push(buf, &mut pos, ic_tag(v.ic_type))
            || (v.chip_type != 1 && !push(buf, &mut pos, "T_"))
        {
            return -1;
        }
        match v.vendor_type {
            0 => {
                if !push(buf, &mut pos, "T") {
                    return -1;
                }
            }
            1 => {
                if !push(buf, &mut pos, "U") {
                    return -1;
                }
            }
            2 => {
                if !push(buf, &mut pos, "S") {
                    return -1;
                }
            }
            _ => {}
        }
        match cut_suffix(v.cut_version) {
            Some(s) => {
                if !push(buf, &mut pos, s) {
                    return -1;
                }
            }
            None => {
                if !push(buf, &mut pos, "UNKNOWN_Cv(")
                    || !push_u8(buf, &mut pos, v.cut_version as u8)
                    || !push(buf, &mut pos, ")_")
                {
                    return -1;
                }
            }
        }
        match rf_suffix(v.rf_type) {
            Some(s) => {
                if !push(buf, &mut pos, s) {
                    return -1;
                }
            }
            None => {
                if !push(buf, &mut pos, "UNKNOWN_RFTYPE(")
                    || !push_u8(buf, &mut pos, v.rf_type as u8)
                    || !push(buf, &mut pos, ")_")
                {
                    return -1;
                }
            }
        }
        if !push(buf, &mut pos, "RomVer(")
            || !push_u8(buf, &mut pos, v.rom_ver)
            || !push(buf, &mut pos, ")\n")
        {
            return -1;
        }
        if pos < buf.len() {
            buf[pos] = 0;
        }
        pos as i32
    }

    #[cfg(all(
        not(host_hal_com_chip_info_test),
        not(host_hal_com_hw_rate_test),
        not(host_hal_com_rsvd_page_test)
    ))]
    extern "C" {
        fn rtw_rust_hal_com_log_info(line: *const core::ffi::c_char);
    }

    #[cfg(all(
        not(host_hal_com_chip_info_test),
        not(host_hal_com_hw_rate_test),
        not(host_hal_com_rsvd_page_test)
    ))]
    #[no_mangle]
    pub extern "C" fn dump_chip_info(chip_version: HalVersion) {
        let mut buf = [0u8; 128];
        let n = format_chip_info(chip_version, &mut buf);
        if n > 0 {
            unsafe {
                rtw_rust_hal_com_log_info(buf.as_ptr() as *const core::ffi::c_char);
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn dump_chip_info_format(
        chip_version: HalVersion,
        out: *mut u8,
        buflen: usize,
    ) -> i32 {
        if out.is_null() || buflen == 0 {
            return -1;
        }
        let buf = unsafe { core::slice::from_raw_parts_mut(out, buflen) };
        format_chip_info(chip_version, buf)
    }
}

#[cfg(all(
    not(host_hal_com_hw_rate_test),
    not(host_hal_com_rsvd_page_test),
    not(host_hal_com_chip_info_test)
))]
extern "C" {
    fn rtw_mi_hal_dump_macaddr(sel: *mut c_void, adapter: *mut c_void);
    #[cfg(config_mi_with_mbssid_cam)]
    fn rtw_mbid_cam_dump(
        sel: *mut c_void,
        fun_name: *const c_char,
        adapter: *mut c_void,
    ) -> i32;
}

#[cfg(all(
    not(host_hal_com_hw_rate_test),
    not(host_hal_com_rsvd_page_test),
    not(host_hal_com_chip_info_test)
))]
#[no_mangle]
pub extern "C" fn rtw_hal_dump_macaddr(sel: *mut c_void, adapter: *mut c_void) {
    #[cfg(config_mi_with_mbssid_cam)]
    unsafe {
        static FN: &[u8] = b"rtw_hal_dump_macaddr\0";
        rtw_mbid_cam_dump(sel, FN.as_ptr() as *const c_char, adapter);
    }
    #[cfg(not(config_mi_with_mbssid_cam))]
    unsafe {
        rtw_mi_hal_dump_macaddr(sel, adapter);
    }
}
