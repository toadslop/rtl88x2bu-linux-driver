// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-110 cfg_phy_para L2. */

#include "host_hal_dm_cfg_phy_para_types.h"

struct host_hal_dm_cfg_phy_para_trace host_hal_dm_cfg_phy_para_trace;

void host_hal_dm_cfg_phy_para_reset(void)
{
	memset(&host_hal_dm_cfg_phy_para_trace, 0,
	       sizeof(host_hal_dm_cfg_phy_para_trace));
}

int rtw_halmac_cfg_phy_para(struct dvobj_host *d, struct rtw_phy_parameter *para)
{
	(void)d;
	host_hal_dm_cfg_phy_para_trace.cfg_calls++;
	host_hal_dm_cfg_phy_para_trace.last_para = *para;
	return (int)host_hal_dm_cfg_phy_para_trace.stub_return;
}

struct dvobj_host *rtw_rust_hal_dm_adapter_to_dvobj(_adapter *adapter)
{
	return adapter ? &adapter->dvobj : NULL;
}

void *rtw_rust_hal_dm_phydm_adapter(struct dm_struct *dm)
{
	return dm ? dm->adapter : NULL;
}