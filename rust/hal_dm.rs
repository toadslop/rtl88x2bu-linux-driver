// SPDX-License-Identifier: GPL-2.0
//! HAL DM — board-type (`W4-30`), ODM endian hooks (`W4-38`), `dump_sta_info` (`W4-31`), `dump_sta_traffic` (`W4-32`) from `hal/hal_dm.c`.

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

type DmStruct = u8;

const ODM_BOARD_DEFAULT: u8 = 0;
const ODM_BOARD_BT: u8 = 1 << 2;
const ODM_BOARD_EXT_PA: u8 = 1 << 3;
const ODM_BOARD_EXT_LNA: u8 = 1 << 4;
const ODM_BOARD_EXT_PA_5G: u8 = 1 << 6;
const ODM_BOARD_EXT_LNA_5G: u8 = 1 << 7;

const ODM_CMNINFO_BOARD_TYPE: u32 = 12;
const ODM_CMNINFO_EXT_LNA: u32 = 14;
const ODM_CMNINFO_5G_EXT_LNA: u32 = 15;
const ODM_CMNINFO_EXT_PA: u32 = 16;
const ODM_CMNINFO_5G_EXT_PA: u32 = 17;

unsafe extern "C" {
    fn odm_cmn_info_init(dm: *mut DmStruct, cmn_info: u32, value: u64);
}

#[no_mangle]
pub extern "C" fn rtw_rust_hal_dm_init_odm_board_type(
    pdm_odm: *mut DmStruct, // struct dm_struct — opaque
    external_lna_2g: u8,
    external_lna_5g: u8,
    external_pa_2g: u8,
    external_pa_5g: u8,
    eeprom_bt_coexist: u8,
) {
    if pdm_odm.is_null() {
        return;
    }

    let mut odm_board_type = ODM_BOARD_DEFAULT;

    if external_lna_2g != 0 {
        odm_board_type |= ODM_BOARD_EXT_LNA;
        unsafe {
            odm_cmn_info_init(pdm_odm, ODM_CMNINFO_EXT_LNA, 1);
        }
    }
    if external_lna_5g != 0 {
        odm_board_type |= ODM_BOARD_EXT_LNA_5G;
        unsafe {
            odm_cmn_info_init(pdm_odm, ODM_CMNINFO_5G_EXT_LNA, 1);
        }
    }
    if external_pa_2g != 0 {
        odm_board_type |= ODM_BOARD_EXT_PA;
        unsafe {
            odm_cmn_info_init(pdm_odm, ODM_CMNINFO_EXT_PA, 1);
        }
    }
    if external_pa_5g != 0 {
        odm_board_type |= ODM_BOARD_EXT_PA_5G;
        unsafe {
            odm_cmn_info_init(pdm_odm, ODM_CMNINFO_5G_EXT_PA, 1);
        }
    }
    if eeprom_bt_coexist != 0 {
        odm_board_type |= ODM_BOARD_BT;
    }

    unsafe {
        odm_cmn_info_init(pdm_odm, ODM_CMNINFO_BOARD_TYPE, odm_board_type as u64);
    }
}

#[cfg(any(
    host_hal_dm_odm_endian_hooks_test,
    not(any(
        host_hal_dm_board_type_test,
        host_hal_dm_dump_sta_info_test,
        host_hal_dm_dump_sta_traffic_test,
    ))
))]
mod odm_endian_hooks {
    use super::DmStruct;

    const ODM_CMNINFO_BAND: u32 = 54;
    const ODM_CMNINFO_SEC_MODE: u32 = 56;
    const ODM_CMNINFO_BW: u32 = 57;
    const ODM_CMNINFO_NET_CLOSED: u32 = 71;

    unsafe extern "C" {
        fn odm_cmn_info_hook(dm: *mut DmStruct, cmn_info: u32, ptr: *mut u8);
    }

    fn convert_to_big_endian<T>(value: *mut T) -> *mut u8 {
        unsafe { (value as *mut u8).add(core::mem::size_of::<T>() - 1) }
    }

    /// Kernel `BAND_TYPE` / `enum channel_width` (int-sized enums, not `u8`).
    type OdmEnumWord = u32;

