// SPDX-License-Identifier: GPL-2.0
//! Channel-plan rest helpers — Rust port of `core/rtw_chplan_rest.c` slices (W3-17).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_chplan_rest_test)]
use std::os::raw::c_int;

#[cfg(not(host_chplan_rest_test))]
use core::ffi::{c_int, c_void};

const RTW_CHF_NO_IR: u8 = 1 << 0;
const RTW_CHF_DFS: u8 = 1 << 1;
const MAX_CHANNEL_NUM: usize = 59;
const RTW_CHD_2G_MAX: u8 = 7;
#[cfg(ieee80211_band_5ghz)]
const RTW_CHD_5G_MAX: u8 = 52;

const RTW_DOMAIN_MAP_VER: &str = "54";
const RTW_DOMAIN_MAP_M_VER: &str = "g";
const RTW_COUNTRY_MAP_VER: &str = "27";

#[repr(C)]
pub struct RtChannelInfo {
    pub channel_num: u8,
    pub flags: u8,
}

#[repr(C)]
pub struct CountryChplan {
    pub alpha2: [u8; 2],
    pub chplan: u8,
    pub en_11ac: u8,
}

#[repr(C)]
pub struct ChplanEnt {
    pub regd_2g: u8,
    pub chd_2g: u8,
    pub regd_5g: u8,
    pub chd_5g: u8,
}

#[cfg(host_chplan_rest_test)]
#[repr(C)]
pub struct RegistryPriv {
    pub wireless_mode: u8,
    pub excl_chs: [u8; MAX_CHANNEL_NUM],
}

#[cfg(host_chplan_rest_test)]
#[repr(C)]
pub struct HostCountryChplan {
    pub alpha2: [u8; 2],
    pub chplan: u8,
    pub en_11ac: u8,
}

#[cfg(host_chplan_rest_test)]
#[repr(C)]
pub struct HostRfCtl {
    pub regd_src: u8,
    pub country_ent: *const HostCountryChplan,
    pub channel_plan: u8,
    pub channel_set: [RtChannelInfo; MAX_CHANNEL_NUM],
}

#[cfg(host_chplan_rest_test)]
#[repr(C)]
pub struct HostNdisConfiguration {
    pub length: u32,
    pub beacon_period: u32,
    pub atim_window: u32,
    pub ds_config: u32,
    pub fh_config: [u32; 5],
}

#[cfg(host_chplan_rest_test)]
#[repr(C)]
pub struct HostWlanBssidEx {
    pub length: u32,
    pub mac_address: [u8; 6],
    pub reserved: [u8; 2],
    pub ssid: [u8; 36],
    pub mesh_id: [u8; 36],
    pub privacy: u32,
    pub rssi: i32,
    pub configuration: HostNdisConfiguration,
}

#[cfg(host_chplan_rest_test)]
#[repr(C)]
pub struct HostChplanAdapter {
    pub registrypriv: RegistryPriv,
    pub rf_ctl: HostRfCtl,
}

extern "C" {
    static RTW_ChannelPlanMap: ChplanEnt;
    static RTW_ChannelPlanMap_size: c_int;

    fn rtw_is_channel_plan_valid(id: u8) -> bool;
    fn rtw_ch2freq(chan: c_int) -> c_int;
    fn rtw_chdef_2g_len(chd: u8) -> u8;
    fn rtw_chdef_2g_ch(chd: u8, i: u8) -> u8;
    fn rtw_chdef_2g_attrib(chd: u8) -> u8;
    #[cfg(ieee80211_band_5ghz)]
    fn rtw_chdef_5g_len(chd: u8) -> u8;
    #[cfg(ieee80211_band_5ghz)]
    fn rtw_chdef_5g_ch(chd: u8, i: u8) -> u8;
    #[cfg(ieee80211_band_5ghz)]
    fn rtw_chdef_5g_attrib(chd: u8) -> u8;

    #[cfg(not(host_chplan_rest_test))]
    fn rtw_rust_rfctl_channel_set(adapter: *mut c_void) -> *mut RtChannelInfo;
    #[cfg(not(host_chplan_rest_test))]
    fn rtw_rust_rfctl_country_ent(adapter: *mut c_void) -> *const CountryChplan;
    #[cfg(not(host_chplan_rest_test))]
    fn rtw_rust_chplan_print_str(sel: *mut c_void, s: *const u8);
    #[cfg(not(host_chplan_rest_test))]
    fn rtw_rust_bss_ds_config(bss: *mut c_void) -> u8;
    #[cfg(not(host_chplan_rest_test))]
    fn rtw_rust_chplan_beacon_hint_info(ch: u8);
}

