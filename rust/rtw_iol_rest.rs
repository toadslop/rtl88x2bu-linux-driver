// SPDX-License-Identifier: GPL-2.0
//! IOL append encoders — Rust port of `core/rtw_iol_rest.c` (W3-50).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(not(host_iol_test))]
use core::ffi::c_void;
#[cfg(host_iol_test)]
use std::os::raw::c_void;

type U8 = u8;
type U16 = u16;
type U32 = u32;
const _SUCCESS: i32 = 1;
const _FAIL: i32 = 0;

// Values from `include/rtw_iol.h` (IOL_CMD_*).
const IOL_CMD_LLT: U8 = 0x00;
const IOL_CMD_WB_REG: U8 = 0x02;
const IOL_CMD_WW_REG: U8 = 0x03;
const IOL_CMD_WD_REG: U8 = 0x04;
const IOL_CMD_DELAY_US: U8 = 0x80;
const IOL_CMD_DELAY_MS: U8 = 0x81;
const IOL_CMD_END: U8 = 0x83;

#[repr(C)]
struct IolCmd {
    rsvd0: U8,
    cmd: U8,
    address: U16,
    value: U32,
}

#[cfg(host_iol_test)]
const TXDESC_OFFSET: U16 = 56;
#[cfg(host_iol_test)]
const MAX_XMITBUF_SZ: U32 = 20480;

#[cfg(host_iol_test)]
#[repr(C)]
pub struct HostPktAttrib {
    pub pktlen: U32,
    pub last_txcmdsz: U32,
}
#[cfg(host_iol_test)]
#[repr(C)]
pub struct HostXmitFrame {
    pub attrib: HostPktAttrib,
    pub buf_addr: *mut U8,
}

#[inline]
fn addr_bytes(cmd: &mut IolCmd) -> &mut [U8; 2] {
    unsafe { &mut *(&mut cmd.address as *mut U16 as *mut [U8; 2]) }
}

#[inline]
fn put_be16(dst: &mut [U8; 2], val: U16) {
    dst[0] = (val >> 8) as U8;
    dst[1] = (val & 0xff) as U8;
}

#[inline]
fn put_be32(cmd: &mut IolCmd, val: U32) {
    let dst =
        unsafe { core::slice::from_raw_parts_mut((&mut cmd.value as *mut U32) as *mut U8, 4) };
    dst[0] = (val >> 24) as U8;
    dst[1] = ((val >> 16) & 0xff) as U8;
    dst[2] = ((val >> 8) & 0xff) as U8;
    dst[3] = (val & 0xff) as U8;
}

#[cfg(not(host_iol_test))]
mod kernel {
    use super::*;
    extern "C" {
        fn rtw_rust_iol_attrib_pktlen(x: *mut c_void) -> U32;
        fn rtw_rust_iol_attrib_last_txcmdsz(x: *mut c_void) -> U32;
        fn rtw_rust_iol_set_attrib_lengths(x: *mut c_void, p: U32, l: U32);
        fn rtw_rust_iol_xframe_buf_addr(x: *mut c_void) -> *mut U8;
        fn rtw_rust_iol_txdesc_offset() -> U16;
        fn rtw_rust_iol_max_xmitbuf_sz() -> U32;
        fn _rtw_memcpy(d: *mut c_void, s: *const c_void, n: usize) -> *mut c_void;
        fn rtw_rust_iol_overflow_log(needed: U32, max_sz: U32);
    }
    pub(super) fn ctx(x: *mut c_void) -> (U16, U32, U32, *mut U8, U32) {
        unsafe {
            (
                rtw_rust_iol_txdesc_offset(),
                rtw_rust_iol_attrib_pktlen(x),
                rtw_rust_iol_attrib_last_txcmdsz(x),
                rtw_rust_iol_xframe_buf_addr(x),
                rtw_rust_iol_max_xmitbuf_sz(),
            )
        }
    }
    pub(super) fn finish(x: *mut c_void, p: U32, l: U32) {
        unsafe {
            rtw_rust_iol_set_attrib_lengths(x, p, l);
        }
    }
    pub(super) fn copy(dst: *mut U8, src: *const U8, n: usize) {
        unsafe {
            _rtw_memcpy(dst as *mut c_void, src as *const c_void, n);
        }
    }
    pub(super) fn overflow_log(needed: U32, max_sz: U32) {
        unsafe {
            rtw_rust_iol_overflow_log(needed, max_sz);
        }
    }
}

