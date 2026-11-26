// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-134 MSR get/set (hal_com_rest.c HALMAC path). */

#include "host_hal_com_msr_types.h"

static u8 rtw_hal_net_type_decision_c_oracle(struct _adapter *adapter, u8 net_type)
{
#if defined(CONFIG_MI_WITH_MBSSID_CAM) && defined(CONFIG_MBSSID_CAM)
	if ((adapter->hw_port == HW_PORT0) && rtw_get_mbid_cam_entry_num(adapter)) {
		if (net_type != _HW_STATE_NOLINK_)
			return _HW_STATE_AP_;
	}
#endif
	return net_type;
}

void rtw_hal_get_msr_c_oracle(struct _adapter *adapter, u8 *net_type)
{
#ifdef RTW_HALMAC
	rtw_halmac_get_network_type(adapter_to_dvobj(adapter), adapter->hw_port, net_type);
#else
	(void)adapter;
	(void)net_type;
#endif
}

void rtw_hal_set_msr_c_oracle(struct _adapter *adapter, u8 net_type)
{
#ifdef RTW_HALMAC
#if defined(CONFIG_MI_WITH_MBSSID_CAM) && defined(CONFIG_MBSSID_CAM)
	net_type = rtw_hal_net_type_decision_c_oracle(adapter, net_type);
#endif
	rtw_halmac_set_network_type(adapter_to_dvobj(adapter), adapter->hw_port, net_type);
#else
	(void)adapter;
	(void)net_type;
#endif
}