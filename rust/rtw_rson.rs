// SPDX-License-Identifier: GPL-2.0
#![allow(dead_code, non_snake_case, non_upper_case_globals)]

#[cfg(host_rson_test)]
use std::os::raw::{c_int, c_long, c_uint};

const _TRUE: c_int = 1;
const _FALSE: c_int = 0;
const ETH_ALEN: usize = 6;
const MAX_IE_SZ: usize = 768;
const RTW_RSON_VER: u8 = 1;
const RTW_RSON_SCORE_NOTCNNT: u8 = 0x1;
const RTW_RSON_SCORE_MAX: u8 = 0xFF;
const RTW_RSON_HC_NOTREADY: u8 = 0xFF;
const RTW_RSON_HC_ROOT: u8 = 0x0;
const RTW_RSON_ALLOWCONNECT: u8 = 0x1;
const RTW_RSON_DENYCONNECT: u8 = 0x0;
const CONFIG_RTW_REPEATER_SON_ID: u32 = 0x02040608;
const BEACON_IE_OFFSET: i32 = 12;
const VENDOR_SPECIFIC_IE: u8 = 221;
static RTW_RSON_OUI: [u8; 3] = [0xFA, 0xFA, 0xFA];

#[repr(C, packed)]
pub struct RtwRsonStruct {
    pub ver: u8,
    pub id: u32,
    pub hopcnt: u8,
    pub connectible: u8,
    pub loading: u8,
    pub res: [u8; 16],
}

#[repr(C)]
pub struct DvobjPriv {
    pub rson_data: RtwRsonStruct,
}

#[repr(C)]
pub struct WlanBssidEx {
    pub length: u32,
    pub mac_address: [u8; 6],
    pub reserved: [u8; 2],
    pub privacy: u32,
    pub rssi: c_long,
    pub ie_length: u32,
    pub ies: [u8; MAX_IE_SZ],
}

#[repr(C)]
pub struct WlanNetwork {
    pub network: WlanBssidEx,
}

#[repr(C)]
pub struct Adapter {
    pub dvobj: DvobjPriv,
}

static mut BLOCK_IDX: u8 = 0;
static mut BLOCK_BSSID: [[u8; 6]; 10] = [[0; 6]; 10];
static mut ROOT_IDX: u8 = 0;
static mut ROOT_BSSID: [[u8; 6]; 10] = [[0; 6]; 10];

fn memcmp6(a: *const u8, b: &[u8; 6]) -> bool {
    unsafe { std::slice::from_raw_parts(a, 6) == b.as_slice() }
}

#[no_mangle]
pub extern "C" fn key_2char2num(hch: u8, lch: u8) -> u8 {
    fn nib(c: u8) -> u8 {
        if c >= b'a' {
            c - b'a' + 10
        } else {
            c - b'0'
        }
    }
    (nib(hch) << 4) | nib(lch)
}

#[no_mangle]
pub extern "C" fn rtw_get_ie(
    pbuf: *const u8,
    index: c_int,
    len: *mut c_int,
    limit: c_int,
) -> *mut u8 {
    if limit < 1 || pbuf.is_null() || len.is_null() {
        return std::ptr::null_mut();
    }
    unsafe {
        let mut p = pbuf;
        let mut i = 0;
        *len = 0;
        loop {
            if *p == index as u8 {
                *len = *(p.add(1)) as c_int;
                return p as *mut u8;
            }
            let tmp = *(p.add(1)) as c_int;
            p = p.add((tmp + 2) as usize);
            i += tmp + 2;
            if i >= limit {
                break;
            }
        }
    }
    std::ptr::null_mut()
}

#[no_mangle]
pub extern "C" fn rtw_cal_rson_score(cand: *mut RtwRsonStruct, rssi: c_long) -> u8 {
    if cand.is_null() {
        return RTW_RSON_SCORE_NOTCNNT;
    }
    unsafe {
        let c = &*cand;
        if c.hopcnt == RTW_RSON_HC_NOTREADY || c.connectible == RTW_RSON_DENYCONNECT {
            return RTW_RSON_SCORE_NOTCNNT;
        }
        RTW_RSON_SCORE_MAX
            .wrapping_sub(c.hopcnt.wrapping_mul(10))
            .wrapping_add((rssi / 10) as u8)
    }
}

