// SPDX-License-Identifier: GPL-2.0
//! W3-122 host L2: `rtw_mi_status_by_ifbmp` (parity with core/rtw_mi_status.c).

const WIFI_ASOC: i32 = 0x00000001;
const WIFI_STA: i32 = 0x00000008;
const WIFI_AP: i32 = 0x00000010;
const WIFI_ADHOC: i32 = 0x00000020;
const WIFI_ADHOC_M: i32 = 0x00000040;
const WIFI_LINKING: i32 = 0x00000080;
const WIFI_WPS: i32 = 0x00000100;
const WIFI_MESH: i32 = 0x00000200;
const WIFI_SURVEY: i32 = 0x00000800;
const SCAN_DISABLE: u8 = 0;
const SCAN_BACK_OP: u8 = 6;

#[repr(C)]
pub struct MiStatusMlmePriv {
    pub fw_state: i32,
}
#[repr(C)]
pub struct MiStatusSitesurveyRes {
    pub state: u8,
}
#[repr(C)]
pub struct MiStatusMlmeExtPriv {
    pub sitesurvey_res: MiStatusSitesurveyRes,
}
#[repr(C)]
pub struct MiStatusTdlsInfo {
    pub link_established: u8,
}
#[repr(C)]
pub struct MiStatusStaPriv {
    pub asoc_sta_count: i32,
}
#[repr(C)]
pub struct MiState {
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
pub struct MiStatusDvobj {
    pub iface_nums: u8,
    pub iface_state: MiState,
    pub padapters: [*mut MiStatusAdapter; 4],
    pub cfg80211_mgmt_tx: [u8; 4],
    pub cfg80211_roch: [u8; 4],
}
#[repr(C)]
pub struct MiStatusAdapter {
    pub iface_id: u8,
    pub mlmepriv: MiStatusMlmePriv,
    pub mlmeextpriv: MiStatusMlmeExtPriv,
    pub stapriv: MiStatusStaPriv,
    pub tdlsinfo: MiStatusTdlsInfo,
    pub dvobj: *mut MiStatusDvobj,
}

fn fw_on(m: &MiStatusMlmePriv, st: i32) -> bool {
    if st == 0 {
        m.fw_state == 0
    } else {
        (m.fw_state & st) != 0
    }
}

fn dv_flag(id: u8, arr: &[u8; 4]) -> bool {
    arr[id as usize] != 0
}

#[no_mangle]
pub unsafe extern "C" fn rtw_mi_status_by_ifbmp(
    dvobj: *mut MiStatusDvobj,
    ifbmp: u8,
    mstate: *mut MiState,
) {
    if dvobj.is_null() || mstate.is_null() {
        return;
    }
    let dvobj = &*dvobj;
    let mstate = &mut *mstate;
    core::ptr::write_bytes(
        mstate as *mut MiState as *mut u8,
        0,
        core::mem::size_of::<MiState>(),
    );

    for i in 0..dvobj.iface_nums as i32 {
        let iface_ptr = dvobj.padapters[i as usize];
        if iface_ptr.is_null() {
            continue;
        }
        let iface = &*iface_ptr;
        if (ifbmp & (1u8 << iface.iface_id)) == 0 {
            continue;
        }
        let ml = &iface.mlmepriv;

        if fw_on(ml, WIFI_STA) {
            mstate.sta_num += 1;
            if fw_on(ml, WIFI_ASOC) {
                mstate.ld_sta_num += 1;
                if iface.tdlsinfo.link_established != 0 {
                    mstate.ld_tdls_num += 1;
                }
            }
            if fw_on(ml, WIFI_LINKING) {
                mstate.lg_sta_num += 1;
            }
        } else if fw_on(ml, WIFI_AP) {
            if fw_on(ml, WIFI_ASOC) {
                mstate.ap_num += 1;
                if iface.stapriv.asoc_sta_count > 2 {
                    mstate.ld_ap_num += 1;
                }
            } else {
                mstate.starting_ap_num += 1;
            }
        } else if fw_on(ml, WIFI_ADHOC | WIFI_ADHOC_M) && fw_on(ml, WIFI_ASOC) {
            mstate.adhoc_num += 1;
            if iface.stapriv.asoc_sta_count > 2 {
                mstate.ld_adhoc_num += 1;
            }
        } else if fw_on(ml, WIFI_MESH) && fw_on(ml, WIFI_ASOC) {
            mstate.mesh_num += 1;
            if iface.stapriv.asoc_sta_count > 2 {
                mstate.ld_mesh_num += 1;
            }
        }

        if fw_on(ml, WIFI_WPS) {
            mstate.uwps_num += 1;
        }
        if fw_on(ml, WIFI_SURVEY) {
            mstate.scan_num += 1;
            let st = iface.mlmeextpriv.sitesurvey_res.state;
            if st != SCAN_DISABLE && st != SCAN_BACK_OP {
                mstate.scan_enter_num += 1;
            }
        }
        if !iface.dvobj.is_null() {
            let dv = &*iface.dvobj;
            if dv_flag(iface.iface_id, &dv.cfg80211_mgmt_tx) {
                mstate.mgmt_tx_num += 1;
            }
            if dv_flag(iface.iface_id, &dv.cfg80211_roch) {
                mstate.roch_num += 1;
            }
        }
    }
}
