// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-213 proc_tdls_display_tdls_sta_info (host L2 PR2). */

#include "host_rtw_debug_proc_tdls_types.h"

int proc_tdls_display_tdls_sta_info(struct seq_file *m)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = (struct _adapter *)rtw_netdev_priv(dev);
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta;
	int i = 0, j = 0;
	_irqL irqL;
	_list *plist, *phead;
	u8 SpaceBtwnItemAndValue = TDLS_DBG_INFO_SPACE_BTWN_ITEM_AND_VALUE;
	u8 SpaceBtwnItemAndValueTmp = 0;
	u8 NumOfTdlsStaToShow = 0;
	u8 FirstMatchFound = _FALSE;

	(void)padapter->tdlsinfo;

	_enter_critical_bh(&pstapriv->sta_hash_lock, &irqL);
	for (i = 0; i < NUM_STA; i++) {
		phead = &(pstapriv->sta_hash[i]);
		plist = get_next(phead);
		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);
			plist = get_next(plist);
			if (psta->tdls_sta_state != TDLS_STATE_NONE) {
				RTW_PRINT_SEL(m,
					      "============[TDLS Peer STA Info: STA %d]============\n",
					      ++NumOfTdlsStaToShow);
				RTW_PRINT_SEL(m, "%-*s = " MAC_FMT "\n",
					      SpaceBtwnItemAndValue, "Mac Address",
					      MAC_ARG(psta->cmn.mac_addr));
				RTW_PRINT_SEL(m, "%-*s =", SpaceBtwnItemAndValue,
					      "TDLS STA State");
				SpaceBtwnItemAndValueTmp = 0;
				FirstMatchFound = _FALSE;
				for (j = 0; j < 32; j++) {
					if (psta->tdls_sta_state & BIT(j)) {
						if (FirstMatchFound == _FALSE) {
							SpaceBtwnItemAndValueTmp = 1;
							FirstMatchFound = _TRUE;
						} else {
							SpaceBtwnItemAndValueTmp =
								SpaceBtwnItemAndValue + 3;
						}
						switch (BIT(j)) {
						case TDLS_INITIATOR_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n",
								      SpaceBtwnItemAndValueTmp,
								      " ",
								      "TDLS_INITIATOR_STATE");
							break;
						case TDLS_RESPONDER_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n",
								      SpaceBtwnItemAndValueTmp,
								      " ",
								      "TDLS_RESPONDER_STATE");
							break;
						case TDLS_LINKED_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n",
								      SpaceBtwnItemAndValueTmp,
								      " ",
								      "TDLS_LINKED_STATE");
							break;
						case TDLS_WAIT_PTR_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n",
								      SpaceBtwnItemAndValueTmp,
								      " ",
								      "TDLS_WAIT_PTR_STATE");
							break;
						case TDLS_ALIVE_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n",
								      SpaceBtwnItemAndValueTmp,
								      " ",
								      "TDLS_ALIVE_STATE");
							break;
						case TDLS_CH_SWITCH_ON_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n",
								      SpaceBtwnItemAndValueTmp,
								      " ",
								      "TDLS_CH_SWITCH_ON_STATE");
							break;
						case TDLS_PEER_AT_OFF_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n",
								      SpaceBtwnItemAndValueTmp,
								      " ",
								      "TDLS_PEER_AT_OFF_STATE");
							break;
						case TDLS_CH_SW_INITIATOR_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n",
								      SpaceBtwnItemAndValueTmp,
								      " ",
								      "TDLS_CH_SW_INITIATOR_STATE");
							break;
						case TDLS_WAIT_CH_RSP_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n",
								      SpaceBtwnItemAndValue,
								      " ",
								      "TDLS_WAIT_CH_RSP_STATE");
							break;
						default:
							RTW_PRINT_SEL(m, "%-*sBIT(%d)\n",
								      SpaceBtwnItemAndValueTmp,
								      " ", j);
							break;
						}
					}
				}

				RTW_PRINT_SEL(m, "%-*s = ", SpaceBtwnItemAndValue,
					      "Wireless Mode");
				for (j = 0; j < 8; j++) {
					if (psta->wireless_mode & BIT(j)) {
						switch (BIT(j)) {
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

				RTW_PRINT_SEL(m, "%-*s = ", SpaceBtwnItemAndValue,
					      "Bandwidth Mode");
				switch (psta->cmn.bw_mode) {
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
				case CHANNEL_WIDTH_5:
					RTW_PRINT_SEL(m, "%s\n", "5MHz");
					break;
				case CHANNEL_WIDTH_10:
					RTW_PRINT_SEL(m, "%s\n", "10MHz");
					break;
				default:
					RTW_PRINT_SEL(m, "(%d)%s\n", psta->cmn.bw_mode,
						      "invalid");
					break;
				}

				RTW_PRINT_SEL(m, "%-*s = ", SpaceBtwnItemAndValue,
					      "Privacy");
				switch (psta->dot118021XPrivacy) {
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
				}

				RTW_PRINT_SEL(m, "%-*s = %d sec/%d sec\n",
					      SpaceBtwnItemAndValue,
					      "TPK Lifetime (Current/Expire)",
					      psta->TPK_count, psta->TDLS_PeerKey_Lifetime);
				RTW_PRINT_SEL(m, "%-*s = %llu\n", SpaceBtwnItemAndValue,
					      "Tx Packets Over Direct Link",
					      (unsigned long long)psta->sta_stats.tx_pkts);
				RTW_PRINT_SEL(m, "%-*s = %llu\n", SpaceBtwnItemAndValue,
					      "Rx Packets Over Direct Link",
					      (unsigned long long)psta->sta_stats.rx_data_pkts);
			}
		}
	}
	_exit_critical_bh(&pstapriv->sta_hash_lock, &irqL);
	if (NumOfTdlsStaToShow == 0) {
		RTW_PRINT_SEL(m, "============[TDLS Peer STA Info]============\n");
		RTW_PRINT_SEL(m, "No TDLS direct link exists!\n");
	}

	return 0;
}
