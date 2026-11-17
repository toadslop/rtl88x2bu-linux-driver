// W4-104 — `rtw_phydm_set_crystal_cap` from hal/hal_dm.c.

use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;

unsafe extern "C" {
    fn rtw_rust_hal_dm_adapter_to_phydm(adapter: *mut c_void) -> *mut DmStruct;
    fn phydm_set_crystal_cap_reg(dm_void: *mut c_void, crystal_cap: U8) -> U8;
}

#[no_mangle]
pub extern "C" fn rtw_phydm_set_crystal_cap(adapter: *mut c_void, crystal_cap: U8) -> U8 {
    // SAFETY: Kernel passes a valid adapter; PHYDM helper accepts the derived dm_struct.
    unsafe {
        let phydm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
        phydm_set_crystal_cap_reg(phydm.cast(), crystal_cap)
    }
}