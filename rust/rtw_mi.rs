// SPDX-License-Identifier: GPL-2.0
//! W3-121 mi channel union helpers (host L2 Rust oracle).
#![allow(dead_code, improper_ctypes, missing_docs, non_snake_case)]

const ASOC: i32 = 0x00000001;
const LINK: i32 = 0x00000080;
const OP_SW: i32 = 0x00800000;

#[cfg(host_mi_ch_union_test)]
#[repr(C)]
pub struct MlmePriv {
    pub fw_state: i32,
}
#[cfg(host_mi_ch_union_test)]
#[repr(C)]
pub struct MlmeExtPriv {
    pub cur_channel: u8,
    pub cur_bwmode: u8,
    pub cur_ch_offset: u8,
}
#[cfg(host_mi_ch_union_test)]
#[repr(C)]
pub struct DvobjPriv {
    pub iface_nums: u8,
    pub union_ch: u8,
    pub union_bw: u8,
    pub union_offset: u8,
    pub union_ch_bak: u8,
    pub union_bw_bak: u8,
    pub union_offset_bak: u8,
    pub padapters: [*mut Adapter; 4],
}
#[cfg(host_mi_ch_union_test)]
#[repr(C)]
pub struct Adapter {
    pub iface_id: u8,
    pub mlmepriv: MlmePriv,
    pub mlmeextpriv: MlmeExtPriv,
    pub oper_ch: u8,
    pub oper_bw: u8,
    pub oper_offset: u8,
    pub dvobj: *mut DvobjPriv,
}

#[cfg(host_mi_ch_union_test)]
fn linked(m: &MlmePriv, st: i32) -> bool {
    (st == 0 && m.fw_state == 0) || (m.fw_state & st) != 0
}

#[cfg(host_mi_ch_union_test)]
#[no_mangle]
pub unsafe extern "C" fn mi_rust_update_union(a: *mut Adapter, ch: u8, off: u8, bw: u8) {
    let a = &mut *a;
    let d = &mut *a.dvobj;
    if ch == 0 {
        d.union_ch_bak = d.union_ch;
        d.union_bw_bak = d.union_bw;
        d.union_offset_bak = d.union_offset;
    }
    d.union_ch = ch;
    d.union_bw = bw;
    d.union_offset = off;
}

#[cfg(host_mi_ch_union_test)]
#[no_mangle]
pub unsafe extern "C" fn mi_rust_stay_ch(a: *mut Adapter) -> u8 {
    let a = &*a;
    let d = &*a.dvobj;
    let uc = if d.union_ch != 0 {
        d.union_ch
    } else {
        d.union_ch_bak
    };
    let ub = if d.union_ch != 0 {
        d.union_bw
    } else {
        d.union_bw_bak
    };
    let uo = if d.union_ch != 0 {
        d.union_offset
    } else {
        d.union_offset_bak
    };
    u8::from(uc == a.oper_ch && ub == a.oper_bw && uo == a.oper_offset)
}

#[cfg(host_mi_ch_union_test)]
#[no_mangle]
pub unsafe extern "C" fn mi_rust_union_ifbmp(
    d: *mut DvobjPriv,
    ifbmp: u8,
    ch: *mut u8,
    bw: *mut u8,
    off: *mut u8,
) -> i32 {
    let d = &*d;
    let mut ch_r = 0u8;
    let mut bw_r = 0u8;
    let mut off_r = 0u8;
    let mut n = 0i32;
    if !ch.is_null() {
        *ch = 0;
    }
    if !bw.is_null() {
        *bw = 0;
    }
    if !off.is_null() {
        *off = 0;
    }
    for i in 0..d.iface_nums as usize {
        let Some(iface) = (|| {
            let p = d.padapters[i];
            if p.is_null() {
                return None;
            }
            Some(&*p)
        })() else {
            continue;
        };
        if ifbmp & (1u8 << iface.iface_id) == 0 {
            continue;
        }
        let mx = &iface.mlmeextpriv;
        if !linked(&iface.mlmepriv, ASOC | LINK) || linked(&iface.mlmepriv, OP_SW) {
            continue;
        }
        if n == 0 {
            ch_r = mx.cur_channel;
            bw_r = mx.cur_bwmode;
            off_r = mx.cur_ch_offset;
            n = 1;
            continue;
        }
        if ch_r != mx.cur_channel {
            return 0;
        }
        if bw_r < mx.cur_bwmode {
            bw_r = mx.cur_bwmode;
            off_r = mx.cur_ch_offset;
        } else if bw_r == mx.cur_bwmode && off_r != mx.cur_ch_offset {
            return 0;
        }
        n += 1;
    }
    if n > 0 {
        if !ch.is_null() {
            *ch = ch_r;
        }
        if !bw.is_null() {
            *bw = bw_r;
        }
        if !off.is_null() {
            *off = off_r;
        }
    }
    n
}

#[cfg(not(host_mi_ch_union_test))]
pub fn mi_ch_union_stub() {}
