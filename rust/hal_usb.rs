// SPDX-License-Identifier: GPL-2.0
//! HAL USB HCI — `usb_set_intf_ops`, vendor wvalue fixup, sync reads/writes, and async writes from
//! `hal/hal_hci/hal_usb.c` (W4-25, W4-28, W4-33, W4-34, W4-37, W4-43, W4-44, W4-45, W4-46, W4-53).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe
)]

type U16 = u16;

const VENDOR_CMD_MAX_DATA_LEN: usize = 254;

/// WLANON PAGE0_REG wvalue offset for RTL8710B USB vendor requests.
#[no_mangle]
pub extern "C" fn usb_vendor_wvalue_fixup(wvalue: U16) -> U16 {
    #[cfg(any(CONFIG_RTL8710B, host_hal_usb_wvalue_test, host_hal_usb_read_test))]
    if wvalue < 0x0100 {
        return wvalue | 0x8000;
    }
    wvalue
}

#[cfg(host_hal_usb_read_test)]
mod sync_read {
    pub type Hdl = *mut u8;

    unsafe extern "C" {
        fn usbctrl_vendorreq(
            pintfhdl: Hdl,
            request: u8,
            value: u16,
            index: u16,
            pdata: *mut u8,
            len: u16,
            requesttype: u8,
        ) -> i32;
    }

    unsafe fn usb_sync_vendor_read(pintfhdl: Hdl, addr: u32, len: u16, data: *mut u8) {
        let request: u8 = 0x05;
        let requesttype: u8 = 0x01;
        let index: u16 = 0;
        let wvalue = super::usb_vendor_wvalue_fixup((addr & 0x0000ffff) as u16);
        unsafe {
            usbctrl_vendorreq(pintfhdl, request, wvalue, index, data, len, requesttype);
        }
    }

    #[no_mangle]
    pub extern "C" fn usb_read8(pintfhdl: Hdl, addr: u32) -> u8 {
        let mut data: u8 = 0;
        unsafe {
            usb_sync_vendor_read(pintfhdl, addr, 1, &mut data as *mut u8);
        }
        data
    }

    #[no_mangle]
    pub extern "C" fn usb_read16(pintfhdl: Hdl, addr: u32) -> u16 {
        let mut data: u16 = 0;
        unsafe {
            usb_sync_vendor_read(pintfhdl, addr, 2, &mut data as *mut u16 as *mut u8);
        }
        data
    }

    #[no_mangle]
    pub extern "C" fn usb_read32(pintfhdl: Hdl, addr: u32) -> u32 {
        let mut data: u32 = 0;
        unsafe {
            usb_sync_vendor_read(pintfhdl, addr, 4, &mut data as *mut u32 as *mut u8);
        }
        data
    }
}

#[cfg(not(any(
    host_hal_usb_wvalue_test,
    host_hal_usb_read_test,
    host_hal_usb_async_write_test
)))]
mod kernel {
    use core::ffi::c_void;

    pub type Hdl = *mut c_void;

    unsafe extern "C" {
        fn usbctrl_vendorreq(
            pintfhdl: Hdl,
            request: u8,
            value: u16,
            index: u16,
            pdata: *mut c_void,
            len: u16,
            requesttype: u8,
        ) -> i32;
        fn _rtw_memcpy(dst: *mut c_void, src: *const c_void, sz: usize) -> *mut c_void;
        fn usb_read_mem(pintfhdl: Hdl, addr: u32, cnt: u32, pmem: *mut u8);
        fn usb_read_port(pintfhdl: Hdl, addr: u32, cnt: u32, pmem: *mut u8) -> u32;
        fn usb_write_mem(pintfhdl: Hdl, addr: u32, cnt: u32, pmem: *mut u8);
        fn usb_write_port(pintfhdl: Hdl, addr: u32, cnt: u32, pmem: *mut u8) -> u32;
        fn usb_read_port_cancel(pintfhdl: Hdl);
        fn usb_write_port_cancel(pintfhdl: Hdl);
        fn usb_read_interrupt(pintfhdl: Hdl, addr: u32) -> u32;
    }