#[no_mangle]
pub extern "C" fn is_match_bssid(mac: *mut u8, bssid_array: *mut [u8; 6], num: c_int) -> c_int {
    if mac.is_null() || bssid_array.is_null() || num <= 0 {
        return _FALSE;
    }
    unsafe {
        let arr = std::slice::from_raw_parts(bssid_array, num as usize);
        for b in arr {
            if memcmp6(mac, b) {
                return _TRUE;
            }
        }
    }
    _FALSE
}

#[no_mangle]
pub extern "C" fn init_rtw_rson_data(dvobj: *mut DvobjPriv) {
    if dvobj.is_null() {
        return;
    }
    unsafe {
        let d = &mut *dvobj;
        d.rson_data.ver = RTW_RSON_VER;
        d.rson_data.id = CONFIG_RTW_REPEATER_SON_ID;
        d.rson_data.hopcnt = RTW_RSON_HC_NOTREADY;
        d.rson_data.connectible = RTW_RSON_DENYCONNECT;
        d.rson_data.loading = 0;
        d.rson_data.res = [0xAA; 16];
    }
}

#[no_mangle]
pub extern "C" fn str2hexbuf(str: *mut u8, hexbuf: *mut u8, len: c_int) -> c_int {
    if str.is_null() || hexbuf.is_null() || len <= 0 {
        return _FALSE;
    }
    unsafe {
        let s = std::ffi::CStr::from_ptr(str as *const i8);
        let bytes = s.to_bytes();
        if bytes.len() < 2 || bytes[0] != b'0' || bytes[1] != b'x' {
            return _FALSE;
        }
        if bytes.len() > (len as usize * 2) + 2 {
            return _FALSE;
        }
        let mut idx = 0usize;
        for i in 0..len as usize {
            if idx + 1 >= bytes.len() - 2 {
                break;
            }
            let off = 2 + idx;
            *hexbuf.add(i) = key_2char2num(bytes[off], bytes[off + 1]);
            idx += 2;
        }
    }
    _TRUE
}

#[no_mangle]
pub extern "C" fn rtw_rson_varify_ie(p: *mut u8) -> u8 {
    if p.is_null() {
        return _FALSE as u8;
    }
    unsafe {
        if *p.add(2 + RTW_RSON_OUI.len()) == 1 {
            _TRUE as u8
        } else {
            _FALSE as u8
        }
    }
}

#[no_mangle]
pub extern "C" fn host_rson_set_block_bssid_count(n: u8) {
    unsafe {
        BLOCK_IDX = n;
    }
}

#[no_mangle]
pub extern "C" fn host_rson_set_root_bssid_count(n: u8) {
    unsafe {
        ROOT_IDX = n;
    }
}

#[no_mangle]
pub extern "C" fn host_rson_set_block_bssid(idx: u8, mac: *const u8) {
    if mac.is_null() || idx >= 10 {
        return;
    }
    unsafe {
        std::ptr::copy_nonoverlapping(mac, BLOCK_BSSID[idx as usize].as_mut_ptr(), 6);
    }
}

#[no_mangle]
pub extern "C" fn host_rson_set_root_bssid(idx: u8, mac: *const u8) {
    if mac.is_null() || idx >= 10 {
        return;
    }
    unsafe {
        std::ptr::copy_nonoverlapping(mac, ROOT_BSSID[idx as usize].as_mut_ptr(), 6);
    }
}

