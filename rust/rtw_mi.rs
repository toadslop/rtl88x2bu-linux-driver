// SPDX-License-Identifier: GPL-2.0
//! W3-121 mi channel union helpers (host L2 Rust oracle).
#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_snake_case,
    unreachable_pub
)]

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

#[cfg(host_mi_netif_buddy_test)]
#[repr(C)]
pub struct MockNdev {
    pub carrier_on: u8,
    pub queue_stopped: u8,
    pub queue_woken: u8,
}
#[cfg(host_mi_netif_buddy_test)]
#[repr(C)]
pub struct NetDevice {
    pub mock: *mut MockNdev,
}
#[cfg(host_mi_netif_buddy_test)]
#[repr(C)]
pub struct NetifDvobj {
    pub iface_nums: u8,
    pub padapters: [*mut NetifAdapter; 4],
}
#[cfg(host_mi_netif_buddy_test)]
#[repr(C)]
pub struct NetifAdapter {
    pub iface_id: u8,
    pub adapter_up: u8,
    pub pnetdev: *mut NetDevice,
    pub dvobj: *mut NetifDvobj,
}

#[cfg(host_mi_netif_buddy_test)]
type NetifOp = unsafe extern "C" fn(*mut NetifAdapter, *mut core::ffi::c_void) -> u8;

#[cfg(host_mi_netif_buddy_test)]
unsafe fn n_carrier_off(n: *mut NetDevice) {
    if !n.is_null() {
        let n = &*n;
        if !n.mock.is_null() {
            (*n.mock).carrier_on = 0;
        }
    }
}
#[cfg(host_mi_netif_buddy_test)]
unsafe fn n_carrier_on(n: *mut NetDevice) {
    if !n.is_null() {
        let n = &*n;
        if !n.mock.is_null() {
            (*n.mock).carrier_on = 1;
        }
    }
}
#[cfg(host_mi_netif_buddy_test)]
unsafe fn n_stop_queue(n: *mut NetDevice) {
    if !n.is_null() {
        let n = &*n;
        if !n.mock.is_null() {
            (*n.mock).queue_stopped = 1;
        }
    }
}
#[cfg(host_mi_netif_buddy_test)]
unsafe fn n_start_queue(n: *mut NetDevice) {
    if !n.is_null() {
        let n = &*n;
        if !n.mock.is_null() {
            (*n.mock).queue_stopped = 0;
            (*n.mock).queue_woken = 1;
        }
    }
}
#[cfg(host_mi_netif_buddy_test)]
unsafe fn n_wake_queue(n: *mut NetDevice) {
    if !n.is_null() {
        let n = &*n;
        if !n.mock.is_null() {
            (*n.mock).queue_woken = 1;
        }
    }
}

#[cfg(host_mi_netif_buddy_test)]
unsafe fn mi_process_netif(pad: *mut NetifAdapter, ex_self: i32, op: NetifOp) -> u8 {
    let dv = &*(*pad).dvobj;
    let mut ret = 0u8;
    for i in 0..dv.iface_nums as usize {
        let p = dv.padapters[i];
        if p.is_null() {
            continue;
        }
        let iface = &*p;
        if iface.adapter_up == 0 {
            continue;
        }
        if ex_self != 0 && std::ptr::eq(p, pad) {
            continue;
        }
        if op(p, core::ptr::null_mut()) == 1 {
            ret += 1;
        }
    }
    ret
}

#[cfg(host_mi_netif_buddy_test)]
unsafe extern "C" fn op_caroff(a: *mut NetifAdapter, _d: *mut core::ffi::c_void) -> u8 {
    let a = &*a;
    n_carrier_off(a.pnetdev);
    n_stop_queue(a.pnetdev);
    1
}
#[cfg(host_mi_netif_buddy_test)]
unsafe extern "C" fn op_caron(a: *mut NetifAdapter, _d: *mut core::ffi::c_void) -> u8 {
    let a = &*a;
    n_carrier_on(a.pnetdev);
    n_start_queue(a.pnetdev);
    1
}
#[cfg(host_mi_netif_buddy_test)]
unsafe extern "C" fn op_stop(a: *mut NetifAdapter, _d: *mut core::ffi::c_void) -> u8 {
    let a = &*a;
    n_stop_queue(a.pnetdev);
    1
}
#[cfg(host_mi_netif_buddy_test)]
unsafe extern "C" fn op_wake(a: *mut NetifAdapter, _d: *mut core::ffi::c_void) -> u8 {
    let a = &*a;
    if !a.pnetdev.is_null() {
        n_wake_queue(a.pnetdev);
    }
    1
}
#[cfg(host_mi_netif_buddy_test)]
unsafe extern "C" fn op_carr_on(a: *mut NetifAdapter, _d: *mut core::ffi::c_void) -> u8 {
    let a = &*a;
    if !a.pnetdev.is_null() {
        n_carrier_on(a.pnetdev);
    }
    1
}
#[cfg(host_mi_netif_buddy_test)]
unsafe extern "C" fn op_carr_off(a: *mut NetifAdapter, _d: *mut core::ffi::c_void) -> u8 {
    let a = &*a;
    if !a.pnetdev.is_null() {
        n_carrier_off(a.pnetdev);
    }
    1
}

