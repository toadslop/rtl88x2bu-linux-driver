// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-33 usb_read8/16/32 (extracted from hal/hal_hci/hal_usb.c). */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct intf_hdl;

u16 usb_vendor_wvalue_fixup(u16 wvalue);
int usbctrl_vendorreq(struct intf_hdl *pintfhdl, u8 request, u16 value, u16 index,
		      void *pdata, u16 len, u8 requesttype);

u8 usb_read8(struct intf_hdl *pintfhdl, u32 addr)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u8 data = 0;

	request = 0x05;
	requesttype = 0x01;
	index = 0;

	wvalue = (u16)(addr & 0x0000ffff);
	len = 1;
	wvalue = usb_vendor_wvalue_fixup(wvalue);

	usbctrl_vendorreq(pintfhdl, request, wvalue, index, &data, len, requesttype);

	return data;
}

u16 usb_read16(struct intf_hdl *pintfhdl, u32 addr)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u16 data = 0;

	request = 0x05;
	requesttype = 0x01;
	index = 0;

	wvalue = (u16)(addr & 0x0000ffff);
	len = 2;
	wvalue = usb_vendor_wvalue_fixup(wvalue);

	usbctrl_vendorreq(pintfhdl, request, wvalue, index, &data, len, requesttype);

	return data;
}

u32 usb_read32(struct intf_hdl *pintfhdl, u32 addr)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u32 data = 0;

	request = 0x05;
	requesttype = 0x01;
	index = 0;

	wvalue = (u16)(addr & 0x0000ffff);
	len = 4;
	wvalue = usb_vendor_wvalue_fixup(wvalue);

	usbctrl_vendorreq(pintfhdl, request, wvalue, index, &data, len, requesttype);

	return data;
}