fn is_alpha2_worldwide(alpha2: &[u8; 2]) -> bool {
    alpha2[0] == b'0' && alpha2[1] == b'0'
}

fn rtw_chset_search_ch(ch_set: &[RtChannelInfo], ch: u32) -> i32 {
    if ch == 0 {
        return -1;
    }
    for (i, ent) in ch_set.iter().enumerate() {
        if ent.channel_num == 0 {
            break;
        }
        if ch == ent.channel_num as u32 {
            return i as i32;
        }
    }
    -1
}

fn process_beacon_hint_inner(
    chset: &mut [RtChannelInfo],
    country_alpha2: Option<&[u8; 2]>,
    ch: u8,
) -> u8 {
    const RTW_CHPLAN_BEACON_HINT_NON_WORLD_WIDE: bool = false;
    const RTW_CHPLAN_BEACON_HINT_ON_2G_CH_1_11: bool = false;
    const RTW_CHPLAN_BEACON_HINT_ON_DFS_CH: bool = false;

    let chset_idx = rtw_chset_search_ch(chset, ch as u32);
    if chset_idx < 0 {
        return 0;
    }
    let idx = chset_idx as usize;
    if (chset[idx].flags & RTW_CHF_NO_IR) != 0
        && (RTW_CHPLAN_BEACON_HINT_NON_WORLD_WIDE
            || country_alpha2.is_none()
            || country_alpha2.is_some_and(|a| is_alpha2_worldwide(a)))
        && (RTW_CHPLAN_BEACON_HINT_ON_2G_CH_1_11 || ch > 11)
        && (RTW_CHPLAN_BEACON_HINT_ON_DFS_CH || (chset[idx].flags & RTW_CHF_DFS) == 0)
    {
        #[cfg(not(host_chplan_rest_test))]
        unsafe {
            rtw_rust_chplan_beacon_hint_info(ch);
        }
        chset[idx].flags &= !RTW_CHF_NO_IR;
        return 1;
    }
    0
}

#[cfg(not(host_chplan_rest_test))]
#[no_mangle]
pub extern "C" fn rtw_process_beacon_hint(adapter: *mut c_void, bss: *mut c_void) -> u8 {
    if adapter.is_null() || bss.is_null() {
        return 0;
    }
    let chset = unsafe { rtw_rust_rfctl_channel_set(adapter) };
    if chset.is_null() {
        return 0;
    }
    let country_ent = unsafe { rtw_rust_rfctl_country_ent(adapter) };
    let ch = unsafe { rtw_rust_bss_ds_config(bss) };
    let chset_slice = unsafe { core::slice::from_raw_parts_mut(chset, MAX_CHANNEL_NUM) };
    let country_alpha2 = if country_ent.is_null() {
        None
    } else {
        Some(unsafe { &(*country_ent).alpha2 })
    };
    process_beacon_hint_inner(chset_slice, country_alpha2, ch)
}

#[cfg(host_chplan_rest_test)]
#[no_mangle]
pub extern "C" fn host_rest_process_beacon_hint(
    adapter: *mut HostChplanAdapter,
    bss: *mut HostWlanBssidEx,
) -> u8 {
    if adapter.is_null() || bss.is_null() {
        return 0;
    }
    let adapter = unsafe { &mut *adapter };
    let bss = unsafe { &*bss };
    let country_alpha2 = if adapter.rf_ctl.country_ent.is_null() {
        None
    } else {
        Some(unsafe { &(*adapter.rf_ctl.country_ent).alpha2 })
    };
    process_beacon_hint_inner(
        &mut adapter.rf_ctl.channel_set,
        country_alpha2,
        bss.configuration.ds_config as u8,
    )
}

#[cfg(not(host_chplan_rest_test))]
fn write_u32(mut val: u32, buf: &mut [u8], pos: &mut usize) {
    let mut tmp = [0u8; 10];
    let mut n = 0usize;
    if val == 0 {
        tmp[0] = b'0';
        n = 1;
    } else {
        while val > 0 {
            tmp[n] = b'0' + (val % 10) as u8;
            val /= 10;
            n += 1;
        }
    }
    while n > 0 {
        if *pos < buf.len() {
            buf[*pos] = tmp[n - 1];
            *pos += 1;
        }
        n -= 1;
    }
}

#[cfg(not(host_chplan_rest_test))]
fn write_i32(val: i32, buf: &mut [u8], pos: &mut usize) {
    if val < 0 {
        if *pos < buf.len() {
            buf[*pos] = b'-';
            *pos += 1;
        }
        write_u32((-(val as i64)) as u32, buf, pos);
    } else {
        write_u32(val as u32, buf, pos);
    }
}