#[cfg(host_mi_netif_buddy_test)]
#[no_mangle]
pub unsafe extern "C" fn mi_rust_call_mi_netif(pad: *mut NetifAdapter, fn_id: i32) -> u8 {
    static OPS: [(NetifOp, i32); 12] = [
        (op_caroff, 0),
        (op_caroff, 1),
        (op_caron, 0),
        (op_caron, 1),
        (op_stop, 0),
        (op_stop, 1),
        (op_wake, 0),
        (op_wake, 1),
        (op_carr_on, 0),
        (op_carr_on, 1),
        (op_carr_off, 0),
        (op_carr_off, 1),
    ];
    if fn_id < 0 || fn_id >= OPS.len() as i32 {
        return 0xff;
    }
    let (op, buddy) = OPS[fn_id as usize];
    mi_process_netif(pad, buddy, op)
}

#[cfg(rust_mi_netif_leaf)]
mod kernel {
    use core::ffi::c_void;

    pub type Padapter = *mut c_void;
    pub type Pnetdev = *mut c_void;

    extern "C" {
        pub fn rtw_rust_mi_iface_nums(padapter: Padapter) -> i32;
        pub fn rtw_rust_mi_iface_at(padapter: Padapter, idx: i32) -> Padapter;
        pub fn rtw_rust_mi_is_adapter_up(iface: Padapter) -> u8;
        pub fn rtw_rust_mi_pnetdev(iface: Padapter) -> Pnetdev;
        pub fn rtw_rust_mi_netif_carrier_off(n: Pnetdev);
        pub fn rtw_rust_mi_netif_carrier_on(n: Pnetdev);
        pub fn rtw_rust_mi_netif_stop_queue(n: Pnetdev);
        pub fn rtw_rust_mi_netif_start_queue(n: Pnetdev);
        pub fn rtw_rust_mi_netif_wake_queue(n: Pnetdev);
    }
}

#[cfg(rust_mi_netif_leaf)]
type MiNetifKernOp = unsafe fn(kernel::Padapter) -> u8;

#[cfg(rust_mi_netif_leaf)]
const _TRUE_K: u8 = 1;

#[cfg(rust_mi_netif_leaf)]
unsafe fn mi_process_netif_kern(
    padapter: kernel::Padapter,
    exclude_self: bool,
    op: MiNetifKernOp,
) -> u8 {
    let n = unsafe { kernel::rtw_rust_mi_iface_nums(padapter) };
    let mut ret = 0u8;
    for i in 0..n {
        let iface = unsafe { kernel::rtw_rust_mi_iface_at(padapter, i) };
        if iface.is_null() {
            continue;
        }
        if unsafe { kernel::rtw_rust_mi_is_adapter_up(iface) } == 0 {
            continue;
        }
        if exclude_self && core::ptr::eq(iface, padapter) {
            continue;
        }
        if unsafe { op(iface) } == _TRUE_K {
            ret += 1;
        }
    }
    ret
}

#[cfg(rust_mi_netif_leaf)]
unsafe fn op_kern_caroff_qstop(iface: kernel::Padapter) -> u8 {
    let n = unsafe { kernel::rtw_rust_mi_pnetdev(iface) };
    unsafe {
        kernel::rtw_rust_mi_netif_carrier_off(n);
        kernel::rtw_rust_mi_netif_stop_queue(n);
    }
    _TRUE_K
}