    unsafe fn usb_sync_vendor_read(pintfhdl: Hdl, addr: u32, len: u16, data: *mut c_void) {
        let request: u8 = 0x05;
        let requesttype: u8 = 0x01;
        let index: u16 = 0;
        let wvalue = super::usb_vendor_wvalue_fixup((addr & 0x0000ffff) as u16);
        unsafe {
            usbctrl_vendorreq(pintfhdl, request, wvalue, index, data, len, requesttype);
        }
    }

    unsafe fn usb_sync_vendor_write_scalar(
        pintfhdl: Hdl,
        addr: u32,
        len: u16,
        data: *mut c_void,
    ) -> i32 {
        let request: u8 = 0x05;
        let requesttype: u8 = 0x00;
        let index: u16 = 0;
        let wvalue = super::usb_vendor_wvalue_fixup((addr & 0x0000ffff) as u16);
        unsafe { usbctrl_vendorreq(pintfhdl, request, wvalue, index, data, len, requesttype) }
    }

    #[no_mangle]
    pub extern "C" fn usb_read8(pintfhdl: Hdl, addr: u32) -> u8 {
        let mut data: u8 = 0;
        unsafe {
            usb_sync_vendor_read(pintfhdl, addr, 1, &mut data as *mut u8 as *mut c_void);
        }
        data
    }

    #[no_mangle]
    pub extern "C" fn usb_read16(pintfhdl: Hdl, addr: u32) -> u16 {
        let mut data: u16 = 0;
        unsafe {
            usb_sync_vendor_read(pintfhdl, addr, 2, &mut data as *mut u16 as *mut c_void);
        }
        data
    }

    #[no_mangle]
    pub extern "C" fn usb_read32(pintfhdl: Hdl, addr: u32) -> u32 {
        let mut data: u32 = 0;
        unsafe {
            usb_sync_vendor_read(pintfhdl, addr, 4, &mut data as *mut u32 as *mut c_void);
        }
        data
    }

    #[no_mangle]
    pub extern "C" fn usb_write8(pintfhdl: Hdl, addr: u32, val: u8) -> i32 {
        let mut data = val;
        unsafe {
            usb_sync_vendor_write_scalar(pintfhdl, addr, 1, &mut data as *mut u8 as *mut c_void)
        }
    }

    #[no_mangle]
    pub extern "C" fn usb_write16(pintfhdl: Hdl, addr: u32, val: u16) -> i32 {
        let mut data = val;
        unsafe {
            usb_sync_vendor_write_scalar(pintfhdl, addr, 2, &mut data as *mut u16 as *mut c_void)
        }
    }

    #[no_mangle]
    pub extern "C" fn usb_write32(pintfhdl: Hdl, addr: u32, val: u32) -> i32 {
        let mut data = val;
        unsafe {
            usb_sync_vendor_write_scalar(pintfhdl, addr, 4, &mut data as *mut u32 as *mut c_void)
        }
    }

    #[no_mangle]
    pub extern "C" fn usb_writeN(pintfhdl: Hdl, addr: u32, length: u32, pdata: *mut u8) -> i32 {
        let request: u8 = 0x05;
        let requesttype: u8 = 0x00;
        let index: u16 = 0;
        let wvalue = (addr & 0x0000ffff) as u16;
        let len = length as u16;
        let mut buf = [0u8; super::VENDOR_CMD_MAX_DATA_LEN];
        if len as usize > buf.len() || pdata.is_null() {
            return -1;
        }
        unsafe {
            _rtw_memcpy(
                buf.as_mut_ptr() as *mut c_void,
                pdata as *const c_void,
                len as usize,
            );
            usbctrl_vendorreq(
                pintfhdl,
                request,
                wvalue,
                index,
                buf.as_mut_ptr() as *mut c_void,
                len,
                requesttype,
            )
        }
    }

    #[cfg(CONFIG_USB_SUPPORT_ASYNC_VDN_REQ)]
    unsafe extern "C" {
        fn _usbctrl_vendorreq_async_write(
            udev: *mut c_void,
            request: u8,
            value: u16,
            index: u16,
            pdata: *mut c_void,
            len: u16,
            requesttype: u8,
        ) -> i32;
    }

    #[cfg(CONFIG_USB_SUPPORT_ASYNC_VDN_REQ)]
    unsafe extern "C" {
        fn rtw_rust_hal_usb_pusbdev_from_intf(pintfhdl: Hdl) -> *mut c_void;
    }

