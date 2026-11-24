// SPDX-License-Identifier: GPL-2.0
/* Production C slice for W4-129 L2 — mirrors hal/hal_com_rest.c. */

#include "host_hal_com_tsf_types.h"

#define rtw_hal_set_tsf_update rtw_rust_hal_com_tsf_hal_set_tsf_update

void rtw_iface_enable_tsf_update(struct _adapter *adapter)
{
	adapter->mlmeextpriv.tsf_update_pause_stime = 0;
	adapter->mlmeextpriv.tsf_update_required = 1;
#ifndef CONFIG_MI_WITH_MBSSID_CAM
	rtw_hal_set_tsf_update(adapter, 1);
#endif
}

void rtw_iface_disable_tsf_update(struct _adapter *adapter)
{
	adapter->mlmeextpriv.tsf_update_required = 0;
	adapter->mlmeextpriv.tsf_update_pause_stime = 0;
	adapter->mlmeextpriv.en_hw_update_tsf = 0;
#ifndef CONFIG_MI_WITH_MBSSID_CAM
	rtw_hal_set_tsf_update(adapter, 0);
#endif
}