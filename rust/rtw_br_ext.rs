// SPDX-License-Identifier: GPL-2.0
#![allow(dead_code, non_snake_case, non_upper_case_globals)]
#[cfg(host_br_ext_test)]
use std::os::raw::{c_int, c_ulong};

const NAT25_IPV4: u8 = 1;
const NAT25_IPV6: u8 = 2;
const NAT25_PPPOE: u8 = 5;
const NAT25_HASH_SIZE: usize = 16;
const MAX_NETWORK_ADDR_LEN: usize = 17;
const NAT25_AGEING_TIME: c_ulong = 300;
const ETH_ALEN: usize = 6;
const ETH_HLEN: usize = 14;
const HZ: c_ulong = 100;
const NDISC_ROUTER_SOLICITATION: u8 = 133;

#[repr(C)]
pub struct Nat25NetworkDbEntry {
    pub ageing_timer: c_ulong,
}
#[repr(C)]
pub struct HostSkBuff {
    pub data: *mut u8,
    pub len: c_int,
}
#[repr(C)]
pub struct Icmp6Hdr {
    pub icmp6_type: u8,
    pub icmp6_code: u8,
    pub icmp6_cksum: u16,
}
#[repr(C)]
pub struct Ipv6Hdr {
    pub priority_version: u8,
    pub flow_lbl: [u8; 3],
    pub payload_len: u16,
    pub nexthdr: u8,
    pub hop_limit: u8,
    pub saddr: [u32; 4],
    pub daddr: [u32; 4],
}

#[no_mangle]
pub static mut host_br_ext_jiffies_val: c_ulong = 0;

fn network_hash(na: &[u8; MAX_NETWORK_ADDR_LEN]) -> c_int {
    let x = if na[0] == NAT25_IPV4 {
        na[7] as c_ulong ^ na[8] as c_ulong ^ na[9] as c_ulong ^ na[10] as c_ulong
    } else if na[0] == NAT25_PPPOE {
        na[0..9].iter().fold(0u8, |a, b| a ^ b) as c_ulong
    } else if na[0] == NAT25_IPV6 {
        na[1..=16].iter().fold(0u8, |a, b| a ^ b) as c_ulong
    } else {
        na.iter().fold(0u8, |a, b| a ^ b) as c_ulong
    };
    (x & (NAT25_HASH_SIZE as c_ulong - 1)) as c_int
}

#[no_mangle]
pub extern "C" fn host_nat25_gen_ipv4(network_addr: *mut u8, ip: u32) {
    if network_addr.is_null() {
        return;
    }
    unsafe {
        let na = std::slice::from_raw_parts_mut(network_addr, MAX_NETWORK_ADDR_LEN);
        na.fill(0);
        na[0] = NAT25_IPV4;
        std::ptr::copy_nonoverlapping(&ip as *const u32 as *const u8, na.as_mut_ptr().add(7), 4);
    }
}

#[no_mangle]
pub extern "C" fn host_nat25_gen_pppoe(network_addr: *mut u8, ac_mac: *mut u8, sid: u16) {
    if network_addr.is_null() || ac_mac.is_null() {
        return;
    }
    unsafe {
        let na = std::slice::from_raw_parts_mut(network_addr, MAX_NETWORK_ADDR_LEN);
        na.fill(0);
        na[0] = NAT25_PPPOE;
        std::ptr::copy_nonoverlapping(&sid as *const u16 as *const u8, na.as_mut_ptr().add(1), 2);
        std::ptr::copy_nonoverlapping(ac_mac, na.as_mut_ptr().add(3), 6);
    }
}

#[no_mangle]
pub extern "C" fn host_nat25_gen_ipv6(network_addr: *mut u8, ip16: *const u8) {
    if network_addr.is_null() || ip16.is_null() {
        return;
    }
    unsafe {
        let na = std::slice::from_raw_parts_mut(network_addr, MAX_NETWORK_ADDR_LEN);
        na.fill(0);
        na[0] = NAT25_IPV6;
        std::ptr::copy_nonoverlapping(ip16, na.as_mut_ptr().add(1), 16);
    }
}

#[no_mangle]
pub extern "C" fn host_nat25_network_hash(network_addr: *mut u8) -> c_int {
    if network_addr.is_null() {
        return 0;
    }
    unsafe {
        let mut na = [0u8; MAX_NETWORK_ADDR_LEN];
        std::ptr::copy_nonoverlapping(network_addr, na.as_mut_ptr(), MAX_NETWORK_ADDR_LEN);
        network_hash(&na)
    }
}

#[no_mangle]
pub extern "C" fn host_nat25_timeout(_priv: *mut c_int) -> c_ulong {
    let _ = _priv;
    unsafe { host_br_ext_jiffies_val - NAT25_AGEING_TIME * HZ }
}

