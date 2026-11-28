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
struct AoacLocFields {
    remote: U8,
    arp: U8,
    nbr: U8,
    gtk_rsp: U8,
    gtk_info: U8,
    probe: U8,
    netlist: U8,
    gtk_ext: U8,
    ndp: U8,
    aoac: U8,
    pno: U8,
}

#[cfg(host_hal_com_fw_aoac_rsvd_test)]
#[repr(C)]
pub struct MlmePriv {
    pub fw_state: S32,
}

#[cfg(host_hal_com_fw_aoac_rsvd_test)]
#[repr(C)]
pub struct PwrctrlPriv {
    pub wowlan_in_resume: U8,
    pub wowlan_aoac_rpt_loc: U8,
    _pad: [U8; 2],
}

#[cfg(host_hal_com_fw_aoac_rsvd_test)]
#[repr(C)]
struct RsvdpageLoc {
    LocProbeRsp: U8,
    LocPsPoll: U8,
    LocNullData: U8,
    LocQosNull: U8,
    LocBTQosNull: U8,
    LocRemoteCtrlInfo: U8,
    LocArpRsp: U8,
    LocNbrAdv: U8,
    LocGTKRsp: U8,
    LocGTKInfo: U8,
    LocProbeReq: U8,
    LocNetList: U8,
    LocGTKEXTMEM: U8,
    LocNDPInfo: U8,
    LocAOACReport: U8,
    LocPNOInfo: U8,
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
    fn rtw_hal_fill_h2c_cmd(
        adapter: *mut c_void,
        element_id: U8,
        cmd_len: U32,
        cmd_buf: *mut U8,
    ) -> S32;
    fn _rtw_memset(p: *mut c_void, v: i32, n: usize) -> *mut c_void;
    #[cfg(host_hal_com_fw_aoac_rsvd_test)]
    fn adapter_to_pwrctl(adapter: *mut c_void) -> *mut PwrctrlPriv;
    #[cfg(host_hal_com_fw_aoac_rsvd_test)]
    fn check_fwstate(pmlmepriv: *mut MlmePriv, state: S32) -> S32;
    #[cfg(not(host_hal_com_fw_aoac_rsvd_test))]
    fn rtw_rust_hal_com_fw_aoac_unpack_loc(loc: *mut c_void, out: *mut AoacLocFields);
    #[cfg(not(host_hal_com_fw_aoac_rsvd_test))]
    fn rtw_rust_hal_com_fw_aoac_check_fwstate(adapter: *mut c_void, state: S32) -> S32;
    #[cfg(not(host_hal_com_fw_aoac_rsvd_test))]
    fn rtw_rust_hal_com_fw_aoac_wowlan_in_resume(adapter: *mut c_void) -> U8;
    #[cfg(not(host_hal_com_fw_aoac_rsvd_test))]
    fn rtw_rust_hal_com_fw_aoac_store_aoac_rpt(adapter: *mut c_void, v: U8);
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

#[cfg(host_hal_com_fw_aoac_rsvd_test)]
fn read_loc(loc: *mut c_void) -> AoacLocFields {
    let l = unsafe { &*(loc as *mut RsvdpageLoc) };
    AoacLocFields {
        remote: l.LocRemoteCtrlInfo,
        arp: l.LocArpRsp,
        nbr: l.LocNbrAdv,
        gtk_rsp: l.LocGTKRsp,
        gtk_info: l.LocGTKInfo,
        probe: l.LocProbeReq,
        netlist: l.LocNetList,
        gtk_ext: l.LocGTKEXTMEM,
        ndp: l.LocNDPInfo,
        aoac: l.LocAOACReport,
        pno: l.LocPNOInfo,
    }
}

#[cfg(not(host_hal_com_fw_aoac_rsvd_test))]
fn read_loc(loc: *mut c_void) -> AoacLocFields {
    let mut out = AoacLocFields {
        remote: 0,
        arp: 0,
        nbr: 0,
        gtk_rsp: 0,
        gtk_info: 0,
        probe: 0,
        netlist: 0,
        gtk_ext: 0,
        ndp: 0,
        aoac: 0,
        pno: 0,
    };
    unsafe { rtw_rust_hal_com_fw_aoac_unpack_loc(loc, &mut out) };
    out
}

#[cfg(any(config_wowlan, host_hal_com_fw_aoac_rsvd_test))]
fn aoac_assoc_path(adapter: *mut c_void, loc: AoacLocFields) {
    let mut parm = [0u8; H2C_AOAC_RSVDPAGE_LOC_LEN as usize];

    set_bits_to_le_1byte(&mut parm[0], 0, 8, loc.remote);
    set_bits_to_le_1byte(&mut parm[1], 0, 8, loc.arp);
    set_bits_to_le_1byte(&mut parm[2], 0, 8, loc.nbr);
    set_bits_to_le_1byte(&mut parm[6], 0, 8, loc.ndp);
    #[cfg(any(config_gtk_ol, host_hal_com_fw_aoac_rsvd_test))]
    {
        set_bits_to_le_1byte(&mut parm[3], 0, 8, loc.gtk_rsp);
        set_bits_to_le_1byte(&mut parm[4], 0, 8, loc.gtk_info);
        set_bits_to_le_1byte(&mut parm[5], 0, 8, loc.gtk_ext);
    }
    unsafe {
        let _ = rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_AOAC_RSVD_PAGE,
            H2C_AOAC_RSVDPAGE_LOC_LEN,
            parm.as_mut_ptr(),
        );
    }

