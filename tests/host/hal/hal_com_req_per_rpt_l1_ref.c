// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stub for W4-118 (body in rust/hal_com_req_per_rpt.rs). */

struct _adapter;

unsigned char rtw_hal_set_req_per_rpt_cmd(struct _adapter *adapter, unsigned char group_macid,
					  unsigned char rpt_type, unsigned int macid_bitmap)
{
	(void)adapter;
	(void)group_macid;
	(void)rpt_type;
	(void)macid_bitmap;
	return 0;
}