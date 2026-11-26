// SPDX-License-Identifier: GPL-2.0
// CONFIG_GPIO_WAKEUP — rtw_hal_set_{output,input}_gpio (W4-140).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;

const REG_GPIO_PIN_CTRL: U32 = 0x0044;
const REG_GPIO_PIN_CTRL_2: U32 = 0x0060;

fn bit(index: U8) -> U8 {
    1u8 << index
}

extern "C" {
    fn rtw_rust_hal_com_read8(adapter: *mut c_void, addr: U32) -> U8;
    fn rtw_rust_hal_com_write8(adapter: *mut c_void, addr: U32, val: U8);
    #[cfg(CONFIG_RTL8192F)]
    fn rtw_rust_hal_com_gpio_pin_output_8192f(adapter: *mut c_void, outputval: *mut U8);
    #[cfg(CONFIG_RTL8192F)]
    fn rtw_rust_hal_com_gpio_pin_input_8192f(adapter: *mut c_void, index: *mut U8);
    #[cfg(not(CONFIG_RTL8192F))]
    fn rtw_rust_hal_com_gpio_pin_invalid_output(
        adapter: *mut c_void,
        index: U8,
        outputval: U8,
    );
    #[cfg(not(CONFIG_RTL8192F))]
    fn rtw_rust_hal_com_gpio_pin_invalid_input(adapter: *mut c_void, index: U8);
}

fn mmio_output_low(adapter: *mut c_void, index: U8, outputval: U8) {
    unsafe {
        let reg_base = REG_GPIO_PIN_CTRL;
        rtw_rust_hal_com_write8(
            adapter,
            reg_base + 3,
            rtw_rust_hal_com_read8(adapter, reg_base + 3) & !bit(index),
        );
        rtw_rust_hal_com_write8(
            adapter,
            reg_base + 2,
            rtw_rust_hal_com_read8(adapter, reg_base + 2) | bit(index),
        );
        if outputval != 0 {
            rtw_rust_hal_com_write8(
                adapter,
                reg_base + 1,
                rtw_rust_hal_com_read8(adapter, reg_base + 1) | bit(index),
            );
        } else {
            rtw_rust_hal_com_write8(
                adapter,
                reg_base + 1,
                rtw_rust_hal_com_read8(adapter, reg_base + 1) & !bit(index),
            );
        }
    }
}

fn mmio_output_high(adapter: *mut c_void, mut index: U8, outputval: U8) {
    index = index.saturating_sub(8);
    unsafe {
        let reg_base = REG_GPIO_PIN_CTRL_2;
        rtw_rust_hal_com_write8(
            adapter,
            reg_base + 3,
            rtw_rust_hal_com_read8(adapter, reg_base + 3) & !bit(index),
        );
        rtw_rust_hal_com_write8(
            adapter,
            reg_base + 2,
            rtw_rust_hal_com_read8(adapter, reg_base + 2) | bit(index),
        );
        if outputval != 0 {
            rtw_rust_hal_com_write8(
                adapter,
                reg_base + 1,
                rtw_rust_hal_com_read8(adapter, reg_base + 1) | bit(index),
            );
        } else {
            rtw_rust_hal_com_write8(
                adapter,
                reg_base + 1,
                rtw_rust_hal_com_read8(adapter, reg_base + 1) & !bit(index),
            );
        }
    }
}

fn mmio_input_low(adapter: *mut c_void, index: U8) {
    unsafe {
        let reg_base = REG_GPIO_PIN_CTRL;
        rtw_rust_hal_com_write8(
            adapter,
            reg_base + 3,
            rtw_rust_hal_com_read8(adapter, reg_base + 3) & !bit(index),
        );
        rtw_rust_hal_com_write8(
            adapter,
            reg_base + 2,
            rtw_rust_hal_com_read8(adapter, reg_base + 2) & !bit(index),
        );
    }
}

fn mmio_input_high(adapter: *mut c_void, mut index: U8) {
    index = index.saturating_sub(8);
    unsafe {
        let reg_base = REG_GPIO_PIN_CTRL_2;
        rtw_rust_hal_com_write8(
            adapter,
            reg_base + 3,
            rtw_rust_hal_com_read8(adapter, reg_base + 3) & !bit(index),
        );
        rtw_rust_hal_com_write8(
            adapter,
            reg_base + 2,
            rtw_rust_hal_com_read8(adapter, reg_base + 2) & !bit(index),
        );
    }
}

#[cfg(any(config_gpio_wakeup, host_hal_com_gpio_pin_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_set_output_gpio(adapter: *mut c_void, index: U8, outputval: U8) {
    #[cfg(CONFIG_RTL8192F)]
    {
        let mut val = outputval;
        unsafe { rtw_rust_hal_com_gpio_pin_output_8192f(adapter, &mut val) };
        return;
    }
    #[cfg(not(CONFIG_RTL8192F))]
    {
        if index <= 7 {
            mmio_output_low(adapter, index, outputval);
        } else if index <= 15 {
            mmio_output_high(adapter, index, outputval);
        } else {
            unsafe { rtw_rust_hal_com_gpio_pin_invalid_output(adapter, index, outputval) };
        }
    }
}

#[cfg(any(config_gpio_wakeup, host_hal_com_gpio_pin_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_set_input_gpio(adapter: *mut c_void, index: U8) {
    #[cfg(CONFIG_RTL8192F)]
    {
        let mut idx = index;
        unsafe { rtw_rust_hal_com_gpio_pin_input_8192f(adapter, &mut idx) };
        return;
    }
    #[cfg(not(CONFIG_RTL8192F))]
    {
        if index <= 7 {
            mmio_input_low(adapter, index);
        } else if index <= 15 {
            mmio_input_high(adapter, index);
        } else {
            unsafe { rtw_rust_hal_com_gpio_pin_invalid_input(adapter, index) };
        }
    }
}