#[cfg(rust_mi_netif_leaf)]
unsafe fn op_kern_caron_qstart(iface: kernel::Padapter) -> u8 {
    let n = unsafe { kernel::rtw_rust_mi_pnetdev(iface) };
    unsafe {
        kernel::rtw_rust_mi_netif_carrier_on(n);
        kernel::rtw_rust_mi_netif_start_queue(n);
    }
    _TRUE_K
}

#[cfg(rust_mi_netif_leaf)]
unsafe fn op_kern_stop_queue(iface: kernel::Padapter) -> u8 {
    unsafe { kernel::rtw_rust_mi_netif_stop_queue(kernel::rtw_rust_mi_pnetdev(iface)) };
    _TRUE_K
}

#[cfg(rust_mi_netif_leaf)]
unsafe fn op_kern_wake_queue(iface: kernel::Padapter) -> u8 {
    let n = unsafe { kernel::rtw_rust_mi_pnetdev(iface) };
    if !n.is_null() {
        unsafe { kernel::rtw_rust_mi_netif_wake_queue(n) };
    }
    _TRUE_K
}

#[cfg(rust_mi_netif_leaf)]
unsafe fn op_kern_carrier_on(iface: kernel::Padapter) -> u8 {
    let n = unsafe { kernel::rtw_rust_mi_pnetdev(iface) };
    if !n.is_null() {
        unsafe { kernel::rtw_rust_mi_netif_carrier_on(n) };
    }
    _TRUE_K
}

#[cfg(rust_mi_netif_leaf)]
unsafe fn op_kern_carrier_off(iface: kernel::Padapter) -> u8 {
    let n = unsafe { kernel::rtw_rust_mi_pnetdev(iface) };
    if !n.is_null() {
        unsafe { kernel::rtw_rust_mi_netif_carrier_off(n) };
    }
    _TRUE_K
}

#[cfg(rust_mi_netif_leaf)]
macro_rules! mi_netif_kern_export {
    ($fn:ident, $buddy:expr, $op:ident) => {
        #[no_mangle]
        pub unsafe extern "C" fn $fn(p: kernel::Padapter) -> u8 {
            unsafe { mi_process_netif_kern(p, $buddy, $op) }
        }
    };
}

#[cfg(rust_mi_netif_leaf)]
mi_netif_kern_export!(rtw_mi_netif_caroff_qstop, false, op_kern_caroff_qstop);
#[cfg(rust_mi_netif_leaf)]
mi_netif_kern_export!(rtw_mi_buddy_netif_caroff_qstop, true, op_kern_caroff_qstop);
#[cfg(rust_mi_netif_leaf)]
mi_netif_kern_export!(rtw_mi_netif_caron_qstart, false, op_kern_caron_qstart);
#[cfg(rust_mi_netif_leaf)]
mi_netif_kern_export!(rtw_mi_buddy_netif_caron_qstart, true, op_kern_caron_qstart);
#[cfg(rust_mi_netif_leaf)]
mi_netif_kern_export!(rtw_mi_netif_stop_queue, false, op_kern_stop_queue);
#[cfg(rust_mi_netif_leaf)]
mi_netif_kern_export!(rtw_mi_buddy_netif_stop_queue, true, op_kern_stop_queue);
#[cfg(rust_mi_netif_leaf)]
mi_netif_kern_export!(rtw_mi_netif_wake_queue, false, op_kern_wake_queue);
#[cfg(rust_mi_netif_leaf)]
mi_netif_kern_export!(rtw_mi_buddy_netif_wake_queue, true, op_kern_wake_queue);
#[cfg(rust_mi_netif_leaf)]
mi_netif_kern_export!(rtw_mi_netif_carrier_on, false, op_kern_carrier_on);
#[cfg(rust_mi_netif_leaf)]
mi_netif_kern_export!(rtw_mi_buddy_netif_carrier_on, true, op_kern_carrier_on);
#[cfg(rust_mi_netif_leaf)]
mi_netif_kern_export!(rtw_mi_netif_carrier_off, false, op_kern_carrier_off);
#[cfg(rust_mi_netif_leaf)]
mi_netif_kern_export!(rtw_mi_buddy_netif_carrier_off, true, op_kern_carrier_off);

#[cfg(not(any(
    host_mi_ch_union_test,
    host_mi_netif_buddy_test,
    rust_mi_netif_leaf
)))]
pub fn mi_ch_union_stub() {}
