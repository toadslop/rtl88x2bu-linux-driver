// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-89 — mirrors hal/hal_dm.c record_ra_info + rtw_phydm_ops_func_init. */

#include "host_hal_dm_phydm_ops_types.h"

static void host_fill_desc_dyntxpwr_stub(void *dm, u8 *desc, u8 dpt_lv)
{
	(void)dm;
	(void)desc;
	(void)dpt_lv;
}

void record_ra_info(void *p_dm_void, u8 macid, struct cmn_sta_info *p_sta,
		    u64 ra_mask)
{
	struct dm_struct *p_dm = (struct dm_struct *)p_dm_void;
	host_adapter *adapter;
	struct dvobj_priv *dvobj;
	struct macid_ctl_t *macid_ctl;

	if (!p_sta)
		return;

	host_hal_dm_phydm_ops_trace.record_calls++;
	host_hal_dm_phydm_ops_trace.last_macid = macid;

	adapter = p_dm ? p_dm->adapter : NULL;
	if (!adapter)
		return;

	dvobj = adapter_to_dvobj(adapter);
	macid_ctl = dvobj_to_macidctl(dvobj);
	if (!macid_ctl)
		return;

	rtw_macid_ctl_set_bw(macid_ctl, macid, (u8)p_sta->ra_info.ra_bw_mode);
	rtw_macid_ctl_set_vht_en(macid_ctl, macid, p_sta->ra_info.is_vht_enable);
	rtw_macid_ctl_set_rate_bmp0(macid_ctl, macid, (u32)ra_mask);
	rtw_macid_ctl_set_rate_bmp1(macid_ctl, macid, (u32)(ra_mask >> 32));

	rtw_update_tx_rate_bmp(dvobj);
}

void rtw_phydm_ops_func_init(struct dm_struct *p_phydm)
{
	struct host_ra_table *p_ra_t;

	if (!p_phydm)
		return;

	host_hal_dm_phydm_ops_trace.init_called = 1;
	p_ra_t = &p_phydm->dm_ra_table;
	p_ra_t->record_ra_info = record_ra_info;
	p_phydm->fill_desc_dyntxpwr = host_fill_desc_dyntxpwr_stub;
	host_hal_dm_phydm_ops_trace.record_ptr_set =
		(p_ra_t->record_ra_info == record_ra_info);
	host_hal_dm_phydm_ops_trace.fill_desc_ptr_set =
		(p_phydm->fill_desc_dyntxpwr == host_fill_desc_dyntxpwr_stub);
}