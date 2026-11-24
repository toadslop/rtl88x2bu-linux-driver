// SPDX-License-Identifier: GPL-2.0
/* Host fixture for W4-137 RCR_AM HW_VAR delegate L2. */

#include "host_hal_com_rcr_am_types.h"
#include <string.h>

struct _adapter {
	struct hal_data hal;
};

static struct _adapter g_adapter;
struct host_hal_rcr_am_trace host_hal_rcr_am_trace;

struct _adapter *host_hal_rcr_am_adapter(void)
{
	return &g_adapter;
}

void host_hal_rcr_am_reset(void)
{
	memset(&g_adapter, 0, sizeof(g_adapter));
	memset(&host_hal_rcr_am_trace, 0, sizeof(host_hal_rcr_am_trace));
}

u8 rtw_hal_rcr_add(struct _adapter *adapter, u32 add)
{
	(void)adapter;

	host_hal_rcr_am_trace.add_count++;
	host_hal_rcr_am_trace.last_add_mask = add;
	return _SUCCESS;
}

u8 rtw_hal_rcr_clear(struct _adapter *adapter, u32 clear)
{
	(void)adapter;

	host_hal_rcr_am_trace.clear_count++;
	host_hal_rcr_am_trace.last_clear_mask = clear;
	return _SUCCESS;
}