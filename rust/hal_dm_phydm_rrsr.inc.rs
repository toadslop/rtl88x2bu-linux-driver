// W4-102 — `rtw_phydm_set_rrsr` and `rtw_phydm_dyn_rrsr_en` from hal/hal_dm.c.

use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;
type U32 = u32;
type U64 = u64;

const ODM_CMNINFO_RRSR_VAL: U32 = 111;
const RRSR_REGISTRY_DEFAULT: U32 = 0xFFFF_FFFF;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn rtw_rust_hal_dm_registry_set_rrsr_value(adapter: *mut c_void) -> U32;
    fn odm_cmn_info_update(dm: *mut DmStruct, cmn_info: U32, value: U64);
    fn phydm_rrsr_set_register(dm_void: *mut c_void, rrsr_val: U32);
    fn phydm_rrsr_en(dm_void: *mut c_void, en_rrsr: U8);
}

#[no_mangle]
pub extern "C" fn rtw_phydm_set_rrsr(adapter: *mut c_void, rrsr_value: U32, write_rrsr: U8) {
    // SAFETY: Kernel passes a valid adapter; PHYDM helpers accept the derived dm_struct.
    unsafe {
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        let reg = rtw_rust_hal_dm_registry_set_rrsr_value(adapter);
        let temp_rrsr = if reg != RRSR_REGISTRY_DEFAULT {
            reg
        } else {
            rrsr_value
        };
        odm_cmn_info_update(phydm, ODM_CMNINFO_RRSR_VAL, temp_rrsr as U64);
        if write_rrsr != 0 {
            phydm_rrsr_set_register(phydm.cast(), temp_rrsr);
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_phydm_dyn_rrsr_en(adapter: *mut c_void, en_rrsr: U8) {
    unsafe {
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        phydm_rrsr_en(phydm.cast(), en_rrsr);
    }
}