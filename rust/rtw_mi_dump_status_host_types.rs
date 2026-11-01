// SPDX-License-Identifier: GPL-2.0
//! Host-only dvobj layout for W3-198 L2 (matches tests/host/include/host_mi_dump_types.h).

use std::os::raw::c_void;

#[repr(C)]
pub struct MiDumpMiState {
    pub sta_num: u8,
    pub ld_sta_num: u8,
    pub lg_sta_num: u8,
    pub ld_tdls_num: u8,
    pub ap_num: u8,
    pub starting_ap_num: u8,
    pub ld_ap_num: u8,
    pub adhoc_num: u8,
    pub ld_adhoc_num: u8,
    pub mesh_num: u8,
    pub ld_mesh_num: u8,
    pub scan_num: u8,
    pub scan_enter_num: u8,
    pub uwps_num: u8,
    pub roch_num: u8,
    pub mgmt_tx_num: u8,
    pub p2p_device_num: u8,
    pub p2p_gc: u8,
    pub p2p_go: u8,
}

#[repr(C)]
pub struct MiDumpDvobj {
    pub iface_nums: u8,
    pub iface_state: MiDumpMiState,
    pub _pad_before_adapters: [u8; 4],
    pub padapters: [*mut c_void; 4],
    pub cfg80211_mgmt_tx: [u8; 4],
    pub cfg80211_roch: [u8; 4],
    pub union_ch: u8,
    pub union_bw: u8,
    pub union_offset: u8,
}

#[repr(C)]
pub struct HostSelCapture {
    pub buf: [u8; 4096],
    pub len: usize,
}

extern "C" {
    pub static mut host_mi_dump_sel: HostSelCapture;
    fn host_mi_dump_adapter_dvobj(adapter: *mut c_void) -> *mut c_void;
}

pub unsafe fn adapter_dvobj(adapter: *mut c_void) -> *mut c_void {
    host_mi_dump_adapter_dvobj(adapter)
}

pub fn snap_from_host_dvobj(dvobj: *mut c_void, snap: &mut super::MiDumpSnap) {
    let d = unsafe { &*(dvobj as *mut MiDumpDvobj) };
    let m = &d.iface_state;
    snap.sta_num = m.sta_num;
    snap.lg_sta_num = m.lg_sta_num;
    snap.ld_sta_num = m.ld_sta_num;
    snap.ld_tdls_num = m.ld_tdls_num;
    snap.ap_num = m.ap_num;
    snap.starting_ap_num = m.starting_ap_num;
    snap.ld_ap_num = m.ld_ap_num;
    snap.adhoc_num = m.adhoc_num;
    snap.ld_adhoc_num = m.ld_adhoc_num;
    snap.mesh_num = m.mesh_num;
    snap.ld_mesh_num = m.ld_mesh_num;
    snap.p2p_device_num = m.p2p_device_num;
    snap.p2p_gc = m.p2p_gc;
    snap.p2p_go = m.p2p_go;
    snap.scan_num = m.scan_num;
    snap.uwps_num = m.uwps_num;
    snap.roch_num = m.roch_num;
    snap.mgmt_tx_num = m.mgmt_tx_num;
    snap.union_ch = d.union_ch;
    snap.union_bw = d.union_bw;
    snap.union_offset = d.union_offset;
}

pub fn capture_print(sel: *mut c_void, line: *const std::os::raw::c_char) {
    unsafe {
        if sel.is_null() {
            return;
        }
        let cap = &mut *(sel as *mut HostSelCapture);
        let mut len = 0usize;
        while *line.add(len) != 0 {
            len += 1;
        }
        let remain = cap.buf.len().saturating_sub(cap.len);
        let n = core::cmp::min(len, remain.saturating_sub(1));
        core::ptr::copy_nonoverlapping(line as *const u8, cap.buf.as_mut_ptr().add(cap.len), n);
        cap.len += n;
    }
}