    #[no_mangle]
    pub extern "C" fn rtw_rust_hal_dm_init_odm_endian_hooks_le(
        pdm_odm: *mut DmStruct,
        band: *mut OdmEnumWord,
        sec_mode: *mut u32,
        bw: *mut OdmEnumWord,
        net_closed: *mut i32,
    ) {
        if pdm_odm.is_null() {
            return;
        }
        unsafe {
            odm_cmn_info_hook(pdm_odm, ODM_CMNINFO_BAND, band as *mut u8);
            odm_cmn_info_hook(pdm_odm, ODM_CMNINFO_SEC_MODE, sec_mode as *mut u8);
            odm_cmn_info_hook(pdm_odm, ODM_CMNINFO_BW, bw as *mut u8);
            odm_cmn_info_hook(pdm_odm, ODM_CMNINFO_NET_CLOSED, net_closed as *mut u8);
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_rust_hal_dm_init_odm_endian_hooks_be(
        pdm_odm: *mut DmStruct,
        band: *mut OdmEnumWord,
        sec_mode: *mut u32,
        bw: *mut OdmEnumWord,
        net_closed: *mut i32,
    ) {
        if pdm_odm.is_null() {
            return;
        }
        unsafe {
            odm_cmn_info_hook(pdm_odm, ODM_CMNINFO_BAND, convert_to_big_endian(band));
            odm_cmn_info_hook(
                pdm_odm,
                ODM_CMNINFO_SEC_MODE,
                convert_to_big_endian(sec_mode),
            );
            odm_cmn_info_hook(pdm_odm, ODM_CMNINFO_BW, convert_to_big_endian(bw));
            odm_cmn_info_hook(
                pdm_odm,
                ODM_CMNINFO_NET_CLOSED,
                convert_to_big_endian(net_closed),
            );
        }
    }
}

#[cfg(any(
    host_hal_dm_dump_sta_info_test,
    host_hal_dm_dump_sta_traffic_test,
    not(any(
        host_hal_dm_board_type_test,
        host_hal_dm_odm_endian_hooks_test,
        host_hal_dm_dump_sta_info_test,
        host_hal_dm_dump_sta_traffic_test
    ))
))]
mod dump_sta_info {
    #[cfg(host_hal_dm_dump_sta_info_test)]
    use std::os::raw::{c_char, c_void};

    #[cfg(not(host_hal_dm_dump_sta_info_test))]
    use core::ffi::{c_char, c_void};

    type U8 = u8;
    type I8 = i8;

    const SM_PS_STATIC: U8 = 0;

    #[repr(C)]
    pub struct HostHalDmDumpStaFixture {
        pub mac_addr: [U8; 6],
        pub mac_id: U8,
        pub wireless_mode: U8,
        pub mimo_type: U8,
        pub sm_ps: U8,
        pub bw_mode: U8,
        pub ra_bw_mode: U8,
        pub rate_id: U8,
        pub rssi: I8,
        pub rssi_level: U8,
        pub is_support_sgi: U8,
        pub is_vht_enable: U8,
        pub disable_ra: U8,
        pub disable_pt: U8,
        pub is_noisy: U8,
        pub txrx_state: U8,
        pub stub_tx_rate: U8,
        pub stub_tx_sgi: U8,
        pub curr_tx_bw: U8,
        pub curr_retry_ratio: U8,
        pub ramask: u64,
    }

    fn yn(v: bool) -> &'static [u8] {
        if v {
            b"Y"
        } else {
            b"N"
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

    fn push_u8_hex2(buf: &mut [u8], pos: &mut usize, n: U8) -> bool {
        const HEX: &[u8; 16] = b"0123456789abcdef";
        push_byte(buf, pos, HEX[(n >> 4) as usize]) && push_byte(buf, pos, HEX[(n & 0xf) as usize])
    }

    fn push_u64_hex16(buf: &mut [u8], pos: &mut usize, n: u64) -> bool {
        for shift in (0..16).rev() {
            let digit = ((n >> (shift * 4)) & 0xf) as u8;
            const HEX: &[u8; 16] = b"0123456789abcdef";
            if !push_byte(buf, pos, HEX[digit as usize]) {
                return false;
            }
        }
        true
    }

    #[cfg(host_hal_dm_dump_sta_info_test)]
    fn ch_width_str(bw: U8) -> &'static [u8] {
        static WIDTHS: [&[u8]; 7] = [
            b"20MHz",
            b"40MHz",
            b"80MHz",
            b"160MHz",
            b"80_80MHz",
            b"5MHz",
            b"10MHz",
        ];
        if bw as usize >= WIDTHS.len() {
            return b"CHANNEL_WIDTH_MAX";
        }
        WIDTHS[bw as usize]
    }

