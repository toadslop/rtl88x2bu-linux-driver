// W4-89 — rtw_phydm_ops_func_init + record_ra_info from hal/hal_dm.c

use core::ffi::c_void;

type DmStruct = u8;
type CmnStaInfo = u8;

#[repr(C)]
struct RtwHalDmStaRaFields {
    ra_bw_mode: u8,
    is_vht_enable: u8,
}

type RecordRaInfoFn =
    unsafe extern "C" fn(*mut c_void, u8, *mut CmnStaInfo, u64);

type FillDescDyntxpwrFn = unsafe extern "C" fn(*mut c_void, *mut u8, u8);

unsafe extern "C" {
    fn rtw_rust_hal_dm_phydm_adapter(p_dm: *mut DmStruct) -> *mut c_void;
    fn rtw_rust_hal_dm_ra_table(p_dm: *mut DmStruct) -> *mut c_void;
    fn rtw_rust_hal_dm_ra_table_set_record_ra(p_ra: *mut c_void, fn_ptr: RecordRaInfoFn);
    fn rtw_rust_hal_dm_set_fill_desc_dyntxpwr(p_dm: *mut DmStruct, fn_ptr: FillDescDyntxpwrFn);
    fn rtw_rust_hal_dm_sta_ra_fields(p_sta: *mut CmnStaInfo, out: *mut RtwHalDmStaRaFields);
    fn rtw_rust_hal_dm_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_rust_hal_dm_dvobj_to_macidctl(dvobj: *mut c_void) -> *mut c_void;
    fn rtw_macid_ctl_set_bw(macid_ctl: *mut c_void, id: u8, bw: u8);
    fn rtw_macid_ctl_set_vht_en(macid_ctl: *mut c_void, id: u8, en: u8);
    fn rtw_macid_ctl_set_rate_bmp0(macid_ctl: *mut c_void, id: u8, bmp: u32);
    fn rtw_macid_ctl_set_rate_bmp1(macid_ctl: *mut c_void, id: u8, bmp: u32);
    fn rtw_update_tx_rate_bmp(dvobj: *mut c_void);
    #[cfg(CONFIG_SUPPORT_DYNAMIC_TXPWR)]
    fn rtw_rust_hal_dm_get_chip_type(adapter: *mut c_void) -> u8;
    #[cfg(CONFIG_SUPPORT_DYNAMIC_TXPWR)]
    fn rtw_rust_hal_dm_set_tx_desc_txpwr_ofset_8822b(desc: *mut u8, value: u8);
    #[cfg(CONFIG_SUPPORT_DYNAMIC_TXPWR)]
    fn rtw_rust_hal_dm_log_dyntxpwr_unsupported();
    #[cfg(CONFIG_SUPPORT_DYNAMIC_TXPWR)]
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    #[cfg(CONFIG_SUPPORT_DYNAMIC_TXPWR)]
    fn rtw_rust_hal_dm_odm_set_dyntxpwr(dm: *mut c_void, desc: *mut u8, mac_id: u8);
}

#[cfg(CONFIG_SUPPORT_DYNAMIC_TXPWR)]
include!("hal_dm_dyntxpwr_body.inc.rs");

#[no_mangle]
pub unsafe extern "C" fn record_ra_info(
    p_dm_void: *mut c_void,
    macid: u8,
    p_sta: *mut CmnStaInfo,
    ra_mask: u64,
) {
    if p_sta.is_null() {
        return;
    }

    // SAFETY: C passes valid `p_dm_void`, `p_sta`, and `ra_mask`; adapter/macid_ctl
    // are validated before use (NULL returns early, matching harmless C divergence).
    unsafe {
        let p_dm = p_dm_void as *mut DmStruct;
        let adapter = rtw_rust_hal_dm_phydm_adapter(p_dm);
        if adapter.is_null() {
            return;
        }

        let mut fields = RtwHalDmStaRaFields {
            ra_bw_mode: 0,
            is_vht_enable: 0,
        };
        rtw_rust_hal_dm_sta_ra_fields(p_sta, &mut fields);

        let dvobj = rtw_rust_hal_dm_adapter_to_dvobj(adapter);
        let macid_ctl = rtw_rust_hal_dm_dvobj_to_macidctl(dvobj);
        if macid_ctl.is_null() {
            return;
        }

        rtw_macid_ctl_set_bw(macid_ctl, macid, fields.ra_bw_mode);
        rtw_macid_ctl_set_vht_en(macid_ctl, macid, fields.is_vht_enable);
        rtw_macid_ctl_set_rate_bmp0(macid_ctl, macid, ra_mask as u32);
        rtw_macid_ctl_set_rate_bmp1(macid_ctl, macid, (ra_mask >> 32) as u32);

        if !dvobj.is_null() {
            rtw_update_tx_rate_bmp(dvobj);
        }
    }
}

#[no_mangle]
pub unsafe extern "C" fn rtw_phydm_ops_func_init(p_phydm: *mut DmStruct) {
    if p_phydm.is_null() {
        return;
    }

    // SAFETY: `p_phydm` is non-NULL; hook tables are written via C layout helpers.
    unsafe {
        let p_ra = rtw_rust_hal_dm_ra_table(p_phydm);
        if !p_ra.is_null() {
            rtw_rust_hal_dm_ra_table_set_record_ra(p_ra, record_ra_info);
        }

        #[cfg(CONFIG_SUPPORT_DYNAMIC_TXPWR)]
        {
            rtw_rust_hal_dm_set_fill_desc_dyntxpwr(p_phydm, rtw_phydm_fill_desc_dpt);
        }
    }
}