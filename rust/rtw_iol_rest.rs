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

#[cfg(host_iol_test)]
use std::os::raw::c_void;

#[cfg(not(host_iol_test))]
use core::ffi::c_void;

type U8 = u8;
type U16 = u16;
type U32 = u32;

const _SUCCESS: i32 = 1;
const _FAIL: i32 = 0;

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
fn put_be16(dst: &mut [U8; 2], val: U16) {
    dst[0] = (val >> 8) as U8;
    dst[1] = (val & 0xff) as U8;
}

#[inline]
fn put_be32_words(cmd: &mut IolCmd, val: U32) {
    let dst = unsafe {
        core::slice::from_raw_parts_mut((&mut cmd.value as *mut U32) as *mut U8, 4)
    };
    dst[0] = (val >> 24) as U8;
    dst[1] = ((val >> 16) & 0xff) as U8;
    dst[2] = ((val >> 8) & 0xff) as U8;
    dst[3] = (val & 0xff) as U8;
}

#[cfg(not(host_iol_test))]
mod kernel {
    use super::*;

    extern "C" {
        fn rtw_rust_iol_attrib_pktlen(xframe: *mut c_void) -> U32;
        fn rtw_rust_iol_attrib_last_txcmdsz(xframe: *mut c_void) -> U32;
        fn rtw_rust_iol_set_attrib_lengths(xframe: *mut c_void, pktlen: U32, last_txcmdsz: U32);
        fn rtw_rust_iol_xframe_buf_addr(xframe: *mut c_void) -> *mut U8;
        fn rtw_rust_iol_txdesc_offset() -> U16;
        fn rtw_rust_iol_max_xmitbuf_sz() -> U32;
        fn _rtw_memcpy(dest: *mut c_void, src: *const c_void, n: usize) -> *mut c_void;
    }

    pub(super) fn pktlen(xframe: *mut c_void) -> U32 {
        unsafe { rtw_rust_iol_attrib_pktlen(xframe) }
    }

    pub(super) fn last_txcmdsz(xframe: *mut c_void) -> U32 {
        unsafe { rtw_rust_iol_attrib_last_txcmdsz(xframe) }
    }

    pub(super) fn set_lengths(xframe: *mut c_void, pktlen: U32, last_txcmdsz: U32) {
        unsafe { rtw_rust_iol_set_attrib_lengths(xframe, pktlen, last_txcmdsz) }
    }

    pub(super) fn buf_addr(xframe: *mut c_void) -> *mut U8 {
        unsafe { rtw_rust_iol_xframe_buf_addr(xframe) }
    }

    pub(super) fn txdesc_offset() -> U16 {
        unsafe { rtw_rust_iol_txdesc_offset() }
    }

    pub(super) fn max_xmitbuf_sz() -> U32 {
        unsafe { rtw_rust_iol_max_xmitbuf_sz() }
    }

    pub(super) fn memcpy(dest: *mut U8, src: *const U8, len: usize) {
        unsafe {
            _rtw_memcpy(dest as *mut c_void, src as *const c_void, len);
        }
    }
}

fn append_cmds(xframe: *mut c_void, iol_cmds: *const U8, cmd_len: U32) -> i32 {
    if xframe.is_null() || iol_cmds.is_null() {
        return _FAIL;
    }

    let (buf_offset, pktlen, last_txcmdsz, buf_addr, max_sz) = {
        #[cfg(host_iol_test)]
        {
            let frame = unsafe { &mut *(xframe as *mut HostXmitFrame) };
            (
                TXDESC_OFFSET,
                frame.attrib.pktlen,
                frame.attrib.last_txcmdsz,
                frame.buf_addr,
                MAX_XMITBUF_SZ,
            )
        }
        #[cfg(not(host_iol_test))]
        {
            (
                kernel::txdesc_offset(),
                kernel::pktlen(xframe),
                kernel::last_txcmdsz(xframe),
                kernel::buf_addr(xframe),
                kernel::max_xmitbuf_sz(),
            )
        }
    };

    if buf_addr.is_null() {
        return _FAIL;
    }

    let ori_len = buf_offset as U32 + pktlen;
    if ori_len + cmd_len + 8 > max_sz {
        return _FAIL;
    }

    let dst = unsafe { buf_addr.add(buf_offset as usize + pktlen as usize) };
    unsafe {
        core::ptr::copy_nonoverlapping(iol_cmds, dst, cmd_len as usize);
    }

    let new_pktlen = pktlen + cmd_len;
    let new_last = last_txcmdsz + cmd_len;
    #[cfg(host_iol_test)]
    {
        let frame = unsafe { &mut *(xframe as *mut HostXmitFrame) };
        frame.attrib.pktlen = new_pktlen;
        frame.attrib.last_txcmdsz = new_last;
    }
    #[cfg(not(host_iol_test))]
    {
        kernel::set_lengths(xframe, new_pktlen, new_last);
    }

    _SUCCESS
}

