// SPDX-License-Identifier: GPL-2.0
/* Host stubs for W3-230 IQK/LCK proc L2. */

#include "host_rtw_debug_proc_cal_types.h"

static unsigned g_iqk_calls;
static bool g_iqk_recovery, g_iqk_clear, g_iqk_segment;
static unsigned g_lck_calls;

void rtw_hal_iqk_test(struct _adapter *adapter, bool recovery, bool clear,
		      bool segment)
{
	(void)adapter;
	g_iqk_calls++;
	g_iqk_recovery = recovery;
	g_iqk_clear = clear;
	g_iqk_segment = segment;
}

void rtw_hal_lck_test(struct _adapter *adapter)
{
	(void)adapter;
	g_lck_calls++;
}

void host_debug_proc_cal_reset_log(void)
{
	g_iqk_calls = 0;
	g_lck_calls = 0;
	g_iqk_recovery = g_iqk_clear = g_iqk_segment = false;
}

unsigned host_debug_proc_cal_iqk_calls(void)
{
	return g_iqk_calls;
}

bool host_debug_proc_cal_iqk_last_recovery(void)
{
	return g_iqk_recovery;
}

bool host_debug_proc_cal_iqk_last_clear(void)
{
	return g_iqk_clear;
}

bool host_debug_proc_cal_iqk_last_segment(void)
{
	return g_iqk_segment;
}

unsigned host_debug_proc_cal_lck_calls(void)
{
	return g_lck_calls;
}
