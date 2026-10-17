// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stubs for W4-33 usb_read8/16/32 (body in rust/hal_usb.rs). */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct intf_hdl;

u8 usb_read8(struct intf_hdl *pintfhdl, u32 addr)
{
	(void)pintfhdl;
	(void)addr;
	return 0;
}

u16 usb_read16(struct intf_hdl *pintfhdl, u32 addr)
{
	(void)pintfhdl;
	(void)addr;
	return 0;
}

u32 usb_read32(struct intf_hdl *pintfhdl, u32 addr)
{
	(void)pintfhdl;
	(void)addr;
	return 0;
}