fn append_iol_cmd(xframe: *mut c_void, cmd: IolCmd) -> i32 {
    append_cmds(xframe, (&cmd as *const IolCmd) as *const U8, 8)
}

#[no_mangle]
pub extern "C" fn rtw_IOL_append_cmds(
    xmit_frame: *mut c_void,
    iol_cmds: *mut U8,
    cmd_len: U32,
) -> i32 {
    append_cmds(xmit_frame, iol_cmds, cmd_len)
}

#[no_mangle]
pub extern "C" fn rtw_IOL_append_LLT_cmd(xmit_frame: *mut c_void, page_boundary: U8) -> i32 {
    let mut cmd = IolCmd {
        rsvd0: 0,
        cmd: IOL_CMD_LLT,
        address: 0,
        value: 0,
    };
    put_be32_words(&mut cmd, page_boundary as U32);
    append_iol_cmd(xmit_frame, cmd)
}

#[no_mangle]
pub extern "C" fn _rtw_IOL_append_WB_cmd(
    xmit_frame: *mut c_void,
    addr: U16,
    value: U8,
) -> i32 {
    let mut cmd = IolCmd {
        rsvd0: 0,
        cmd: IOL_CMD_WB_REG,
        address: 0,
        value: 0,
    };
    put_be16(
        unsafe {
            &mut *(&mut cmd.address as *mut U16 as *mut [U8; 2])
        },
        addr,
    );
    put_be32_words(&mut cmd, value as U32);
    append_iol_cmd(xmit_frame, cmd)
}

#[no_mangle]
pub extern "C" fn _rtw_IOL_append_WW_cmd(
    xmit_frame: *mut c_void,
    addr: U16,
    value: U16,
) -> i32 {
    let mut cmd = IolCmd {
        rsvd0: 0,
        cmd: IOL_CMD_WW_REG,
        address: 0,
        value: 0,
    };
    put_be16(
        unsafe {
            &mut *(&mut cmd.address as *mut U16 as *mut [U8; 2])
        },
        addr,
    );
    put_be32_words(&mut cmd, value as U32);
    append_iol_cmd(xmit_frame, cmd)
}

#[no_mangle]
pub extern "C" fn _rtw_IOL_append_WD_cmd(
    xmit_frame: *mut c_void,
    addr: U16,
    value: U32,
) -> i32 {
    let mut cmd = IolCmd {
        rsvd0: 0,
        cmd: IOL_CMD_WD_REG,
        address: 0,
        value: 0,
    };
    put_be16(
        unsafe {
            &mut *(&mut cmd.address as *mut U16 as *mut [U8; 2])
        },
        addr,
    );
    put_be32_words(&mut cmd, value);
    append_iol_cmd(xmit_frame, cmd)
}

#[no_mangle]
pub extern "C" fn rtw_IOL_append_DELAY_US_cmd(xmit_frame: *mut c_void, us: U16) -> i32 {
    let mut cmd = IolCmd {
        rsvd0: 0,
        cmd: IOL_CMD_DELAY_US,
        address: 0,
        value: 0,
    };
    put_be32_words(&mut cmd, us as U32);
    append_iol_cmd(xmit_frame, cmd)
}

#[no_mangle]
pub extern "C" fn rtw_IOL_append_DELAY_MS_cmd(xmit_frame: *mut c_void, ms: U16) -> i32 {
    let mut cmd = IolCmd {
        rsvd0: 0,
        cmd: IOL_CMD_DELAY_MS,
        address: 0,
        value: 0,
    };
    put_be32_words(&mut cmd, ms as U32);
    append_iol_cmd(xmit_frame, cmd)
}

#[no_mangle]
pub extern "C" fn rtw_IOL_append_END_cmd(xmit_frame: *mut c_void) -> i32 {
    let cmd = IolCmd {
        rsvd0: 0,
        cmd: IOL_CMD_END,
        address: 0,
        value: 0,
    };
    append_iol_cmd(xmit_frame, cmd)
}
