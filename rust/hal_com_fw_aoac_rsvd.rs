// SPDX-License-Identifier: GPL-2.0
// WOW AOAC reserved-page H2C (W4-145); body under CONFIG_WOWLAN.

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;
type S32 = i32;

const H2C_AOAC_RSVD_PAGE: U8 = 0x83;
const H2C_AOAC_RSVDPAGE3: U8 = 0x88;
const H2C_AOAC_RSVDPAGE_LOC_LEN: U32 = 7;
const WIFI_ASOC_STATE: S32 = 0x00000001;

#[repr(C)]
pub struct MlmePriv {
    pub fw_state: S32,
}

#[repr(C)]
pub struct PwrctrlPriv {
    pub wowlan_in_resume: U8,
    pub wowlan_aoac_rpt_loc: U8,
    _pad: [U8; 2],
}

#[repr(C)]
pub struct RsvdpageLoc {
    pub LocRemoteCtrlInfo: U8,
    pub LocArpRsp: U8,
    pub LocNbrAdv: U8,
    pub LocGTKRsp: U8,
    pub LocGTKInfo: U8,
    pub LocGTKEXTMEM: U8,
    pub LocNDPInfo: U8,
    pub LocAOACReport: U8,
    pub LocPNOInfo: U8,
}

#[repr(C)]
pub struct Adapter {
    pub mlmepriv: MlmePriv,
    pub pwrctl: PwrctrlPriv,
}

#[inline]
fn set_bits_to_le_1byte(byte: &mut U8, bit_offset: U8, bit_len: U8, value: U8) {
    let mask = if bit_len >= 8 {
        0xffu8
    } else {
        (1u8 << bit_len).wrapping_sub(1)
    };
    let cleared = *byte & !(mask.wrapping_shl(bit_offset as u32));
    *byte = cleared | ((value & mask).wrapping_shl(bit_offset as u32));
}

extern "C" {
    fn adapter_to_pwrctl(adapter: *mut c_void) -> *mut PwrctrlPriv;
    fn check_fwstate(pmlmepriv: *mut MlmePriv, state: S32) -> S32;
    fn rtw_hal_fill_h2c_cmd(
        adapter: *mut c_void,
        element_id: U8,
        cmd_len: U32,
        cmd_buf: *mut U8,
    ) -> S32;
    fn _rtw_memset(p: *mut c_void, v: i32, n: usize);
    #[cfg(all(not(host_hal_com_fw_aoac_rsvd_test), config_wowlan))]
    fn rtw_rust_hal_com_log_fw_aoac_rsvd_assoc(
        rwc: U8,
        arp: U8,
        nbr: U8,
        ndp: U8,
        gtk_rsp: U8,
        gtk_info: U8,
        probe: U8,
        netlist: U8,
    );
    #[cfg(all(not(host_hal_com_fw_aoac_rsvd_test), config_wowlan))]
    fn rtw_rust_hal_com_log_fw_aoac_rsvd_aoac_report(aoac: U8);
    #[cfg(all(not(host_hal_com_fw_aoac_rsvd_test), config_wowlan))]
    fn rtw_rust_hal_com_log_fw_aoac_rsvd_nlo(nlo: U8);
}

#[cfg(any(config_wowlan, host_hal_com_fw_aoac_rsvd_test))]
fn aoac_assoc_path(adapter: *mut c_void, loc: &RsvdpageLoc, pwrpriv: *mut PwrctrlPriv) {
    let mut parm = [0u8; H2C_AOAC_RSVDPAGE_LOC_LEN as usize];

    #[cfg(not(host_hal_com_fw_aoac_rsvd_test))]
    unsafe {
        rtw_rust_hal_com_log_fw_aoac_rsvd_assoc(
            loc.LocRemoteCtrlInfo,
            loc.LocArpRsp,
            loc.LocNbrAdv,
            loc.LocNDPInfo,
            loc.LocGTKRsp,
            loc.LocGTKInfo,
            0,
            0,
        );
    }

    set_bits_to_le_1byte(&mut parm[0], 0, 8, loc.LocRemoteCtrlInfo);
    set_bits_to_le_1byte(&mut parm[1], 0, 8, loc.LocArpRsp);
    set_bits_to_le_1byte(&mut parm[2], 0, 8, loc.LocNbrAdv);
    set_bits_to_le_1byte(&mut parm[6], 0, 8, loc.LocNDPInfo);
    #[cfg(any(config_gtk_ol, host_hal_com_fw_aoac_rsvd_test))]
    {
        set_bits_to_le_1byte(&mut parm[3], 0, 8, loc.LocGTKRsp);
        set_bits_to_le_1byte(&mut parm[4], 0, 8, loc.LocGTKInfo);
        set_bits_to_le_1byte(&mut parm[5], 0, 8, loc.LocGTKEXTMEM);
    }
    unsafe {
        let _ = rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_AOAC_RSVD_PAGE,
            H2C_AOAC_RSVDPAGE_LOC_LEN,
            parm.as_mut_ptr(),
        );
    }

    unsafe { _rtw_memset(parm.as_mut_ptr() as *mut c_void, 0, parm.len()) };
    set_bits_to_le_1byte(&mut parm[1], 0, 8, loc.LocAOACReport);
    unsafe {
        let _ = rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_AOAC_RSVDPAGE3,
            H2C_AOAC_RSVDPAGE_LOC_LEN,
            parm.as_mut_ptr(),
        );
        (*pwrpriv).wowlan_aoac_rpt_loc = loc.LocAOACReport;
    }
}

#[cfg(any(config_wowlan, host_hal_com_fw_aoac_rsvd_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_set_FwAoacRsvdPage_cmd(
    padapter: *mut c_void,
    rsvdpageloc: *mut RsvdpageLoc,
) {
    let padapter = padapter as *mut Adapter;
    let loc = unsafe { &*rsvdpageloc };
    let pwrpriv = unsafe { adapter_to_pwrctl(padapter as *mut c_void) };
    let pmlmepriv = unsafe { &mut (*padapter).mlmepriv };

    if unsafe { check_fwstate(pmlmepriv, WIFI_ASOC_STATE) } != 0 {
        aoac_assoc_path(padapter as *mut c_void, loc, pwrpriv);
        return;
    }
    #[cfg(any(config_pno_support, host_hal_com_fw_aoac_rsvd_test))]
    {
        if unsafe { (*pwrpriv).wowlan_in_resume } == 0 {
            let mut parm = [0u8; H2C_AOAC_RSVDPAGE_LOC_LEN as usize];
            #[cfg(not(host_hal_com_fw_aoac_rsvd_test))]
            unsafe {
                rtw_rust_hal_com_log_fw_aoac_rsvd_nlo(loc.LocPNOInfo);
            }
            set_bits_to_le_1byte(&mut parm[0], 0, 8, loc.LocPNOInfo);
            unsafe {
                let _ = rtw_hal_fill_h2c_cmd(
                    padapter as *mut c_void,
                    H2C_AOAC_RSVDPAGE3,
                    H2C_AOAC_RSVDPAGE_LOC_LEN,
                    parm.as_mut_ptr(),
                );
            }
        }
    }
}

#[cfg(not(any(config_wowlan, host_hal_com_fw_aoac_rsvd_test)))]
#[no_mangle]
pub extern "C" fn rtw_hal_set_FwAoacRsvdPage_cmd(
    _padapter: *mut c_void,
    _rsvdpageloc: *mut c_void,
) {
}