    unsafe {
        _rtw_memset(
            parm.as_mut_ptr() as *mut c_void,
            0,
            H2C_AOAC_RSVDPAGE_LOC_LEN as usize,
        );
    };
    #[cfg(not(host_hal_com_fw_aoac_rsvd_test))]
    unsafe {
        rtw_rust_hal_com_log_fw_aoac_rsvd_aoac_report(loc.aoac);
    }
    set_bits_to_le_1byte(&mut parm[1], 0, 8, loc.aoac);
    unsafe {
        let _ = rtw_hal_fill_h2c_cmd(
            adapter,
            H2C_AOAC_RSVDPAGE3,
            H2C_AOAC_RSVDPAGE_LOC_LEN,
            parm.as_mut_ptr(),
        );
    }
    #[cfg(host_hal_com_fw_aoac_rsvd_test)]
    {
        let pwrpriv = unsafe { adapter_to_pwrctl(adapter) };
        unsafe { (*pwrpriv).wowlan_aoac_rpt_loc = loc.aoac };
    }
    #[cfg(not(host_hal_com_fw_aoac_rsvd_test))]
    unsafe {
        rtw_rust_hal_com_fw_aoac_store_aoac_rpt(adapter, loc.aoac);
    }
}

#[cfg(any(config_wowlan, host_hal_com_fw_aoac_rsvd_test))]
#[no_mangle]
pub extern "C" fn rtw_hal_set_FwAoacRsvdPage_cmd(
    padapter: *mut c_void,
    rsvdpageloc: *mut c_void,
) {
    let loc = read_loc(rsvdpageloc);

    #[cfg(all(not(host_hal_com_fw_aoac_rsvd_test), config_wowlan))]
    unsafe {
        rtw_rust_hal_com_log_fw_aoac_rsvd_assoc(
            loc.remote,
            loc.arp,
            loc.nbr,
            loc.ndp,
            loc.gtk_rsp,
            loc.gtk_info,
            loc.probe,
            loc.netlist,
        );
    }

    let asoc = unsafe {
        #[cfg(host_hal_com_fw_aoac_rsvd_test)]
        {
            let pmlmepriv = &mut (*(padapter as *mut AdapterHost)).mlmepriv;
            check_fwstate(pmlmepriv, WIFI_ASOC_STATE)
        }
        #[cfg(not(host_hal_com_fw_aoac_rsvd_test))]
        {
            rtw_rust_hal_com_fw_aoac_check_fwstate(padapter, WIFI_ASOC_STATE)
        }
    };

    if asoc != 0 {
        aoac_assoc_path(padapter, loc);
        return;
    }
    #[cfg(any(config_pno_support, host_hal_com_fw_aoac_rsvd_test))]
    {
        let in_resume = unsafe {
            #[cfg(host_hal_com_fw_aoac_rsvd_test)]
            {
                (*adapter_to_pwrctl(padapter)).wowlan_in_resume
            }
            #[cfg(not(host_hal_com_fw_aoac_rsvd_test))]
            {
                rtw_rust_hal_com_fw_aoac_wowlan_in_resume(padapter)
            }
        };
        if in_resume == 0 {
            let mut parm = [0u8; H2C_AOAC_RSVDPAGE_LOC_LEN as usize];
            #[cfg(not(host_hal_com_fw_aoac_rsvd_test))]
            unsafe {
                rtw_rust_hal_com_log_fw_aoac_rsvd_nlo(loc.pno);
            }
            set_bits_to_le_1byte(&mut parm[0], 0, 8, loc.pno);
            unsafe {
                let _ = rtw_hal_fill_h2c_cmd(
                    padapter,
                    H2C_AOAC_RSVDPAGE3,
                    H2C_AOAC_RSVDPAGE_LOC_LEN,
                    parm.as_mut_ptr(),
                );
            }
        }
    }
}

#[cfg(host_hal_com_fw_aoac_rsvd_test)]
#[repr(C)]
struct AdapterHost {
    mlmepriv: MlmePriv,
    pwrctl: PwrctrlPriv,
}

#[cfg(not(any(config_wowlan, host_hal_com_fw_aoac_rsvd_test)))]
#[no_mangle]
pub extern "C" fn rtw_hal_set_FwAoacRsvdPage_cmd(
    _padapter: *mut c_void,
    _rsvdpageloc: *mut c_void,
) {
}