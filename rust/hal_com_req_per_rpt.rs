// SPDX-License-Identifier: GPL-2.0
// H2C request PER report cmd (W4-118).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;

const H2C_REQ_PER_RPT: U8 = 0x4e;
const H2C_REQ_PER_RPT_LEN: U32 = 5;

#[inline]
fn bit_len_mask_8(bit_len: u32) -> U8 {
    (1u8 << bit_len) - 1
}

#[inline]
fn bit_len_mask_32(bit_len: u32) -> U32 {
    if bit_len >= 32 {
        u32::MAX
    } else {
        (1u32 << bit_len) - 1
    }
}

#[inline]
fn le_bits_cleared_to_1byte(p: *const U8, bit_offset: u32, bit_len: u32) -> U8 {
    unsafe {
        let val = *p;
        let mask = bit_len_mask_8(bit_len) << bit_offset;
        val & !mask
    }
}

#[inline]
fn write_le4byte(p: *mut U8, value: U32) {
    unsafe {
        *p = (value & 0xff) as U8;
        *p.add(1) = ((value >> 8) & 0xff) as U8;
        *p.add(2) = ((value >> 16) & 0xff) as U8;
        *p.add(3) = ((value >> 24) & 0xff) as U8;
    }
}

#[inline]
fn le_bits_cleared_to_4byte(p: *const U8, bit_offset: u32, bit_len: u32) -> U32 {
    unsafe {
        let val = (p as u32)
            | ((*p.add(1) as u32) << 8)
            | ((*p.add(2) as u32) << 16)
            | ((*p.add(3) as u32) << 24);
        let mask = bit_len_mask_32(bit_len) << bit_offset;
        val & !mask
    }
}

#[inline]
fn set_bits_to_le_1byte(p: *mut U8, bit_offset: u32, bit_len: u32, value: U8) {
    if bit_offset == 0 && bit_len == 8 {
        unsafe { *p = value };
        return;
    }
    let cleared = le_bits_cleared_to_1byte(p, bit_offset, bit_len);
    let mask = bit_len_mask_8(bit_len);
    unsafe { *p = cleared | ((value & mask) << bit_offset) };
}

#[inline]
fn set_bits_to_le_4byte(p: *mut U8, bit_offset: u32, bit_len: u32, value: U32) {
    if bit_offset == 0 && bit_len == 32 {
        write_le4byte(p, value);
        return;
    }
    let cleared = le_bits_cleared_to_4byte(p, bit_offset, bit_len);
    let mask = bit_len_mask_32(bit_len);
    write_le4byte(p, cleared | ((value & mask) << bit_offset));
}

extern "C" {
    fn rtw_hal_fill_h2c_cmd(
        adapter: *mut c_void,
        element_id: U8,
        cmd_len: U32,
        cmd_buf: *mut U8,
    ) -> i32;
}

#[cfg(rtw_per_cmd_support_fw)]
#[no_mangle]
pub extern "C" fn rtw_hal_set_req_per_rpt_cmd(
    adapter: *mut c_void,
    group_macid: U8,
    rpt_type: U8,
    macid_bitmap: U32,
) -> U8 {
    let mut cmd_buf = [0u8; H2C_REQ_PER_RPT_LEN as usize];

    set_bits_to_le_1byte(cmd_buf.as_mut_ptr(), 0, 4, group_macid);
    set_bits_to_le_1byte(cmd_buf.as_mut_ptr(), 4, 4, rpt_type);
    set_bits_to_le_4byte(cmd_buf[1..].as_mut_ptr(), 0, 32, macid_bitmap);

    unsafe {
        rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_REQ_PER_RPT,
            H2C_REQ_PER_RPT_LEN,
            cmd_buf.as_mut_ptr(),
        ) as U8
    }
}