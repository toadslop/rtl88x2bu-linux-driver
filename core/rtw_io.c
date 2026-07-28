/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
/*

The purpose of rtw_io.c

a. provides the API

b. provides the protocol engine

c. provides the software interface between caller and the hardware interface


Compiler Flag Option:

1. CONFIG_SDIO_HCI:
    a. USE_SYNC_IRP:  Only sync operations are provided.
    b. USE_ASYNC_IRP:Both sync/async operations are provided.

2. CONFIG_USB_HCI:
   a. USE_ASYNC_IRP: Both sync/async operations are provided.

3. CONFIG_CFIO_HCI:
   b. USE_SYNC_IRP: Only sync operations are provided.


Only sync read/rtw_write_mem operations are provided.

jackson@realtek.com.tw

*/

#define _RTW_IO_C_

#include <drv_types.h>
#include <hal_data.h>

#if defined(CONFIG_SDIO_HCI) || defined(CONFIG_PLATFORM_RTL8197D)
	#define rtw_le16_to_cpu(val)		val
	#define rtw_le32_to_cpu(val)		val
	#define rtw_cpu_to_le16(val)		val
	#define rtw_cpu_to_le32(val)		val
#else
	#define rtw_le16_to_cpu(val)		le16_to_cpu(val)
	#define rtw_le32_to_cpu(val)		le32_to_cpu(val)
	#define rtw_cpu_to_le16(val)		cpu_to_le16(val)
	#define rtw_cpu_to_le32(val)		cpu_to_le32(val)
#endif


u8 _rtw_read8(_adapter *adapter, u32 addr)
{
	u8 r_val;
	/* struct	io_queue  	*pio_queue = (struct io_queue *)adapter->pio_queue; */
	struct io_priv *pio_priv = &adapter->iopriv;
	struct	intf_hdl		*pintfhdl = &(pio_priv->intf);
	u8(*_read8)(struct intf_hdl *pintfhdl, u32 addr);
	_read8 = pintfhdl->io_ops._read8;

	r_val = _read8(pintfhdl, addr);
	return r_val;
}

u16 _rtw_read16(_adapter *adapter, u32 addr)
{
	u16 r_val;
	/* struct	io_queue  	*pio_queue = (struct io_queue *)adapter->pio_queue; */
	struct io_priv *pio_priv = &adapter->iopriv;
	struct	intf_hdl		*pintfhdl = &(pio_priv->intf);
	u16(*_read16)(struct intf_hdl *pintfhdl, u32 addr);
	_read16 = pintfhdl->io_ops._read16;

	r_val = _read16(pintfhdl, addr);
	return rtw_le16_to_cpu(r_val);
}

u32 _rtw_read32(_adapter *adapter, u32 addr)
{
	u32 r_val;
	/* struct	io_queue  	*pio_queue = (struct io_queue *)adapter->pio_queue; */
	struct io_priv *pio_priv = &adapter->iopriv;
	struct	intf_hdl		*pintfhdl = &(pio_priv->intf);
	u32(*_read32)(struct intf_hdl *pintfhdl, u32 addr);
	_read32 = pintfhdl->io_ops._read32;

	r_val = _read32(pintfhdl, addr);
	return rtw_le32_to_cpu(r_val);

}

int _rtw_write8(_adapter *adapter, u32 addr, u8 val)
{
	/* struct	io_queue  	*pio_queue = (struct io_queue *)adapter->pio_queue; */
	struct io_priv *pio_priv = &adapter->iopriv;
	struct	intf_hdl		*pintfhdl = &(pio_priv->intf);
	int (*_write8)(struct intf_hdl *pintfhdl, u32 addr, u8 val);
	int ret;
	_write8 = pintfhdl->io_ops._write8;

	ret = _write8(pintfhdl, addr, val);

	return RTW_STATUS_CODE(ret);
}
int _rtw_write16(_adapter *adapter, u32 addr, u16 val)
{
	/* struct	io_queue  	*pio_queue = (struct io_queue *)adapter->pio_queue; */
	struct io_priv *pio_priv = &adapter->iopriv;
	struct	intf_hdl		*pintfhdl = &(pio_priv->intf);
	int (*_write16)(struct intf_hdl *pintfhdl, u32 addr, u16 val);
	int ret;
	_write16 = pintfhdl->io_ops._write16;

	val = rtw_cpu_to_le16(val);
	ret = _write16(pintfhdl, addr, val);

	return RTW_STATUS_CODE(ret);
}
int _rtw_write32(_adapter *adapter, u32 addr, u32 val)
{
	/* struct	io_queue  	*pio_queue = (struct io_queue *)adapter->pio_queue; */
	struct io_priv *pio_priv = &adapter->iopriv;
	struct	intf_hdl		*pintfhdl = &(pio_priv->intf);
	int (*_write32)(struct intf_hdl *pintfhdl, u32 addr, u32 val);
	int ret;
	_write32 = pintfhdl->io_ops._write32;

	val = rtw_cpu_to_le32(val);
	ret = _write32(pintfhdl, addr, val);

	return RTW_STATUS_CODE(ret);
}

