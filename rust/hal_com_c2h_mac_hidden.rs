// SPDX-License-Identifier: GPL-2.0
// C2H mac-hidden report path (W4-57).
use core::ffi::c_void;

type U8 = u8;
type U32 = u32;
type S32 = i32;

const MAC_HIDDEN_RPT_LEN: usize = 8;
const MAC_HIDDEN_RPT_2_LEN: usize = 5;
const REG_C2HEVT_MSG_NORMAL: U32 = 0x01a0;
const C2H_DEFEATURE_RSVD: U8 = 0xfd;
const C2H_MAC_HIDDEN_RPT: U8 = 0x19;
const C2H_DBG: U8 = 0x00;
const RTW_USB: U8 = 1 << 1;
const RTW_PCIE: U8 = 1 << 0;
const SUCCESS: S32 = 1;
const FAIL: S32 = 0;

extern "C" {
    fn rtw_rust_hal_com_warn_c2h_short_len(func: *const U8, len: U8, expect: i32);
    fn rtw_rust_hal_com_mac_hidden_rpt1_apply(
        adapter: *mut c_void,
        data: *mut U8,
        package_type: U8,
        hci_type: U8,
        wl_func: U8,
        hw_stype: U8,
        bw: U8,
        ant_num: U8,
        protocol: U8,
    ) -> S32;
    fn rtw_rust_hal_com_mac_hidden_rpt2_apply(adapter: *mut c_void, data: *mut U8) -> S32;
    fn rtw_rust_hal_com_read8(adapter: *mut c_void, addr: U32) -> U8;
    fn rtw_rust_hal_com_write8(adapter: *mut c_void, addr: U32, val: U8);
    fn rtw_rust_hal_com_set_not_xmitframe_fw_dl(adapter: *mut c_void, val: U8);
    fn rtw_rust_hal_com_rtw_hal_fw_dl(adapter: *mut c_void, notify: U8) -> S32;
    fn rtw_rust_hal_com_get_intf_type(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_is_hw_init_completed(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_hal_power_on(adapter: *mut c_void);
    fn rtw_rust_hal_com_hal_power_off(adapter: *mut c_void);
    fn rtw_rust_hal_com_cannot_io(adapter: *mut c_void) -> U8;
    fn rtw_rust_hal_com_msleep_os(ms: i32);
    fn rtw_rust_hal_com_get_current_time() -> u64;
    fn rtw_rust_hal_com_get_passing_time_ms(start: u64) -> U32;
    fn rtw_rust_hal_com_log_mac_hidden_read(ok: U8, cnt: U32, ms: i32, fwdl: S32, id: U8);
}

static FN_C2H_MAC_HIDDEN: &[U8] = b"c2h_mac_hidden_rpt_hdl\0";
static FN_C2H_MAC_HIDDEN_2: &[U8] = b"c2h_mac_hidden_rpt_2_hdl\0";
#[inline]
fn le_bits_to_1byte(ptr: *const U8, bit_offset: u32, bit_len: u32) -> U8 {
    unsafe { (*ptr >> bit_offset) as U8 & ((1u32 << bit_len) - 1) as U8 }
}

#[no_mangle]
pub extern "C" fn c2h_mac_hidden_rpt_hdl(adapter: *mut c_void, data: *mut U8, len: U8) -> S32 {
    if (len as usize) < MAC_HIDDEN_RPT_LEN {
        unsafe {
            rtw_rust_hal_com_warn_c2h_short_len(
                FN_C2H_MAC_HIDDEN.as_ptr(),
                len,
                MAC_HIDDEN_RPT_LEN as i32,
            );
        }
        return FAIL;
    }
    let hci_type = le_bits_to_1byte(unsafe { data.add(4) }, 0, 4);
    let package_type = le_bits_to_1byte(unsafe { data.add(4) }, 4, 3);
    let wl_func = le_bits_to_1byte(unsafe { data.add(5) }, 0, 4);
    let hw_stype = le_bits_to_1byte(unsafe { data.add(5) }, 4, 4);
    let bw = le_bits_to_1byte(unsafe { data.add(6) }, 0, 3);
    let ant_num = le_bits_to_1byte(unsafe { data.add(6) }, 5, 3);
    let protocol = le_bits_to_1byte(unsafe { data.add(7) }, 2, 2);
    unsafe {
        rtw_rust_hal_com_mac_hidden_rpt1_apply(
            adapter,
            data,
            package_type,
            hci_type,
            wl_func,
            hw_stype,
            bw,
            ant_num,
            protocol,
        )
    }
}

#[no_mangle]
pub extern "C" fn c2h_mac_hidden_rpt_2_hdl(adapter: *mut c_void, data: *mut U8, len: U8) -> S32 {
    if (len as usize) < MAC_HIDDEN_RPT_2_LEN {
        unsafe {
            rtw_rust_hal_com_warn_c2h_short_len(
                FN_C2H_MAC_HIDDEN_2.as_ptr(),
                len,
                MAC_HIDDEN_RPT_2_LEN as i32,
            );
        }
        return FAIL;
    }
    unsafe { rtw_rust_hal_com_mac_hidden_rpt2_apply(adapter, data) }
}

#[no_mangle]
pub extern "C" fn hal_read_mac_hidden_rpt(adapter: *mut c_void) -> S32 {
    let mut ret = FAIL;
    let mut mac_hidden_rpt = [0u8; MAC_HIDDEN_RPT_LEN + MAC_HIDDEN_RPT_2_LEN];
    let mut start = unsafe { rtw_rust_hal_com_get_current_time() };
    let mut cnt: U32 = 0;
    let timeout_ms: U32 = 800;
    let min_cnt: U32 = 10;
    let mut id = C2H_DEFEATURE_RSVD;

    let hci_type = unsafe { rtw_rust_hal_com_get_intf_type(adapter) };
    if (hci_type == RTW_USB || hci_type == RTW_PCIE)
        && unsafe { rtw_rust_hal_com_is_hw_init_completed(adapter) } == 0
    {
        unsafe { rtw_rust_hal_com_hal_power_on(adapter) };
    }
    unsafe { rtw_rust_hal_com_write8(adapter, REG_C2HEVT_MSG_NORMAL, C2H_DEFEATURE_RSVD) };

    unsafe { rtw_rust_hal_com_set_not_xmitframe_fw_dl(adapter, 1) };
    let ret_fwdl = unsafe { rtw_rust_hal_com_rtw_hal_fw_dl(adapter, 0) };
    unsafe { rtw_rust_hal_com_set_not_xmitframe_fw_dl(adapter, 0) };

    if ret_fwdl == SUCCESS {
        start = unsafe { rtw_rust_hal_com_get_current_time() };
        loop {
            cnt += 1;
            id = unsafe { rtw_rust_hal_com_read8(adapter, REG_C2HEVT_MSG_NORMAL) };
            if id == C2H_MAC_HIDDEN_RPT || unsafe { rtw_rust_hal_com_cannot_io(adapter) } != 0 {
                break;
            }
            unsafe { rtw_rust_hal_com_msleep_os(10) };
            let elapsed = unsafe { rtw_rust_hal_com_get_passing_time_ms(start) };
            if elapsed >= timeout_ms && cnt >= min_cnt {
                break;
            }
        }

        if id == C2H_MAC_HIDDEN_RPT {
            for i in 0..mac_hidden_rpt.len() {
                mac_hidden_rpt[i] = unsafe {
                    rtw_rust_hal_com_read8(adapter, REG_C2HEVT_MSG_NORMAL + 2 + i as U32)
                };
            }
        }

        unsafe { rtw_rust_hal_com_write8(adapter, REG_C2HEVT_MSG_NORMAL, C2H_DBG) };

        if id == C2H_MAC_HIDDEN_RPT {
            ret = SUCCESS;
        }
    }

    let _ = c2h_mac_hidden_rpt_hdl(adapter, mac_hidden_rpt.as_mut_ptr(), MAC_HIDDEN_RPT_LEN as U8);
    let _ = c2h_mac_hidden_rpt_2_hdl(
        adapter,
        unsafe { mac_hidden_rpt.as_mut_ptr().add(MAC_HIDDEN_RPT_LEN) },
        MAC_HIDDEN_RPT_2_LEN as U8,
    );

    if (hci_type == RTW_USB || hci_type == RTW_PCIE)
        && unsafe { rtw_rust_hal_com_is_hw_init_completed(adapter) } == 0
    {
        unsafe { rtw_rust_hal_com_hal_power_off(adapter) };
    }

    let ms = unsafe { rtw_rust_hal_com_get_passing_time_ms(start) } as i32;
    unsafe {
        rtw_rust_hal_com_log_mac_hidden_read(
            if ret == SUCCESS { 1 } else { 0 },
            cnt,
            ms,
            ret_fwdl,
            id,
        );
    };
    ret
}
