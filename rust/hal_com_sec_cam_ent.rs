// SPDX-License-Identifier: GPL-2.0
// Security CAM entry pack/unpack (W4-55).

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

#[no_mangle]
pub extern "C" fn rtw_sec_write_cam_ent(
    adapter: *mut core::ffi::c_void,
    id: u8,
    ctrl: u16,
    mac: *mut u8,
    key: *mut u8,
) {
    #[cfg(CONFIG_RTL8192F)]
    let (mut wdata1, mut addr1) = (0u32, 0u8);
    for j in (0i32..=7).rev() {
        let wdata = match j {
            0 => {
                let (m0, m1) = unsafe { (*mac, *mac.add(1)) };
                (ctrl as u32) | ((m0 as u32) << 16) | ((m1 as u32) << 24)
            }
            1 => unsafe {
                let (m2, m3, m4, m5) = (*mac.add(2), *mac.add(3), *mac.add(4), *mac.add(5));
                (m2 as u32) | ((m3 as u32) << 8) | ((m4 as u32) << 16) | ((m5 as u32) << 24)
            },
            6 | 7 => 0,
            _ => unsafe {
                let i = ((j - 2) << 2) as usize;
                let (k0, k1, k2, k3) = (
                    *key.add(i),
                    *key.add(i + 1),
                    *key.add(i + 2),
                    *key.add(i + 3),
                );
                (k0 as u32) | ((k1 as u32) << 8) | ((k2 as u32) << 16) | ((k3 as u32) << 24)
            },
        };
        let addr = (id << 3) + (j as u8);
        #[cfg(CONFIG_RTL8192F)]
        if j == 1 {
            wdata1 = wdata;
            addr1 = addr;
            continue;
        }
        rtw_sec_write_cam(adapter, addr, wdata);
    }
    #[cfg(CONFIG_RTL8192F)]
    rtw_sec_write_cam(adapter, addr1, wdata1);
}
