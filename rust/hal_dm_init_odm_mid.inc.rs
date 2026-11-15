// W4-85 — Init_ODM_ComInfo mid block: GPA/LNA, adaptivity, halrf init, pointer hooks.

use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;
type U16 = u16;
type U32 = u32;
type U64 = u64;
type S8 = i8;

const TRUE: i32 = 1;
const _TRUE: U8 = 1;

const ODM_CMNINFO_GPA: U32 = 18;
const ODM_CMNINFO_APA: U32 = 19;
const ODM_CMNINFO_GLNA: U32 = 20;
const ODM_CMNINFO_ALNA: U32 = 21;
const ODM_CMNINFO_RFE_TYPE: U32 = 10;
const ODM_CMNINFO_EXT_TRSW: U32 = 23;
const ODM_CMNINFO_REGRFKFREEENABLE: U32 = 36;
const ODM_CMNINFO_RFKFREEENABLE: U32 = 37;
const ODM_CMNINFO_EFUSE0X3D8: U32 = 40;
const ODM_CMNINFO_EFUSE0X3D7: U32 = 41;
const ODM_CMNINFO_X_CAP_SETTING: U32 = 43;
const ODM_CMNINFO_RF_ANTENNA_TYPE: U32 = 97;
const ODM_CMNINFO_WITH_EXT_ANTENNA_SWITCH: U32 = 98;
const ODM_CMNINFO_BE_FIX_TX_ANT: U32 = 99;
const ODM_CMNINFO_MANUAL_SUPPORTABILITY: U32 = 84;
const ODM_CMNINFO_ADAPTIVITY: U32 = 63;
const ODM_CMNINFO_IQKPAOFF: U32 = 30;

const PHYDM_ADAPINFO_CARRIER_SENSE_ENABLE: U8 = 0;
const PHYDM_ADAPINFO_TH_L2H_INI: U8 = 1;
const PHYDM_ADAPINFO_TH_EDCCA_HL_DIFF: U8 = 2;

const HALRF_CMNINFO_EEPROM_THERMAL_VALUE: U8 = 2;
const HALRF_CMNINFO_PWT_TYPE: U8 = 6;
const HALRF_CMNINFO_MP_POWER_TRACKING_TYPE: U8 = 12;

#[repr(C)]
pub struct RtwHalDmInitOdmMidInit {
    pub type_gpa: U16,
    pub type_apa: U16,
    pub type_glna: U16,
    pub type_alna: U16,
    pub rfe_type: U16,
    pub crystal_cap: U8,
    pub reg_pwr_trim_enable: U8,
    pub rf_kfree_enable: U8,
    pub trx_ant_div_type: U8,
    pub b_fix_tx_ant: U8,
    pub with_ext_ant_switch: U8,
    pub efuse0x3d7: U8,
    pub efuse0x3d8: U8,
    pub eeprom_thermal_meter: U8,
    pub txpwr_pg_mode: U8,
    pub adaptivity_mode: U8,
    pub adaptivity_th_l2h_ini: S8,
    pub adaptivity_th_edcca_hl_diff: S8,
}

unsafe extern "C" {
    fn odm_cmn_info_init(dm: *mut DmStruct, cmn_info: U32, value: U64);
    fn odm_cmn_info_hook(dm: *mut DmStruct, cmn_info: U32, ptr: *mut u8);
    fn phydm_adaptivity_info_init(dm: *mut DmStruct, id: U8, value: i32);
    fn halrf_cmn_info_init(dm: *mut DmStruct, id: U8, value: U64);

    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;

    fn rtw_rust_hal_dm_init_odm_mid_init_vals(adapter: *mut c_void, out: *mut RtwHalDmInitOdmMidInit);
    fn rtw_rust_hal_dm_init_odm_mid_hook_phydm_ability(adapter: *mut c_void) -> *mut u32;
    fn rtw_rust_hal_dm_init_odm_mid_hook_adaptivity_en(adapter: *mut c_void) -> *mut U8;
    fn rtw_odm_adaptivity_needed(adapter: *mut c_void) -> U8;
    fn rtw_odm_adaptivity_config_msg(sel: *mut c_void, adapter: *mut c_void);
    fn rtw_hal_update_iqk_fw_offload_cap(adapter: *mut c_void);
    fn rtw_rust_hal_dm_init_odm_iqk_pa_off(phydm: *mut DmStruct);
    fn rtw_rust_hal_dm_init_odm_param_offload_cap(adapter: *mut c_void);
}

