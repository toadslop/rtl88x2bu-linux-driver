// SPDX-License-Identifier: GPL-2.0
//! W3-115 sdio cmd52/53 I/O wrappers — Rust port (host L2 scope).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_sdio_cmd_test)]
use std::os::raw::{c_int, c_void};

#[cfg(host_sdio_cmd_test)]
const _FAIL: u8 = 0;
#[cfg(host_sdio_cmd_test)]
const _SUCCESS: u8 = 1;
#[cfg(host_sdio_cmd_test)]
const SD_IO_TRY_CNT: c_int = 8;
#[cfg(host_sdio_cmd_test)]
const MAX_CONTINUAL_IO_ERR: c_int = SD_IO_TRY_CNT;
#[cfg(host_sdio_cmd_test)]
const _TRUE: u8 = 1;
#[cfg(host_sdio_cmd_test)]
const _FALSE: u8 = 0;

#[cfg(host_sdio_cmd_test)]
#[repr(C)]
pub struct host_adapter {
    pub surprise_removed: u8,
}

#[cfg(host_sdio_cmd_test)]
#[repr(C)]
pub struct host_dvobj {
    pub primary_adapter: *mut host_adapter,
    pub intf_ops: *mut host_sdio_if_ops,
    pub continual_io_error: c_int,
    pub io_fail_remaining: c_int,
    pub io_mock_err: c_int,
    pub read_fill: u8,
}

#[cfg(host_sdio_cmd_test)]
#[repr(C)]
pub struct host_sdio_if_ops {
    pub read:
        Option<unsafe extern "C" fn(*mut host_dvobj, u32, *mut c_void, usize, c_int) -> c_int>,
    pub write:
        Option<unsafe extern "C" fn(*mut host_dvobj, u32, *mut c_void, usize, c_int) -> c_int>,
}

#[cfg(host_sdio_cmd_test)]
static mut G_AD: host_adapter = host_adapter {
    surprise_removed: 0,
};
#[cfg(host_sdio_cmd_test)]
static mut G_OPS: host_sdio_if_ops = host_sdio_if_ops {
    read: None,
    write: None,
};
#[cfg(host_sdio_cmd_test)]
static mut G_DV: host_dvobj = host_dvobj {
    primary_adapter: std::ptr::null_mut(),
    intf_ops: std::ptr::null_mut(),
    continual_io_error: 0,
    io_fail_remaining: 0,
    io_mock_err: 1,
    read_fill: 0xA5,
};
#[cfg(host_sdio_cmd_test)]
static mut G_LAST_ADDR: u32 = 0;
#[cfg(host_sdio_cmd_test)]
static mut G_IO_COUNT: c_int = 0;

#[cfg(host_sdio_cmd_test)]
unsafe fn mock_io_err(d: &mut host_dvobj) -> c_int {
    let err = if d.io_mock_err != 0 {
        d.io_mock_err
    } else {
        1
    };
    G_IO_COUNT += 1;
    d.io_fail_remaining -= 1;
    err
}

#[cfg(host_sdio_cmd_test)]
unsafe extern "C" fn mock_read(
    d: *mut host_dvobj,
    addr: u32,
    buf: *mut c_void,
    len: usize,
    _fixed: c_int,
) -> c_int {
    let d = &mut *d;
    if d.io_fail_remaining > 0 {
        return mock_io_err(d);
    }
    G_LAST_ADDR = addr;
    G_IO_COUNT += 1;
    std::ptr::write_bytes(buf as *mut u8, d.read_fill, len);
    0
}

#[cfg(host_sdio_cmd_test)]
unsafe extern "C" fn mock_write(
    d: *mut host_dvobj,
    addr: u32,
    _buf: *mut c_void,
    _len: usize,
    _fixed: c_int,
) -> c_int {
    let d = &mut *d;
    if d.io_fail_remaining > 0 {
        return mock_io_err(d);
    }
    G_LAST_ADDR = addr;
    G_IO_COUNT += 1;
    0
}

#[cfg(host_sdio_cmd_test)]
unsafe fn host_inc_and_chk_continual_io_error(d: &mut host_dvobj) -> u8 {
    d.continual_io_error += 1;
    if d.continual_io_error > MAX_CONTINUAL_IO_ERR {
        _TRUE
    } else {
        _FALSE
    }
}

