// SPDX-License-Identifier: GPL-2.0
//! HAL PHY helpers — Rust port of `hal/hal_phy.c` / `hal/hal_phy_rest.c`
//! (W4-09 PHY_CalculateBitShift, W4-18 RF shadow read/write/refresh,
//! W4-19 RF shadow compare/recover flag helpers, W4-20 compare/recover,
//! W4-35 RF6052 max reg query).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe
)]

type U8 = u8;
type U32 = u32;

#[no_mangle]
pub extern "C" fn PHY_CalculateBitShift(bit_mask: U32) -> U32 {
    for i in 0..=31 {
        if (bit_mask >> i) & 0x1 == 1 {
            return i;
        }
    }
    32
}

const RF6052_MAX_REG_88E_U32: U32 = 0xFF;
const RF6052_MAX_REG_92C_U32: U32 = 0x7F;

const HARDWARE_TYPE_RTL8188EE: U8 = 0;
const HARDWARE_TYPE_RTL8188EU: U8 = 1;
const HARDWARE_TYPE_RTL8188ES: U8 = 2;

fn is_hardware_type_8188e(hw_type: U8) -> bool {
    hw_type == HARDWARE_TYPE_RTL8188EE
        || hw_type == HARDWARE_TYPE_RTL8188EU
        || hw_type == HARDWARE_TYPE_RTL8188ES
}

fn rf6052_real_max_reg_u32(is_8188e: bool) -> U32 {
    if is_8188e {
        RF6052_MAX_REG_88E_U32
    } else {
        RF6052_MAX_REG_92C_U32
    }
}

#[cfg(host_hal_phy_rf_max_reg_test)]
#[repr(C)]
pub struct HostHalPhyRfMaxRegAdapter {
    pub hw_type: U8,
}

#[cfg(all(
    not(host_hal_phy_rf_max_reg_test),
    not(host_hal_phy_rf_shadow_test),
    not(host_hal_phy_bit_shift_test),
))]
extern "C" {
    fn rtw_rust_hal_com_get_hw_type(adapter: *mut core::ffi::c_void) -> U8;
}

#[cfg(any(
    host_hal_phy_rf_max_reg_test,
    config_rf_shadow_rw,
    host_hal_phy_rf_shadow_test,
    host_hal_phy_rf_shadow_compare_test,
))]
#[no_mangle]
pub extern "C" fn rtw_rust_hal_phy_query_rf_max_reg(adapter: *mut core::ffi::c_void) -> U32 {
    if adapter.is_null() {
        return RF6052_MAX_REG_92C_U32;
    }

    #[cfg(host_hal_phy_rf_max_reg_test)]
    {
        let a = unsafe { &*(adapter as *const HostHalPhyRfMaxRegAdapter) };
        return rf6052_real_max_reg_u32(is_hardware_type_8188e(a.hw_type));
    }

    #[cfg(all(host_hal_phy_rf_shadow_test, not(host_hal_phy_rf_max_reg_test)))]
    {
        let a = unsafe { &*(adapter as *const rf_shadow::HostRfShadowAdapter) };
        return rf6052_real_max_reg_u32(a.is_8188e != 0);
    }

    #[cfg(not(any(host_hal_phy_rf_max_reg_test, host_hal_phy_rf_shadow_test)))]
    {
        let hw = unsafe { rtw_rust_hal_com_get_hw_type(adapter) };
        return rf6052_real_max_reg_u32(is_hardware_type_8188e(hw));
    }
}

#[cfg(any(config_rf_shadow_rw, host_hal_phy_rf_shadow_test))]
mod rf_shadow {
    use super::{rtw_rust_hal_phy_query_rf_max_reg, U32, U8, RF6052_MAX_REG_88E_U32, RF6052_MAX_REG_92C_U32};

    pub const RF6052_MAX_REG_88E: usize = RF6052_MAX_REG_88E_U32 as usize;
    pub const RF6052_MAX_REG_92C: usize = RF6052_MAX_REG_92C_U32 as usize;
    pub const RF6052_MAX_REG: usize = RF6052_MAX_REG_88E;
    pub const RF6052_MAX_PATH: usize = 2;
    const B_RF_REG_OFFSET_MASK: U32 = 0xfffff;
    const TRUE: U8 = 1;
    const FALSE: U8 = 0;

    #[repr(C)]
    #[derive(Copy, Clone)]
    pub struct RfShadowT {
        pub Value: U32,
        pub Compare: U8,
        pub ErrorOrNot: U8,
        pub Recorver: U8,
        pub Driver_Write: U8,
    }

