// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stubs for W4-52 security CAM read/write (body in rust/hal_com.rs). */

struct _adapter;

unsigned int rtw_sec_read_cam(struct _adapter *adapter, unsigned char addr)
{
	(void)adapter;
	(void)addr;
	return 0;
}

void rtw_sec_write_cam(struct _adapter *adapter, unsigned char addr, unsigned int wdata)
{
	(void)adapter;
	(void)addr;
	(void)wdata;
}