    #[cfg(CONFIG_USB_SUPPORT_ASYNC_VDN_REQ)]
    unsafe fn usb_write_async_inner(
        udev: *mut c_void,
        addr: u32,
        pdata: *mut c_void,
        len: u16,
    ) -> i32 {
        let request: u8 = 0x05;
        let requesttype: u8 = 0x00;
        let index: u16 = 0;
        let wvalue = (addr & 0x0000ffff) as u16;
        unsafe {
            _usbctrl_vendorreq_async_write(udev, request, wvalue, index, pdata, len, requesttype)
        }
    }

    #[cfg(CONFIG_USB_SUPPORT_ASYNC_VDN_REQ)]
    #[no_mangle]
    pub extern "C" fn usb_async_write8(pintfhdl: Hdl, addr: u32, val: u8) -> i32 {
        let mut data = val;
        let udev = unsafe { rtw_rust_hal_usb_pusbdev_from_intf(pintfhdl) };
        if udev.is_null() {
            return -1;
        }
        unsafe { usb_write_async_inner(udev, addr, &mut data as *mut u8 as *mut c_void, 1) }
    }

    #[cfg(CONFIG_USB_SUPPORT_ASYNC_VDN_REQ)]
    #[no_mangle]
    pub extern "C" fn usb_async_write16(pintfhdl: Hdl, addr: u32, val: u16) -> i32 {
        let mut data = val;
        let udev = unsafe { rtw_rust_hal_usb_pusbdev_from_intf(pintfhdl) };
        if udev.is_null() {
            return -1;
        }
        unsafe { usb_write_async_inner(udev, addr, &mut data as *mut u16 as *mut c_void, 2) }
    }

    #[cfg(CONFIG_USB_SUPPORT_ASYNC_VDN_REQ)]
    #[no_mangle]
    pub extern "C" fn usb_async_write32(pintfhdl: Hdl, addr: u32, val: u32) -> i32 {
        let mut data = val;
        let udev = unsafe { rtw_rust_hal_usb_pusbdev_from_intf(pintfhdl) };
        if udev.is_null() {
            return -1;
        }
        unsafe { usb_write_async_inner(udev, addr, &mut data as *mut u32 as *mut c_void, 4) }
    }

    #[repr(C)]
    struct IoOps {
        _read8: Option<unsafe extern "C" fn(Hdl, u32) -> u8>,
        _read16: Option<unsafe extern "C" fn(Hdl, u32) -> u16>,
        _read32: Option<unsafe extern "C" fn(Hdl, u32) -> u32>,
        _write8: Option<unsafe extern "C" fn(Hdl, u32, u8) -> i32>,
        _write16: Option<unsafe extern "C" fn(Hdl, u32, u16) -> i32>,
        _write32: Option<unsafe extern "C" fn(Hdl, u32, u32) -> i32>,
        _writeN: Option<unsafe extern "C" fn(Hdl, u32, u32, *mut u8) -> i32>,
        _write8_async: Option<unsafe extern "C" fn(Hdl, u32, u8) -> i32>,
        _write16_async: Option<unsafe extern "C" fn(Hdl, u32, u16) -> i32>,
        _write32_async: Option<unsafe extern "C" fn(Hdl, u32, u32) -> i32>,
        _read_mem: Option<unsafe extern "C" fn(Hdl, u32, u32, *mut u8)>,
        _write_mem: Option<unsafe extern "C" fn(Hdl, u32, u32, *mut u8)>,
        _sync_irp_protocol_rw: Option<unsafe extern "C" fn(*mut c_void)>,
        _read_interrupt: Option<unsafe extern "C" fn(Hdl, u32) -> u32>,
        _read_port: Option<unsafe extern "C" fn(Hdl, u32, u32, *mut u8) -> u32>,
        _write_port: Option<unsafe extern "C" fn(Hdl, u32, u32, *mut u8) -> u32>,
        _write_scsi: Option<unsafe extern "C" fn(Hdl, u32, *mut u8) -> u32>,
        _read_port_cancel: Option<unsafe extern "C" fn(Hdl)>,
        _write_port_cancel: Option<unsafe extern "C" fn(Hdl)>,
    }

