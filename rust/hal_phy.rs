// SPDX-License-Identifier: GPL-2.0
//! HAL PHY helpers — Rust port of `hal/hal_phy.c` / `hal/hal_phy_rest.c`
//! (W4-09 PHY_CalculateBitShift, W4-18 RF shadow read/write/refresh).

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

#[cfg(any(config_rf_shadow_rw, host_hal_phy_rf_shadow_test))]
mod rf_shadow {
    use super::{U32, U8};

    pub const RF6052_MAX_REG_88E: usize = 0xFF;
    pub const RF6052_MAX_REG_92C: usize = 0x7F;
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

    #[cfg(host_hal_phy_rf_shadow_test)]
    fn max_reg(adapter: Adapter) -> usize {
        if adapter.is_null() {
            return RF6052_MAX_REG_92C;
        }
        let a = unsafe { &*(adapter as *const HostRfShadowAdapter) };
        if a.is_8188e != 0 {
            RF6052_MAX_REG_88E
        } else {
            RF6052_MAX_REG_92C
        }
    }

    #[cfg(not(host_hal_phy_rf_shadow_test))]
    extern "C" {
        fn rtw_rust_hal_phy_rf_max_reg(adapter: Adapter) -> U32;
    }

    #[cfg(not(host_hal_phy_rf_shadow_test))]
    fn max_reg(adapter: Adapter) -> usize {
        if adapter.is_null() {
            return RF6052_MAX_REG_92C;
        }
        let n = unsafe { rtw_rust_hal_phy_rf_max_reg(adapter) };
        n as usize
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
}
