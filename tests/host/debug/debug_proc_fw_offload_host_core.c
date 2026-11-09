// SPDX-License-Identifier: GPL-2.0
/* Host stubs for W3-228 fw_offload proc L2. */

#include "host_rtw_debug_proc_fw_offload_types.h"

static unsigned g_thread_cmd_count;

void rtw_run_in_thread_cmd(void *adapter, void *func, void *ctx)
{
	(void)adapter;
	(void)func;
	(void)ctx;
	g_thread_cmd_count++;
}

unsigned host_fw_offload_thread_cmd_count(void)
{
	return g_thread_cmd_count;
}

void host_fw_offload_reset_thread_cmd_count(void)
{
	g_thread_cmd_count = 0;
}
