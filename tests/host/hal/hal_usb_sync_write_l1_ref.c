// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stubs for W4-34 usb_write8/16/32/N (body in rust/hal_usb.rs). */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct intf_hdl;

int usb_write8(struct intf_hdl *pintfhdl, u32 addr, u8 val)
{
	(void)pintfhdl;
	(void)addr;
	(void)val;
	return 0;
}

int usb_write16(struct intf_hdl *pintfhdl, u32 addr, u16 val)
{
	(void)pintfhdl;
	(void)addr;
	(void)val;
	return 0;
}

int usb_write32(struct intf_hdl *pintfhdl, u32 addr, u32 val)
{
	(void)pintfhdl;
	(void)addr;
	(void)val;
	return 0;
}

int usb_writeN(struct intf_hdl *pintfhdl, u32 addr, u32 length, u8 *pdata)
{
	(void)pintfhdl;
	(void)addr;
	(void)length;
	(void)pdata;
	return 0;
}
