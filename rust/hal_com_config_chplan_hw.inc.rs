// SPDX-License-Identifier: GPL-2.0
// HW branch of `hal_com_config_channel_plan` (W4-90 part 1).

use core::ffi::c_char;

type U8 = u8;

const CHPLAN_UNSET: i32 = -1;
const NO_COUNTRY: U8 = 0xff;
const EEPROM_CHANNEL_PLAN_BY_HW_MASK: U8 = 0x80;

#[repr(C)]
pub struct CountryChplan {
    pub alpha2: [U8; 2],
    pub chplan: U8,
    pub en_11ac: U8,
}

#[repr(C)]
pub struct HalConfigChplanHwOut {
    pub chplan: i32,
    pub country_chplan: U8,
    pub force_hw_chplan: U8,
    pub goto_sw_config: U8,
}

extern "C" {
    fn rtw_get_chplan_from_country(country_code: *const c_char) -> *const CountryChplan;
    fn rtw_is_channel_plan_valid(id: U8) -> bool;
}

#[cfg(not(host_hal_com_config_chplan_hw_test))]
extern "C" {
    fn rtw_rust_hal_com_log_unsupported_hw_alpha2(func: *const c_char, a: c_char, b: c_char);
    fn rtw_rust_hal_com_log_unsupported_hw_chplan(func: *const c_char, plan: U8);
}

fn is_alpha2_no_specified(alpha2: *const c_char) -> bool {
    if alpha2.is_null() {
        return true;
    }
    unsafe {
        let raw = alpha2 as *const u16;
        core::ptr::read_unaligned(raw) == 0xffff
    }
}

struct HwBranchScratch {
    country_ent: *const CountryChplan,
}

fn config_chplan_hw_branch_impl(
    hw_alpha2: *const c_char,
    hw_chplan: U8,
    force_hw_chplan_in: U8,
    out: &mut HalConfigChplanHwOut,
    scratch: &mut HwBranchScratch,
) {
    let mut force_hw_chplan = force_hw_chplan_in;
    let mut chplan = CHPLAN_UNSET;
    scratch.country_ent = core::ptr::null();

    out.chplan = CHPLAN_UNSET;
    out.country_chplan = NO_COUNTRY;
    out.force_hw_chplan = force_hw_chplan;
    out.goto_sw_config = 0;

    if !hw_alpha2.is_null() && !is_alpha2_no_specified(hw_alpha2) {
        let ent = unsafe { rtw_get_chplan_from_country(hw_alpha2) };
        if !ent.is_null() {
            scratch.country_ent = ent;
            chplan = unsafe { (*ent).chplan as i32 };
            out.chplan = chplan;
            out.country_chplan = unsafe { (*ent).chplan };
            out.goto_sw_config = 1;
            return;
        }
        #[cfg(not(host_hal_com_config_chplan_hw_test))]
        unsafe {
            static FN: &[u8] = b"hal_com_config_channel_plan\0";
            let a = *hw_alpha2;
            let b = *hw_alpha2.add(1);
            rtw_rust_hal_com_log_unsupported_hw_alpha2(FN.as_ptr() as *const c_char, a, b);
        }
    }

    if unsafe { rtw_is_channel_plan_valid(hw_chplan) } {
        chplan = hw_chplan as i32;
    } else if force_hw_chplan != 0 {
        #[cfg(not(host_hal_com_config_chplan_hw_test))]
        unsafe {
            static FN: &[u8] = b"hal_com_config_channel_plan\0";
            rtw_rust_hal_com_log_unsupported_hw_chplan(FN.as_ptr() as *const c_char, hw_chplan);
        }
        force_hw_chplan = 0;
    }

    out.chplan = chplan;
    out.force_hw_chplan = force_hw_chplan;
    if !scratch.country_ent.is_null() {
        out.country_chplan = unsafe { (*scratch.country_ent).chplan };
    }
}

#[repr(C)]
pub struct RustConfigChplanHwState {
    pub chplan: i32,
    pub country_ent: *const CountryChplan,
    pub force_hw_chplan: U8,
    pub goto_sw_config: U8,
}

#[cfg(not(host_hal_com_config_chplan_hw_test))]
#[no_mangle]
pub unsafe extern "C" fn rtw_rust_hal_com_config_chplan_hw_branch(
    hw_alpha2: *mut c_char,
    hw_chplan: U8,
    force_hw_chplan_in: U8,
    st: *mut RustConfigChplanHwState,
) {
    if st.is_null() {
        return;
    }
    let mut out = HalConfigChplanHwOut {
        chplan: CHPLAN_UNSET,
        country_chplan: NO_COUNTRY,
        force_hw_chplan: force_hw_chplan_in,
        goto_sw_config: 0,
    };
    let mut scratch = HwBranchScratch {
        country_ent: core::ptr::null(),
    };
    config_chplan_hw_branch_impl(hw_alpha2, hw_chplan, force_hw_chplan_in, &mut out, &mut scratch);
    unsafe {
        (*st).chplan = out.chplan;
        (*st).force_hw_chplan = out.force_hw_chplan;
        (*st).goto_sw_config = out.goto_sw_config;
        (*st).country_ent = scratch.country_ent;
    }
}

#[cfg(host_hal_com_config_chplan_hw_test)]
#[no_mangle]
pub unsafe extern "C" fn hal_com_config_chplan_hw_branch(
    hw_alpha2: *const c_char,
    hw_chplan: U8,
    force_hw_chplan_in: U8,
    out: *mut HalConfigChplanHwOut,
) {
    if out.is_null() {
        return;
    }
    let mut scratch = HwBranchScratch {
        country_ent: core::ptr::null(),
    };
    config_chplan_hw_branch_impl(hw_alpha2, hw_chplan, force_hw_chplan_in, &mut *out, &mut scratch);
}