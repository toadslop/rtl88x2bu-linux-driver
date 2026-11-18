// W4-110 — `rtw_phydm_cfg_phy_para` from hal/hal_dm.c.

use core::ffi::c_void;

type DmStruct = u8;
type U8 = u8;
type U16 = u16;
type U32 = u32;

const HAL_STATUS_SUCCESS: i32 = 0;
const HAL_STATUS_FAILURE: i32 = 1;

const PHYDM_HALMAC_CMD_MAC_W8: U32 = 0;
const PHYDM_HALMAC_CMD_MAC_W16: U32 = 1;
const PHYDM_HALMAC_CMD_MAC_W32: U32 = 2;
const PHYDM_HALMAC_CMD_BB_W8: U32 = 3;
const PHYDM_HALMAC_CMD_BB_W16: U32 = 4;
const PHYDM_HALMAC_CMD_BB_W32: U32 = 5;
const PHYDM_HALMAC_CMD_RF_W: U32 = 6;
const PHYDM_HALMAC_CMD_DELAY_US: U32 = 7;
const PHYDM_HALMAC_CMD_DELAY_MS: U32 = 8;
const PHYDM_HALMAC_CMD_END: U32 = 0xFF;

const RF_PATH_A: U32 = 0;
const RF_PATH_B: U32 = 1;
const RF_PATH_C: U32 = 2;
const RF_PATH_D: U32 = 3;

#[repr(C)]
#[derive(Copy, Clone)]
struct MacReg {
    value: U32,
    msk: U32,
    offset: U16,
    msk_en: U8,
    size: U8,
}

#[repr(C)]
#[derive(Copy, Clone)]
struct BbReg {
    value: U32,
    msk: U32,
    offset: U16,
    msk_en: U8,
    size: U8,
}

#[repr(C)]
#[derive(Copy, Clone)]
struct RfReg {
    value: U32,
    msk: U32,
    offset: U8,
    msk_en: U8,
    path: U8,
}

#[repr(C)]
#[derive(Copy, Clone)]
struct DelayReg {
    unit: U8,
    value: U16,
}

#[repr(C)]
union RtwPhyParaData {
    mac: MacReg,
    bb: BbReg,
    rf: RfReg,
    delay: DelayReg,
}

#[repr(C)]
struct RtwPhyParameter {
    cmd: U8,
    data: RtwPhyParaData,
}

unsafe extern "C" {
    fn rtw_rust_hal_dm_phydm_adapter(dm: *mut DmStruct) -> *mut c_void;
    fn rtw_rust_hal_dm_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    // C takes `struct dvobj_priv *`; host/kernel both pass the adapter dvobj as `c_void`.
    #[cfg(config_rtw_halmac)]
    fn rtw_halmac_cfg_phy_para(d: *mut c_void, para: *mut RtwPhyParameter) -> i32;
}

fn rf_path_to_index(e_rf_path: U32) -> U8 {
    match e_rf_path {
        RF_PATH_B => 1,
        RF_PATH_C => 2,
        RF_PATH_D => 3,
        _ => 0,
    }
}

fn fill_mac(para: &mut RtwPhyParameter, offset: U32, data: U32, mask: U32, size: U8) {
    let msk_en = if mask != 0 { 1u8 } else { 0u8 };
    para.cmd = 0;
    para.data.mac = MacReg {
        offset: offset as U16,
        value: data,
        msk: mask,
        msk_en,
        size,
    };
}

fn fill_bb(para: &mut RtwPhyParameter, offset: U32, data: U32, mask: U32, size: U8) {
    let msk_en = if mask != 0 { 1u8 } else { 0u8 };
    para.cmd = 1;
    para.data.bb = BbReg {
        offset: offset as U16,
        value: data,
        msk: mask,
        msk_en,
        size,
    };
}

#[no_mangle]
pub extern "C" fn rtw_phydm_cfg_phy_para(
    p_dm_odm: *mut DmStruct,
    config_type: U32,
    offset: U32,
    data: U32,
    mask: U32,
    e_rf_path: U32,
    delay_time: U32,
) -> i32 {
    #[cfg(config_rtw_halmac)]
    {
        let mut para = RtwPhyParameter {
            cmd: 0,
            data: RtwPhyParaData { mac: MacReg {
                value: 0,
                msk: 0,
                offset: 0,
                msk_en: 0,
                size: 0,
            } },
        };

        match config_type {
            PHYDM_HALMAC_CMD_MAC_W8 => fill_mac(&mut para, offset, data, mask, 1),
            PHYDM_HALMAC_CMD_MAC_W16 => fill_mac(&mut para, offset, data, mask, 2),
            PHYDM_HALMAC_CMD_MAC_W32 => fill_mac(&mut para, offset, data, mask, 4),
            PHYDM_HALMAC_CMD_BB_W8 => fill_bb(&mut para, offset, data, mask, 1),
            PHYDM_HALMAC_CMD_BB_W16 => fill_bb(&mut para, offset, data, mask, 2),
            PHYDM_HALMAC_CMD_BB_W32 => fill_bb(&mut para, offset, data, mask, 4),
            PHYDM_HALMAC_CMD_RF_W => {
                let msk_en = if mask != 0 { 1u8 } else { 0u8 };
                para.cmd = 2;
                para.data.rf = RfReg {
                    offset: offset as U8,
                    value: data,
                    msk: mask,
                    msk_en,
                    path: rf_path_to_index(e_rf_path),
                };
            }
            PHYDM_HALMAC_CMD_DELAY_US => {
                para.cmd = 3;
                para.data.delay = DelayReg {
                    unit: 0,
                    value: delay_time as U16,
                };
            }
            PHYDM_HALMAC_CMD_DELAY_MS => {
                para.cmd = 3;
                para.data.delay = DelayReg {
                    unit: 1,
                    value: delay_time as U16,
                };
            }
            PHYDM_HALMAC_CMD_END => {
                para.cmd = 0xFF;
            }
            _ => return HAL_STATUS_FAILURE,
        }

        let adapter = unsafe { rtw_rust_hal_dm_phydm_adapter(p_dm_odm) };
        let dvobj = unsafe { rtw_rust_hal_dm_adapter_to_dvobj(adapter) };
        if unsafe { rtw_halmac_cfg_phy_para(dvobj, (&mut para) as *mut RtwPhyParameter) } != 0 {
            return HAL_STATUS_FAILURE;
        }
    }
    HAL_STATUS_SUCCESS
}