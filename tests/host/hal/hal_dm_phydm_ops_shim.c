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

#if defined(CONFIG_SUPPORT_DYNAMIC_TXPWR) && defined(RUST_HAL_DM_PHYDM_OPS_ORACLE)
u8 rtw_rust_hal_dm_get_chip_type(host_adapter *adapter)
{
	(void)adapter;
	return 0;
}

void rtw_rust_hal_dm_set_tx_desc_txpwr_ofset_8822b(u8 *desc, u8 value)
{
	(void)desc;
	(void)value;
}

void rtw_rust_hal_dm_log_dyntxpwr_unsupported(void)
{
}

struct dm_struct;

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(host_adapter *adapter)
{
	(void)adapter;
	return NULL;
}

void rtw_rust_hal_dm_odm_set_dyntxpwr(void *dm, u8 *desc, u8 mac_id)
{
	(void)dm;
	(void)desc;
	(void)mac_id;
}
#endif

struct rtw_hal_dm_sta_ra_fields {
	u8 ra_bw_mode;
	u8 is_vht_enable;
};

void *rtw_rust_hal_dm_phydm_adapter(struct dm_struct *dm)
{
	return dm ? dm->adapter : NULL;
}

void *rtw_rust_hal_dm_ra_table(struct dm_struct *dm)
{
	return dm ? &dm->dm_ra_table : NULL;
}

void rtw_rust_hal_dm_ra_table_set_record_ra(struct host_ra_table *ra,
	void (*fn)(void *, u8, struct cmn_sta_info *, u64))
{
	if (ra)
		ra->record_ra_info = fn;
}

void rtw_rust_hal_dm_set_fill_desc_dyntxpwr(struct dm_struct *dm,
	void (*fn)(void *, u8 *, u8))
{
	if (dm)
		dm->fill_desc_dyntxpwr = fn;
}

void rtw_rust_hal_dm_sta_ra_fields(struct cmn_sta_info *sta,
	struct rtw_hal_dm_sta_ra_fields *out)
{
	if (!sta || !out)
		return;

	out->ra_bw_mode = (u8)sta->ra_info.ra_bw_mode;
	out->is_vht_enable = sta->ra_info.is_vht_enable;
}

struct dvobj_priv *rtw_rust_hal_dm_adapter_to_dvobj(host_adapter *adapter)
{
	return adapter_to_dvobj(adapter);
}

struct macid_ctl_t *rtw_rust_hal_dm_dvobj_to_macidctl(struct dvobj_priv *dvobj)
{
	return dvobj_to_macidctl(dvobj);
}