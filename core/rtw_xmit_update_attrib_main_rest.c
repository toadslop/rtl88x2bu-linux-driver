/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _RTW_XMIT_UPDATE_ATTRIB_MAIN_REST_C_

#include <drv_types.h>

#ifdef CONFIG_WMMPS_STA
/*
 * update_attrib_trigger_frame_info
 * For Station mode, if a specific TID of driver setting and an AP support uapsd function, the data
 * frame with corresponding TID will be a trigger frame when driver is in wmm power saving mode.
 */
void update_attrib_trigger_frame_info(_adapter *padapter, struct pkt_attrib *pattrib)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct qos_priv *pqospriv = &pmlmepriv->qospriv;
	u8 trigger_frame_en = 0;

	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE) == _TRUE) {
		if ((pwrpriv->pwr_mode == PS_MODE_MIN) || (pwrpriv->pwr_mode == PS_MODE_MAX)) {
			if ((pqospriv->uapsd_ap_supported) &&
			    ((pqospriv->uapsd_tid & BIT(pattrib->priority)) == _TRUE)) {
				trigger_frame_en = 1;
				RTW_INFO("[WMMPS]" FUNC_ADPT_FMT ": This is a Trigger Frame\n",
					 FUNC_ADPT_ARG(padapter));
			}
		}
	}

	pattrib->trigger_frame = trigger_frame_en;
}
#endif /* CONFIG_WMMPS_STA */