#[no_mangle]
pub extern "C" fn rtw_get_rson_struct(bssid: *mut WlanBssidEx, out: *mut RtwRsonStruct) -> c_int {
    if bssid.is_null() || out.is_null() {
        return -22;
    }
    unsafe {
        let b = &mut *bssid;
        let r = &mut *out;
        r.ver = 0;
        r.id = 0;
        r.hopcnt = 0;
        r.connectible = 0;
        r.loading = 0;
        if is_match_bssid(
            b.mac_address.as_mut_ptr(),
            ROOT_BSSID.as_mut_ptr(),
            ROOT_IDX as c_int,
        ) == _TRUE
        {
            r.id = CONFIG_RTW_REPEATER_SON_ID;
            r.ver = RTW_RSON_VER;
            r.hopcnt = RTW_RSON_HC_ROOT;
            r.connectible = RTW_RSON_ALLOWCONNECT;
            return _TRUE;
        }
        let mut limit = b.ie_length as i32 - BEACON_IE_OFFSET;
        let mut p = b.ies.as_mut_ptr().add(BEACON_IE_OFFSET as usize);
        loop {
            let mut len: c_int = 0;
            p = rtw_get_ie(p, VENDOR_SPECIFIC_IE as c_int, &mut len, limit);
            limit -= len;
            if p.is_null() || len == 0 {
                break;
            }
            if std::slice::from_raw_parts(p.add(2), 3) == RTW_RSON_OUI && rtw_rson_varify_ie(p) != 0
            {
                let mut q = p.add(2 + 3);
                r.ver = *q;
                q = q.add(1);
                r.id = u32::from_le_bytes([*q.add(0), *q.add(1), *q.add(2), *q.add(3)]);
                q = q.add(4);
                r.hopcnt = *q;
                q = q.add(1);
                r.connectible = *q;
                q = q.add(1);
                r.loading = *q;
                return _TRUE;
            }
        }
    }
    -74
}

#[no_mangle]
pub extern "C" fn rtw_rson_choose(
    candidate: *mut *mut WlanNetwork,
    competitor: *mut WlanNetwork,
) -> c_int {
    if candidate.is_null() || competitor.is_null() {
        return _FALSE;
    }
    unsafe {
        let comp = &mut *competitor;
        if is_match_bssid(
            comp.network.mac_address.as_mut_ptr(),
            BLOCK_BSSID.as_mut_ptr(),
            BLOCK_IDX as c_int,
        ) == _TRUE
        {
            return _FALSE;
        }
        let mut rson_comp = RtwRsonStruct {
            ver: 0,
            id: 0,
            hopcnt: 0,
            connectible: 0,
            loading: 0,
            res: [0; 16],
        };
        if rtw_get_rson_struct(&mut comp.network, &mut rson_comp) != _TRUE
            || rson_comp.id != CONFIG_RTW_REPEATER_SON_ID
        {
            return _FALSE;
        }
        let comp_score = rtw_cal_rson_score(&mut rson_comp, comp.network.rssi);
        if comp_score == RTW_RSON_SCORE_NOTCNNT {
            return _FALSE;
        }
        if (*candidate).is_null() {
            return _TRUE;
        }
        let cand = &mut **candidate;
        let mut rson_cand = RtwRsonStruct {
            ver: 0,
            id: 0,
            hopcnt: 0,
            connectible: 0,
            loading: 0,
            res: [0; 16],
        };
        if rtw_get_rson_struct(&mut cand.network, &mut rson_cand) != _TRUE {
            return _FALSE;
        }
        let cand_score = rtw_cal_rson_score(&mut rson_cand, cand.network.rssi);
        if (comp_score as i16) - (cand_score as i16) > 8 {
            return _TRUE;
        }
    }
    _FALSE
}

#[no_mangle]
pub extern "C" fn rtw_rson_append_ie(
    padapter: *mut Adapter,
    pframe: *mut u8,
    len: *mut c_uint,
) -> c_uint {
    if padapter.is_null() || pframe.is_null() || len.is_null() {
        return 0;
    }
    unsafe {
        let d = &mut (*padapter).dvobj.rson_data;
        let mut ptr = pframe;
        *ptr = VENDOR_SPECIFIC_IE;
        ptr = ptr.add(1);
        let ie_len = 3 + std::mem::size_of::<RtwRsonStruct>() as u8;
        *ptr = ie_len;
        ptr = ptr.add(1);
        std::ptr::copy_nonoverlapping(RTW_RSON_OUI.as_ptr(), ptr, 3);
        ptr = ptr.add(3);
        *ptr = d.ver;
        ptr = ptr.add(1);
        let id_le = d.id.to_le_bytes();
        std::ptr::copy_nonoverlapping(id_le.as_ptr(), ptr, 4);
        ptr = ptr.add(4);
        *ptr = d.hopcnt;
        ptr = ptr.add(1);
        *ptr = d.connectible;
        ptr = ptr.add(1);
        *ptr = d.loading;
        ptr = ptr.add(1);
        std::ptr::copy_nonoverlapping(d.res.as_ptr(), ptr, 16);
        *len += (ie_len as u32) + 2;
        ie_len as c_uint
    }
}
