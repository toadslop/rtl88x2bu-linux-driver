// SPDX-License-Identifier: GPL-2.0
//! W3-63 WMM IE restructure — host L2 oracle and kernel port.

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    private_interfaces,
    unused_imports,
    unreachable_pub
)]

#[cfg(host_mlme_wmm_rsn_test)]
use std::os::raw::{c_int, c_void};

#[cfg(all(not(host_mlme_wmm_rsn_test), rust_mlme_wmm_rsn))]
use core::ffi::{c_int, c_void};

type U8 = u8;
type U16 = u16;
type U32 = u32;

#[cfg(host_mlme_wmm_rsn_test)]
mod host {
    use super::*;

    #[repr(C)]
    pub struct QosPriv {
        pub uapsd_max_sp_len: U8,
        pub uapsd_tid: U16,
    }

    #[repr(C)]
    pub struct MlmePriv {
        pub qospriv: QosPriv,
    }

    #[repr(C)]
    pub struct Adapter {
        pub mlmepriv: MlmePriv,
        pub scratch: [U8; 256],
    }
}

#[cfg(host_mlme_wmm_rsn_test)]
use host::Adapter;

#[cfg(all(not(host_mlme_wmm_rsn_test), rust_mlme_wmm_rsn))]
type Adapter = c_void;

#[cfg(all(not(host_mlme_wmm_rsn_test), rust_mlme_wmm_rsn))]
extern "C" {
    fn rtw_mlme_wmm_rsn_qos(a: *mut Adapter) -> *mut U8;
}

#[no_mangle]
pub extern "C" fn rtw_restruct_wmm_ie(
    adapter: *mut Adapter,
    in_ie: *mut U8,
    out_ie: *mut U8,
    in_len: U32,
    initial_out_len: U32,
) -> U32 {
    if adapter.is_null() || in_ie.is_null() || out_ie.is_null() {
        return initial_out_len;
    }
    let mut ielength = initial_out_len;
    let mut i = 12u32;
    let mut qos_info = 0u8;
    while i < in_len {
        ielength = initial_out_len;
        unsafe {
            if *in_ie.add(i as usize) == 0xDD
                && *in_ie.add(i as usize + 2) == 0x00
                && *in_ie.add(i as usize + 3) == 0x50
                && *in_ie.add(i as usize + 4) == 0xF2
                && *in_ie.add(i as usize + 5) == 0x02
                && i + 5 < in_len
            {
                for j in i..i + 9 {
                    *out_ie.add(ielength as usize) = *in_ie.add(j as usize);
                    ielength += 1;
                }
                *out_ie.add(initial_out_len as usize + 1) = 0x07;
                *out_ie.add(initial_out_len as usize + 6) = 0x00;
                let (max_sp, tid) = qos_fields(adapter);
                match max_sp {
                    1 => qos_info |= 1 << 5,
                    2 => qos_info |= 1 << 6,
                    3 => {
                        qos_info |= 1 << 5;
                        qos_info |= 1 << 6;
                    }
                    _ => {}
                }
                if (tid & 0x80 != 0) && (tid & 0x40 != 0) {
                    qos_info |= 1;
                }
                if (tid & 0x20 != 0) && (tid & 0x10 != 0) {
                    qos_info |= 2;
                }
                if (tid & 0x04 != 0) && (tid & 0x02 != 0) {
                    qos_info |= 4;
                }
                if (tid & 0x08 != 0) && (tid & 0x01 != 0) {
                    qos_info |= 8;
                }
                *out_ie.add(initial_out_len as usize + 8) = qos_info;
                break;
            }
            i += (*in_ie.add(i as usize + 1) as u32) + 2;
        }
    }
    ielength
}

fn qos_fields(adapter: *mut Adapter) -> (U8, U16) {
    unsafe {
        #[cfg(host_mlme_wmm_rsn_test)]
        {
            let pq = &(*adapter).mlmepriv.qospriv;
            (pq.uapsd_max_sp_len, pq.uapsd_tid)
        }
        #[cfg(all(not(host_mlme_wmm_rsn_test), rust_mlme_wmm_rsn))]
        {
            let q = rtw_mlme_wmm_rsn_qos(adapter);
            (*q, *(q.add(2) as *const U16))
        }
    }
}
