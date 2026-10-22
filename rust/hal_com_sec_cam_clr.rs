// SPDX-License-Identifier: GPL-2.0
// Security CAM clear + GK probe (W4-54). Uses rtw_sec_read_cam / rtw_sec_write_cam (W4-52).

const CAM_CTRL_GK_BIT: u16 = 1 << 6;

#[no_mangle]
pub extern "C" fn rtw_sec_clr_cam_ent(adapter: *mut core::ffi::c_void, id: u8) {
    let addr = id << 3;
    rtw_sec_write_cam(adapter, addr, 0);
}

#[no_mangle]
pub extern "C" fn rtw_sec_read_cam_is_gk(adapter: *mut core::ffi::c_void, id: u8) -> bool {
    let rdata = rtw_sec_read_cam(adapter, (id << 3) | 0);
    let ctrl = rdata as u16;
    (ctrl & CAM_CTRL_GK_BIT) != 0
}
