// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-89 — mirrors hal/hal_dm.c record_ra_info + rtw_phydm_ops_func_init. */

#include "host_hal_dm_phydm_ops_types.h"

#ifdef CONFIG_SUPPORT_DYNAMIC_TXPWR
void rtw_phydm_fill_desc_dpt(void *dm, u8 *desc, u8 dpt_lv)
{
	(void)dm;
	(void)desc;
	(void)dpt_lv;
}
#endif

void record_ra_info(void *p_dm_void, u8 macid, struct cmn_sta_info *p_sta,
		    u64 ra_mask)
{
	struct dm_struct *p_dm = (struct dm_struct *)p_dm_void;
	host_adapter *adapter = p_dm->adapter;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct macid_ctl_t *macid_ctl = dvobj_to_macidctl(dvobj);

	if (p_sta) {
		host_hal_dm_phydm_ops_trace.record_calls++;
		host_hal_dm_phydm_ops_trace.last_macid = macid;

		rtw_macid_ctl_set_bw(macid_ctl, macid, (u8)p_sta->ra_info.ra_bw_mode);
		rtw_macid_ctl_set_vht_en(macid_ctl, macid, p_sta->ra_info.is_vht_enable);
		rtw_macid_ctl_set_rate_bmp0(macid_ctl, macid, (u32)ra_mask);
		rtw_macid_ctl_set_rate_bmp1(macid_ctl, macid, (u32)(ra_mask >> 32));

		rtw_update_tx_rate_bmp(dvobj);
	}
}

void rtw_phydm_ops_func_init(struct dm_struct *p_phydm)
{
	struct host_ra_table *p_ra_t = &p_phydm->dm_ra_table;

	host_hal_dm_phydm_ops_trace.init_called = 1;
	p_ra_t->record_ra_info = record_ra_info;
#ifdef CONFIG_SUPPORT_DYNAMIC_TXPWR
	p_phydm->fill_desc_dyntxpwr = rtw_phydm_fill_desc_dpt;
	host_hal_dm_phydm_ops_trace.fill_desc_ptr_set =
		(p_phydm->fill_desc_dyntxpwr == rtw_phydm_fill_desc_dpt);
#else
	host_hal_dm_phydm_ops_trace.fill_desc_ptr_set = 1;
#endif
	host_hal_dm_phydm_ops_trace.record_ptr_set =
		(p_ra_t->record_ra_info == record_ra_info);
}