    #[no_mangle]
    pub extern "C" fn usb_set_intf_ops(_padapter: *mut c_void, pops: *mut c_void) {
        if pops.is_null() {
            return;
        }
        let pops = unsafe { &mut *(pops as *mut IoOps) };
        unsafe {
            core::ptr::write_bytes(
                pops as *mut IoOps as *mut u8,
                0,
                core::mem::size_of::<IoOps>(),
            );
        }

        pops._read8 = Some(usb_read8);
        pops._read16 = Some(usb_read16);
        pops._read32 = Some(usb_read32);
        pops._read_mem = Some(usb_read_mem);
        pops._read_port = Some(usb_read_port);
        pops._write8 = Some(usb_write8);
        pops._write16 = Some(usb_write16);
        pops._write32 = Some(usb_write32);
        pops._writeN = Some(usb_writeN);
        #[cfg(CONFIG_USB_SUPPORT_ASYNC_VDN_REQ)]
        {
            pops._write8_async = Some(usb_async_write8);
            pops._write16_async = Some(usb_async_write16);
            pops._write32_async = Some(usb_async_write32);
        }
        pops._write_mem = Some(usb_write_mem);
        pops._write_port = Some(usb_write_port);
        pops._read_port_cancel = Some(usb_read_port_cancel);
        pops._write_port_cancel = Some(usb_write_port_cancel);
        #[cfg(CONFIG_USB_INTERRUPT_IN_PIPE)]
        {
            pops._read_interrupt = Some(usb_read_interrupt);
        }
    }

    unsafe extern "C" {
        static rtw_rust_hal_usb_off_adapter_registrypriv: usize;
        static rtw_rust_hal_usb_off_registrypriv_recvbuf_nr: usize;
        static rtw_rust_hal_usb_off_adapter_recvpriv: usize;
        static rtw_rust_hal_usb_off_recvpriv_pallocated_recv_buf: usize;
        static rtw_rust_hal_usb_off_recvpriv_precv_buf: usize;
        static rtw_rust_hal_usb_off_recvpriv_free_recv_skb_queue: usize;
        static rtw_rust_hal_usb_off_recvpriv_rx_skb_queue: usize;
        static rtw_rust_hal_usb_size_recv_buf: usize;
        fn rtw_os_recvbuf_resource_free(padapter: Hdl, precvbuf: Hdl);
        fn _rtw_mfree(p: *mut c_void, sz: u32);
        fn _rtw_skb_queue_purge(list: *mut c_void);
        fn _rtw_skb_free(skb: *mut c_void);
        fn rtw_free_skb_premem(skb: *mut c_void) -> i32;
        fn rtw_rust_hal_usb_skb_queue_len(list: *mut c_void) -> u32;
        fn rtw_rust_hal_usb_skb_dequeue(list: *mut c_void) -> *mut c_void;
        fn rtw_rust_hal_usb_warn_rx_skb_queue_not_empty();
        fn rtw_rust_hal_usb_warn_free_recv_skb_queue_not_empty(len: u32);
        fn _rtw_zmalloc(sz: u32) -> *mut c_void;
        fn rtw_rust_hal_usb_recv_tasklets_init(padapter: Hdl);
        fn rtw_rust_hal_usb_recv_queues_init(padapter: Hdl);
        fn rtw_rust_hal_usb_int_in_pipe_init(padapter: Hdl, ini_in_buf_sz: u16) -> i32;
        fn rtw_rust_hal_usb_int_in_pipe_undo(padapter: Hdl, ini_in_buf_sz: u16);
        fn rtw_rust_hal_usb_log_recvbuf_alloc(padapter: Hdl);
        fn rtw_rust_hal_usb_init_recv_priv_p2(padapter: Hdl) -> i32;
    }

    #[cfg(CONFIG_USB_INTERRUPT_IN_PIPE)]
    unsafe extern "C" {
        static rtw_rust_hal_usb_off_recvpriv_int_in_urb: usize;
        static rtw_rust_hal_usb_off_recvpriv_int_in_buf: usize;
        fn usb_free_urb(urb: *mut c_void);
    }

    unsafe fn adapter_recvbuf_nr(padapter: Hdl) -> u8 {
        unsafe {
            let reg = (padapter as *mut u8).add(rtw_rust_hal_usb_off_adapter_registrypriv);
            *reg.add(rtw_rust_hal_usb_off_registrypriv_recvbuf_nr)
        }
    }

