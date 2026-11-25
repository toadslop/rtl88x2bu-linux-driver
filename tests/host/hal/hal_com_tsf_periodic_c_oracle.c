// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-130 periodic TSF — hal/hal_com_rest.c mirror. */

#include "host_hal_com_tsf_periodic_types.h"

struct dvobj_priv *adapter_to_dvobj(struct _adapter *adapter);
struct _adapter *dvobj_get_primary_adapter(struct dvobj_priv *dvobj);
systime rtw_get_current_time(void);
systime rtw_ms_to_systime(int ms);
int rtw_time_after(systime a, systime b);
u32 rtw_get_passing_time_ms(systime start);
void _set_timer(struct _timer *t, u32 ms);
void rtw_hal_rcr_set_chk_bssid(struct _adapter *adapter, u8 self_action);
u8 rtw_periodic_tsf_update_end_cmd(struct _adapter *adapter);
int dev_is_surprise_removed(struct dvobj_priv *dvobj);
int dev_is_drv_stopped(struct dvobj_priv *dvobj);

static void periodic_chk_body(struct _adapter *adapter)
{
#ifdef CONFIG_MI_WITH_MBSSID_CAM
	(void)adapter;
#else
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct _adapter *iface;
	struct mlme_ext_priv *mlmeext;
	int i;
	u32 restore_ms = 0;

	if (dvobj->periodic_tsf_update_etime) {
		if (rtw_time_after(rtw_get_current_time(), dvobj->periodic_tsf_update_etime)) {
			dvobj->periodic_tsf_update_etime = 0;
			rtw_hal_rcr_set_chk_bssid(adapter, MLME_ACTION_NONE);
		}
		return;
	}

	if (dvobj->rf_ctl.offch_state != OFFCHS_NONE)
		return;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface)
			continue;

		mlmeext = &iface->mlmeextpriv;

		if (mlmeext->tsf_update_required
		    && mlmeext->tsf_update_pause_stime
		    && rtw_get_passing_time_ms(mlmeext->tsf_update_pause_stime)
			    > mlmeext->mlmext_info.bcn_interval * mlmeext->tsf_update_pause_factor
		) {
			if (restore_ms < mlmeext->mlmext_info.bcn_interval * mlmeext->tsf_update_restore_factor)
				restore_ms = mlmeext->mlmext_info.bcn_interval * mlmeext->tsf_update_restore_factor;
		}
	}

	if (!restore_ms)
		return;

	dvobj->periodic_tsf_update_etime = rtw_get_current_time() + rtw_ms_to_systime(restore_ms);
	if (!dvobj->periodic_tsf_update_etime)
		dvobj->periodic_tsf_update_etime++;

	rtw_hal_rcr_set_chk_bssid(adapter, MLME_ACTION_NONE);

	_set_timer(&dvobj->periodic_tsf_update_end_timer, restore_ms);
#endif
}

void rtw_hal_periodic_tsf_update_chk_c_oracle(struct _adapter *adapter)
{
	periodic_chk_body(adapter);
}

void rtw_hal_periodic_tsf_update_end_timer_hdl_c_oracle(void *ctx)
{
	struct dvobj_priv *dvobj = (struct dvobj_priv *)ctx;

	if (dev_is_surprise_removed(dvobj) || dev_is_drv_stopped(dvobj))
		return;

	rtw_periodic_tsf_update_end_cmd(dvobj_get_primary_adapter(dvobj));
}