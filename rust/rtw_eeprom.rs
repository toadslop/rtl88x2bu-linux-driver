// SPDX-License-Identifier: GPL-2.0
//! W3-116 eeprom bit-bang — Rust port (host L2).

#![allow(dead_code, improper_ctypes, non_snake_case, non_camel_case_types)]

#[cfg(host_eeprom_bitbang_test)]
mod host {
    const _TRUE: u16 = 1;
    const _FALSE: u16 = 0;
    const _EEDO: u16 = 1;
    const _EEDI: u16 = 2;
    const _EESK: u16 = 4;
    const _EECS: u16 = 8;
    const EE_9346CR: u32 = 0x000A;
    const CLOCK_RATE: u32 = 50;

    #[repr(C)]
    pub struct host_eeprom_adapter {
        pub surprise_removed: u8,
    }

    static mut G_AD: host_eeprom_adapter = host_eeprom_adapter {
        surprise_removed: 0,
    };
    static mut G_REG: u8 = 0;
    static mut G_WRITES: i32 = 0;
    static mut G_READ_SEQ: [u8; 128] = [0; 128];
    static mut G_READ_SEQ_LEN: i32 = 0;
    static mut G_READ_SEQ_I: i32 = 0;

    unsafe fn rd(_p: *mut host_eeprom_adapter, _a: u32) -> u8 {
        if G_READ_SEQ_LEN > 0 {
            let i = (G_READ_SEQ_I as usize) % (G_READ_SEQ_LEN as usize);
            G_READ_SEQ_I += 1;
            return G_READ_SEQ[i];
        }
        G_REG
    }
    unsafe fn wr(_p: *mut host_eeprom_adapter, _a: u32, v: u8) {
        G_REG = v;
        G_WRITES += 1;
    }
    unsafe fn sr(p: *mut host_eeprom_adapter) -> bool {
        (*p).surprise_removed != 0
    }

    #[no_mangle]
    pub extern "C" fn host_eeprom_reset() {
        unsafe {
            G_AD.surprise_removed = 0;
            G_REG = 0;
            G_WRITES = 0;
            G_READ_SEQ_LEN = 0;
            G_READ_SEQ_I = 0;
        }
    }
    #[no_mangle]
    pub extern "C" fn host_eeprom_set_surprise(on: u8) {
        unsafe {
            G_AD.surprise_removed = if on != 0 { 1 } else { 0 };
        }
    }
    #[no_mangle]
    pub extern "C" fn host_eeprom_set_reg(v: u8) {
        unsafe {
            G_REG = v;
        }
    }
    #[no_mangle]
    pub extern "C" fn host_eeprom_reg() -> u8 {
        unsafe { G_REG }
    }
    #[no_mangle]
    pub extern "C" fn host_eeprom_write_count() -> i32 {
        unsafe { G_WRITES }
    }
    #[no_mangle]
    pub extern "C" fn host_eeprom_set_read_sequence(vals: *const u8, n: i32) {
        unsafe {
            G_READ_SEQ_LEN = 0;
            G_READ_SEQ_I = 0;
            if vals.is_null() || n <= 0 {
                return;
            }
            let mut m = n as usize;
            if m > 128 {
                m = 128;
            }
            core::ptr::copy_nonoverlapping(vals, G_READ_SEQ.as_mut_ptr(), m);
            G_READ_SEQ_LEN = m as i32;
        }
    }
    #[no_mangle]
    pub extern "C" fn host_eeprom_adapter() -> *mut host_eeprom_adapter {
        core::ptr::addr_of_mut!(G_AD)
    }

    #[no_mangle]
    pub extern "C" fn up_clk(p: *mut host_eeprom_adapter, x: *mut u16) {
        unsafe {
            *x |= _EESK;
            wr(p, EE_9346CR, *x as u8);
        }
    }
    #[no_mangle]
    pub extern "C" fn down_clk(p: *mut host_eeprom_adapter, x: *mut u16) {
        unsafe {
            *x &= !_EESK;
            wr(p, EE_9346CR, *x as u8);
        }
    }
    #[no_mangle]
    pub extern "C" fn shift_out_bits(p: *mut host_eeprom_adapter, data: u16, count: u16) {
        unsafe {
            if sr(p) {
                return;
            }
            let mut mask = 1u16 << (count - 1);
            let mut x = rd(p, EE_9346CR) as u16;
            x &= !(_EEDO | _EEDI);
            while mask != 0 {
                x &= !_EEDI;
                if data & mask != 0 {
                    x |= _EEDI;
                }
                if sr(p) {
                    return;
                }
                wr(p, EE_9346CR, x as u8);
                up_clk(p, &mut x);
                down_clk(p, &mut x);
                mask >>= 1;
            }
            x &= !_EEDI;
            wr(p, EE_9346CR, x as u8);
        }
    }
    #[no_mangle]
    pub extern "C" fn shift_in_bits(p: *mut host_eeprom_adapter) -> u16 {
        unsafe {
            if sr(p) {
                return 0;
            }
            let mut x = rd(p, EE_9346CR) as u16;
            x &= !(_EEDO | _EEDI);
            let mut d = 0u16;
            for _ in 0..16 {
                d <<= 1;
                up_clk(p, &mut x);
                if sr(p) {
                    return d;
                }
                x = rd(p, EE_9346CR) as u16;
                x &= !_EEDI;
                if x & _EEDO != 0 {
                    d |= 1;
                }
                down_clk(p, &mut x);
            }
            d
        }
    }
    #[no_mangle]
    pub extern "C" fn standby(p: *mut host_eeprom_adapter) {
        unsafe {
            let mut x = rd(p, EE_9346CR);
            x &= !((_EECS | _EESK) as u8);
            wr(p, EE_9346CR, x);
            x |= _EECS as u8;
            wr(p, EE_9346CR, x);
        }
    }
    #[no_mangle]
    pub extern "C" fn wait_eeprom_cmd_done(p: *mut host_eeprom_adapter) -> u16 {
        unsafe {
            standby(p);
            for _ in 0..200 {
                if rd(p, EE_9346CR) & (_EEDO as u8) != 0 {
                    return _TRUE;
                }
            }
            _FALSE
        }
    }
    #[no_mangle]
    pub extern "C" fn eeprom_clean(p: *mut host_eeprom_adapter) {
        unsafe {
            if sr(p) {
                return;
            }
            let mut x = rd(p, EE_9346CR) as u16;
            x &= !(_EECS | _EEDI);
            wr(p, EE_9346CR, x as u8);
            up_clk(p, &mut x);
            down_clk(p, &mut x);
        }
    }
}

#[cfg(not(host_eeprom_bitbang_test))]
fn _rtw_eeprom_ko_stub() {}
