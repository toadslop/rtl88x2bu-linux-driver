// SPDX-License-Identifier: GPL-2.0
//! SHA-256 core — Rust port of `core/crypto/sha256-internal.c` (W2-05b).
//!
//! Public ABI: `sha256_vector`. Internal state helpers mirror `sha256_i.h`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_crypto_test)]
use std::os::raw::{c_int, c_ulong};
#[cfg(not(host_crypto_test))]
use core::ffi::{c_int, c_ulong};

const SHA256_BLOCK_SIZE: usize = 64;
const SHA256_MAC_LEN: usize = 32;

#[repr(C)]
pub struct Sha256State {
    length: u64,
    state: [u32; 8],
    curlen: u32,
    buf: [u8; SHA256_BLOCK_SIZE],
}

const K: [u32; 64] = [
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4,
    0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe,
    0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f,
    0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
    0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116,
    0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7,
    0xc67178f2,
];

fn ror(x: u32, y: u32) -> u32 {
    x.rotate_right(y & 31)
}

fn ch(x: u32, y: u32, z: u32) -> u32 {
    z ^ (x & (y ^ z))
}

fn maj(x: u32, y: u32, z: u32) -> u32 {
    (x | y) & z | (x & y)
}

fn sigma0(x: u32) -> u32 {
    ror(x, 2) ^ ror(x, 13) ^ ror(x, 22)
}

fn sigma1(x: u32) -> u32 {
    ror(x, 6) ^ ror(x, 11) ^ ror(x, 25)
}

fn gamma0(x: u32) -> u32 {
    ror(x, 7) ^ ror(x, 18) ^ (x >> 3)
}

fn gamma1(x: u32) -> u32 {
    ror(x, 17) ^ ror(x, 19) ^ (x >> 10)
}

fn get_be32(buf: &[u8]) -> u32 {
    ((buf[0] as u32) << 24)
        | ((buf[1] as u32) << 16)
        | ((buf[2] as u32) << 8)
        | (buf[3] as u32)
}

fn put_be32(dst: &mut [u8], val: u32) {
    dst[0] = (val >> 24) as u8;
    dst[1] = (val >> 16) as u8;
    dst[2] = (val >> 8) as u8;
    dst[3] = val as u8;
}

fn put_be64(dst: &mut [u8], val: u64) {
    put_be32(&mut dst[0..4], (val >> 32) as u32);
    put_be32(&mut dst[4..8], val as u32);
}

fn sha256_compress(md: &mut Sha256State, buf: &[u8]) -> c_int {
    let mut s = md.state;
    let mut w = [0u32; 64];

    for i in 0..16 {
        w[i] = get_be32(&buf[4 * i..4 * i + 4]);
    }
    for i in 16..64 {
        w[i] = gamma1(w[i - 2])
            .wrapping_add(w[i - 7])
            .wrapping_add(gamma0(w[i - 15]))
            .wrapping_add(w[i - 16]);
    }

    for i in 0..64 {
        let t0 = s[7]
            .wrapping_add(sigma1(s[4]))
            .wrapping_add(ch(s[4], s[5], s[6]))
            .wrapping_add(K[i])
            .wrapping_add(w[i]);
        let t1 = sigma0(s[0]).wrapping_add(maj(s[0], s[1], s[2]));
        let d = s[3].wrapping_add(t0);
        let h = t0.wrapping_add(t1);
        s[7] = s[6];
        s[6] = s[5];
        s[5] = s[4];
        s[4] = d;
        s[3] = s[2];
        s[2] = s[1];
        s[1] = s[0];
        s[0] = h;
    }

    for i in 0..8 {
        md.state[i] = md.state[i].wrapping_add(s[i]);
    }
    0
}

fn sha256_init(md: &mut Sha256State) {
    md.curlen = 0;
    md.length = 0;
    md.state = [
        0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19,
    ];
}

fn sha256_process_inner(md: &mut Sha256State, input: &[u8]) -> c_int {
    let mut inlen = input.len();
    let mut offset = 0usize;

    if md.curlen as usize >= md.buf.len() {
        return -1;
    }

    while inlen > 0 {
        if md.curlen == 0 && inlen >= SHA256_BLOCK_SIZE {
            if sha256_compress(md, &input[offset..offset + SHA256_BLOCK_SIZE]) < 0 {
                return -1;
            }
            md.length += (SHA256_BLOCK_SIZE as u64) * 8;
            offset += SHA256_BLOCK_SIZE;
            inlen -= SHA256_BLOCK_SIZE;
        } else {
            let room = SHA256_BLOCK_SIZE - md.curlen as usize;
            let n = core::cmp::min(inlen, room);
            md.buf[md.curlen as usize..md.curlen as usize + n]
                .copy_from_slice(&input[offset..offset + n]);
            md.curlen += n as u32;
            offset += n;
            inlen -= n;
            if md.curlen as usize == SHA256_BLOCK_SIZE {
                let block = md.buf;
                if sha256_compress(md, &block) < 0 {
                    return -1;
                }
                md.length += (SHA256_BLOCK_SIZE as u64) * 8;
                md.curlen = 0;
            }
        }
    }
    0
}

