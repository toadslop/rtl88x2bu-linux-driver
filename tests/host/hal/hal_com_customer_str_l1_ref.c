// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stubs for W4-86 customer string sync API (body in rust/hal_com_customer_str.rs). */

struct _adapter;

long rtw_hal_customer_str_read(struct _adapter *adapter, unsigned char *cs)
{
	(void)adapter;
	(void)cs;
	return 0;
}

long rtw_hal_h2c_customer_str_write(struct _adapter *adapter, const unsigned char *cs)
{
	(void)adapter;
	(void)cs;
	return 0;
}

long rtw_hal_customer_str_write(struct _adapter *adapter, const unsigned char *cs)
{
	(void)adapter;
	(void)cs;
	return 0;
}