#[cfg(not(host_chplan_rest_test))]
fn write_hex_byte_val(val: u8, buf: &mut [u8], pos: &mut usize) {
    const HEX: &[u8; 16] = b"0123456789abcdef";
    if *pos + 1 < buf.len() {
        buf[*pos] = HEX[(val >> 4) as usize];
        *pos += 1;
        buf[*pos] = HEX[(val & 0x0f) as usize];
        *pos += 1;
    }
}

#[cfg(not(host_chplan_rest_test))]
fn write_bytes(src: &[u8], buf: &mut [u8], pos: &mut usize) {
    for &b in src {
        if *pos < buf.len() {
            buf[*pos] = b;
            *pos += 1;
        }
    }
}

#[cfg(not(host_chplan_rest_test))]
fn print_line(sel: *mut c_void, line: &str) {
    unsafe { rtw_rust_chplan_print_str(sel, line.as_ptr()) };
}

#[cfg(not(host_chplan_rest_test))]
fn write_hex_byte(buf: &mut [u8], val: u8) -> usize {
    const HEX: &[u8; 16] = b"0123456789ABCDEF";
    buf[0] = b'0';
    buf[1] = b'x';
    buf[2] = HEX[(val >> 4) as usize];
    buf[3] = HEX[(val & 0x0f) as usize];
    buf[4] = b' ';
    5
}

#[cfg(not(host_chplan_rest_test))]
fn chplan_ent_equal(a: &ChplanEnt, b: &ChplanEnt) -> bool {
    a.regd_2g == b.regd_2g && a.chd_2g == b.chd_2g && a.regd_5g == b.regd_5g && a.chd_5g == b.chd_5g
}

#[cfg(not(host_chplan_rest_test))]
#[no_mangle]
pub extern "C" fn dump_chplan_ver(sel: *mut c_void) {
    let mut buf = [0u8; 12];
    let mut pos = 0;
    write_bytes(RTW_DOMAIN_MAP_VER.as_bytes(), &mut buf, &mut pos);
    write_bytes(RTW_DOMAIN_MAP_M_VER.as_bytes(), &mut buf, &mut pos);
    write_bytes(b"-", &mut buf, &mut pos);
    write_bytes(RTW_COUNTRY_MAP_VER.as_bytes(), &mut buf, &mut pos);
    write_bytes(b"\n", &mut buf, &mut pos);
    if let Ok(line) = core::str::from_utf8(&buf[..pos]) {
        print_line(sel, line);
    }
}

#[cfg(not(host_chplan_rest_test))]
#[no_mangle]
pub extern "C" fn dump_chplan_id_list(sel: *mut c_void) {
    let map_size = unsafe { RTW_ChannelPlanMap_size } as usize;
    let mut first = true;
    for i in 0..map_size {
        if !unsafe { rtw_is_channel_plan_valid(i as u8) } {
            continue;
        }
        let mut buf = [0u8; 8];
        let n = write_hex_byte(&mut buf, i as u8);
        if first {
            first = false;
        }
        buf[n] = 0;
        print_line(sel, core::str::from_utf8(&buf[..n]).unwrap_or(""));
    }
}

#[cfg(not(host_chplan_rest_test))]
fn chdef_2g_same(i: u8, j: u8) -> bool {
    let len_i = unsafe { rtw_chdef_2g_len(i) };
    let len_j = unsafe { rtw_chdef_2g_len(j) };
    if len_i != len_j {
        return false;
    }
    for k in 0..len_i {
        if unsafe { rtw_chdef_2g_ch(i, k) } != unsafe { rtw_chdef_2g_ch(j, k) } {
            return false;
        }
    }
    unsafe { rtw_chdef_2g_attrib(i) == rtw_chdef_2g_attrib(j) }
}

#[cfg(all(not(host_chplan_rest_test), ieee80211_band_5ghz))]
fn chdef_5g_same(i: u8, j: u8) -> bool {
    let len_i = unsafe { rtw_chdef_5g_len(i) };
    let len_j = unsafe { rtw_chdef_5g_len(j) };
    if len_i != len_j {
        return false;
    }
    for k in 0..len_i {
        if unsafe { rtw_chdef_5g_ch(i, k) } != unsafe { rtw_chdef_5g_ch(j, k) } {
            return false;
        }
    }
    unsafe { rtw_chdef_5g_attrib(i) == rtw_chdef_5g_attrib(j) }
}