    #[cfg(host_hal_dm_dump_sta_info_test)]
    fn hdata_rate(rate: U8) -> &'static [u8] {
        extern "C" {
            fn rtw_rust_hal_com_hdata_rate(hw_rate: U8) -> *const c_char;
        }
        c_str_bytes(unsafe { rtw_rust_hal_com_hdata_rate(rate) })
    }

    #[cfg(not(host_hal_dm_dump_sta_info_test))]
    fn ch_width_str(bw: U8) -> &'static [u8] {
        extern "C" {
            fn rtw_rust_hal_dm_ch_width_str(bw: U8) -> *const c_char;
        }
        c_str_bytes(unsafe { rtw_rust_hal_dm_ch_width_str(bw) })
    }

    #[cfg(not(host_hal_dm_dump_sta_info_test))]
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

    fn format_dump_sta(
        fx: &HostHalDmDumpStaFixture,
        curr_tx_rate: U8,
        curr_tx_sgi: U8,
        buf: &mut [u8],
    ) -> i32 {
        let mut pos = 0usize;

        if !push_bytes(buf, &mut pos, b"============ STA [") {
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
        if !push_bytes(buf, &mut pos, b"]  ===================\nmac_id : ") {
            return -1;
        }
        if !push_u8_dec(buf, &mut pos, fx.mac_id) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b"\nwireless_mode : 0x") {
            return -1;
        }
        if !push_u8_hex2(buf, &mut pos, fx.wireless_mode) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b"\nmimo_type : ") {
            return -1;
        }
        if !push_u8_dec(buf, &mut pos, fx.mimo_type) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b"\nstatic smps : ") {
            return -1;
        }
        if !push_bytes(buf, &mut pos, yn(fx.sm_ps == SM_PS_STATIC)) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b"\nbw_mode : ") {
            return -1;
        }
        if !push_bytes(buf, &mut pos, ch_width_str(fx.bw_mode)) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b", ra_bw_mode : ") {
            return -1;
        }
        if !push_bytes(buf, &mut pos, ch_width_str(fx.ra_bw_mode)) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b"\nrate_id : ") {
            return -1;
        }
        if !push_u8_dec(buf, &mut pos, fx.rate_id) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b"\nrssi : ") {
            return -1;
        }
        if !push_i8_dec(buf, &mut pos, fx.rssi) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b" (%), rssi_level : ") {
            return -1;
        }
        if !push_u8_dec(buf, &mut pos, fx.rssi_level) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b"\nis_support_sgi : ") {
            return -1;
        }
        if !push_bytes(buf, &mut pos, yn(fx.is_support_sgi != 0)) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b", is_vht_enable : ") {
            return -1;
        }
        if !push_bytes(buf, &mut pos, yn(fx.is_vht_enable != 0)) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b"\ndisable_ra : ") {
            return -1;
        }
        if !push_bytes(buf, &mut pos, yn(fx.disable_ra != 0)) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b", disable_pt : ") {
            return -1;
        }
        if !push_bytes(buf, &mut pos, yn(fx.disable_pt != 0)) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b"\nis_noisy : ") {
            return -1;
        }
        if !push_bytes(buf, &mut pos, yn(fx.is_noisy != 0)) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b"\ntxrx_state : ") {
            return -1;
        }
        if !push_u8_dec(buf, &mut pos, fx.txrx_state) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b"\ncurr_tx_rate : ") {
            return -1;
        }
        if !push_bytes(buf, &mut pos, hdata_rate(curr_tx_rate)) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b" (") {
            return -1;
        }
        if !push_bytes(buf, &mut pos, if curr_tx_sgi != 0 { b"S" } else { b"L" }) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b")\ncurr_tx_bw : ") {
            return -1;
        }
        if !push_bytes(buf, &mut pos, ch_width_str(fx.curr_tx_bw)) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b"\ncurr_retry_ratio : ") {
            return -1;
        }
        if !push_u8_dec(buf, &mut pos, fx.curr_retry_ratio) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b"\nra_mask : 0x") {
            return -1;
        }
        if !push_u64_hex16(buf, &mut pos, fx.ramask) {
            return -1;
        }
        if !push_bytes(buf, &mut pos, b"\n\n") {
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

    #[cfg(not(host_hal_dm_dump_sta_info_test))]
    #[repr(C)]
    struct RtwHalDmStaDumpFields {
        mac_addr: [U8; 6],
        mac_id: U8,
        wireless_mode: U8,
        mimo_type: U8,
        sm_ps: U8,
        bw_mode: U8,
        ra_bw_mode: U8,
        rate_id: U8,
        rssi: I8,
        rssi_level: U8,
        is_support_sgi: U8,
        is_vht_enable: U8,
        disable_ra: U8,
        disable_pt: U8,
        is_noisy: U8,
        txrx_state: U8,
        curr_tx_bw: U8,
        curr_retry_ratio: U8,
        ramask: u64,
    }

    #[cfg(not(host_hal_dm_dump_sta_info_test))]
    fn fields_to_fixture(f: &RtwHalDmStaDumpFields) -> HostHalDmDumpStaFixture {
        HostHalDmDumpStaFixture {
            mac_addr: f.mac_addr,
            mac_id: f.mac_id,
            wireless_mode: f.wireless_mode,
            mimo_type: f.mimo_type,
            sm_ps: f.sm_ps,
            bw_mode: f.bw_mode,
            ra_bw_mode: f.ra_bw_mode,
            rate_id: f.rate_id,
            rssi: f.rssi,
            rssi_level: f.rssi_level,
            is_support_sgi: f.is_support_sgi,
            is_vht_enable: f.is_vht_enable,
            disable_ra: f.disable_ra,
            disable_pt: f.disable_pt,
            is_noisy: f.is_noisy,
            txrx_state: f.txrx_state,
            stub_tx_rate: 0,
            stub_tx_sgi: 0,
            curr_tx_bw: f.curr_tx_bw,
            curr_retry_ratio: f.curr_retry_ratio,
            ramask: f.ramask,
        }
    }

    #[cfg(host_hal_dm_dump_sta_info_test)]
    #[no_mangle]
    pub extern "C" fn dump_sta_info_format(
        fx: *const HostHalDmDumpStaFixture,
        buf: *mut u8,
        buflen: usize,
    ) -> i32 {
        if fx.is_null() || buf.is_null() || buflen == 0 {
            return -1;
        }
        let fx = unsafe { &*fx };
        let out = unsafe { core::slice::from_raw_parts_mut(buf, buflen) };
        let n = format_dump_sta(fx, fx.stub_tx_rate, fx.stub_tx_sgi, out);
        if n > 0 && (n as usize) < buflen {
            out[n as usize] = 0;
        }
        n
    }

    #[cfg(not(host_hal_dm_dump_sta_info_test))]
    #[no_mangle]
    pub extern "C" fn dump_sta_info(sel: *mut c_void, psta: *mut c_void) {
        if psta.is_null() {
            return;
        }
        extern "C" {
            fn rtw_rust_hal_dm_sta_fields(psta: *mut c_void, out: *mut RtwHalDmStaDumpFields);
            fn rtw_rust_hal_dm_current_tx_rate(adapter: *mut c_void, psta: *mut c_void) -> U8;
            fn rtw_rust_hal_dm_current_tx_sgi(adapter: *mut c_void, psta: *mut c_void) -> U8;
        }
        let mut fields = RtwHalDmStaDumpFields {
            mac_addr: [0; 6],
            mac_id: 0,
            wireless_mode: 0,
            mimo_type: 0,
            sm_ps: 0,
            bw_mode: 0,
            ra_bw_mode: 0,
            rate_id: 0,
            rssi: 0,
            rssi_level: 0,
            is_support_sgi: 0,
            is_vht_enable: 0,
            disable_ra: 0,
            disable_pt: 0,
            is_noisy: 0,
            txrx_state: 0,
            curr_tx_bw: 0,
            curr_retry_ratio: 0,
            ramask: 0,
        };
        unsafe {
            rtw_rust_hal_dm_sta_fields(psta, &mut fields);
        }
        let fx = fields_to_fixture(&fields);
        extern "C" {
            fn rtw_rust_hal_dm_sta_adapter(psta: *mut c_void) -> *mut c_void;
        }
        let adapter = unsafe { rtw_rust_hal_dm_sta_adapter(psta) };
        let curr_tx_rate = unsafe { rtw_rust_hal_dm_current_tx_rate(adapter, psta) };
        let curr_tx_sgi = unsafe { rtw_rust_hal_dm_current_tx_sgi(adapter, psta) };
        let mut buf = [0u8; 1024];
        let n = format_dump_sta(&fx, curr_tx_rate, curr_tx_sgi, &mut buf);
        if n > 0 {
            dump_lines(sel, &buf[..n as usize], n as usize);
        }
    }
}

#[cfg(any(
    host_hal_dm_dump_sta_traffic_test,
    not(any(
        host_hal_dm_board_type_test,
        host_hal_dm_odm_endian_hooks_test,
        host_hal_dm_dump_sta_info_test,
        host_hal_dm_dump_sta_traffic_test
    ))
))]
mod dump_sta_traffic {
    include!("hal_dm_dump_sta_traffic.inc.rs");
}
