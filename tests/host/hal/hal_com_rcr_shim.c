// SPDX-License-Identifier: GPL-2.0
/* Host fixture for W4-127 RCR leaf L2. */

#include "host_hal_com_rcr_types.h"
#include <string.h>

struct _adapter {
	struct hal_data hal;
};

static struct _adapter g_adapter;
static u32 g_reg_rcr;
static u8 g_set_fail;
struct host_hal_rcr_trace host_hal_rcr_trace;

struct hal_data *host_hal_rcr_hal_data(struct _adapter *adapter)
{
	return &adapter->hal;
}

void host_hal_rcr_reset(void)
{
	memset(&g_adapter, 0, sizeof(g_adapter));
	memset(&host_hal_rcr_trace, 0, sizeof(host_hal_rcr_trace));
	g_reg_rcr = 0;
	g_set_fail = 0;
}

struct _adapter *host_hal_rcr_adapter(void)
{
	return &g_adapter;
}

void host_hal_rcr_set_cached(u32 rcr)
{
	GET_HAL_DATA(&g_adapter)->ReceiveConfig = rcr;
}

void host_hal_rcr_set_register(u32 rcr)
{
	g_reg_rcr = rcr;
}

void host_hal_rcr_set_write_fail(u8 fail)
{
	g_set_fail = fail;
}

void rtw_hal_get_hwreg(struct _adapter *padapter, u32 variable, u8 *val)
{
	(void)padapter;

	if (variable != HW_VAR_RCR)
		return;

	host_hal_rcr_trace.get_count++;
	/* hw_var_rcr_get: always refresh ReceiveConfig from REG_RCR. */
	GET_HAL_DATA(&g_adapter)->ReceiveConfig = g_reg_rcr;
	host_hal_rcr_trace.last_get_rcr = g_reg_rcr;
	if (val)
		*(u32 *)val = g_reg_rcr;
}

u8 rtw_hal_set_hwreg(struct _adapter *padapter, u32 variable, u8 *val)
{
	u32 rcr;

	(void)padapter;
	if (variable != HW_VAR_RCR || !val)
		return _FAIL;

	host_hal_rcr_trace.set_count++;
	rcr = *(u32 *)val;
	host_hal_rcr_trace.last_set_rcr = rcr;

	if (g_set_fail)
		return _FAIL;

	/* hw_var_rcr_config: update cache only after a successful register write. */
	g_reg_rcr = rcr;
	GET_HAL_DATA(&g_adapter)->ReceiveConfig = rcr;
	return _SUCCESS;
}

void rtw_rust_hal_com_get_hwreg(void *adapter, u32 variable, u8 *val)
{
	rtw_hal_get_hwreg(adapter, variable, val);
}

u8 rtw_rust_hal_com_set_hwreg_u8(void *adapter, u32 variable, u8 *val)
{
	return rtw_hal_set_hwreg(adapter, variable, val);
}

int rtw_rust_hal_com_hw_var_rcr(void)
{
	return HW_VAR_RCR;
}

u32 host_hal_rcr_receive_config(struct _adapter *adapter)
{
	return GET_HAL_DATA(adapter)->ReceiveConfig;
}

u32 rtw_rust_hal_com_receive_config(void *adapter)
{
	return host_hal_rcr_receive_config((struct _adapter *)adapter);
}