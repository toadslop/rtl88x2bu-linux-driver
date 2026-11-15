// SPDX-License-Identifier: GPL-2.0
// SW/done tail and full `hal_com_config_channel_plan` (W4-91).

#[cfg(not(host_hal_com_config_chplan_full_test))]
use core::ffi::c_void;

const RTW_CHPLAN_UNSPECIFIED: U8 = 0xff;
const DEF_CHPLAN: U8 = 0x7f;

#[cfg(not(any(host_hal_com_config_chplan_hw_test, host_hal_com_config_chplan_full_test)))]
extern "C" {
    fn rtw_rust_hal_com_log_unsupported_sw_alpha2(func: *const c_char, a: c_char, b: c_char);
    fn rtw_rust_hal_com_log_unsupported_sw_chplan(func: *const c_char, plan: U8);
    fn rtw_rust_hal_com_apply_chplan_fields(
        padapter: *mut c_void,
        country_ent: *const CountryChplan,
        chplan: i32,
        force_hw_chplan: U8,
    );
}

#[cfg(host_hal_com_config_chplan_full_test)]
#[repr(C)]
pub struct HostConfigChplanAdapterRfctl {
    pub channel_plan: U8,
    pub country_ent: *const CountryChplan,
}

#[cfg(host_hal_com_config_chplan_full_test)]
#[repr(C)]
pub struct HostConfigChplanAdapterHal {
    pub b_disable_sw_channel_plan: U8,
}

#[cfg(host_hal_com_config_chplan_full_test)]
#[repr(C)]
pub struct HostConfigChplanAdapter {
    pub rfctl: HostConfigChplanAdapterRfctl,
    pub hal_data: HostConfigChplanAdapterHal,
}

fn config_chplan_sw_and_done(
    sw_alpha2: *const c_char,
    sw_chplan: U8,
    force_hw_chplan: U8,
    mut chplan: i32,
    mut country_ent: *const CountryChplan,
) -> (i32, *const CountryChplan, U8) {
    if force_hw_chplan != 0 {
        return (chplan, country_ent, force_hw_chplan);
    }

    if !sw_alpha2.is_null() && !is_alpha2_no_specified(sw_alpha2) {
        let ent = unsafe { rtw_get_chplan_from_country(sw_alpha2) };
        if !ent.is_null() {
            country_ent = ent;
            chplan = unsafe { (*ent).chplan as i32 };
            return (chplan, country_ent, force_hw_chplan);
        }
        #[cfg(not(any(host_hal_com_config_chplan_hw_test, host_hal_com_config_chplan_full_test)))]
        unsafe {
            static FN: &[u8] = b"hal_com_config_channel_plan\0";
            rtw_rust_hal_com_log_unsupported_sw_alpha2(
                FN.as_ptr() as *const c_char,
                *sw_alpha2,
                *sw_alpha2.add(1),
            );
        }
    }

    if unsafe { rtw_is_channel_plan_valid(sw_chplan) } {
        country_ent = core::ptr::null();
        chplan = sw_chplan as i32;
    } else if sw_chplan != RTW_CHPLAN_UNSPECIFIED {
        #[cfg(not(any(host_hal_com_config_chplan_hw_test, host_hal_com_config_chplan_full_test)))]
        unsafe {
            static FN: &[u8] = b"hal_com_config_channel_plan\0";
            rtw_rust_hal_com_log_unsupported_sw_chplan(FN.as_ptr() as *const c_char, sw_chplan);
        }
    }

    if chplan == CHPLAN_UNSET {
        chplan = DEF_CHPLAN as i32;
    }

    (chplan, country_ent, force_hw_chplan)
}

fn config_chplan_full(
    hw_alpha2: *const c_char,
    hw_chplan_raw: U8,
    sw_alpha2: *const c_char,
    sw_chplan: U8,
    autoload_fail: U8,
) -> (i32, *const CountryChplan, U8) {
    let mut hw_out = HalConfigChplanHwOut {
        chplan: CHPLAN_UNSET,
        country_chplan: NO_COUNTRY,
        force_hw_chplan: 0,
        early_goto_sw_config: 0,
    };
    let mut scratch = HwBranchScratch {
        country_ent: core::ptr::null(),
    };
    config_chplan_hw_full(hw_alpha2, hw_chplan_raw, autoload_fail, &mut hw_out, &mut scratch);

    let chplan = hw_out.chplan;
    let country_ent = scratch.country_ent;
    let force_hw = hw_out.force_hw_chplan;

    config_chplan_sw_and_done(
        sw_alpha2,
        sw_chplan,
        force_hw,
        chplan,
        country_ent,
    )
}

#[cfg(not(host_hal_com_config_chplan_full_test))]
#[no_mangle]
pub unsafe extern "C" fn hal_com_config_channel_plan(
    padapter: *mut c_void,
    hw_alpha2: *mut c_char,
    hw_chplan: U8,
    sw_alpha2: *mut c_char,
    sw_chplan: U8,
    autoload_fail: U8,
) {
    let (chplan, country_ent, force_hw) = config_chplan_full(
        hw_alpha2 as *const c_char,
        hw_chplan,
        sw_alpha2 as *const c_char,
        sw_chplan,
        autoload_fail,
    );
    unsafe {
        rtw_rust_hal_com_apply_chplan_fields(padapter, country_ent, chplan, force_hw);
    }
}

#[cfg(host_hal_com_config_chplan_full_test)]
#[no_mangle]
pub unsafe extern "C" fn host_hal_com_config_channel_plan(
    padapter: *mut HostConfigChplanAdapter,
    hw_alpha2: *const c_char,
    hw_chplan: U8,
    sw_alpha2: *const c_char,
    sw_chplan: U8,
    autoload_fail: U8,
) {
    if padapter.is_null() {
        return;
    }
    let (chplan, country_ent, force_hw) =
        config_chplan_full(hw_alpha2, hw_chplan, sw_alpha2, sw_chplan, autoload_fail);
    (*padapter).rfctl.channel_plan = chplan as U8;
    (*padapter).rfctl.country_ent = country_ent;
    (*padapter).hal_data.b_disable_sw_channel_plan = force_hw;
}