int _rtw_writeN(_adapter *adapter, u32 addr , u32 length , u8 *pdata)
{
	/* struct	io_queue  	*pio_queue = (struct io_queue *)adapter->pio_queue; */
	struct io_priv *pio_priv = &adapter->iopriv;
	struct	intf_hdl	*pintfhdl = (struct intf_hdl *)(&(pio_priv->intf));
	int (*_writeN)(struct intf_hdl *pintfhdl, u32 addr, u32 length, u8 *pdata);
	int ret;
	_writeN = pintfhdl->io_ops._writeN;

	ret = _writeN(pintfhdl, addr, length, pdata);

	return RTW_STATUS_CODE(ret);
}

#ifdef CONFIG_SDIO_HCI
u8 _rtw_sd_f0_read8(_adapter *adapter, u32 addr)
{
	u8 r_val = 0x00;
	struct io_priv *pio_priv = &adapter->iopriv;
	struct intf_hdl *pintfhdl = &(pio_priv->intf);
	u8(*_sd_f0_read8)(struct intf_hdl *pintfhdl, u32 addr);

	_sd_f0_read8 = pintfhdl->io_ops._sd_f0_read8;

	if (_sd_f0_read8)
		r_val = _sd_f0_read8(pintfhdl, addr);
	else
		RTW_WARN(FUNC_ADPT_FMT" _sd_f0_read8 callback is NULL\n", FUNC_ADPT_ARG(adapter));

	return r_val;
}

#ifdef CONFIG_SDIO_INDIRECT_ACCESS
u8 _rtw_sd_iread8(_adapter *adapter, u32 addr)
{
	u8 r_val = 0x00;
	struct io_priv *pio_priv = &adapter->iopriv;
	struct intf_hdl *pintfhdl = &(pio_priv->intf);
	u8(*_sd_iread8)(struct intf_hdl *pintfhdl, u32 addr);

	_sd_iread8 = pintfhdl->io_ops._sd_iread8;

	if (_sd_iread8)
		r_val = _sd_iread8(pintfhdl, addr);
	else
		RTW_ERR(FUNC_ADPT_FMT" _sd_iread8 callback is NULL\n", FUNC_ADPT_ARG(adapter));

	return r_val;
}

u16 _rtw_sd_iread16(_adapter *adapter, u32 addr)
{
	u16 r_val = 0x00;
	struct io_priv *pio_priv = &adapter->iopriv;
	struct intf_hdl *pintfhdl = &(pio_priv->intf);
	u16(*_sd_iread16)(struct intf_hdl *pintfhdl, u32 addr);

	_sd_iread16 = pintfhdl->io_ops._sd_iread16;

	if (_sd_iread16)
		r_val = _sd_iread16(pintfhdl, addr);
	else
		RTW_ERR(FUNC_ADPT_FMT" _sd_iread16 callback is NULL\n", FUNC_ADPT_ARG(adapter));

	return r_val;
}

u32 _rtw_sd_iread32(_adapter *adapter, u32 addr)
{
	u32 r_val = 0x00;
	struct io_priv *pio_priv = &adapter->iopriv;
	struct intf_hdl *pintfhdl = &(pio_priv->intf);
	u32(*_sd_iread32)(struct intf_hdl *pintfhdl, u32 addr);

	_sd_iread32 = pintfhdl->io_ops._sd_iread32;

	if (_sd_iread32)
		r_val = _sd_iread32(pintfhdl, addr);
	else
		RTW_ERR(FUNC_ADPT_FMT" _sd_iread32 callback is NULL\n", FUNC_ADPT_ARG(adapter));

	return r_val;
}

