// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-109 HALMAC DM leaf delegate L2. */

#include "host_hal_dm_halmac_dm_leaf_types.h"

struct host_hal_dm_halmac_dm_leaf_trace host_hal_dm_halmac_dm_leaf_trace;

void host_hal_dm_halmac_dm_leaf_reset(void)
{
	memset(&host_hal_dm_halmac_dm_leaf_trace, 0,
	       sizeof(host_hal_dm_halmac_dm_leaf_trace));
}

int rtw_halmac_rfe_ctrl_cfg(struct dvobj_host *d, u8 gpio)
{
	(void)d;
	host_hal_dm_halmac_dm_leaf_trace.rfe_calls++;
	host_hal_dm_halmac_dm_leaf_trace.last_gpio = gpio;
	return (int)host_hal_dm_halmac_dm_leaf_trace.rfe_stub_return;
}

int rtw_halmac_iqk(struct dvobj_host *d, u8 clear, u8 segment)
{
	(void)d;
	host_hal_dm_halmac_dm_leaf_trace.iqk_calls++;
	host_hal_dm_halmac_dm_leaf_trace.last_clear = clear;
	host_hal_dm_halmac_dm_leaf_trace.last_segment = segment;
	return (int)host_hal_dm_halmac_dm_leaf_trace.iqk_stub_return;
}