    unsafe fn recvpriv_base(padapter: Hdl) -> *mut u8 {
        unsafe { (padapter as *mut u8).add(rtw_rust_hal_usb_off_adapter_recvpriv) }
    }

    const C_SUCCESS: i32 = 1;
    const C_FAIL: i32 = 0;

    unsafe fn n_byte_align(value: usize, alignment: usize) -> usize {
        if alignment <= 1 {
            return value;
        }
        ((value + alignment - 1) / alignment) * alignment
    }

    fn rtw_rust_hal_usb_init_recv_priv_p1(padapter: Hdl, ini_in_buf_sz: u16) -> i32 {
        if padapter.is_null() {
            return C_FAIL;
        }
        unsafe {
            rtw_rust_hal_usb_recv_tasklets_init(padapter);
            if rtw_rust_hal_usb_int_in_pipe_init(padapter, ini_in_buf_sz) == C_FAIL {
                rtw_rust_hal_usb_int_in_pipe_undo(padapter, ini_in_buf_sz);
                return C_FAIL;
            }
            rtw_rust_hal_usb_recv_queues_init(padapter);

            let recvbuf_nr = adapter_recvbuf_nr(padapter);
            rtw_rust_hal_usb_log_recvbuf_alloc(padapter);

            let alloc_sz = recvbuf_nr as u32 * rtw_rust_hal_usb_size_recv_buf as u32 + 4;
            let pallocated = _rtw_zmalloc(alloc_sz);
            if pallocated.is_null() {
                rtw_rust_hal_usb_int_in_pipe_undo(padapter, ini_in_buf_sz);
                return C_FAIL;
            }

            let precvpriv = recvpriv_base(padapter);
            let precv_buf = n_byte_align(pallocated as usize, 4) as *mut u8;
            *(precvpriv.add(rtw_rust_hal_usb_off_recvpriv_pallocated_recv_buf) as *mut *mut u8) =
                pallocated as *mut u8;
            *(precvpriv.add(rtw_rust_hal_usb_off_recvpriv_precv_buf) as *mut *mut u8) = precv_buf;
            C_SUCCESS
        }
    }

    #[no_mangle]
    pub extern "C" fn usb_init_recv_priv(padapter: Hdl, ini_in_buf_sz: u16) -> i32 {
        if padapter.is_null() {
            return C_FAIL;
        }
        let res = rtw_rust_hal_usb_init_recv_priv_p1(padapter, ini_in_buf_sz);
        if res == C_FAIL {
            return C_FAIL;
        }
        unsafe { rtw_rust_hal_usb_init_recv_priv_p2(padapter) }
    }

