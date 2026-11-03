// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-213 proc_tdls_display_network_info (host L2 PR1). */

#include "host_rtw_debug_proc_tdls_types.h"

int proc_tdls_display_network_info(struct seq_file *m)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = (struct _adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct wlan_network *cur_network = &pmlmepriv->cur_network;
	int i = 0;
	u8 SpaceBtwnItemAndValue = TDLS_DBG_INFO_SPACE_BTWN_ITEM_AND_VALUE;

	RTW_PRINT_SEL(m, "============[Associated AP/GO Info]============\n");

	if ((pmlmepriv->fw_state & WIFI_STATION_STATE) &&
	    (pmlmepriv->fw_state & WIFI_ASOC_STATE)) {
		RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "BSSID",
			      cur_network->network.Ssid.Ssid);
		RTW_PRINT_SEL(m, "%-*s = " MAC_FMT "\n", SpaceBtwnItemAndValue,
			      "Mac Address", MAC_ARG(cur_network->network.MacAddress));

		RTW_PRINT_SEL(m, "%-*s = ", SpaceBtwnItemAndValue, "Wireless Mode");
		for (i = 0; i < 8; i++) {
			if (pmlmeext->cur_wireless_mode & BIT(i)) {
				switch (BIT(i)) {
				case WIRELESS_11B:
					RTW_PRINT_SEL(m, "%4s", "11B ");
					break;
				case WIRELESS_11G:
					RTW_PRINT_SEL(m, "%4s", "11G ");
					break;
				case WIRELESS_11A:
					RTW_PRINT_SEL(m, "%4s", "11A ");
					break;
				case WIRELESS_11_24N:
					RTW_PRINT_SEL(m, "%7s", "11_24N ");
					break;
				case WIRELESS_11_5N:
					RTW_PRINT_SEL(m, "%6s", "11_5N ");
					break;
				case WIRELESS_AUTO:
					RTW_PRINT_SEL(m, "%5s", "AUTO ");
					break;
				case WIRELESS_11AC:
					RTW_PRINT_SEL(m, "%5s", "11AC ");
					break;
				default:
					break;
				}
			}
		}
		RTW_PRINT_SEL(m, "\n");

		RTW_PRINT_SEL(m, "%-*s = ", SpaceBtwnItemAndValue, "Privacy");
		switch (padapter->securitypriv.dot11PrivacyAlgrthm) {
		case _NO_PRIVACY_:
			RTW_PRINT_SEL(m, "%s\n", "NO PRIVACY");
			break;
		case _WEP40_:
			RTW_PRINT_SEL(m, "%s\n", "WEP 40");
			break;
		case _TKIP_:
			RTW_PRINT_SEL(m, "%s\n", "TKIP");
			break;
		case _TKIP_WTMIC_:
			RTW_PRINT_SEL(m, "%s\n", "TKIP WTMIC");
			break;
		case _AES_:
			RTW_PRINT_SEL(m, "%s\n", "AES");
			break;
		case _WEP104_:
			RTW_PRINT_SEL(m, "%s\n", "WEP 104");
			break;
		case _SMS4_:
			RTW_PRINT_SEL(m, "%s\n", "SMS4");
			break;
		default:
			RTW_PRINT_SEL(m, "(%u)\n", padapter->securitypriv.dot11PrivacyAlgrthm);
			break;
		}

		RTW_PRINT_SEL(m, "%-*s = %d\n", SpaceBtwnItemAndValue, "Channel",
			      pmlmeext->cur_channel);
		RTW_PRINT_SEL(m, "%-*s = ", SpaceBtwnItemAndValue, "Channel Offset");
		switch (pmlmeext->cur_ch_offset) {
		case HAL_PRIME_CHNL_OFFSET_DONT_CARE:
			RTW_PRINT_SEL(m, "%s\n", "N/A");
			break;
		case HAL_PRIME_CHNL_OFFSET_LOWER:
			RTW_PRINT_SEL(m, "%s\n", "Lower");
			break;
		case HAL_PRIME_CHNL_OFFSET_UPPER:
			RTW_PRINT_SEL(m, "%s\n", "Upper");
			break;
		default:
			RTW_PRINT_SEL(m, "(%u)\n", pmlmeext->cur_ch_offset);
			break;
		}

		RTW_PRINT_SEL(m, "%-*s = ", SpaceBtwnItemAndValue, "Bandwidth Mode");
		switch (pmlmeext->cur_bwmode) {
		case CHANNEL_WIDTH_20:
			RTW_PRINT_SEL(m, "%s\n", "20MHz");
			break;
		case CHANNEL_WIDTH_40:
			RTW_PRINT_SEL(m, "%s\n", "40MHz");
			break;
		case CHANNEL_WIDTH_80:
			RTW_PRINT_SEL(m, "%s\n", "80MHz");
			break;
		case CHANNEL_WIDTH_160:
			RTW_PRINT_SEL(m, "%s\n", "160MHz");
			break;
		case CHANNEL_WIDTH_80_80:
			RTW_PRINT_SEL(m, "%s\n", "80MHz + 80MHz");
			break;
		default:
			RTW_PRINT_SEL(m, "(%u)\n", pmlmeext->cur_bwmode);
			break;
		}
	} else {
		RTW_PRINT_SEL(m, "No association with AP/GO exists!\n");
	}

	return 0;
}
