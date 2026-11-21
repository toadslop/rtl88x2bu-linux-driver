// SPDX-License-Identifier: GPL-2.0
// CONFIG_TX_AC_LIFETIME — TX AC lifetime setters (W4-124).

use core::ffi::c_void;

type U8 = u8;
type U16 = u16;
type U32 = u32;

const TX_ACLT_CONF_NUM: usize = 3;
const TX_ACLT_CONF_DEFAULT: usize = 0;
const TX_ACLT_CONF_AP_M2U: usize = 1;
const REG_LIFETIME_EN: U32 = 0x0426;
const REG_PKT_LIFE_TIME: U32 = 0x04C0;

#[repr(C)]
pub struct TxAcltConf {
    pub en: U8,
    pub vo_vi: U32,
    pub be_bk: U32,
}

extern "C" {
    fn rtw_rust_hal_com_tx_aclt_unit_factor(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_tx_aclt_force_conf(adapter: *mut c_void) -> *mut TxAcltConf;
    fn rtw_rust_hal_com_tx_aclt_conf_at(adapter: *mut c_void, idx: U8) -> *mut TxAcltConf;
    fn rtw_rust_hal_com_tx_aclt_flags(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_tx_aclt_ap_m2u_num(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_tx_aclt_sta_op_num(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_tx_aclt_read8(adapter: *mut c_void, addr: U32) -> U8;
    fn rtw_rust_hal_com_tx_aclt_read32(adapter: *mut c_void, addr: U32) -> U32;
    fn rtw_rust_hal_com_tx_aclt_write8(adapter: *mut c_void, addr: U32, val: U8);
    fn rtw_rust_hal_com_tx_aclt_write32(adapter: *mut c_void, addr: U32, val: U32);
}

fn scale_lifetime(unit_factor: U8, input: U32) -> U32 {
    let div = (unit_factor as U32).wrapping_mul(32);
    if div == 0 {
        return 0;
    }
    let mut v = input / div;
    if v > 0xFFFF {
        v = 0xFFFF;
    }
    v
}

fn fold_lifetime(adapter: *mut c_void) -> (U8, U16, U16) {
    let mut lt_en: U8 = 0;
    let mut lt_vo_vi: U16 = 0xFFFF;
    let mut lt_be_bk: U16 = 0xFFFF;
    let flags = unsafe { rtw_rust_hal_com_tx_aclt_flags(adapter) };
    let ap_m2u = unsafe { rtw_rust_hal_com_tx_aclt_ap_m2u_num(adapter) };
    let sta_op = unsafe { rtw_rust_hal_com_tx_aclt_sta_op_num(adapter) };

    for i in 0..TX_ACLT_CONF_NUM {
        if flags & (1u8 << i) == 0 {
            continue;
        }
        let conf = unsafe { rtw_rust_hal_com_tx_aclt_conf_at(adapter, i as U8) };
        if conf.is_null() {
            continue;
        }
        let conf = unsafe { &*conf };
        if i == TX_ACLT_CONF_DEFAULT {
            lt_en = conf.en;
            if conf.vo_vi != 0 {
                lt_vo_vi = conf.vo_vi as U16;
            }
            if conf.be_bk != 0 {
                lt_be_bk = conf.be_bk as U16;
            }
        } else if i == TX_ACLT_CONF_AP_M2U && ap_m2u != 0 && sta_op != 0 {
            lt_en |= conf.en;
            if conf.vo_vi != 0 && (lt_vo_vi as U32) > conf.vo_vi {
                lt_vo_vi = conf.vo_vi as U16;
            }
            if conf.be_bk != 0 && (lt_be_bk as U32) > conf.be_bk {
                lt_be_bk = conf.be_bk as U16;
            }
        }
    }
    (lt_en, lt_vo_vi, lt_be_bk)
}

#[no_mangle]
pub extern "C" fn rtw_hal_set_tx_aclt_force_val(
    adapter: *mut c_void,
    input: *mut TxAcltConf,
    arg_num: u8,
) {
    if adapter.is_null() || input.is_null() {
        return;
    }
    let unit = unsafe { rtw_rust_hal_com_tx_aclt_unit_factor(adapter) };
    let conf = unsafe { rtw_rust_hal_com_tx_aclt_force_conf(adapter) };
    if conf.is_null() {
        return;
    }
    let input = unsafe { &*input };
    let conf = unsafe { &mut *conf };
    if arg_num >= 1 {
        conf.en = if input.en == 0xFF {
            input.en
        } else {
            input.en & 0xF
        };
    }
    if arg_num >= 2 {
        conf.vo_vi = scale_lifetime(unit, input.vo_vi);
    }
    if arg_num >= 3 {
        conf.be_bk = scale_lifetime(unit, input.be_bk);
    }
}

#[no_mangle]
pub extern "C" fn rtw_hal_set_tx_aclt_conf(
    adapter: *mut c_void,
    conf_idx: U8,
    input: *mut TxAcltConf,
    arg_num: U8,
) {
    if adapter.is_null() || input.is_null() || conf_idx as usize >= TX_ACLT_CONF_NUM {
        return;
    }
    let unit = unsafe { rtw_rust_hal_com_tx_aclt_unit_factor(adapter) };
    let conf = unsafe { rtw_rust_hal_com_tx_aclt_conf_at(adapter, conf_idx) };
    if conf.is_null() {
        return;
    }
    let input = unsafe { &*input };
    let conf = unsafe { &mut *conf };
    if arg_num >= 1 && input.en != 0xFF {
        conf.en = input.en & 0xF;
    }
    if arg_num >= 2 {
        conf.vo_vi = scale_lifetime(unit, input.vo_vi);
    }
    if arg_num >= 3 {
        conf.be_bk = scale_lifetime(unit, input.be_bk);
    }
}

#[no_mangle]
pub extern "C" fn rtw_hal_update_tx_aclt(adapter: *mut c_void) {
    if adapter.is_null() {
        return;
    }
    let (mut lt_en, mut lt_vo_vi, mut lt_be_bk) = fold_lifetime(adapter);

    let lt_en_ori = unsafe { rtw_rust_hal_com_tx_aclt_read8(adapter, REG_LIFETIME_EN) };
    let lt_ori = unsafe { rtw_rust_hal_com_tx_aclt_read32(adapter, REG_PKT_LIFE_TIME) };

    let force = unsafe { rtw_rust_hal_com_tx_aclt_force_conf(adapter) };
    if !force.is_null() {
        let force = unsafe { &*force };
        if force.en != 0xFF {
            lt_en = force.en;
        }
        if force.vo_vi != 0 {
            lt_vo_vi = force.vo_vi as U16;
        }
        if force.be_bk != 0 {
            lt_be_bk = force.be_bk as U16;
        }
    }

    let lt_en = (lt_en_ori & 0xF0) | (lt_en & 0x0F);
    let lt = ((lt_be_bk as U32) << 16) | (lt_vo_vi as U32);

    if lt_en != lt_en_ori {
        unsafe { rtw_rust_hal_com_tx_aclt_write8(adapter, REG_LIFETIME_EN, lt_en) };
    }
    if lt != lt_ori {
        unsafe { rtw_rust_hal_com_tx_aclt_write32(adapter, REG_PKT_LIFE_TIME, lt) };
    }
}