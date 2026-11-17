// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-107 PHYDM RA registered hook L2. */

#include "host_hal_dm_phydm_ra_registed_types.h"

struct host_hal_dm_phydm_ra_registed_trace host_hal_dm_phydm_ra_registed_trace;

void host_hal_dm_phydm_ra_registed_reset(void)
{
	memset(&host_hal_dm_phydm_ra_registed_trace, 0,
	       sizeof(host_hal_dm_phydm_ra_registed_trace));
}

struct dvobj_priv *adapter_to_dvobj(_adapter *adapter)
{
	return adapter ? &adapter->dvobj : NULL;
}

struct macid_ctl_t *dvobj_to_macidctl(struct dvobj_priv *dvobj)
{
	return dvobj ? &dvobj->macid_ctl : NULL;
}

void phydm_ra_registed(void *dm_void, u8 macid, u8 rssi_from_assoc)
{
	(void)dm_void;
	host_hal_dm_phydm_ra_registed_trace.phydm_ra_calls++;
	host_hal_dm_phydm_ra_registed_trace.last_mac_id = macid;
	host_hal_dm_phydm_ra_registed_trace.last_rssi = rssi_from_assoc;
}

void dump_sta_info(void *sel, struct sta_info *psta)
{
	(void)sel;
	(void)psta;
	host_hal_dm_phydm_ra_registed_trace.dump_sta_calls++;
}