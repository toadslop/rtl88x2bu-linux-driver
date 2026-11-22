// SPDX-License-Identifier: GPL-2.0
//! Host L2 staticlib for W4-130 periodic TSF in `hal_com_tsf_periodic.inc.rs`.

#![allow(dead_code, non_snake_case, non_upper_case_globals)]

use core::ffi::{c_ulong, c_void};

type U8 = u8;

include!("hal_com_tsf_periodic.inc.rs");