int _rtw_sd_iwrite8(_adapter *adapter, u32 addr, u8 val)
{
	struct io_priv *pio_priv = &adapter->iopriv;
	struct intf_hdl *pintfhdl = &(pio_priv->intf);
	int (*_sd_iwrite8)(struct intf_hdl *pintfhdl, u32 addr, u8 val);
	int ret = -1;

	_sd_iwrite8 = pintfhdl->io_ops._sd_iwrite8;

	if (_sd_iwrite8)
		ret = _sd_iwrite8(pintfhdl, addr, val);
	else
		RTW_ERR(FUNC_ADPT_FMT" _sd_iwrite8 callback is NULL\n", FUNC_ADPT_ARG(adapter));

	return RTW_STATUS_CODE(ret);
}

int _rtw_sd_iwrite16(_adapter *adapter, u32 addr, u16 val)
{
	struct io_priv *pio_priv = &adapter->iopriv;
	struct intf_hdl *pintfhdl = &(pio_priv->intf);
	int (*_sd_iwrite16)(struct intf_hdl *pintfhdl, u32 addr, u16 val);
	int ret = -1;

	_sd_iwrite16 = pintfhdl->io_ops._sd_iwrite16;

	if (_sd_iwrite16)
		ret = _sd_iwrite16(pintfhdl, addr, val);
	else
		RTW_ERR(FUNC_ADPT_FMT" _sd_iwrite16 callback is NULL\n", FUNC_ADPT_ARG(adapter));

	return RTW_STATUS_CODE(ret);
}
int _rtw_sd_iwrite32(_adapter *adapter, u32 addr, u32 val)
{
	struct io_priv *pio_priv = &adapter->iopriv;
	struct intf_hdl *pintfhdl = &(pio_priv->intf);
	int (*_sd_iwrite32)(struct intf_hdl *pintfhdl, u32 addr, u32 val);
	int ret = -1;

	_sd_iwrite32 = pintfhdl->io_ops._sd_iwrite32;

	if (_sd_iwrite32)
		ret = _sd_iwrite32(pintfhdl, addr, val);
	else
		RTW_ERR(FUNC_ADPT_FMT" _sd_iwrite32 callback is NULL\n", FUNC_ADPT_ARG(adapter));

	return RTW_STATUS_CODE(ret);
}

#endif /* CONFIG_SDIO_INDIRECT_ACCESS */

#endif /* CONFIG_SDIO_HCI */

int _rtw_write8_async(_adapter *adapter, u32 addr, u8 val)
{
	/* struct	io_queue  	*pio_queue = (struct io_queue *)adapter->pio_queue; */
	struct io_priv *pio_priv = &adapter->iopriv;
	struct	intf_hdl		*pintfhdl = &(pio_priv->intf);
	int (*_write8_async)(struct intf_hdl *pintfhdl, u32 addr, u8 val);
	int ret;
	_write8_async = pintfhdl->io_ops._write8_async;

	ret = _write8_async(pintfhdl, addr, val);

	return RTW_STATUS_CODE(ret);
}
int _rtw_write16_async(_adapter *adapter, u32 addr, u16 val)
{
	/* struct	io_queue  	*pio_queue = (struct io_queue *)adapter->pio_queue; */
	struct io_priv *pio_priv = &adapter->iopriv;
	struct	intf_hdl		*pintfhdl = &(pio_priv->intf);
	int (*_write16_async)(struct intf_hdl *pintfhdl, u32 addr, u16 val);
	int ret;
	_write16_async = pintfhdl->io_ops._write16_async;
	val = rtw_cpu_to_le16(val);
	ret = _write16_async(pintfhdl, addr, val);

	return RTW_STATUS_CODE(ret);
}
int _rtw_write32_async(_adapter *adapter, u32 addr, u32 val)
{
	/* struct	io_queue  	*pio_queue = (struct io_queue *)adapter->pio_queue; */
	struct io_priv *pio_priv = &adapter->iopriv;
	struct	intf_hdl		*pintfhdl = &(pio_priv->intf);
	int (*_write32_async)(struct intf_hdl *pintfhdl, u32 addr, u32 val);
	int ret;
	_write32_async = pintfhdl->io_ops._write32_async;
	val = rtw_cpu_to_le32(val);
	ret = _write32_async(pintfhdl, addr, val);

	return RTW_STATUS_CODE(ret);
}

