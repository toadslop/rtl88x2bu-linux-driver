// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: FW media status H2C exports (W4-139). */

struct _adapter;

typedef int s32;
typedef unsigned char u8;
typedef _Bool bool;

s32 rtw_hal_set_FwMediaStatusRpt_cmd(struct _adapter *adapter, bool opmode,
				     bool miracast, bool miracast_sink, u8 role,
				     u8 macid, bool macid_ind, u8 macid_end)
{
	(void)adapter;
	(void)opmode;
	(void)miracast;
	(void)miracast_sink;
	(void)role;
	(void)macid;
	(void)macid_ind;
	(void)macid_end;
	return 0;
}

s32 rtw_hal_set_FwMediaStatusRpt_single_cmd(struct _adapter *adapter,
					    bool opmode, bool miracast,
					    bool miracast_sink, u8 role,
					    u8 macid)
{
	(void)adapter;
	(void)opmode;
	(void)miracast;
	(void)miracast_sink;
	(void)role;
	(void)macid;
	return 0;
}

s32 rtw_hal_set_FwMediaStatusRpt_range_cmd(struct _adapter *adapter,
					   bool opmode, bool miracast,
					   bool miracast_sink, u8 role,
					   u8 macid, u8 macid_end)
{
	(void)adapter;
	(void)opmode;
	(void)miracast;
	(void)miracast_sink;
	(void)role;
	(void)macid;
	(void)macid_end;
	return 0;
}