    #[no_mangle]
    pub static mut rtw_rust_rf_shadow: [[RfShadowT; RF6052_MAX_REG]; RF6052_MAX_PATH] =
        [[RfShadowT {
            Value: 0,
            Compare: 0,
            ErrorOrNot: 0,
            Recorver: 0,
            Driver_Write: 0,
        }; RF6052_MAX_REG]; RF6052_MAX_PATH];

    #[cfg(host_hal_phy_rf_shadow_test)]
    pub type Adapter = *mut core::ffi::c_void;

    #[cfg(not(host_hal_phy_rf_shadow_test))]
    pub type Adapter = *mut u8;

    #[cfg(host_hal_phy_rf_shadow_test)]
    #[repr(C)]
    pub struct HostRfShadowAdapter {
        pub is_8188e: U8,
    }

    fn max_reg(adapter: Adapter) -> usize {
        if adapter.is_null() {
            return RF6052_MAX_REG_92C;
        }
        let n = unsafe { rtw_rust_hal_phy_query_rf_max_reg(adapter as *mut core::ffi::c_void) };
        n as usize
    }

    #[cfg(any(config_rf_shadow_rw, host_hal_phy_rf_shadow_compare_test))]
    extern "C" {
        fn rtw_hal_read_rfreg(adapter: Adapter, path: i32, offset: U32, mask: U32) -> U32;
        fn rtw_hal_write_rfreg(adapter: Adapter, path: i32, offset: U32, mask: U32, data: U32);
    }

    #[no_mangle]
    pub extern "C" fn rtw_rust_hal_phy_rf_max_reg(adapter: Adapter) -> U32 {
        max_reg(adapter) as U32
    }

    #[no_mangle]
    pub extern "C" fn PHY_RFShadowRead(_adapter: Adapter, e_rf_path: i32, offset: U32) -> U32 {
        let path = e_rf_path as usize;
        let off = offset as usize;
        if path >= RF6052_MAX_PATH || off >= RF6052_MAX_REG {
            return 0;
        }
        unsafe { rtw_rust_rf_shadow[path][off].Value }
    }

    #[no_mangle]
    pub extern "C" fn PHY_RFShadowWrite(_adapter: Adapter, e_rf_path: i32, offset: U32, data: U32) {
        let path = e_rf_path as usize;
        let off = offset as usize;
        if path >= RF6052_MAX_PATH || off >= RF6052_MAX_REG {
            return;
        }
        unsafe {
            rtw_rust_rf_shadow[path][off].Value = data & B_RF_REG_OFFSET_MASK;
            rtw_rust_rf_shadow[path][off].Driver_Write = TRUE;
        }
    }

    #[no_mangle]
    pub extern "C" fn PHY_RFShadowRefresh(adapter: Adapter) {
        let max = max_reg(adapter);
        for path in 0..RF6052_MAX_PATH {
            for off in 0..max {
                unsafe {
                    let e = &mut rtw_rust_rf_shadow[path][off];
                    e.Value = 0;
                    e.Compare = FALSE;
                    e.Recorver = FALSE;
                    e.ErrorOrNot = FALSE;
                    e.Driver_Write = FALSE;
                }
            }
        }
    }