void _rtw_read_mem(_adapter *adapter, u32 addr, u32 cnt, u8 *pmem)
{
	void (*_read_mem)(struct intf_hdl *pintfhdl, u32 addr, u32 cnt, u8 *pmem);
	/* struct	io_queue  	*pio_queue = (struct io_queue *)adapter->pio_queue; */
	struct io_priv *pio_priv = &adapter->iopriv;
	struct	intf_hdl		*pintfhdl = &(pio_priv->intf);


	if (RTW_CANNOT_RUN(adapter)) {
		return;
	}

	_read_mem = pintfhdl->io_ops._read_mem;

	_read_mem(pintfhdl, addr, cnt, pmem);


}

void _rtw_write_mem(_adapter *adapter, u32 addr, u32 cnt, u8 *pmem)
{
	void (*_write_mem)(struct intf_hdl *pintfhdl, u32 addr, u32 cnt, u8 *pmem);
	/* struct	io_queue  	*pio_queue = (struct io_queue *)adapter->pio_queue; */
	struct io_priv *pio_priv = &adapter->iopriv;
	struct	intf_hdl		*pintfhdl = &(pio_priv->intf);


	_write_mem = pintfhdl->io_ops._write_mem;

	_write_mem(pintfhdl, addr, cnt, pmem);


}

void _rtw_read_port(_adapter *adapter, u32 addr, u32 cnt, u8 *pmem)
{
	u32(*_read_port)(struct intf_hdl *pintfhdl, u32 addr, u32 cnt, u8 *pmem);
	/* struct	io_queue  	*pio_queue = (struct io_queue *)adapter->pio_queue; */
	struct io_priv *pio_priv = &adapter->iopriv;
	struct	intf_hdl		*pintfhdl = &(pio_priv->intf);


	if (RTW_CANNOT_RUN(adapter)) {
		return;
	}

	_read_port = pintfhdl->io_ops._read_port;

	_read_port(pintfhdl, addr, cnt, pmem);


}

void _rtw_read_port_cancel(_adapter *adapter)
{
	void (*_read_port_cancel)(struct intf_hdl *pintfhdl);
	struct io_priv *pio_priv = &adapter->iopriv;
	struct intf_hdl *pintfhdl = &(pio_priv->intf);

	_read_port_cancel = pintfhdl->io_ops._read_port_cancel;

	RTW_DISABLE_FUNC(adapter, DF_RX_BIT);

	if (_read_port_cancel)
		_read_port_cancel(pintfhdl);
}

u32 _rtw_write_port(_adapter *adapter, u32 addr, u32 cnt, u8 *pmem)
{
	u32(*_write_port)(struct intf_hdl *pintfhdl, u32 addr, u32 cnt, u8 *pmem);
	/* struct	io_queue  	*pio_queue = (struct io_queue *)adapter->pio_queue; */
	struct io_priv *pio_priv = &adapter->iopriv;
	struct	intf_hdl		*pintfhdl = &(pio_priv->intf);
	u32 ret = _SUCCESS;


	_write_port = pintfhdl->io_ops._write_port;

	ret = _write_port(pintfhdl, addr, cnt, pmem);


	return ret;
}

u32 _rtw_write_port_and_wait(_adapter *adapter, u32 addr, u32 cnt, u8 *pmem, int timeout_ms)
{
	int ret = _SUCCESS;
	struct xmit_buf *pxmitbuf = (struct xmit_buf *)pmem;
	struct submit_ctx sctx;

	rtw_sctx_init(&sctx, timeout_ms);
	pxmitbuf->sctx = &sctx;

	ret = _rtw_write_port(adapter, addr, cnt, pmem);

	if (ret == _SUCCESS) {
		ret = rtw_sctx_wait(&sctx, __func__);

		if (ret != _SUCCESS)
			pxmitbuf->sctx = NULL;
	}

	return ret;
}