#[cfg(host_sdio_cmd_test)]
unsafe fn sdio_io(
    d: *mut host_dvobj,
    addr: u32,
    buf: *mut u8,
    len: usize,
    write: u8,
    cmd52: u8,
) -> u8 {
    let dv = &mut *d;
    let addr_drv = if cmd52 != 0 {
        addr | (1u32 << 17)
    } else {
        addr
    };
    let mut retry = 0u8;

    if (*dv.primary_adapter).surprise_removed != 0 {
        return _FAIL;
    }

    loop {
        let ops = &*dv.intf_ops;
        let err = if write != 0 {
            ops.write.unwrap()(d, addr_drv, buf as *mut c_void, len, 0)
        } else {
            ops.read.unwrap()(d, addr_drv, buf as *mut c_void, len, 0)
        };
        if err == 0 {
            dv.continual_io_error = 0;
            break;
        }
        retry += 1;
        let stop_retry = host_inc_and_chk_continual_io_error(dv);
        if err == -1 || stop_retry == _TRUE || retry > SD_IO_TRY_CNT as u8 {
            host_sdio_cmd_set_surprise(1);
            return _FAIL;
        }
        if (addr & 0x10000) != 0 || (addr & 0xE000) == 0 {
            continue;
        }
        return _FAIL;
    }
    _SUCCESS
}

#[cfg(host_sdio_cmd_test)]
#[no_mangle]
pub extern "C" fn host_sdio_cmd_reset() {
    unsafe {
        G_AD.surprise_removed = 0;
        G_OPS.read = Some(mock_read);
        G_OPS.write = Some(mock_write);
        G_DV.primary_adapter = std::ptr::addr_of_mut!(G_AD);
        G_DV.intf_ops = std::ptr::addr_of_mut!(G_OPS);
        G_DV.continual_io_error = 0;
        G_DV.io_fail_remaining = 0;
        G_DV.io_mock_err = 1;
        G_DV.read_fill = 0xA5;
        G_LAST_ADDR = 0;
        G_IO_COUNT = 0;
    }
}

#[cfg(host_sdio_cmd_test)]
#[no_mangle]
pub extern "C" fn host_sdio_cmd_set_surprise(on: u8) {
    unsafe {
        G_AD.surprise_removed = if on != 0 { 1 } else { 0 };
    }
}

#[cfg(host_sdio_cmd_test)]
#[no_mangle]
pub extern "C" fn host_sdio_cmd_surprise() -> u8 {
    unsafe { G_AD.surprise_removed }
}

#[cfg(host_sdio_cmd_test)]
#[no_mangle]
pub extern "C" fn host_sdio_cmd_last_addr() -> u32 {
    unsafe { G_LAST_ADDR }
}

#[cfg(host_sdio_cmd_test)]
#[no_mangle]
pub extern "C" fn host_sdio_cmd_io_count() -> c_int {
    unsafe { G_IO_COUNT }
}

#[cfg(host_sdio_cmd_test)]
#[no_mangle]
pub extern "C" fn host_sdio_cmd_dvobj() -> *mut host_dvobj {
    std::ptr::addr_of_mut!(G_DV)
}

#[cfg(host_sdio_cmd_test)]
#[no_mangle]
pub extern "C" fn rtw_sdio_read_cmd52(
    d: *mut host_dvobj,
    addr: u32,
    buf: *mut u8,
    len: usize,
) -> u8 {
    unsafe { sdio_io(d, addr, buf, len, 0, 1) }
}

#[cfg(host_sdio_cmd_test)]
#[no_mangle]
pub extern "C" fn rtw_sdio_read_cmd53(
    d: *mut host_dvobj,
    addr: u32,
    buf: *mut u8,
    len: usize,
) -> u8 {
    unsafe { sdio_io(d, addr, buf, len, 0, 0) }
}

#[cfg(host_sdio_cmd_test)]
#[no_mangle]
pub extern "C" fn rtw_sdio_write_cmd52(
    d: *mut host_dvobj,
    addr: u32,
    buf: *mut u8,
    len: usize,
) -> u8 {
    unsafe { sdio_io(d, addr, buf, len, 1, 1) }
}

#[cfg(host_sdio_cmd_test)]
#[no_mangle]
pub extern "C" fn rtw_sdio_write_cmd53(
    d: *mut host_dvobj,
    addr: u32,
    buf: *mut u8,
    len: usize,
) -> u8 {
    unsafe { sdio_io(d, addr, buf, len, 1, 0) }
}

#[cfg(host_sdio_cmd_test)]
#[no_mangle]
pub extern "C" fn rtw_sdio_f0_read(d: *mut host_dvobj, addr: u32, buf: *mut u8, len: usize) -> u8 {
    unsafe {
        let addr = addr | (1u32 << 18);
        let ops = &*(*d).intf_ops;
        let err = ops.read.unwrap()(d, addr, buf as *mut c_void, len, 0);
        if err != 0 {
            _FAIL
        } else {
            _SUCCESS
        }
    }
}
