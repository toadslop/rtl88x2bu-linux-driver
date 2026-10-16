// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-25 usb_set_intf_ops (symbol L1). */

struct _adapter;
struct _io_ops;

void usb_set_intf_ops(struct _adapter *padapter, struct _io_ops *pops)
{
	(void)padapter;
	(void)pops;
}