#[cfg(not(host_chplan_rest_test))]
#[no_mangle]
pub extern "C" fn dump_chplan_test(sel: *mut c_void) {
    let mut buf = [0u8; 96];
    for i in 0..RTW_CHD_2G_MAX {
        for j in 0..i {
            if chdef_2g_same(i, j) {
                let mut pos = 0usize;
                write_bytes(b"2G chd:", &mut buf, &mut pos);
                write_u32(i as u32, &mut buf, &mut pos);
                write_bytes(b" and ", &mut buf, &mut pos);
                write_u32(j as u32, &mut buf, &mut pos);
                write_bytes(b" is the same\n", &mut buf, &mut pos);
                buf[pos] = 0;
                print_line(sel, core::str::from_utf8(&buf[..pos]).unwrap_or(""));
            }
        }
    }
    for i in 0..RTW_CHD_2G_MAX {
        let len = unsafe { rtw_chdef_2g_len(i) };
        for j in 0..len {
            let ch = unsafe { rtw_chdef_2g_ch(i, j) };
            if unsafe { rtw_ch2freq(ch as c_int) } == 0 {
                let mut pos = 0usize;
                write_bytes(b"2G invalid ch:", &mut buf, &mut pos);
                write_u32(ch as u32, &mut buf, &mut pos);
                write_bytes(b" at (", &mut buf, &mut pos);
                write_i32(i as i32, &mut buf, &mut pos);
                write_bytes(b",", &mut buf, &mut pos);
                write_i32(j as i32, &mut buf, &mut pos);
                write_bytes(b")\n", &mut buf, &mut pos);
                buf[pos] = 0;
                print_line(sel, core::str::from_utf8(&buf[..pos]).unwrap_or(""));
            }
        }
    }

    #[cfg(ieee80211_band_5ghz)]
    {
        for i in 0..RTW_CHD_5G_MAX {
            for j in 0..i {
                if chdef_5g_same(i, j) {
                    let mut pos = 0usize;
                    write_bytes(b"5G chd:", &mut buf, &mut pos);
                    write_u32(i as u32, &mut buf, &mut pos);
                    write_bytes(b" and ", &mut buf, &mut pos);
                    write_u32(j as u32, &mut buf, &mut pos);
                    write_bytes(b" is the same\n", &mut buf, &mut pos);
                    buf[pos] = 0;
                    print_line(sel, core::str::from_utf8(&buf[..pos]).unwrap_or(""));
                }
            }
        }
        for i in 0..RTW_CHD_5G_MAX {
            let len = unsafe { rtw_chdef_5g_len(i) };
            for j in 0..len {
                let ch = unsafe { rtw_chdef_5g_ch(i, j) };
                if unsafe { rtw_ch2freq(ch as c_int) } == 0 {
                    let mut pos = 0usize;
                    write_bytes(b"5G invalid ch:", &mut buf, &mut pos);
                    write_u32(ch as u32, &mut buf, &mut pos);
                    write_bytes(b" at (", &mut buf, &mut pos);
                    write_i32(i as i32, &mut buf, &mut pos);
                    write_bytes(b",", &mut buf, &mut pos);
                    write_i32(j as i32, &mut buf, &mut pos);
                    write_bytes(b")\n", &mut buf, &mut pos);
                    buf[pos] = 0;
                    print_line(sel, core::str::from_utf8(&buf[..pos]).unwrap_or(""));
                }
            }
        }
    }

    let map_size = unsafe { RTW_ChannelPlanMap_size } as usize;
    let map =
        unsafe { core::slice::from_raw_parts(&RTW_ChannelPlanMap as *const ChplanEnt, map_size) };
    for i in 0..map_size {
        if !unsafe { rtw_is_channel_plan_valid(i as u8) } {
            continue;
        }
        for j in 0..i {
            if !unsafe { rtw_is_channel_plan_valid(j as u8) } {
                continue;
            }
            if chplan_ent_equal(&map[i], &map[j]) {
                let mut pos = 0usize;
                write_bytes(b"channel plan 0x", &mut buf, &mut pos);
                write_hex_byte_val(i as u8, &mut buf, &mut pos);
                write_bytes(b" and 0x", &mut buf, &mut pos);
                write_hex_byte_val(j as u8, &mut buf, &mut pos);
                write_bytes(b" is the same\n", &mut buf, &mut pos);
                buf[pos] = 0;
                print_line(sel, core::str::from_utf8(&buf[..pos]).unwrap_or(""));
            }
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_chplan_rest_probe() -> c_int {
    0x1717
}