    #[no_mangle]
    pub extern "C" fn usb_free_recv_priv(padapter: Hdl, ini_in_buf_sz: u16) {
        if padapter.is_null() {
            return;
        }
        unsafe {
            let recvbuf_nr = adapter_recvbuf_nr(padapter);
            let precvpriv = recvpriv_base(padapter);
            let mut precvbuf =
                *(precvpriv.add(rtw_rust_hal_usb_off_recvpriv_precv_buf) as *mut *mut u8);
            for _ in 0..recvbuf_nr {
                rtw_os_recvbuf_resource_free(padapter, precvbuf as Hdl);
                precvbuf = precvbuf.add(rtw_rust_hal_usb_size_recv_buf);
            }

            let pallocated =
                *(precvpriv.add(rtw_rust_hal_usb_off_recvpriv_pallocated_recv_buf) as *mut *mut u8);
            if !pallocated.is_null() {
                let alloc_sz = recvbuf_nr as u32 * rtw_rust_hal_usb_size_recv_buf as u32 + 4;
                _rtw_mfree(pallocated as *mut c_void, alloc_sz);
            }

            #[cfg(CONFIG_USB_INTERRUPT_IN_PIPE)]
            {
                let int_in_urb =
                    *(precvpriv.add(rtw_rust_hal_usb_off_recvpriv_int_in_urb) as *mut *mut c_void);
                if !int_in_urb.is_null() {
                    usb_free_urb(int_in_urb);
                }
                let int_in_buf =
                    *(precvpriv.add(rtw_rust_hal_usb_off_recvpriv_int_in_buf) as *mut *mut u8);
                if !int_in_buf.is_null() {
                    _rtw_mfree(int_in_buf as *mut c_void, ini_in_buf_sz as u32);
                }
            }
            #[cfg(not(CONFIG_USB_INTERRUPT_IN_PIPE))]
            let _ini_in_buf_sz = ini_in_buf_sz;

            let rx_skb_queue =
                precvpriv.add(rtw_rust_hal_usb_off_recvpriv_rx_skb_queue) as *mut c_void;
            if rtw_rust_hal_usb_skb_queue_len(rx_skb_queue) != 0 {
                rtw_rust_hal_usb_warn_rx_skb_queue_not_empty();
            }
            _rtw_skb_queue_purge(rx_skb_queue);

            let free_recv_skb_queue =
                precvpriv.add(rtw_rust_hal_usb_off_recvpriv_free_recv_skb_queue) as *mut c_void;
            let qlen = rtw_rust_hal_usb_skb_queue_len(free_recv_skb_queue);
            if qlen != 0 {
                rtw_rust_hal_usb_warn_free_recv_skb_queue_not_empty(qlen);
            }

            #[cfg(not(CONFIG_USE_USB_BUFFER_ALLOC_RX))]
            {
                #[cfg(all(CONFIG_PREALLOC_RECV_SKB, CONFIG_PREALLOC_RX_SKB_BUFFER))]
                {
                    loop {
                        let skb = rtw_rust_hal_usb_skb_dequeue(free_recv_skb_queue);
                        if skb.is_null() {
                            break;
                        }
                        if rtw_free_skb_premem(skb) != 0 {
                            _rtw_skb_free(skb);
                        }
                    }
                }
                #[cfg(not(all(CONFIG_PREALLOC_RECV_SKB, CONFIG_PREALLOC_RX_SKB_BUFFER)))]
                {
                    _rtw_skb_queue_purge(free_recv_skb_queue);
                }
            }
        }
    }

    #[cfg(c2h_wk)]
    const C2H_REG_LEN: usize = 16;
    #[cfg(c2h_wk)]
    const C2H_SUCCESS: i32 = 1;

    #[cfg(c2h_wk)]
    unsafe extern "C" {
        static rtw_rust_hal_usb_off_adapter_evtpriv: usize;
        static rtw_rust_hal_usb_off_evtpriv_c2h_queue: usize;
        static rtw_rust_hal_usb_off_evtpriv_c2h_wk: usize;
        fn rtw_hal_c2h_reg_hdr_parse(
            adapter: Hdl,
            buf: *mut u8,
            id: *mut u8,
            seq: *mut u8,
            plen: *mut u8,
            payload: *mut *mut u8,
        ) -> u8;
        fn rtw_hal_c2h_id_handle_directly(
            adapter: Hdl,
            id: u8,
            seq: u8,
            plen: u8,
            payload: *mut u8,
        ) -> i32;
        fn rtw_hal_c2h_handler(adapter: Hdl, id: u8, seq: u8, plen: u8, payload: *mut u8) -> i32;
        fn rtw_cbuf_push(cbuf: *mut c_void, buf: *mut c_void) -> u8;
        fn rtw_malloc(sz: u32) -> *mut c_void;
        fn _set_workitem(pwork: *mut c_void);
        fn rtw_rust_hal_usb_err_c2h_cbuf_push_fail();
    }

    #[cfg(c2h_wk)]
    unsafe fn evtpriv_base(adapter: Hdl) -> *mut u8 {
        unsafe { (adapter as *mut u8).add(rtw_rust_hal_usb_off_adapter_evtpriv) }
    }

    #[cfg(c2h_wk)]
    unsafe fn adapter_c2h_queue(adapter: Hdl) -> *mut c_void {
        unsafe {
            let pevtpriv = evtpriv_base(adapter);
            *(pevtpriv.add(rtw_rust_hal_usb_off_evtpriv_c2h_queue) as *mut *mut c_void)
        }
    }

    #[cfg(c2h_wk)]
    unsafe fn c2h_push_fail() {
        unsafe {
            rtw_rust_hal_usb_err_c2h_cbuf_push_fail();
        }
    }