fn sha256_done_inner(md: &mut Sha256State, out: &mut [u8]) -> c_int {
    if md.curlen as usize >= md.buf.len() {
        return -1;
    }

    md.length += (md.curlen as u64) * 8;
    md.buf[md.curlen as usize] = 0x80;
    md.curlen += 1;

    if md.curlen > 56 {
        while (md.curlen as usize) < SHA256_BLOCK_SIZE {
            md.buf[md.curlen as usize] = 0;
            md.curlen += 1;
        }
        let block = md.buf;
        sha256_compress(md, &block);
        md.curlen = 0;
    }

    while (md.curlen as usize) < 56 {
        md.buf[md.curlen as usize] = 0;
        md.curlen += 1;
    }

    put_be64(&mut md.buf[56..64], md.length);
    let block = md.buf;
    sha256_compress(md, &block);

    for i in 0..8 {
        put_be32(&mut out[4 * i..4 * i + 4], md.state[i]);
    }
    0
}

/// Typed SHA-256 over a slice vector (oracle: `sha256_vector`).
pub fn sha256_vector_typed(parts: &[&[u8]], mac: &mut [u8; SHA256_MAC_LEN]) -> Result<(), ()> {
    let mut ctx = Sha256State {
        length: 0,
        state: [0; 8],
        curlen: 0,
        buf: [0; SHA256_BLOCK_SIZE],
    };

    sha256_init(&mut ctx);
    for part in parts {
        if sha256_process_inner(&mut ctx, part) != 0 {
            return Err(());
        }
    }
    if sha256_done_inner(&mut ctx, mac) != 0 {
        return Err(());
    }
    Ok(())
}

/// C ABI: `sha256_vector` from `core/crypto/sha256-internal.c`.
#[no_mangle]
pub extern "C" fn sha256_vector(
    num_elem: usize,
    addr: *const *const u8,
    len: *const usize,
    mac: *mut u8,
) -> c_int {
    if mac.is_null() {
        return -1;
    }
    if num_elem > 0 && (addr.is_null() || len.is_null()) {
        return -1;
    }

    let mut ctx = Sha256State {
        length: 0,
        state: [0; 8],
        curlen: 0,
        buf: [0; SHA256_BLOCK_SIZE],
    };

    sha256_init(&mut ctx);
    for i in 0..num_elem {
        let inlen = unsafe { *len.add(i) };
        let ptr = unsafe { *addr.add(i) };
        if inlen > 0 && ptr.is_null() {
            return -1;
        }
        let slice = if inlen == 0 {
            &[][..]
        } else {
            unsafe { core::slice::from_raw_parts(ptr, inlen) }
        };
        if sha256_process_inner(&mut ctx, slice) != 0 {
            return -1;
        }
    }
    if sha256_done_inner(
        &mut ctx,
        unsafe { core::slice::from_raw_parts_mut(mac, SHA256_MAC_LEN) },
    ) != 0 {
        return -1;
    }
    0
}

/// Link-time probe for L1 (distinct from the exported crypto symbols).
#[no_mangle]
pub extern "C" fn rtw_rust_sha256_internal_probe() -> c_int {
    SHA256_MAC_LEN as c_int
}

// C-internal symbols kept for parity with sha256_i.h (used by sha256.c later).
#[no_mangle]
pub extern "C" fn _sha256_init(md: *mut Sha256State) {
    if md.is_null() {
        return;
    }
    sha256_init(unsafe { &mut *md });
}

#[no_mangle]
pub extern "C" fn sha256_process(
    md: *mut Sha256State,
    input: *const u8,
    inlen: c_ulong,
) -> c_int {
    if md.is_null() {
        return -1;
    }
    // C does not check `in` when `inlen > 0` (null is UB); return -1 instead.
    let slice = if inlen == 0 {
        &[][..]
    } else if input.is_null() {
        return -1;
    } else {
        unsafe { core::slice::from_raw_parts(input, inlen as usize) }
    };
    sha256_process_inner(unsafe { &mut *md }, slice)
}

#[no_mangle]
pub extern "C" fn sha256_done(md: *mut Sha256State, out: *mut u8) -> c_int {
    if md.is_null() || out.is_null() {
        return -1;
    }
    let out_slice = unsafe { core::slice::from_raw_parts_mut(out, SHA256_MAC_LEN) };
    sha256_done_inner(unsafe { &mut *md }, out_slice)
}
