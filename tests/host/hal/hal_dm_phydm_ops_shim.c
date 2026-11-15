// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-89 phydm ops init + record_ra_info L2 test. */

#include <string.h>
#include "host_hal_dm_phydm_ops_types.h"

struct host_hal_dm_phydm_ops_trace host_hal_dm_phydm_ops_trace;

void host_hal_dm_phydm_ops_reset(void)
{
	memset(&host_hal_dm_phydm_ops_trace, 0, sizeof(host_hal_dm_phydm_ops_trace));
}

struct dvobj_priv *adapter_to_dvobj(host_adapter *adapter)
{
	return adapter ? &adapter->dvobj : NULL;
}

struct macid_ctl_t *dvobj_to_macidctl(struct dvobj_priv *dvobj)
{
	return dvobj ? &dvobj->macid_ctl : NULL;
}

void rtw_macid_ctl_set_bw(struct macid_ctl_t *macid_ctl, u8 id, u8 bw)
{
	if (!macid_ctl || id >= macid_ctl->num)
		return;
	macid_ctl->bw[id] = bw;
	host_hal_dm_phydm_ops_trace.last_macid = id;
	host_hal_dm_phydm_ops_trace.last_bw = bw;
}

void rtw_macid_ctl_set_vht_en(struct macid_ctl_t *macid_ctl, u8 id, u8 en)
{
	if (!macid_ctl || id >= macid_ctl->num)
		return;
	macid_ctl->vht_en[id] = en;
	host_hal_dm_phydm_ops_trace.last_vht_en = en;
}

void rtw_macid_ctl_set_rate_bmp0(struct macid_ctl_t *macid_ctl, u8 id, u32 bmp)
{
	if (!macid_ctl || id >= macid_ctl->num)
		return;
	macid_ctl->rate_bmp0[id] = bmp;
	host_hal_dm_phydm_ops_trace.last_bmp0 = bmp;
}

void rtw_macid_ctl_set_rate_bmp1(struct macid_ctl_t *macid_ctl, u8 id, u32 bmp)
{
	if (!macid_ctl || id >= macid_ctl->num)
		return;
	macid_ctl->rate_bmp1[id] = bmp;
	host_hal_dm_phydm_ops_trace.last_bmp1 = bmp;
}

void rtw_update_tx_rate_bmp(struct dvobj_priv *dvobj)
{
	(void)dvobj;
	host_hal_dm_phydm_ops_trace.update_tx_rate_bmp_calls++;
}