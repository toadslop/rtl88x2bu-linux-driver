// SPDX-License-Identifier: GPL-2.0
// C2H per-rate report handler (W4-58 PR1).

use core::ffi::c_void;

type S32 = i32;

const SUCCESS: S32 = 1;
const FAIL: S32 = 0;

extern "C" {
    fn rtw_rust_hal_com_warn_per_rate_len(len: u8);
    fn rtw_rust_hal_com_warn_per_rate_format();
    fn rtw_rust_hal_com_per_rate_type0_apply(
        adapter: *mut c_void,
        idx: i32,
        mac_id: u8,
        per: u8,
        rate: u8,
        bw: u8,
        total_pkt: u16,
    );
}

#[inline]
fn b(data: *const u8, off: usize) -> u8 {
    unsafe { *data.add(off) }
}

#[inline]
fn le_u16_at(ptr: *const u8) -> u16 {
    unsafe { (ptr.read() as u16) | ((ptr.add(1).read() as u16) << 8) }
}

#[no_mangle]
pub extern "C" fn c2h_per_rate_rpt_hdl(adapter: *mut c_void, data: *mut u8, len: u8) -> S32 {
    let mut mac_id = [0u8; 2];
    let mut per = [0u8; 2];
    let mut rate = [0u8; 2];
    let mut bw = [0u8; 2];
    let mut total_pkt = [0u16; 2];
    let mut macid_cnt = 1i32;

    if len != 7 && len != 12 {
        unsafe { rtw_rust_hal_com_warn_per_rate_len(len) };
        return FAIL;
    }

    mac_id[0] = b(data, 0);
    per[0] = b(data, 1);
    rate[0] = b(data, 2);
    bw[0] = b(data, 3) & 0x3;
    total_pkt[0] = le_u16_at(unsafe { data.add(4) });

    mac_id[1] = b(data, 6);
    if mac_id[1] != 0xff {
        if len != 12 {
            unsafe { rtw_rust_hal_com_warn_per_rate_format() };
            return FAIL;
        }
        macid_cnt = 2;
        per[1] = b(data, 7);
        rate[1] = b(data, 8);
        bw[1] = b(data, 9) & 0x3;
        total_pkt[1] = le_u16_at(unsafe { data.add(10) });
    }

    for i in 0..macid_cnt {
        unsafe {
            rtw_rust_hal_com_per_rate_type0_apply(
                adapter,
                i,
                mac_id[i as usize],
                per[i as usize],
                rate[i as usize],
                bw[i as usize],
                total_pkt[i as usize],
            );
        }
    }
    SUCCESS
}