void _rtw_write_port_cancel(_adapter *adapter)
{
	void (*_write_port_cancel)(struct intf_hdl *pintfhdl);
	struct io_priv *pio_priv = &adapter->iopriv;
	struct intf_hdl *pintfhdl = &(pio_priv->intf);

	_write_port_cancel = pintfhdl->io_ops._write_port_cancel;

	RTW_DISABLE_FUNC(adapter, DF_TX_BIT);

	if (_write_port_cancel)
		_write_port_cancel(pintfhdl);
}
int rtw_init_io_priv(_adapter *padapter, void (*set_intf_ops)(_adapter *padapter, struct _io_ops *pops))
{
	struct io_priv	*piopriv = &padapter->iopriv;
	struct intf_hdl *pintf = &piopriv->intf;

	if (set_intf_ops == NULL)
		return _FAIL;

	piopriv->padapter = padapter;
	pintf->padapter = padapter;
	pintf->pintf_dev = adapter_to_dvobj(padapter);

	set_intf_ops(padapter, &pintf->io_ops);

	return _SUCCESS;
}

/* Continual I/O error + DBG_IO sniff matchers live in core/rtw_io_rest.c (W3-18). */

#ifdef DBG_IO
void dbg_rtw_reg_read_monitor(_adapter *adapter, u32 addr, u32 len, u32 val, const char *caller, const int line)
{
	if (match_read_sniff(adapter, addr, len, val)) {
		switch (len) {
		case 1:
			RTW_INFO("DBG_IO %s:%d read8(0x%04x) return 0x%02x\n"
				, caller, line, addr, val);
			break;
		case 2:
			RTW_INFO("DBG_IO %s:%d read16(0x%04x) return 0x%04x\n"
				, caller, line, addr, val);
			break;
		case 4:
			RTW_INFO("DBG_IO %s:%d read32(0x%04x) return 0x%08x\n"
				, caller, line, addr, val);
			break;
		default:
			RTW_INFO("DBG_IO %s:%d readN(0x%04x, %u)\n"
				, caller, line, addr, len);
		}
	}
}

void dbg_rtw_reg_write_monitor(_adapter *adapter, u32 addr, u32 len, u32 val, const char *caller, const int line)
{
	if (match_write_sniff(adapter, addr, len, val)) {
		switch (len) {
		case 1:
			RTW_INFO("DBG_IO %s:%d write8(0x%04x, 0x%02x)\n"
				, caller, line, addr, val);
			break;
		case 2:
			RTW_INFO("DBG_IO %s:%d write16(0x%04x, 0x%04x)\n"
				, caller, line, addr, val);
			break;
		case 4:
			RTW_INFO("DBG_IO %s:%d write32(0x%04x, 0x%08x)\n"
				, caller, line, addr, val);
			break;
		default:
			RTW_INFO("DBG_IO %s:%d rtw_writeN(0x%04x, %u)\n"
				, caller, line, addr, len);
		}
	}
}

u8 dbg_rtw_read8(_adapter *adapter, u32 addr, const char *caller, const int line)
{
	u8 val = _rtw_read8(adapter, addr);

	if (match_read_sniff(adapter, addr, 1, val)) {
		RTW_INFO("DBG_IO %s:%d rtw_read8(0x%04x) return 0x%02x\n"
			, caller, line, addr, val);
	}

	return val;
}

u16 dbg_rtw_read16(_adapter *adapter, u32 addr, const char *caller, const int line)
{
	u16 val = _rtw_read16(adapter, addr);

	if (match_read_sniff(adapter, addr, 2, val)) {
		RTW_INFO("DBG_IO %s:%d rtw_read16(0x%04x) return 0x%04x\n"
			, caller, line, addr, val);
	}

	return val;
}

u32 dbg_rtw_read32(_adapter *adapter, u32 addr, const char *caller, const int line)
{
	u32 val = _rtw_read32(adapter, addr);

	if (match_read_sniff(adapter, addr, 4, val)) {
		RTW_INFO("DBG_IO %s:%d rtw_read32(0x%04x) return 0x%08x\n"
			, caller, line, addr, val);
	}

	return val;
}

