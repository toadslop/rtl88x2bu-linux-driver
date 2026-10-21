// SPDX-License-Identifier: GPL-2.0
// Security CAM MMIO read/write (W4-52). Included from hal_com_reqtxrpt_mi_mac.rs.

type Systime = core::ffi::c_ulong;

const REG_CAMCMD: u32 = 0x0670;
const REG_CAMWRITE: u32 = 0x0674;
const REG_CAMREAD: u32 = 0x0678;
const CAM_WRITE: u32 = 1 << 16;
const CAM_POLLINIG: u32 = 1 << 31;
const SEC_CAM_ACCESS_TIMEOUT_MS: u32 = 200;

extern "C" {
    fn rtw_rust_hal_com_sec_cam_mutex(adapter: *mut core::ffi::c_void) -> *mut core::ffi::c_void;
    fn rtw_rust_hal_com_enter_critical_mutex(mutex: *mut core::ffi::c_void);
    fn rtw_rust_hal_com_exit_critical_mutex(mutex: *mut core::ffi::c_void);
    fn rtw_rust_hal_com_read32(adapter: *mut core::ffi::c_void, addr: u32) -> u32;
    fn rtw_rust_hal_com_write32(adapter: *mut core::ffi::c_void, addr: u32, val: u32);
    fn rtw_rust_hal_com_is_surprise_removed(adapter: *mut core::ffi::c_void) -> u8;
    fn rtw_rust_hal_com_get_current_time() -> Systime;
    fn rtw_rust_hal_com_get_passing_time_ms(start: Systime) -> u32;
    fn rtw_rust_hal_com_log_sec_cam(
        adapter: *mut core::ffi::c_void,
        is_write: u8,
        addr: u8,
        data: u32,
        timeout: u8,
        cnt: u32,
        start: Systime,
        end: Systime,
    );
}

#[inline]
fn poll_cam_cmd(adapter: *mut core::ffi::c_void) -> (u8, u32, Systime, Systime) {
    let mut cnt: u32 = 0;
    let mut timeout: u8 = 0;
    let start = unsafe { rtw_rust_hal_com_get_current_time() };
    loop {
        if unsafe { rtw_rust_hal_com_is_surprise_removed(adapter) } != 0 {
            break;
        }
        cnt += 1;
        if unsafe { rtw_rust_hal_com_read32(adapter, REG_CAMCMD) } & CAM_POLLINIG == 0 {
            break;
        }
        if unsafe { rtw_rust_hal_com_get_passing_time_ms(start) } > SEC_CAM_ACCESS_TIMEOUT_MS {
            timeout = 1;
            break;
        }
    }
    (timeout, cnt, start, unsafe { rtw_rust_hal_com_get_current_time() })
}

#[no_mangle]
pub extern "C" fn rtw_sec_read_cam(adapter: *mut core::ffi::c_void, addr: u8) -> u32 {
    let mutex = unsafe { rtw_rust_hal_com_sec_cam_mutex(adapter) };
    unsafe { rtw_rust_hal_com_enter_critical_mutex(mutex) };
    unsafe { rtw_rust_hal_com_write32(adapter, REG_CAMCMD, CAM_POLLINIG | (addr as u32)) };
    let (timeout, cnt, start, end) = poll_cam_cmd(adapter);
    let rdata = unsafe { rtw_rust_hal_com_read32(adapter, REG_CAMREAD) };
    unsafe { rtw_rust_hal_com_exit_critical_mutex(mutex) };
    unsafe {
        rtw_rust_hal_com_log_sec_cam(adapter, 0, addr, rdata, timeout, cnt, start, end);
    }
    rdata
}

#[no_mangle]
pub extern "C" fn rtw_sec_write_cam(adapter: *mut core::ffi::c_void, addr: u8, wdata: u32) {
    let mutex = unsafe { rtw_rust_hal_com_sec_cam_mutex(adapter) };
    unsafe { rtw_rust_hal_com_enter_critical_mutex(mutex) };
    unsafe {
        rtw_rust_hal_com_write32(adapter, REG_CAMWRITE, wdata);
        rtw_rust_hal_com_write32(adapter, REG_CAMCMD, CAM_POLLINIG | CAM_WRITE | (addr as u32));
    };
    let (timeout, cnt, start, end) = poll_cam_cmd(adapter);
    unsafe { rtw_rust_hal_com_exit_critical_mutex(mutex) };
    unsafe {
        rtw_rust_hal_com_log_sec_cam(adapter, 1, addr, wdata, timeout, cnt, start, end);
    }
}
