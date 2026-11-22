// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-129 — hal/hal_com_rest.c rtw_iface_*_tsf_update. */

#include "host_hal_com_tsf_types.h"

static void hal_set_tsf_update_oracle(struct _adapter *adapter, u8 en)
{
	rtw_rust_hal_com_tsf_hal_set_tsf_update(adapter, en);
}

void rtw_iface_enable_tsf_update_c_oracle(struct _adapter *adapter)
{
	adapter->mlmeextpriv.tsf_update_pause_stime = 0;
	adapter->mlmeextpriv.tsf_update_required = 1;
#ifndef CONFIG_MI_WITH_MBSSID_CAM
	hal_set_tsf_update_oracle(adapter, 1);
#endif
}

void rtw_iface_disable_tsf_update_c_oracle(struct _adapter *adapter)
{
	adapter->mlmeextpriv.tsf_update_required = 0;
	adapter->mlmeextpriv.tsf_update_pause_stime = 0;
	adapter->mlmeextpriv.en_hw_update_tsf = 0;
#ifndef CONFIG_MI_WITH_MBSSID_CAM
	hal_set_tsf_update_oracle(adapter, 0);
#endif
}