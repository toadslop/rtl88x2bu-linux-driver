// SPDX-License-Identifier: GPL-2.0
// rtw_hal_get_rsvd_page — HALMAC reserved page FIFO dump (W4-121).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;

const HAL_DEF_TX_PAGE_SIZE: i32 = 26;
const HAL_FIFO_SEL_RSVD_PAGE: U8 = 2;
const _FALSE: i32 = 0;
const _TRUE: i32 = 1;

#[cfg(host_hal_com_get_rsvd_page_test)]
#[repr(C)]
struct Adapter {
    dvobj: *mut c_void,
}

#[cfg(host_hal_com_get_rsvd_page_test)]
extern "C" {
    fn rtw_hal_get_def_var(adapter: *mut c_void, def: i32, val: *mut c_void);
    fn rtw_halmac_dump_fifo(
        d: *mut c_void,
        fifo_sel: U8,
        addr: U32,
        size: U32,
        buffer: *mut U8,
    ) -> i32;
}

#[cfg(not(host_hal_com_get_rsvd_page_test))]
extern "C" {
    fn rtw_hal_get_def_var(adapter: *mut c_void, variable: i32, value: *mut c_void);
    fn rtw_rust_hal_com_adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void;
    fn rtw_halmac_dump_fifo(
        d: *mut c_void,
        fifo_sel: U8,
        addr: U32,
        size: U32,
        buffer: *mut U8,
    ) -> i32;
}

#[cfg(host_hal_com_get_rsvd_page_test)]
fn adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void {
    if adapter.is_null() {
        return core::ptr::null_mut();
    }
    unsafe { (*(adapter as *mut Adapter)).dvobj }
}

#[cfg(not(host_hal_com_get_rsvd_page_test))]
fn adapter_to_dvobj(adapter: *mut c_void) -> *mut c_void {
    unsafe { rtw_rust_hal_com_adapter_to_dvobj(adapter) }
}

fn get_tx_page_size(adapter: *mut c_void) -> U32 {
    let mut page_size: U32 = 0;
    unsafe {
        rtw_hal_get_def_var(
            adapter,
            HAL_DEF_TX_PAGE_SIZE,
            (&mut page_size as *mut U32).cast::<c_void>(),
        );
    }
    page_size
}

#[no_mangle]
pub extern "C" fn rtw_hal_get_rsvd_page(
    adapter: *mut c_void,
    page_offset: U32,
    page_num: U32,
    buffer: *mut U8,
    buffer_size: U32,
) -> i32 {
    let page_size = get_tx_page_size(adapter);
    let addr = page_offset.wrapping_mul(page_size);
    let size = page_num.wrapping_mul(page_size);
    let mut rst = _FALSE;

    if buffer_size < size {
        return rst;
    }

    #[cfg(any(config_rtw_halmac, host_hal_com_get_rsvd_page_test))]
    {
        let dvobj = adapter_to_dvobj(adapter);
        if unsafe {
            rtw_halmac_dump_fifo(dvobj, HAL_FIFO_SEL_RSVD_PAGE, addr, size, buffer)
        } < 0
        {
            rst = _FALSE;
        } else {
            rst = _TRUE;
        }
    }

    #[cfg(all(not(config_rtw_halmac), not(host_hal_com_get_rsvd_page_test)))]
    {
        let _ = (adapter, page_offset, page_num, buffer, addr, size);
        rst = _TRUE;
    }

    rst
}