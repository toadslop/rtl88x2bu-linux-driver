// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stubs for W4-50 C2H IQK / txpwr offload (body in rust/hal_com.rs). */

struct _adapter;

void c2h_iqk_offload(struct _adapter *adapter, unsigned char *data, unsigned char len)
{
	(void)adapter;
	(void)data;
	(void)len;
}

int c2h_iqk_offload_wait(struct _adapter *adapter, unsigned int timeout_ms)
{
	(void)adapter;
	(void)timeout_ms;
	return 0;
}
