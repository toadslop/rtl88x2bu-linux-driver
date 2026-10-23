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

int c2h_customer_str_rpt_2_hdl(struct _adapter *adapter, unsigned char *data,
			       unsigned char len)
{
	(void)adapter;
	(void)data;
	(void)len;
	return 0;
}

int hal_read_mac_hidden_rpt(struct _adapter *adapter)
{
	(void)adapter;
	return 0;
}

<<<<<<< HEAD
int c2h_mac_hidden_rpt_hdl(struct _adapter *adapter, unsigned char *data, unsigned char len)
=======
int c2h_per_rate_rpt_hdl(struct _adapter *adapter, unsigned char *data,
			 unsigned char len)
>>>>>>> 82470a33 (rust(hal): port c2h_lps_status_rpt (W4-58 PR2))
{
	(void)adapter;
	(void)data;
	(void)len;
	return 0;
}

<<<<<<< HEAD
int c2h_mac_hidden_rpt_2_hdl(struct _adapter *adapter, unsigned char *data, unsigned char len)
=======
int c2h_lps_status_rpt(struct _adapter *adapter, unsigned char *data,
		       unsigned char len)
>>>>>>> 82470a33 (rust(hal): port c2h_lps_status_rpt (W4-58 PR2))
{
	(void)adapter;
	(void)data;
	(void)len;
	return 0;
}
<<<<<<< HEAD

=======
>>>>>>> 82470a33 (rust(hal): port c2h_lps_status_rpt (W4-58 PR2))
