// SPDX-License-Identifier: GPL-2.0
//! W3-120 odm radar/txpwr leaf — host L2 Rust oracle only.
#![allow(dead_code, improper_ctypes, non_snake_case, non_camel_case_types)]

use std::os::raw::c_void;

type Adapter = c_void;
type DmStruct = c_void;
type DvobjPriv = c_void;
type RfCtl = c_void;

const RTW_DFS_REGD_NONE: u8 = 0;
const RTW_DFS_REGD_NUM: u8 = 4;
const PHYDM_DFS_DOMAIN_UNKNOWN: u32 = 0;
const PHYDM_DFS_DOMAIN_FCC: u32 = 1;
const PHYDM_DFS_DOMAIN_MKK: u32 = 2;
const PHYDM_DFS_DOMAIN_ETSI: u32 = 3;
const ODM_CMNINFO_DFS_REGION_DOMAIN: u32 = 42;

extern "C" {
    fn host_adapter_to_phydm(adapter: *mut Adapter) -> *mut DmStruct;
    fn host_dvobj_to_phydm(dvobj: *mut DvobjPriv) -> *mut DmStruct;
    fn host_dvobj_to_rfctl(dvobj: *mut DvobjPriv) -> *mut RfCtl;
    fn rtw_rfctl_get_dfs_domain(rfctl: *mut RfCtl) -> u8;
    fn odm_cmn_info_init(dm: *mut DmStruct, cmn_info: u32, value: u32);
    fn phy_get_txpwr_single_mbm(
        adapter: *mut Adapter,
        rfpath: u8,
        rs: u8,
        rate: u8,
        bw: u8,
        cch: u8,
        o1: u8,
        o2: u8,
        o3: u8,
        p: *mut c_void,
    ) -> i16;
    fn mgn_rate_to_rs(rate: u8) -> u8;
    fn host_odm_dm_adapter(dm: *mut DmStruct) -> *mut Adapter;
    fn phydm_radar_detect_reset(dm: *mut DmStruct);
    fn phydm_radar_detect_disable(dm: *mut DmStruct);
    fn phydm_radar_detect_enable(dm: *mut DmStruct);
    fn phydm_radar_detect(dm: *mut DmStruct) -> i32;
    fn phydm_dfs_polling_time(dm: *mut DmStruct) -> u8;
}

fn dfs_regd_to_phydm(region: u8) -> u32 {
    match region {
        1 => PHYDM_DFS_DOMAIN_FCC,
        2 => PHYDM_DFS_DOMAIN_MKK,
        3 => PHYDM_DFS_DOMAIN_ETSI,
        _ if region >= RTW_DFS_REGD_NUM => PHYDM_DFS_DOMAIN_UNKNOWN,
        _ => PHYDM_DFS_DOMAIN_UNKNOWN,
    }
}

#[no_mangle]
pub extern "C" fn rtw_odm_get_tx_power_mbm(
    dm: *mut DmStruct,
    rfpath: u8,
    rate: u8,
    bw: u8,
    cch: u8,
) -> i16 {
    if dm.is_null() {
        return 0;
    }
    unsafe {
        let adapter = host_odm_dm_adapter(dm);
        if adapter.is_null() {
            return 0;
        }
        phy_get_txpwr_single_mbm(
            adapter,
            rfpath,
            mgn_rate_to_rs(rate),
            rate,
            bw,
            cch,
            0,
            0,
            0,
            std::ptr::null_mut(),
        )
    }
}

#[no_mangle]
pub extern "C" fn rtw_odm_radar_detect_reset(adapter: *mut Adapter) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        phydm_radar_detect_reset(host_adapter_to_phydm(adapter));
    }
}

#[no_mangle]
pub extern "C" fn rtw_odm_radar_detect_disable(adapter: *mut Adapter) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        phydm_radar_detect_disable(host_adapter_to_phydm(adapter));
    }
}

#[no_mangle]
pub extern "C" fn rtw_odm_radar_detect_enable(adapter: *mut Adapter) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        phydm_radar_detect_enable(host_adapter_to_phydm(adapter));
    }
}

#[no_mangle]
pub extern "C" fn rtw_odm_radar_detect(adapter: *mut Adapter) -> i32 {
    if adapter.is_null() {
        return 0;
    }
    unsafe { phydm_radar_detect(host_adapter_to_phydm(adapter)) }
}

#[no_mangle]
pub extern "C" fn rtw_odm_update_dfs_region(dvobj: *mut DvobjPriv) {
    if dvobj.is_null() {
        return;
    }
    unsafe {
        let region = rtw_rfctl_get_dfs_domain(host_dvobj_to_rfctl(dvobj));
        odm_cmn_info_init(
            host_dvobj_to_phydm(dvobj),
            ODM_CMNINFO_DFS_REGION_DOMAIN,
            dfs_regd_to_phydm(region),
        );
    }
}

#[no_mangle]
pub extern "C" fn rtw_odm_radar_detect_polling_int_ms(dvobj: *mut DvobjPriv) -> u8 {
    if dvobj.is_null() {
        return 0;
    }
    unsafe { phydm_dfs_polling_time(host_dvobj_to_phydm(dvobj)) }
}
