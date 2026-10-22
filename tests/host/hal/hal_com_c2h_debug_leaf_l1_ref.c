// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stub for W4-56 PR1 c2h_defeature_dbg_hdl. */

struct _adapter;

int c2h_defeature_dbg_hdl(struct _adapter *adapter, unsigned char *data,
			  unsigned char len)
{
	(void)adapter;
	(void)data;
	(void)len;
	return 0;
}
