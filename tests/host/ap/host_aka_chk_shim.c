// SPDX-License-Identifier: GPL-2.0
#include "host_ap_aka_chk_types.h"

static int host_nulldata_ret = _SUCCESS;
static int host_last_ps = -1;
static u8 host_nulldata_called;

void host_aka_chk_reset(void)
{
	host_nulldata_ret = _SUCCESS;
	host_last_ps = -1;
	host_nulldata_called = 0;
}

void host_aka_chk_set_nulldata_ret(int ret)
{
	host_nulldata_ret = ret;
}

int host_aka_chk_last_nulldata_ps(void)
{
	return host_last_ps;
}

u8 host_aka_chk_nulldata_called(void)
{
	return host_nulldata_called;
}

int issue_nulldata(_adapter *adapter, u8 *target_addr, int pwr, int ps, int retry)
{
	(void)adapter;
	(void)target_addr;
	(void)pwr;
	(void)retry;
	host_nulldata_called = 1;
	host_last_ps = ps;
	return host_nulldata_ret;
}