    fn flag_set_all(compare: bool, adapter: Adapter) {
        let max = max_reg(adapter);
        for path in 0..RF6052_MAX_PATH {
            for off in 0..max {
                let on = off == 0x26 || off == 0x27;
                let ty = if on { TRUE } else { FALSE };
                unsafe {
                    let e = &mut rtw_rust_rf_shadow[path][off];
                    if compare {
                        e.Compare = ty;
                    } else {
                        e.Recorver = ty;
                    }
                }
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn PHY_RFShadowCompareFlagSet(
        _adapter: Adapter,
        e_rf_path: i32,
        offset: U32,
        ty: U8,
    ) {
        let path = e_rf_path as usize;
        let off = offset as usize;
        if path >= RF6052_MAX_PATH || off >= RF6052_MAX_REG {
            return;
        }
        unsafe {
            rtw_rust_rf_shadow[path][off].Compare = ty;
        }
    }

    #[no_mangle]
    pub extern "C" fn PHY_RFShadowRecorverFlagSet(
        _adapter: Adapter,
        e_rf_path: i32,
        offset: U32,
        ty: U8,
    ) {
        let path = e_rf_path as usize;
        let off = offset as usize;
        if path >= RF6052_MAX_PATH || off >= RF6052_MAX_REG {
            return;
        }
        unsafe {
            rtw_rust_rf_shadow[path][off].Recorver = ty;
        }
    }

    #[no_mangle]
    pub extern "C" fn PHY_RFShadowCompareFlagSetAll(adapter: Adapter) {
        flag_set_all(true, adapter);
    }

    #[no_mangle]
    pub extern "C" fn PHY_RFShadowRecorverFlagSetAll(adapter: Adapter) {
        flag_set_all(false, adapter);
    }

    #[cfg(host_hal_phy_rf_shadow_test)]
    #[no_mangle]
    pub extern "C" fn PHY_RFShadowTestPeekCompare(e_rf_path: i32, offset: U32) -> U8 {
        let path = e_rf_path as usize;
        let off = offset as usize;
        if path >= RF6052_MAX_PATH || off >= RF6052_MAX_REG {
            return 0;
        }
        unsafe { rtw_rust_rf_shadow[path][off].Compare }
    }

    #[cfg(host_hal_phy_rf_shadow_test)]
    #[no_mangle]
    pub extern "C" fn PHY_RFShadowTestPeekRecorver(e_rf_path: i32, offset: U32) -> U8 {
        let path = e_rf_path as usize;
        let off = offset as usize;
        if path >= RF6052_MAX_PATH || off >= RF6052_MAX_REG {
            return 0;
        }
        unsafe { rtw_rust_rf_shadow[path][off].Recorver }
    }

    #[cfg(host_hal_phy_rf_shadow_compare_test)]
    #[no_mangle]
    pub extern "C" fn PHY_RFShadowTestPeekErrorOrNot(e_rf_path: i32, offset: U32) -> U8 {
        let path = e_rf_path as usize;
        let off = offset as usize;
        if path >= RF6052_MAX_PATH || off >= RF6052_MAX_REG {
            return 0;
        }
        unsafe { rtw_rust_rf_shadow[path][off].ErrorOrNot }
    }

    #[cfg(any(config_rf_shadow_rw, host_hal_phy_rf_shadow_compare_test))]
    #[no_mangle]
    pub extern "C" fn PHY_RFShadowCompare(adapter: Adapter, e_rf_path: i32, offset: U32) -> U8 {
        let path = e_rf_path as usize;
        let off = offset as usize;
        if path >= RF6052_MAX_PATH || off >= RF6052_MAX_REG {
            return FALSE;
        }
        unsafe {
            let e = &mut rtw_rust_rf_shadow[path][off];
            if e.Compare != TRUE {
                return FALSE;
            }
            let reg = rtw_hal_read_rfreg(adapter, e_rf_path, offset, B_RF_REG_OFFSET_MASK);
            if e.Value != reg {
                e.ErrorOrNot = TRUE;
            }
            e.ErrorOrNot
        }
    }

    #[cfg(any(config_rf_shadow_rw, host_hal_phy_rf_shadow_compare_test))]
    #[no_mangle]
    pub extern "C" fn PHY_RFShadowRecorver(adapter: Adapter, e_rf_path: i32, offset: U32) {
        let path = e_rf_path as usize;
        let off = offset as usize;
        if path >= RF6052_MAX_PATH || off >= RF6052_MAX_REG {
            return;
        }
        unsafe {
            let e = &mut rtw_rust_rf_shadow[path][off];
            if e.ErrorOrNot != TRUE {
                return;
            }
            if e.Recorver != TRUE {
                return;
            }
            rtw_hal_write_rfreg(adapter, e_rf_path, offset, B_RF_REG_OFFSET_MASK, e.Value);
        }
    }

    #[cfg(any(config_rf_shadow_rw, host_hal_phy_rf_shadow_compare_test))]
    fn compare_or_recover_all(adapter: Adapter, compare: bool) {
        let max = max_reg(adapter);
        for path in 0..RF6052_MAX_PATH {
            for off in 0..max {
                if compare {
                    PHY_RFShadowCompare(adapter, path as i32, off as U32);
                } else {
                    PHY_RFShadowRecorver(adapter, path as i32, off as U32);
                }
            }
        }
    }

    #[cfg(any(config_rf_shadow_rw, host_hal_phy_rf_shadow_compare_test))]
    #[no_mangle]
    pub extern "C" fn PHY_RFShadowCompareAll(adapter: Adapter) {
        compare_or_recover_all(adapter, true);
    }

    #[cfg(any(config_rf_shadow_rw, host_hal_phy_rf_shadow_compare_test))]
    #[no_mangle]
    pub extern "C" fn PHY_RFShadowRecorverAll(adapter: Adapter) {
        compare_or_recover_all(adapter, false);
    }
}