fn init_odm_values(phydm: *mut DmStruct, init: &RtwHalDmInitOdmMidInit) {
    unsafe {
        odm_cmn_info_init(phydm, ODM_CMNINFO_GPA, init.type_gpa as U64);
        odm_cmn_info_init(phydm, ODM_CMNINFO_APA, init.type_apa as U64);
        odm_cmn_info_init(phydm, ODM_CMNINFO_GLNA, init.type_glna as U64);
        odm_cmn_info_init(phydm, ODM_CMNINFO_ALNA, init.type_alna as U64);
        odm_cmn_info_init(phydm, ODM_CMNINFO_RFE_TYPE, init.rfe_type as U64);
        odm_cmn_info_init(phydm, ODM_CMNINFO_X_CAP_SETTING, init.crystal_cap as U64);
        odm_cmn_info_init(phydm, ODM_CMNINFO_EXT_TRSW, 0);
        odm_cmn_info_init(
            phydm,
            ODM_CMNINFO_REGRFKFREEENABLE,
            init.reg_pwr_trim_enable as U64,
        );
        odm_cmn_info_init(phydm, ODM_CMNINFO_RFKFREEENABLE, init.rf_kfree_enable as U64);
        odm_cmn_info_init(phydm, ODM_CMNINFO_RF_ANTENNA_TYPE, init.trx_ant_div_type as U64);
        odm_cmn_info_init(phydm, ODM_CMNINFO_BE_FIX_TX_ANT, init.b_fix_tx_ant as U64);
        odm_cmn_info_init(
            phydm,
            ODM_CMNINFO_WITH_EXT_ANTENNA_SWITCH,
            init.with_ext_ant_switch as U64,
        );
        odm_cmn_info_init(phydm, ODM_CMNINFO_EFUSE0X3D7, init.efuse0x3d7 as U64);
        odm_cmn_info_init(phydm, ODM_CMNINFO_EFUSE0X3D8, init.efuse0x3d8 as U64);
        halrf_cmn_info_init(
            phydm,
            HALRF_CMNINFO_EEPROM_THERMAL_VALUE,
            init.eeprom_thermal_meter as U64,
        );
        halrf_cmn_info_init(phydm, HALRF_CMNINFO_PWT_TYPE, 0);
        halrf_cmn_info_init(
            phydm,
            HALRF_CMNINFO_MP_POWER_TRACKING_TYPE,
            init.txpwr_pg_mode as U64,
        );
    }
}

fn init_adaptivity(phydm: *mut DmStruct, adapter: *mut c_void, init: &RtwHalDmInitOdmMidInit) {
    unsafe {
        let phydm_ability = rtw_rust_hal_dm_init_odm_mid_hook_phydm_ability(adapter);
        if !phydm_ability.is_null() {
            odm_cmn_info_hook(
                phydm,
                ODM_CMNINFO_MANUAL_SUPPORTABILITY,
                phydm_ability as *mut u8,
            );
        }
        let adapt_en = rtw_rust_hal_dm_init_odm_mid_hook_adaptivity_en(adapter);
        if !adapt_en.is_null() {
            odm_cmn_info_hook(phydm, ODM_CMNINFO_ADAPTIVITY, adapt_en);
        }
        let carrier_sense = if init.adaptivity_mode != 0 {
            TRUE
        } else {
            0
        };
        phydm_adaptivity_info_init(
            phydm,
            PHYDM_ADAPINFO_CARRIER_SENSE_ENABLE,
            carrier_sense,
        );
        phydm_adaptivity_info_init(
            phydm,
            PHYDM_ADAPINFO_TH_L2H_INI,
            init.adaptivity_th_l2h_ini as i32,
        );
        phydm_adaptivity_info_init(
            phydm,
            PHYDM_ADAPINFO_TH_EDCCA_HL_DIFF,
            init.adaptivity_th_edcca_hl_diff as i32,
        );
    }
}

#[no_mangle]
pub extern "C" fn rtw_rust_hal_dm_init_odm_cominfo_mid(adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    unsafe {
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        if phydm.is_null() {
            return;
        }
        let mut init = RtwHalDmInitOdmMidInit {
            type_gpa: 0,
            type_apa: 0,
            type_glna: 0,
            type_alna: 0,
            rfe_type: 0,
            crystal_cap: 0,
            reg_pwr_trim_enable: 0,
            rf_kfree_enable: 0,
            trx_ant_div_type: 0,
            b_fix_tx_ant: 0,
            with_ext_ant_switch: 0,
            efuse0x3d7: 0,
            efuse0x3d8: 0,
            eeprom_thermal_meter: 0,
            txpwr_pg_mode: 0,
            adaptivity_mode: 0,
            adaptivity_th_l2h_ini: 0,
            adaptivity_th_edcca_hl_diff: 0,
        };
        rtw_rust_hal_dm_init_odm_mid_init_vals(adapter, &mut init);
        init_odm_values(phydm, &init);
        init_adaptivity(phydm, adapter, &init);
        if rtw_odm_adaptivity_needed(adapter) == _TRUE {
            rtw_odm_adaptivity_config_msg(core::ptr::null_mut(), adapter);
        }
        rtw_rust_hal_dm_init_odm_iqk_pa_off(phydm);
        rtw_hal_update_iqk_fw_offload_cap(adapter);
        rtw_rust_hal_dm_init_odm_param_offload_cap(adapter);
    }
}