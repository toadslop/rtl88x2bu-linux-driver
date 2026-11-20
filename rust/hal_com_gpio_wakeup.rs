// SPDX-License-Identifier: GPL-2.0
// CONFIG_GPIO_WAKEUP — rtw_hal_switch_gpio_wl_ctrl (W4-123).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;

const TRUE: U8 = 1;

#[cfg(not(host_hal_com_gpio_wakeup_test))]
extern "C" {
    fn rtw_rust_hal_com_set_hwreg(adapter: *mut c_void, variable: U32, val: *mut U8);
    fn rtw_rust_hal_com_hw_set_gpio_wl_ctrl_var() -> i32;
    fn rtw_rust_hal_com_gpio_wl_series_match(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_gpio_wl_led_allow_disable(adapter: *mut c_void, enable: U8) -> U8;
}

#[cfg(host_hal_com_gpio_wakeup_test)]
extern "C" {
    fn rtw_rust_hal_com_set_hwreg(adapter: *mut c_void, variable: U32, val: *mut U8);
    fn rtw_rust_hal_com_hw_set_gpio_wl_ctrl_var() -> i32;
    fn rtw_rust_hal_com_gpio_wl_series_match(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_gpio_wl_led_allow_disable(adapter: *mut c_void, enable: U8) -> U8;
}

fn prog_gpio_wl_ctrl(adapter: *mut c_void, enable: U8) {
    let mut val = enable;
    unsafe {
        rtw_rust_hal_com_set_hwreg(
            adapter,
            rtw_rust_hal_com_hw_set_gpio_wl_ctrl_var() as U32,
            &mut val,
        );
    }
}

fn index_path_active(adapter: *mut c_void, index: U8, enable: U8) -> bool {
    if index != 13 && index != 14 {
        return false;
    }
    #[cfg(all(config_rtl8821a, config_sdio_hci))]
    {
        return unsafe { rtw_rust_hal_com_gpio_wl_led_allow_disable(adapter, enable) } != 0;
    }
    #[cfg(not(all(config_rtl8821a, config_sdio_hci)))]
    {
        let _ = (adapter, enable);
        return true;
    }
}

#[cfg(any(config_gpio_wakeup, host_hal_com_gpio_wakeup_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_switch_gpio_wl_ctrl(adapter: *mut c_void, index: U8, enable: U8) {
    if unsafe { rtw_rust_hal_com_gpio_wl_series_match(adapter) } == TRUE {
        prog_gpio_wl_ctrl(adapter, enable);
    }
    if index_path_active(adapter, index, enable) {
        prog_gpio_wl_ctrl(adapter, enable);
    }
}