    #[cfg(c2h_wk)]
    #[no_mangle]
    pub extern "C" fn usb_c2h_hisr_hdl(adapter: Hdl, buf: *mut u8) {
        let mut id: u8 = 0;
        let mut seq: u8 = 0;
        let mut plen: u8 = 0;
        let mut payload: *mut u8 = core::ptr::null_mut();
        unsafe {
            if rtw_hal_c2h_reg_hdr_parse(adapter, buf, &mut id, &mut seq, &mut plen, &mut payload)
                != C2H_SUCCESS as u8
            {
                return;
            }

            let c2h_queue = adapter_c2h_queue(adapter);
            if rtw_hal_c2h_id_handle_directly(adapter, id, seq, plen, payload) != 0 {
                rtw_hal_c2h_handler(adapter, id, seq, plen, payload);
                let pevtpriv = evtpriv_base(adapter);
                if rtw_cbuf_push(c2h_queue, pevtpriv as *mut c_void) != C2H_SUCCESS as u8 {
                    c2h_push_fail();
                }
            } else {
                let c2h_evt = rtw_malloc(C2H_REG_LEN as u32);
                if !c2h_evt.is_null() {
                    _rtw_memcpy(c2h_evt, buf as *const c_void, C2H_REG_LEN);
                    if rtw_cbuf_push(c2h_queue, c2h_evt) != C2H_SUCCESS as u8 {
                        c2h_push_fail();
                    }
                } else if rtw_cbuf_push(c2h_queue, core::ptr::null_mut()) != C2H_SUCCESS as u8 {
                    c2h_push_fail();
                }
            }

            let pevtpriv = evtpriv_base(adapter);
            let c2h_wk = pevtpriv.add(rtw_rust_hal_usb_off_evtpriv_c2h_wk) as *mut c_void;
            _set_workitem(c2h_wk);
        }
    }
}

#[cfg(host_hal_usb_async_write_test)]
mod async_write_host {
    pub type Hdl = *mut u8;

    #[repr(C)]
    struct IntfHdl {
        pintf_dev: *mut u8,
    }

    #[repr(C)]
    struct DvobjPriv {
        pusbdev: *mut u8,
    }

    unsafe extern "C" {
        fn _usbctrl_vendorreq_async_write(
            udev: *mut u8,
            request: u8,
            value: u16,
            index: u16,
            pdata: *mut u8,
            len: u16,
            requesttype: u8,
        ) -> i32;
    }

    unsafe fn usb_write_async_inner(udev: *mut u8, addr: u32, pdata: *mut u8, len: u16) -> i32 {
        let request: u8 = 0x05;
        let requesttype: u8 = 0x00;
        let index: u16 = 0;
        let wvalue = (addr & 0x0000ffff) as u16;
        unsafe {
            _usbctrl_vendorreq_async_write(udev, request, wvalue, index, pdata, len, requesttype)
        }
    }

    #[no_mangle]
    pub extern "C" fn usb_async_write8(pintfhdl: Hdl, addr: u32, val: u8) -> i32 {
        let mut data = val;
        let intf = unsafe { &*(pintfhdl as *const IntfHdl) };
        let dvobj = unsafe { &*(intf.pintf_dev as *const DvobjPriv) };
        unsafe { usb_write_async_inner(dvobj.pusbdev, addr, &mut data as *mut u8, 1) }
    }

    #[no_mangle]
    pub extern "C" fn usb_async_write16(pintfhdl: Hdl, addr: u32, val: u16) -> i32 {
        let mut data = val;
        let intf = unsafe { &*(pintfhdl as *const IntfHdl) };
        let dvobj = unsafe { &*(intf.pintf_dev as *const DvobjPriv) };
        unsafe { usb_write_async_inner(dvobj.pusbdev, addr, &mut data as *mut u16 as *mut u8, 2) }
    }

    #[no_mangle]
    pub extern "C" fn usb_async_write32(pintfhdl: Hdl, addr: u32, val: u32) -> i32 {
        let mut data = val;
        let intf = unsafe { &*(pintfhdl as *const IntfHdl) };
        let dvobj = unsafe { &*(intf.pintf_dev as *const DvobjPriv) };
        unsafe { usb_write_async_inner(dvobj.pusbdev, addr, &mut data as *mut u32 as *mut u8, 4) }
    }
}
