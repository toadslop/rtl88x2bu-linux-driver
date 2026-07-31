/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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
#define _RTW_VHT_REST_C_

#ifdef HOST_VHT_RESTRUCTURE_TEST
#include "host_vht_rest.h"
#elif defined(HOST_VHT_TEST)
#include <stdint.h>
typedef uint8_t u8;
#else
#include <drv_types.h>
#endif

#ifdef CONFIG_80211AC_VHT

#if (!defined(CONFIG_RUST) || defined(HOST_VHT_TEST)) && \
	(!defined(HOST_VHT_RESTRUCTURE_TEST) || defined(HOST_VHT_TEST))

void rtw_vht_nss_to_mcsmap(u8 nss, u8 *target_mcs_map, u8 *cur_mcs_map)
{
	u8	i, j;
	u8	cur_rate, target_rate;

	for (i = 0; i < 2; i++) {
		target_mcs_map[i] = 0;
		for (j = 0; j < 8; j += 2) {
			cur_rate = (cur_mcs_map[i] >> j) & 3;
			if (cur_rate == 3) /* 0x3 indicates not supported that num of SS */
				target_rate = 3;
			else if (nss <= ((j / 2) + i * 4))
				target_rate = 3;
			else
				target_rate = cur_rate;

			target_mcs_map[i] |= (target_rate << j);
		}
	}
}

#ifdef ROKU_PRIVATE
u8 VHT_get_ss_from_map(u8 *vht_mcs_map)
{
	u8 i, j;
	u8 ss = 0;

	for (i = 0; i < 2; i++) {
		if (vht_mcs_map[i] != 0xff) {
			for (j = 0; j < 8; j += 2) {
				if (((vht_mcs_map[i] >> j) & 0x03) == 0x03)
					break;
				ss++;
			}
		}
	}

	return ss;
}
#endif /* ROKU_PRIVATE */

#endif /* nss C oracle guard */

#if !defined(HOST_VHT_TEST) || defined(HOST_VHT_RESTRUCTURE_TEST)

u32 rtw_restructure_vht_ie(_adapter *padapter, u8 *in_ie, u8 *out_ie, uint in_len, uint *pout_len)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(padapter);
	RT_CHANNEL_INFO *chset = rfctl->channel_set;
	u32	ielen;
	u8 max_bw;
	u8 oper_ch, oper_bw = CHANNEL_WIDTH_20, oper_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	u8 *out_vht_op_ie, *ht_op_ie, *vht_cap_ie, *vht_op_ie;
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct vht_priv	*pvhtpriv = &pmlmepriv->vhtpriv;

	rtw_vht_use_default_setting(padapter);

	ht_op_ie = rtw_get_ie(in_ie + 12, WLAN_EID_HT_OPERATION, &ielen, in_len - 12);
	if (!ht_op_ie || ielen != HT_OP_IE_LEN)
		goto exit;
	vht_cap_ie = rtw_get_ie(in_ie + 12, EID_VHTCapability, &ielen, in_len - 12);
	if (!vht_cap_ie || ielen != VHT_CAP_IE_LEN)
		goto exit;
	vht_op_ie = rtw_get_ie(in_ie + 12, EID_VHTOperation, &ielen, in_len - 12);
	if (!vht_op_ie || ielen != VHT_OP_IE_LEN)
		goto exit;

	/* VHT Capabilities element */
	*pout_len += rtw_build_vht_cap_ie(padapter, out_ie + *pout_len);

	/* VHT Operation element */
	out_vht_op_ie = out_ie + *pout_len;
	rtw_set_ie(out_vht_op_ie, EID_VHTOperation, VHT_OP_IE_LEN, vht_op_ie + 2 , pout_len);

	/* get primary channel from HT_OP_IE */
	oper_ch = GET_HT_OP_ELE_PRI_CHL(ht_op_ie + 2);

	/* find the largest bw supported by both registry and hal */
	max_bw = hal_largest_bw(padapter, REGSTY_BW_5G(pregistrypriv));

	if (max_bw >= CHANNEL_WIDTH_40) {
		/* get bw offset form HT_OP_IE */
		if (GET_HT_OP_ELE_STA_CHL_WIDTH(ht_op_ie + 2)) {
			switch (GET_HT_OP_ELE_2ND_CHL_OFFSET(ht_op_ie + 2)) {
			case SCA:
				oper_bw = CHANNEL_WIDTH_40;
				oper_offset = HAL_PRIME_CHNL_OFFSET_LOWER;
				break;
			case SCB:
				oper_bw = CHANNEL_WIDTH_40;
				oper_offset = HAL_PRIME_CHNL_OFFSET_UPPER;
				break;
			}
		}

		if (oper_bw == CHANNEL_WIDTH_40) {
			switch (GET_VHT_OPERATION_ELE_CHL_WIDTH(vht_op_ie + 2)) {
			case 1: /* 80MHz */
			case 2: /* 160MHz */
			case 3: /* 80+80 */
				oper_bw = CHANNEL_WIDTH_80; /* only support up to 80MHz for now */
				break;
			}

			oper_bw = rtw_min(oper_bw, max_bw);

			/* try downgrage bw to fit in channel plan setting */
			while (!rtw_chset_is_chbw_valid(chset, oper_ch, oper_bw, oper_offset, 1, 1)
				|| (IS_DFS_SLAVE_WITH_RD(rfctl)
					&& !rtw_rfctl_dfs_domain_unknown(rfctl)
					&& rtw_chset_is_chbw_non_ocp(chset, oper_ch, oper_bw, oper_offset))
			) {
				oper_bw--;
				if (oper_bw == CHANNEL_WIDTH_20) {
					oper_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
					break;
				}
			}
		}
	}

	rtw_warn_on(!rtw_chset_is_chbw_valid(chset, oper_ch, oper_bw, oper_offset, 1, 1));
	if (IS_DFS_SLAVE_WITH_RD(rfctl) && !rtw_rfctl_dfs_domain_unknown(rfctl))
		rtw_warn_on(rtw_chset_is_chbw_non_ocp(chset, oper_ch, oper_bw, oper_offset));

	/* update VHT_OP_IE */
	if (oper_bw < CHANNEL_WIDTH_80) {
		SET_VHT_OPERATION_ELE_CHL_WIDTH(out_vht_op_ie + 2, 0);
		SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ1(out_vht_op_ie + 2, 0);
		SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ2(out_vht_op_ie + 2, 0);
	} else if (oper_bw == CHANNEL_WIDTH_80) {
		u8 cch = rtw_get_center_ch(oper_ch, oper_bw, oper_offset);

		SET_VHT_OPERATION_ELE_CHL_WIDTH(out_vht_op_ie + 2, 1);
		SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ1(out_vht_op_ie + 2, cch);
		SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ2(out_vht_op_ie + 2, 0);
	} else {
		RTW_ERR(FUNC_ADPT_FMT" unsupported BW:%u\n", FUNC_ADPT_ARG(padapter), oper_bw);
		rtw_warn_on(1);
	}

	/* Operating Mode Notification element */
	*pout_len += rtw_build_vht_op_mode_notify_ie(padapter, out_ie + *pout_len, oper_bw);

	pvhtpriv->vht_option = _TRUE;

exit:
	return pvhtpriv->vht_option;
}

#endif /* !HOST_VHT_TEST || HOST_VHT_RESTRUCTURE_TEST */

#endif /* CONFIG_80211AC_VHT */
