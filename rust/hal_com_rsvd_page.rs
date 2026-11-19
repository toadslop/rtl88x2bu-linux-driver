// SPDX-License-Identifier: GPL-2.0
// rtw_hal_get_rsvd_page — HALMAC reserved page FIFO dump (W4-121).

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;

const HAL_DEF_TX_PAGE_SIZE: i32 = 25;
const HAL_FIFO_SEL_RSVD_PAGE: U8 = 2;
const _FALSE: i32 = 0;
const _TRUE: i32 = 1;

const REG_PKT_BUFF_ACCESS_CTRL: U32 = 0x0106;
const REG_TDECTRL: U32 = 0x0208;
const REG_PKTBUF_DBG_CTRL: U32 = 0x0140;
const REG_PKTBUF_DBG_DATA_L: U32 = 0x0144;
const REG_PKTBUF_DBG_DATA_H: U32 = 0x0148;

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
    fn rtw_rust_hal_com_read8(adapter: *mut c_void, addr: U32) -> U8;
    fn rtw_rust_hal_com_write8(adapter: *mut c_void, addr: U32, val: U8);
    fn rtw_rust_hal_com_read32(adapter: *mut c_void, addr: U32) -> U32;
    fn rtw_rust_hal_com_write32(adapter: *mut c_void, addr: U32, val: U32);
    fn rtw_rust_hal_com_err_get_rsvd_page_buf_short(buffer_size: U32, size: U32);
    fn _rtw_memcpy(dst: *mut c_void, src: *const c_void, sz: U32);
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

#[cfg(all(not(config_rtw_halmac), not(host_hal_com_get_rsvd_page_test)))]
fn get_rsvd_page_legacy(
    adapter: *mut c_void,
    page_offset: U32,
    buffer_size: U32,
    buffer: *mut U8,
) -> i32 {
    if buffer.is_null() {
        return _FALSE;
    }
    let page_size = get_tx_page_size(adapter);
    let txbndy = unsafe { rtw_rust_hal_com_read8(adapter, REG_TDECTRL + 1) };
    let offset = ((txbndy as U32 + page_offset).wrapping_mul(page_size) / 8) as u16;
    let count = (buffer_size / 8).wrapping_add(1);
    unsafe {
        rtw_rust_hal_com_write8(adapter, REG_PKT_BUFF_ACCESS_CTRL, 0x69);
    }
    for i in 0..count {
        unsafe {
            rtw_rust_hal_com_write32(
                adapter,
                REG_PKTBUF_DBG_CTRL,
                u32::from(offset.wrapping_add(i as u16)),
            );
            let data_low = rtw_rust_hal_com_read32(adapter, REG_PKTBUF_DBG_DATA_L);
            let data_high = rtw_rust_hal_com_read32(adapter, REG_PKTBUF_DBG_DATA_H);
            let dst = buffer.add((i.wrapping_mul(8)) as usize);
            _rtw_memcpy(dst.cast(), &data_low as *const U32 as *const c_void, 4);
            _rtw_memcpy(
                dst.add(4).cast(),
                &data_high as *const U32 as *const c_void,
                4,
            );
        }
    }
    unsafe {
        rtw_rust_hal_com_write8(adapter, REG_PKT_BUFF_ACCESS_CTRL, 0);
    }
    _TRUE
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
        #[cfg(not(host_hal_com_get_rsvd_page_test))]
        unsafe {
            rtw_rust_hal_com_err_get_rsvd_page_buf_short(buffer_size, size);
        }
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
        let _ = (page_num, addr);
        rst = get_rsvd_page_legacy(adapter, page_offset, buffer_size, buffer);
    }

    rst
}