#[no_mangle]
pub extern "C" fn host_nat25_has_expired(
    _priv: *mut c_int,
    fdb: *mut Nat25NetworkDbEntry,
) -> c_int {
    if fdb.is_null() {
        return 0;
    }
    unsafe { c_int::from((*fdb).ageing_timer <= host_nat25_timeout(_priv)) }
}

#[no_mangle]
pub extern "C" fn host_scan_tlv(data: *mut u8, len: c_int, tag: u8, len8b: u8) -> *mut u8 {
    if data.is_null() || len <= 0 {
        return std::ptr::null_mut();
    }
    unsafe {
        let mut cur = data;
        let mut rem = len as isize;
        while rem > 0 {
            if *cur == tag && *cur.add(1) == len8b && rem >= (len8b as isize) * 8 {
                return cur.add(2);
            }
            let step = (*cur.add(1) as isize) * 8;
            rem -= step;
            cur = cur.add(step as usize);
        }
    }
    std::ptr::null_mut()
}

#[no_mangle]
pub extern "C" fn host_update_nd_link_layer_addr(
    data: *mut u8,
    len: c_int,
    replace_mac: *mut u8,
) -> c_int {
    if data.is_null() || replace_mac.is_null() || len < 8 {
        return 0;
    }
    unsafe {
        if (*(data as *const Icmp6Hdr)).icmp6_type != NDISC_ROUTER_SOLICITATION {
            return 0;
        }
        let mac = host_scan_tlv(data.add(8), len - 8, 1, 1);
        if mac.is_null() {
            return 0;
        }
        std::ptr::copy_nonoverlapping(replace_mac, mac, 6);
        1
    }
}

#[no_mangle]
pub extern "C" fn host_convert_ipv6_mac_to_mc(skb: *mut HostSkBuff) {
    if skb.is_null() {
        return;
    }
    unsafe {
        let skb = &mut *skb;
        if skb.data.is_null() {
            return;
        }
        let iph = &*(skb.data.add(ETH_HLEN) as *const Ipv6Hdr);
        *skb.data = 0x33;
        *skb.data.add(1) = 0x33;
        std::ptr::copy_nonoverlapping(iph.daddr[3].to_ne_bytes().as_ptr(), skb.data.add(2), 4);
    }
}

#[no_mangle]
pub extern "C" fn host_skb_pull_and_merge(skb: *mut HostSkBuff, src: *mut u8, len: c_int) -> c_int {
    if skb.is_null() || src.is_null() || len <= 0 {
        return -1;
    }
    unsafe {
        let skb = &mut *skb;
        if skb.data.is_null() {
            return -1;
        }
        let tail = skb.data.add(skb.len as usize);
        if src.add(len as usize) > tail || skb.len < len {
            return -1;
        }
        let end = src.add(len as usize);
        if tail < end {
            return -1;
        }
        let tail_len = tail as usize - end as usize;
        if tail_len > 0 {
            std::ptr::copy(src.add(len as usize), src, tail_len);
        }
        skb.len -= len;
        0
    }
}

#[repr(C)]
pub struct HostNat25DbEntry {
    pub next_hash: *mut HostNat25DbEntry,
    pub pprev_hash: *mut *mut HostNat25DbEntry,
    pub use_count: c_int,
    pub mac_addr: [u8; ETH_ALEN],
    pub _pad_before_ageing: [u8; 6],
    pub ageing_timer: c_ulong,
    pub network_addr: [u8; MAX_NETWORK_ADDR_LEN],
}

#[repr(C)]
pub struct HostNat25DbAdapter {
    pub nethash: [*mut HostNat25DbEntry; NAT25_HASH_SIZE],
    pub scdb_entry: *mut HostNat25DbEntry,
    pub scdb_mac: [u8; ETH_ALEN],
    pub scdb_ip: [u8; 4],
}

#[cfg(host_br_ext_test)]
fn db_has_expired(fdb: *mut HostNat25DbEntry) -> bool {
    if fdb.is_null() {
        return false;
    }
    unsafe { (*fdb).ageing_timer <= host_br_ext_jiffies_val - NAT25_AGEING_TIME * HZ }
}

#[cfg(host_br_ext_test)]
unsafe fn hash_link(priv_: *mut HostNat25DbAdapter, ent: *mut HostNat25DbEntry, hash: c_int) {
    let priv_ = &mut *priv_;
    let h = hash as usize;
    (*ent).next_hash = priv_.nethash[h];
    if !(*ent).next_hash.is_null() {
        (*(*ent).next_hash).pprev_hash = &mut (*ent).next_hash;
    }
    priv_.nethash[h] = ent;
    (*ent).pprev_hash = &mut priv_.nethash[h];
}

#[cfg(host_br_ext_test)]
unsafe fn hash_unlink(ent: *mut HostNat25DbEntry) {
    *(*ent).pprev_hash = (*ent).next_hash;
    if !(*ent).next_hash.is_null() {
        (*(*ent).next_hash).pprev_hash = (*ent).pprev_hash;
    }
    (*ent).next_hash = std::ptr::null_mut();
    (*ent).pprev_hash = std::ptr::null_mut();
}

