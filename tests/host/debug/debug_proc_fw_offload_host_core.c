// SPDX-License-Identifier: GPL-2.0
/* Host stubs for W3-228 fw_offload + W3-229 fw_tbtt_rpt proc L2. */

#include "host_rtw_debug_proc_fw_offload_types.h"

static unsigned g_thread_cmd_count;
static unsigned g_hal_bcn_offload_cmd_count;

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

void rtw_hal_set_fw_ap_bcn_offload_cmd(void *adapter, int fw_bcn_en,
				       u8 tbtt_rpt_map)
{
	(void)adapter;
	g_hal_bcn_offload_cmd_count++;
	(void)fw_bcn_en;
	(void)tbtt_rpt_map;
}

unsigned host_fw_tbtt_hal_cmd_count(void)
{
	return g_hal_bcn_offload_cmd_count;
}

void host_fw_tbtt_reset_hal_cmd_count(void)
{
	g_hal_bcn_offload_cmd_count = 0;
}
