// SPDX-License-Identifier: GPL-2.0
/* Host fixture + FFI for W4-129 TSF iface enable/disable L2. */

#include "host_hal_com_tsf_types.h"
#include <string.h>

static struct _adapter g_adapter;
struct host_hal_com_tsf_trace host_hal_com_tsf_trace;

void host_hal_com_tsf_reset(void)
{
	memset(&g_adapter, 0, sizeof(g_adapter));
	memset(&host_hal_com_tsf_trace, 0, sizeof(host_hal_com_tsf_trace));
}

struct _adapter *host_hal_com_tsf_adapter(void)
{
	return &g_adapter;
}

void rtw_rust_hal_com_tsf_set_pause_stime(void *adapter, systime stime)
{
	((struct _adapter *)adapter)->mlmeextpriv.tsf_update_pause_stime = stime;
}

void rtw_rust_hal_com_tsf_set_update_required(void *adapter, u8 v)
{
	((struct _adapter *)adapter)->mlmeextpriv.tsf_update_required = v;
}

void rtw_rust_hal_com_tsf_set_en_hw_update(void *adapter, u8 v)
{
	((struct _adapter *)adapter)->mlmeextpriv.en_hw_update_tsf = v;
}

u8 rtw_rust_hal_com_tsf_update_required(void *adapter)
{
	return ((struct _adapter *)adapter)->mlmeextpriv.tsf_update_required;
}

u8 rtw_rust_hal_com_tsf_en_hw_update(void *adapter)
{
	return ((struct _adapter *)adapter)->mlmeextpriv.en_hw_update_tsf;
}

systime rtw_rust_hal_com_tsf_pause_stime(void *adapter)
{
	return ((struct _adapter *)adapter)->mlmeextpriv.tsf_update_pause_stime;
}

void rtw_rust_hal_com_tsf_hal_set_tsf_update(void *adapter, u8 en)
{
	(void)adapter;
	host_hal_com_tsf_trace.set_tsf_update_count++;
	host_hal_com_tsf_trace.last_set_tsf_en = en;
}