#[no_mangle]
pub extern "C" fn host_nat25_db_network_insert(
    priv_: *mut HostNat25DbAdapter,
    mac_addr: *mut u8,
    network_addr: *mut u8,
) {
    if priv_.is_null() || mac_addr.is_null() || network_addr.is_null() {
        return;
    }
    unsafe {
        let hash = host_nat25_network_hash(network_addr);
        let mut db = (*priv_).nethash[hash as usize];
        while !db.is_null() {
            if std::slice::from_raw_parts((*db).network_addr.as_ptr(), MAX_NETWORK_ADDR_LEN)
                == std::slice::from_raw_parts(network_addr, MAX_NETWORK_ADDR_LEN)
            {
                std::ptr::copy_nonoverlapping(mac_addr, (*db).mac_addr.as_mut_ptr(), ETH_ALEN);
                (*db).ageing_timer = host_br_ext_jiffies_val;
                return;
            }
            db = (*db).next_hash;
        }
        let db = Box::into_raw(Box::new(HostNat25DbEntry {
            next_hash: std::ptr::null_mut(),
            pprev_hash: std::ptr::null_mut(),
            use_count: 1,
            mac_addr: [0; ETH_ALEN],
            _pad_before_ageing: [0; 6],
            ageing_timer: host_br_ext_jiffies_val,
            network_addr: [0; MAX_NETWORK_ADDR_LEN],
        }));
        std::ptr::copy_nonoverlapping(
            network_addr,
            (*db).network_addr.as_mut_ptr(),
            MAX_NETWORK_ADDR_LEN,
        );
        std::ptr::copy_nonoverlapping(mac_addr, (*db).mac_addr.as_mut_ptr(), ETH_ALEN);
        hash_link(priv_, db, hash);
    }
}

#[no_mangle]
pub extern "C" fn host_nat25_db_network_lookup_and_replace(
    priv_: *mut HostNat25DbAdapter,
    skb: *mut HostSkBuff,
    network_addr: *mut u8,
) -> c_int {
    if priv_.is_null() || skb.is_null() || network_addr.is_null() {
        return 0;
    }
    unsafe {
        let hash = host_nat25_network_hash(network_addr);
        let mut db = (*priv_).nethash[hash as usize];
        while !db.is_null() {
            if std::slice::from_raw_parts((*db).network_addr.as_ptr(), MAX_NETWORK_ADDR_LEN)
                == std::slice::from_raw_parts(network_addr, MAX_NETWORK_ADDR_LEN)
            {
                if !db_has_expired(db) {
                    std::ptr::copy_nonoverlapping((*db).mac_addr.as_ptr(), (*skb).data, ETH_ALEN);
                    (*db).use_count += 1;
                }
                return 1;
            }
            db = (*db).next_hash;
        }
    }
    0
}

#[no_mangle]
pub extern "C" fn host_nat25_db_cleanup(priv_: *mut HostNat25DbAdapter) {
    if priv_.is_null() {
        return;
    }
    unsafe {
        let priv_ = &mut *priv_;
        for i in 0..NAT25_HASH_SIZE {
            let mut f = priv_.nethash[i];
            while !f.is_null() {
                let g = (*f).next_hash;
                if priv_.scdb_entry == f {
                    priv_.scdb_mac.fill(0);
                    priv_.scdb_ip.fill(0);
                    priv_.scdb_entry = std::ptr::null_mut();
                }
                hash_unlink(f);
                let _ = Box::from_raw(f);
                f = g;
            }
            priv_.nethash[i] = std::ptr::null_mut();
        }
    }
}

#[no_mangle]
pub extern "C" fn host_nat25_db_expire(priv_: *mut HostNat25DbAdapter) {
    if priv_.is_null() {
        return;
    }
    unsafe {
        let priv_ = &mut *priv_;
        for i in 0..NAT25_HASH_SIZE {
            let mut f = priv_.nethash[i];
            while !f.is_null() {
                let g = (*f).next_hash;
                if db_has_expired(f) {
                    (*f).use_count -= 1;
                    if (*f).use_count == 0 {
                        if priv_.scdb_entry == f {
                            priv_.scdb_mac.fill(0);
                            priv_.scdb_ip.fill(0);
                            priv_.scdb_entry = std::ptr::null_mut();
                        }
                        hash_unlink(f);
                        let _ = Box::from_raw(f);
                    }
                }
                f = g;
            }
        }
    }
}

#[no_mangle]
pub extern "C" fn host_nat25_db_count(priv_: *mut HostNat25DbAdapter) -> c_int {
    if priv_.is_null() {
        return 0;
    }
    unsafe {
        let mut n = 0;
        for i in 0..NAT25_HASH_SIZE {
            let mut db = (*priv_).nethash[i];
            while !db.is_null() {
                n += 1;
                db = (*db).next_hash;
            }
        }
        n
    }
}
