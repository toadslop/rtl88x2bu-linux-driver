// SPDX-License-Identifier: GPL-2.0
/* C oracle — PMAC_Get_Pkt_Param (W3-113 PR3). */
#include "host_mp_pmac_rate.h"
#include "host_mp_pmac_types.h"

#define RTW_INFO(...) do { } while (0)

void PMAC_Get_Pkt_Param(PRT_PMAC_TX_INFO pPMacTxInfo, PRT_PMAC_PKT_INFO pPMacPktInfo)
{
	u8 TX_RATE_HEX = 0, MCS = 0;
	u8 TX_RATE = pPMacTxInfo->TX_RATE;

	if (MPT_IS_2SS_RATE(TX_RATE))
		pPMacPktInfo->Nss = 2;
	else if (MPT_IS_3SS_RATE(TX_RATE))
		pPMacPktInfo->Nss = 3;
	else if (MPT_IS_4SS_RATE(TX_RATE))
		pPMacPktInfo->Nss = 4;
	else
		pPMacPktInfo->Nss = 1;

	if (MPT_IS_CCK_RATE(TX_RATE)) {
		switch (TX_RATE) {
		case MPT_RATE_1M:
			TX_RATE_HEX = MCS = 0;
			break;
		case MPT_RATE_2M:
			TX_RATE_HEX = MCS = 1;
			break;
		case MPT_RATE_55M:
			TX_RATE_HEX = MCS = 2;
			break;
		case MPT_RATE_11M:
			TX_RATE_HEX = MCS = 3;
			break;
		}
	} else if (MPT_IS_OFDM_RATE(TX_RATE)) {
		MCS = TX_RATE - MPT_RATE_6M;
		TX_RATE_HEX = MCS + 4;
	} else if (MPT_IS_HT_RATE(TX_RATE)) {
		MCS = TX_RATE - MPT_RATE_MCS0;
		TX_RATE_HEX = MCS + 12;
	} else if (MPT_IS_VHT_RATE(TX_RATE)) {
		TX_RATE_HEX = TX_RATE - MPT_RATE_VHT1SS_MCS0 + 44;

		if (MPT_IS_VHT_2S_RATE(TX_RATE))
			MCS = TX_RATE - MPT_RATE_VHT2SS_MCS0;
		else if (MPT_IS_VHT_3S_RATE(TX_RATE))
			MCS = TX_RATE - MPT_RATE_VHT3SS_MCS0;
		else if (MPT_IS_VHT_4S_RATE(TX_RATE))
			MCS = TX_RATE - MPT_RATE_VHT4SS_MCS0;
		else
			MCS = TX_RATE - MPT_RATE_VHT1SS_MCS0;
	}

	pPMacPktInfo->MCS = MCS;
	pPMacTxInfo->TX_RATE_HEX = TX_RATE_HEX;

	pPMacPktInfo->Nsts = pPMacPktInfo->Nss;
	if (pPMacTxInfo->bSTBC) {
		if (pPMacPktInfo->Nss == 1) {
			pPMacTxInfo->m_STBC = 2;
			pPMacPktInfo->Nsts = pPMacPktInfo->Nss * 2;
		} else
			pPMacTxInfo->m_STBC = 1;
	} else
		pPMacTxInfo->m_STBC = 1;
}