fn append_cmds(xframe: *mut c_void, iol_cmds: *const U8, cmd_len: U32) -> i32 {
    // Rust hardening: C would dereference NULL; return _FAIL instead.
    if xframe.is_null() || iol_cmds.is_null() {
        return _FAIL;
    }
    let (off, pktlen, last, buf, max_sz) = {
        #[cfg(host_iol_test)]
        {
            let f = unsafe { &mut *(xframe as *mut HostXmitFrame) };
            (
                TXDESC_OFFSET,
                f.attrib.pktlen,
                f.attrib.last_txcmdsz,
                f.buf_addr,
                MAX_XMITBUF_SZ,
            )
        }
        #[cfg(not(host_iol_test))]
        {
            kernel::ctx(xframe)
        }
    };
    if buf.is_null() {
        return _FAIL;
    }
    let needed = off as U32 + pktlen + cmd_len + 8;
    if needed > max_sz {
        #[cfg(not(host_iol_test))]
        kernel::overflow_log(needed, max_sz);
        return _FAIL;
    }
    let dst = unsafe { buf.add(off as usize + pktlen as usize) };
    #[cfg(host_iol_test)]
    unsafe {
        core::ptr::copy_nonoverlapping(iol_cmds, dst, cmd_len as usize);
    }
    #[cfg(not(host_iol_test))]
    {
        kernel::copy(dst, iol_cmds, cmd_len as usize);
    }
    let (np, nl) = (pktlen + cmd_len, last + cmd_len);
    #[cfg(host_iol_test)]
    {
        let f = unsafe { &mut *(xframe as *mut HostXmitFrame) };
        f.attrib.pktlen = np;
        f.attrib.last_txcmdsz = nl;
    }
    #[cfg(not(host_iol_test))]
    {
        kernel::finish(xframe, np, nl);
    }
    _SUCCESS
}

fn append_iol_cmd(xframe: *mut c_void, cmd: IolCmd) -> i32 {
    append_cmds(xframe, (&cmd as *const IolCmd) as *const U8, 8)
}

fn append_reg(xframe: *mut c_void, cmd_id: U8, addr: U16, value: U32) -> i32 {
    let mut cmd = IolCmd {
        rsvd0: 0,
        cmd: cmd_id,
        address: 0,
        value: 0,
    };
    put_be16(addr_bytes(&mut cmd), addr);
    put_be32(&mut cmd, value);
    append_iol_cmd(xframe, cmd)
}

fn append_value(xframe: *mut c_void, cmd_id: U8, value: U32) -> i32 {
    let mut cmd = IolCmd {
        rsvd0: 0,
        cmd: cmd_id,
        address: 0,
        value: 0,
    };
    put_be32(&mut cmd, value);
    append_iol_cmd(xframe, cmd)
}

#[no_mangle]
pub extern "C" fn rtw_IOL_append_cmds(x: *mut c_void, cmds: *mut U8, len: U32) -> i32 {
    append_cmds(x, cmds, len)
}
#[no_mangle]
pub extern "C" fn rtw_IOL_append_LLT_cmd(x: *mut c_void, page: U8) -> i32 {
    append_value(x, IOL_CMD_LLT, page as U32)
}
#[no_mangle]
pub extern "C" fn _rtw_IOL_append_WB_cmd(x: *mut c_void, addr: U16, val: U8) -> i32 {
    append_reg(x, IOL_CMD_WB_REG, addr, val as U32)
}
#[no_mangle]
pub extern "C" fn _rtw_IOL_append_WW_cmd(x: *mut c_void, addr: U16, val: U16) -> i32 {
    append_reg(x, IOL_CMD_WW_REG, addr, val as U32)
}
#[no_mangle]
pub extern "C" fn _rtw_IOL_append_WD_cmd(x: *mut c_void, addr: U16, val: U32) -> i32 {
    append_reg(x, IOL_CMD_WD_REG, addr, val)
}
#[no_mangle]
pub extern "C" fn rtw_IOL_append_DELAY_US_cmd(x: *mut c_void, us: U16) -> i32 {
    append_value(x, IOL_CMD_DELAY_US, us as U32)
}
#[no_mangle]
pub extern "C" fn rtw_IOL_append_DELAY_MS_cmd(x: *mut c_void, ms: U16) -> i32 {
    append_value(x, IOL_CMD_DELAY_MS, ms as U32)
}
#[no_mangle]
pub extern "C" fn rtw_IOL_append_END_cmd(x: *mut c_void) -> i32 {
    append_value(x, IOL_CMD_END, 0)
}
