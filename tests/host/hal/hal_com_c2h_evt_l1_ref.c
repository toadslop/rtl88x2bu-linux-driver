// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stubs for W4-47 c2h_evt_* (body in rust/hal_com.rs). */

struct _adapter;

void c2h_evt_clear(struct _adapter *adapter)
{
	(void)adapter;
}

int c2h_evt_read_88xx(struct _adapter *adapter, unsigned char *buf)
{
	(void)adapter;
	(void)buf;
	return 0;
}
