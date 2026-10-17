// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-34 usb_write8/16/32/N (extracted from hal/hal_hci/hal_usb.c). */

#include <string.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct intf_hdl;

#define VENDOR_CMD_MAX_DATA_LEN 254

#define _rtw_memcpy(d, s, n) memcpy((d), (s), (n))

u16 usb_vendor_wvalue_fixup(u16 wvalue);
int usbctrl_vendorreq(struct intf_hdl *pintfhdl, u8 request, u16 value, u16 index,
		      void *pdata, u16 len, u8 requesttype);

int usb_write8(struct intf_hdl *pintfhdl, u32 addr, u8 val)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u8 data;
	int ret;

	request = 0x05;
	requesttype = 0x00;
	index = 0;

	wvalue = (u16)(addr & 0x0000ffff);
	len = 1;
	data = val;
	wvalue = usb_vendor_wvalue_fixup(wvalue);

	ret = usbctrl_vendorreq(pintfhdl, request, wvalue, index, &data, len, requesttype);

	return ret;
}

int usb_write16(struct intf_hdl *pintfhdl, u32 addr, u16 val)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u16 data;
	int ret;

	request = 0x05;
	requesttype = 0x00;
	index = 0;

	wvalue = (u16)(addr & 0x0000ffff);
	len = 2;
	data = val;
	wvalue = usb_vendor_wvalue_fixup(wvalue);

	ret = usbctrl_vendorreq(pintfhdl, request, wvalue, index, &data, len, requesttype);

	return ret;
}

int usb_write32(struct intf_hdl *pintfhdl, u32 addr, u32 val)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u32 data;
	int ret;

	request = 0x05;
	requesttype = 0x00;
	index = 0;

	wvalue = (u16)(addr & 0x0000ffff);
	len = 4;
	data = val;
	wvalue = usb_vendor_wvalue_fixup(wvalue);

	ret = usbctrl_vendorreq(pintfhdl, request, wvalue, index, &data, len, requesttype);

	return ret;
}

int usb_writeN(struct intf_hdl *pintfhdl, u32 addr, u32 length, u8 *pdata)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u8 buf[VENDOR_CMD_MAX_DATA_LEN] = {0};
	int ret;

	request = 0x05;
	requesttype = 0x00;
	index = 0;

	wvalue = (u16)(addr & 0x0000ffff);
	len = length;
	_rtw_memcpy(buf, pdata, len);
	ret = usbctrl_vendorreq(pintfhdl, request, wvalue, index, buf, len, requesttype);

	return ret;
}
