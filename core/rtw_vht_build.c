/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_VHT_BUILD_C_

#ifdef HOST_VHT_BUILD_TEST
#include "host_vht_build_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#ifdef CONFIG_80211AC_VHT

#if !defined(CONFIG_RUST_VHT_BUILD) || defined(HOST_VHT_BUILD_TEST)

u32 rtw_build_vht_operation_ie(_adapter *padapter, u8 *pbuf, u8 channel)
{
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct vht_priv *pvhtpriv = &pmlmepriv->vhtpriv;
	u8 ChnlWidth, center_freq, bw_mode;
	u32 len = 0;
	u8 operation[5];

	_rtw_memset(operation, 0, 5);

	bw_mode = REGSTY_BW_5G(pregistrypriv);

	if (hal_chk_bw_cap(padapter, BW_CAP_80M | BW_CAP_160M)
	    && REGSTY_BW_5G(pregistrypriv) >= CHANNEL_WIDTH_80) {
		center_freq = rtw_get_center_ch(channel, bw_mode, HAL_PRIME_CHNL_OFFSET_LOWER);
		ChnlWidth = 1;
	} else {
		center_freq = 0;
		ChnlWidth = 0;
	}

	SET_VHT_OPERATION_ELE_CHL_WIDTH(operation, ChnlWidth);
	SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ1(operation, center_freq);
	SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ2(operation, 0);

	_rtw_memcpy(operation + 3, pvhtpriv->vht_mcs_map, 2);

	rtw_set_ie(pbuf, EID_VHTOperation, 5, operation, &len);

	return len;
}

#endif /* !CONFIG_RUST_VHT_BUILD || HOST_VHT_BUILD_TEST */

#endif /* CONFIG_80211AC_VHT */
