// SPDX-License-Identifier: GPL-2.0
// CONFIG_GPIO_API — rtw_hal_get_gpio / set_gpio_output_value / config_gpio (W4-142).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;

const REG_GPIO_PIN_CTRL: U32 = 0x0044;
const REG_GPIO_PIN_CTRL_2: U32 = 0x0060;
const PS_DENY_IOCTL: U32 = 5;
const _FAIL: i32 = 0;

/// Match kernel `BIT(x)` on `int` masked through a single MMIO `u8` (no shift panic).
#[inline]
fn byte_bit(byte: U8, index: U8) -> U8 {
    if index >= 8 {
        return 0;
    }
    ((byte as u32 >> index) & 1) as u8
}

#[inline]
fn set_byte_bit(byte: U8, index: U8) -> U8 {
    if index >= 8 {
        return byte;
    }
    byte | (1u8 << index)
}

#[inline]
fn clear_byte_bit(byte: U8, index: U8) -> U8 {
    if index >= 8 {
        return byte;
    }
    byte & !(1u8 << index)
}

#[cfg(not(host_hal_com_gpio_api_test))]
extern "C" {
    fn rtw_hal_gpio_func_check(adapter: *mut c_void, gpio_num: U8) -> i32;
    fn rtw_hal_gpio_multi_func_reset(adapter: *mut c_void, gpio_num: U8);
    fn rtw_ps_deny(adapter: *mut c_void, reason: U32);
    fn rtw_ps_deny_cancel(adapter: *mut c_void, reason: U32);
    fn LeaveAllPowerSaveModeDirect(adapter: *mut c_void);
    fn rtw_rust_hal_com_read8(adapter: *mut c_void, addr: U32) -> U8;
    fn rtw_rust_hal_com_write8(adapter: *mut c_void, addr: U32, val: U8);
    fn rtw_rust_hal_com_gpio_api_log_get_rf_pwrstate(adapter: *mut c_void);
    fn rtw_rust_hal_com_gpio_api_log_get_done(
        func: *const u8,
        direction: U8,
        value: U8,
    );
    fn rtw_rust_hal_com_gpio_api_log_set_output(
        func: *const u8,
        gpio_num: U8,
        is_high: U8,
    );
    fn rtw_rust_hal_com_gpio_api_log_set_input(func: *const u8);
    fn rtw_rust_hal_com_gpio_api_log_config(
        func: *const u8,
        gpio_num: U8,
        is_output: U8,
    );
}

#[cfg(host_hal_com_gpio_api_test)]
extern "C" {
    fn rtw_hal_gpio_func_check(adapter: *mut c_void, gpio_num: U8) -> i32;
    fn rtw_hal_gpio_multi_func_reset(adapter: *mut c_void, gpio_num: U8);
    fn rtw_ps_deny(adapter: *mut c_void, reason: U32);
    fn rtw_ps_deny_cancel(adapter: *mut c_void, reason: U32);
    fn LeaveAllPowerSaveModeDirect(adapter: *mut c_void);
    fn rtw_read8(adapter: *mut c_void, addr: U32) -> U8;
    fn rtw_write8(adapter: *mut c_void, addr: U32, val: U8);
    fn host_gpio_api_log_get_rf_pwrstate(adapter: *mut c_void);
}

#[inline]
unsafe fn hal_read8(adapter: *mut c_void, addr: U32) -> U8 {
    #[cfg(host_hal_com_gpio_api_test)]
    {
        unsafe { rtw_read8(adapter, addr) }
    }
    #[cfg(not(host_hal_com_gpio_api_test))]
    {
        unsafe { rtw_rust_hal_com_read8(adapter, addr) }
    }
}

#[inline]
unsafe fn hal_write8(adapter: *mut c_void, addr: U32, val: U8) {
    #[cfg(host_hal_com_gpio_api_test)]
    {
        unsafe { rtw_write8(adapter, addr, val) };
    }
    #[cfg(not(host_hal_com_gpio_api_test))]
    {
        unsafe { rtw_rust_hal_com_write8(adapter, addr, val) };
    }
}

fn gpio_regs(gpio_num: U8) -> (U32, U32, U32, U8) {
    let mut gpio_pin_input_val = REG_GPIO_PIN_CTRL;
    let mut gpio_pin_output_val = REG_GPIO_PIN_CTRL + 1;
    let mut gpio_pin_output_en = REG_GPIO_PIN_CTRL + 2;
    let mut gpio_num_to_set = gpio_num;

    if gpio_num > 7 {
        gpio_pin_input_val = REG_GPIO_PIN_CTRL_2;
        gpio_pin_output_val = REG_GPIO_PIN_CTRL_2 + 1;
        gpio_pin_output_en = REG_GPIO_PIN_CTRL_2 + 2;
        gpio_num_to_set = gpio_num - 8;
    }
    (
        gpio_pin_input_val,
        gpio_pin_output_val,
        gpio_pin_output_en,
        gpio_num_to_set,
    )
}

