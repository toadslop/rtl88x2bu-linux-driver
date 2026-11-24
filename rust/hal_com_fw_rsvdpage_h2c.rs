// SPDX-License-Identifier: GPL-2.0
// rtw_hal_set_FwRsvdPage_cmd — H2C reserved page locations (W4-136).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;

const H2C_RSVD_PAGE: U8 = 0x00;
const H2C_RSVDPAGE_LOC_LEN: U32 = 5;

#[repr(C)]
pub struct RsvdPageLoc {
    pub loc_probe_rsp: U8,
    pub loc_ps_poll: U8,
    pub loc_null_data: U8,
    pub loc_qos_null: U8,
    pub loc_bt_qos_null: U8,
}

#[cfg(not(host_hal_com_fw_rsvdpage_h2c_test))]
extern "C" {
    fn rtw_hal_fill_h2c_cmd(
        adapter: *mut c_void,
        element_id: U8,
        cmd_len: U32,
        cmd_buf: *mut U8,
    ) -> i32;
    fn rtw_rust_hal_com_log_fw_rsvdpage_loc(a: U8, b: U8, c: U8, d: U8, e: U8);
}

#[cfg(host_hal_com_fw_rsvdpage_h2c_test)]
extern "C" {
    fn rtw_hal_fill_h2c_cmd(
        adapter: *mut c_void,
        element_id: U8,
        cmd_len: U32,
        cmd_buf: *mut U8,
    ) -> i32;
}

#[no_mangle]
pub extern "C" fn rtw_hal_set_FwRsvdPage_cmd(
    adapter: *mut c_void,
    rsvdpageloc: *const RsvdPageLoc,
) {
    let loc = unsafe { &*rsvdpageloc };
    let mut parm = [0u8; H2C_RSVDPAGE_LOC_LEN as usize];

    #[cfg(not(host_hal_com_fw_rsvdpage_h2c_test))]
    unsafe {
        rtw_rust_hal_com_log_fw_rsvdpage_loc(
            loc.loc_probe_rsp,
            loc.loc_ps_poll,
            loc.loc_null_data,
            loc.loc_qos_null,
            loc.loc_bt_qos_null,
        );
    }

    parm[0] = loc.loc_probe_rsp;
    parm[1] = loc.loc_ps_poll;
    parm[2] = loc.loc_null_data;
    parm[3] = loc.loc_qos_null;
    parm[4] = loc.loc_bt_qos_null;

    unsafe {
        let _ = rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_RSVD_PAGE,
            H2C_RSVDPAGE_LOC_LEN,
            parm.as_mut_ptr(),
        );
    }
}