int dbg_rtw_write8(_adapter *adapter, u32 addr, u8 val, const char *caller, const int line)
{
	if (match_write_sniff(adapter, addr, 1, val)) {
		RTW_INFO("DBG_IO %s:%d rtw_write8(0x%04x, 0x%02x)\n"
			, caller, line, addr, val);
	}

	return _rtw_write8(adapter, addr, val);
}
int dbg_rtw_write16(_adapter *adapter, u32 addr, u16 val, const char *caller, const int line)
{
	if (match_write_sniff(adapter, addr, 2, val)) {
		RTW_INFO("DBG_IO %s:%d rtw_write16(0x%04x, 0x%04x)\n"
			, caller, line, addr, val);
	}

	return _rtw_write16(adapter, addr, val);
}
int dbg_rtw_write32(_adapter *adapter, u32 addr, u32 val, const char *caller, const int line)
{
	if (match_write_sniff(adapter, addr, 4, val)) {
		RTW_INFO("DBG_IO %s:%d rtw_write32(0x%04x, 0x%08x)\n"
			, caller, line, addr, val);
	}

	return _rtw_write32(adapter, addr, val);
}
int dbg_rtw_writeN(_adapter *adapter, u32 addr , u32 length , u8 *data, const char *caller, const int line)
{
	if (match_write_sniff(adapter, addr, length, 0)) {
		RTW_INFO("DBG_IO %s:%d rtw_writeN(0x%04x, %u)\n"
			, caller, line, addr, length);
	}

	return _rtw_writeN(adapter, addr, length, data);
}

#ifdef CONFIG_SDIO_HCI
u8 dbg_rtw_sd_f0_read8(_adapter *adapter, u32 addr, const char *caller, const int line)
{
	u8 val = _rtw_sd_f0_read8(adapter, addr);

#if 0
	if (match_read_sniff(adapter, addr, 1, val)) {
		RTW_INFO("DBG_IO %s:%d rtw_sd_f0_read8(0x%04x) return 0x%02x\n"
			, caller, line, addr, val);
	}
#endif

	return val;
}

#ifdef CONFIG_SDIO_INDIRECT_ACCESS
u8 dbg_rtw_sd_iread8(_adapter *adapter, u32 addr, const char *caller, const int line)
{
	u8 val = rtw_sd_iread8(adapter, addr);

	if (match_read_sniff(adapter, addr, 1, val)) {
		RTW_INFO("DBG_IO %s:%d rtw_sd_iread8(0x%04x) return 0x%02x\n"
			, caller, line, addr, val);
	}

	return val;
}

u16 dbg_rtw_sd_iread16(_adapter *adapter, u32 addr, const char *caller, const int line)
{
	u16 val = _rtw_sd_iread16(adapter, addr);

	if (match_read_sniff(adapter, addr, 2, val)) {
		RTW_INFO("DBG_IO %s:%d rtw_sd_iread16(0x%04x) return 0x%04x\n"
			, caller, line, addr, val);
	}

	return val;
}

u32 dbg_rtw_sd_iread32(_adapter *adapter, u32 addr, const char *caller, const int line)
{
	u32 val = _rtw_sd_iread32(adapter, addr);

	if (match_read_sniff(adapter, addr, 4, val)) {
		RTW_INFO("DBG_IO %s:%d rtw_sd_iread32(0x%04x) return 0x%08x\n"
			, caller, line, addr, val);
	}

	return val;
}

int dbg_rtw_sd_iwrite8(_adapter *adapter, u32 addr, u8 val, const char *caller, const int line)
{
	if (match_write_sniff(adapter, addr, 1, val)) {
		RTW_INFO("DBG_IO %s:%d rtw_sd_iwrite8(0x%04x, 0x%02x)\n"
			, caller, line, addr, val);
	}

	return _rtw_sd_iwrite8(adapter, addr, val);
}
int dbg_rtw_sd_iwrite16(_adapter *adapter, u32 addr, u16 val, const char *caller, const int line)
{
	if (match_write_sniff(adapter, addr, 2, val)) {
		RTW_INFO("DBG_IO %s:%d rtw_sd_iwrite16(0x%04x, 0x%04x)\n"
			, caller, line, addr, val);
	}

	return _rtw_sd_iwrite16(adapter, addr, val);
}
int dbg_rtw_sd_iwrite32(_adapter *adapter, u32 addr, u32 val, const char *caller, const int line)
{
	if (match_write_sniff(adapter, addr, 4, val)) {
		RTW_INFO("DBG_IO %s:%d rtw_sd_iwrite32(0x%04x, 0x%08x)\n"
			, caller, line, addr, val);
	}

	return _rtw_sd_iwrite32(adapter, addr, val);
}

#endif /* CONFIG_SDIO_INDIRECT_ACCESS */

#endif /* CONFIG_SDIO_HCI */

#endif
