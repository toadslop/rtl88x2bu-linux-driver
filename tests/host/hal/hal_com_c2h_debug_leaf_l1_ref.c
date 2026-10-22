// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stubs for W4-56 C2H defeature + customer-string (PR2 partial). */

struct _adapter;

int c2h_defeature_dbg_hdl(struct _adapter *adapter, unsigned char *data,
			  unsigned char len)
{
	(void)adapter;
	(void)data;
	(void)len;
	return 0;
}

long rtw_hal_h2c_customer_str_req(struct _adapter *adapter)
{
	(void)adapter;
	return 0;
}

int c2h_customer_str_rpt_hdl(struct _adapter *adapter, unsigned char *data,
			     unsigned char len)
{
	(void)adapter;
	(void)data;
	(void)len;
	return 0;
}
