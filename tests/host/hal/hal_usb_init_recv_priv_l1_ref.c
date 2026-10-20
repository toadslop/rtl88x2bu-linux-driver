// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stub for W4-46 usb_init_recv_priv (body in rust/hal_usb.rs). */

struct _adapter;

int usb_init_recv_priv(struct _adapter *padapter, unsigned short ini_in_buf_sz)
{
	(void)padapter;
	(void)ini_in_buf_sz;
	return 1;
}
