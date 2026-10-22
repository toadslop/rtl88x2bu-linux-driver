// SPDX-License-Identifier: GPL-2.0
// Security CAM entry read (W4-55 PR1). write in PR2.

extern "C" {
    fn _rtw_memcpy(dst: *mut core::ffi::c_void, src: *const core::ffi::c_void, sz: u32);
    fn rtw_rust_hal_com_warn_on(condition: i32);
}

#[inline]
unsafe fn cp(dst: *mut u8, src: *const u8, n: u32) {
    if !dst.is_null() && !src.is_null() && n != 0 {
        unsafe {
            _rtw_memcpy(
                dst as *mut core::ffi::c_void,
                src as *const core::ffi::c_void,
                n,
            );
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_sec_read_cam_ent(
    adapter: *mut core::ffi::c_void,
    id: u8,
    ctrl: *mut u8,
    mac: *mut u8,
    key: *mut u8,
) {
    if ctrl.is_null() && mac.is_null() && key.is_null() {
        unsafe { rtw_rust_hal_com_warn_on(1) };
        return;
    }
    let (begin, end) = if ctrl.is_null() && mac.is_null() {
        (2u8, if key.is_null() { 0 } else { 5 })
    } else if key.is_null() && mac.is_null() {
        (0, 0)
    } else if key.is_null() {
        (0, 2)
    } else {
        (0, 5)
    };
    let base = id << 3;
    for i in begin..=end {
        let r = rtw_sec_read_cam(adapter, base | i).to_ne_bytes();
        match i {
            0 => {
                unsafe { cp(ctrl, r.as_ptr(), 2) };
                unsafe { cp(mac, r.as_ptr().add(2), 2) };
            }
            1 => unsafe { cp(mac.add(2), r.as_ptr(), 4) },
            _ => unsafe { cp(key.add(((i - 2) * 4) as usize), r.as_ptr(), 4) },
        }
    }
}
