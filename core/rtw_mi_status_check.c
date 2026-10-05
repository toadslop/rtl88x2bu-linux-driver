// SPDX-License-Identifier: GPL-2.0
#define _RTW_MI_STATUS_CHECK_C_

#ifdef HOST_MI_STATUS_TEST
#include "host_mi_status_types.h"
#define RTW_INFO(...) ((void)0)
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

void rtw_mi_update_iface_status(struct mlme_priv *pmlmepriv, sint state)
{
	_adapter *adapter = container_of(pmlmepriv, _adapter, mlmepriv);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct mi_state *iface_state = &dvobj->iface_state;
	struct mi_state tmp_mstate;

	if ((u32)state == (u32)WIFI_MONITOR_STATE
		|| (u32)state == 0xFFFFFFFFU
	)
		return;

	rtw_mi_status(adapter, &tmp_mstate);
	_rtw_memcpy(iface_state, &tmp_mstate, sizeof(struct mi_state));
}

u8 rtw_mi_check_status(_adapter *adapter, u8 type)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct mi_state *iface_state = &dvobj->iface_state;
	u8 ret = _FALSE;

	switch (type) {
	case MI_LINKED:
		if (MSTATE_STA_LD_NUM(iface_state) || MSTATE_AP_NUM(iface_state) || MSTATE_ADHOC_NUM(iface_state) || MSTATE_MESH_NUM(iface_state))
			ret = _TRUE;
		break;
	case MI_ASSOC:
		if (MSTATE_STA_LD_NUM(iface_state) || MSTATE_AP_LD_NUM(iface_state) || MSTATE_ADHOC_LD_NUM(iface_state) || MSTATE_MESH_LD_NUM(iface_state))
			ret = _TRUE;
		break;
	case MI_UNDER_WPS:
		if (MSTATE_WPS_NUM(iface_state))
			ret = _TRUE;
		break;

	case MI_AP_MODE:
		if (MSTATE_AP_NUM(iface_state))
			ret = _TRUE;
		break;
	case MI_AP_ASSOC:
		if (MSTATE_AP_LD_NUM(iface_state))
			ret = _TRUE;
		break;

	case MI_ADHOC:
		if (MSTATE_ADHOC_NUM(iface_state))
			ret = _TRUE;
		break;
	case MI_ADHOC_ASSOC:
		if (MSTATE_ADHOC_LD_NUM(iface_state))
			ret = _TRUE;
		break;

#ifdef CONFIG_RTW_MESH
	case MI_MESH:
		if (MSTATE_MESH_NUM(iface_state))
			ret = _TRUE;
		break;
	case MI_MESH_ASSOC:
		if (MSTATE_MESH_LD_NUM(iface_state))
			ret = _TRUE;
		break;
#endif

	case MI_STA_NOLINK:
		if (MSTATE_STA_NUM(iface_state) && (!(MSTATE_STA_LD_NUM(iface_state) || MSTATE_STA_LG_NUM(iface_state))))
			ret = _TRUE;
		break;
	case MI_STA_LINKED:
		if (MSTATE_STA_LD_NUM(iface_state))
			ret = _TRUE;
		break;
	case MI_STA_LINKING:
		if (MSTATE_STA_LG_NUM(iface_state))
			ret = _TRUE;
		break;

	default:
		break;
	}
	return ret;
}