#[cfg(any(config_gpio_api, host_hal_com_gpio_api_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_get_gpio(adapter: *mut c_void, gpio_num: U8) -> U8 {
    let mut value = 0u8;
    let (gpio_pin_input_val, gpio_pin_output_val, gpio_pin_output_en, gpio_num_to_set) =
        gpio_regs(gpio_num);

    if unsafe { rtw_hal_gpio_func_check(adapter, gpio_num) } == _FAIL {
        return value;
    }

    unsafe {
        rtw_ps_deny(adapter, PS_DENY_IOCTL);
        #[cfg(not(host_hal_com_gpio_api_test))]
        {
            rtw_rust_hal_com_gpio_api_log_get_rf_pwrstate(adapter);
        }
        #[cfg(host_hal_com_gpio_api_test)]
        {
            host_gpio_api_log_get_rf_pwrstate(adapter);
        }
        LeaveAllPowerSaveModeDirect(adapter);

        let direction = byte_bit(hal_read8(adapter, gpio_pin_output_en), gpio_num_to_set);

        value = if direction != 0 {
            byte_bit(hal_read8(adapter, gpio_pin_output_val), gpio_num_to_set)
        } else {
            byte_bit(hal_read8(adapter, gpio_pin_input_val), gpio_num_to_set)
        };

        rtw_ps_deny_cancel(adapter, PS_DENY_IOCTL);
        #[cfg(not(host_hal_com_gpio_api_test))]
        unsafe {
            rtw_rust_hal_com_gpio_api_log_get_done(
                b"rtw_hal_get_gpio\0".as_ptr(),
                direction,
                value,
            );
        }
    }

    value
}

#[cfg(any(config_gpio_api, host_hal_com_gpio_api_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_set_gpio_output_value(adapter: *mut c_void, gpio_num: U8, is_high: U8) -> i32 {
    let mut res: U8 = 0xff;
    let (_, gpio_pin_output_val, gpio_pin_output_en, gpio_num_to_set) = gpio_regs(gpio_num);

    if unsafe { rtw_hal_gpio_func_check(adapter, gpio_num) } == _FAIL {
        return -1;
    }

    unsafe {
        rtw_ps_deny(adapter, PS_DENY_IOCTL);
        LeaveAllPowerSaveModeDirect(adapter);

        let direction = byte_bit(hal_read8(adapter, gpio_pin_output_en), gpio_num_to_set);

        if direction != 0 {
            if is_high != 0 {
                hal_write8(
                    adapter,
                    gpio_pin_output_val,
                    set_byte_bit(hal_read8(adapter, gpio_pin_output_val), gpio_num_to_set),
                );
            } else {
                hal_write8(
                    adapter,
                    gpio_pin_output_val,
                    clear_byte_bit(hal_read8(adapter, gpio_pin_output_val), gpio_num_to_set),
                );
            }
            res = 0;
            #[cfg(not(host_hal_com_gpio_api_test))]
            unsafe {
                rtw_rust_hal_com_gpio_api_log_set_output(
                    b"rtw_hal_set_gpio_output_value\0".as_ptr(),
                    gpio_num,
                    is_high,
                );
            }
        } else {
            #[cfg(not(host_hal_com_gpio_api_test))]
            unsafe {
                rtw_rust_hal_com_gpio_api_log_set_input(
                    b"rtw_hal_set_gpio_output_value\0".as_ptr(),
                );
            }
        }

        rtw_ps_deny_cancel(adapter, PS_DENY_IOCTL);
    }

    res as i32
}

#[cfg(any(config_gpio_api, host_hal_com_gpio_api_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_config_gpio(adapter: *mut c_void, gpio_num: U8, is_output: U8) -> i32 {
    if unsafe { rtw_hal_gpio_func_check(adapter, gpio_num) } == _FAIL {
        return -1;
    }

    #[cfg(not(host_hal_com_gpio_api_test))]
    unsafe {
        rtw_rust_hal_com_gpio_api_log_config(
            b"rtw_hal_config_gpio\0".as_ptr(),
            gpio_num,
            is_output,
        );
    }

    unsafe {
        rtw_ps_deny(adapter, PS_DENY_IOCTL);
        LeaveAllPowerSaveModeDirect(adapter);
        rtw_hal_gpio_multi_func_reset(adapter, gpio_num);
    }

    let mut gpio_ctrl_reg_to_set = REG_GPIO_PIN_CTRL + 2;
    let mut gpio_num_to_set = gpio_num;
    if gpio_num > 7 {
        gpio_ctrl_reg_to_set = REG_GPIO_PIN_CTRL_2 + 2;
        gpio_num_to_set = gpio_num - 8;
    }

    unsafe {
        if is_output != 0 {
            hal_write8(
                adapter,
                gpio_ctrl_reg_to_set,
                set_byte_bit(hal_read8(adapter, gpio_ctrl_reg_to_set), gpio_num_to_set),
            );
        } else {
            hal_write8(
                adapter,
                gpio_ctrl_reg_to_set,
                clear_byte_bit(hal_read8(adapter, gpio_ctrl_reg_to_set), gpio_num_to_set),
            );
        }
        rtw_ps_deny_cancel(adapter, PS_DENY_IOCTL);
    }

    0
}