// SPDX-License-Identifier: GPL-2.0
/* C oracle — HT_SIG_generator (W3-113 PR5). */
#include "host_mp_pmac_types.h"

void HT_SIG_generator(PRT_PMAC_TX_INFO pPMacTxInfo, PRT_PMAC_PKT_INFO pPMacPktInfo)
{
	u32 i;
	bool sig_bi[48] = {0}, crc8[8] = {0};

	for (i = 0; i < 7; i++)
		sig_bi[i] = (pPMacPktInfo->MCS >> i) & 0x1;
	sig_bi[7] = pPMacTxInfo->BandWidth;
	for (i = 0; i < 16; i++)
		sig_bi[i + 8] = (pPMacTxInfo->PacketLength >> i) & 0x1;
	sig_bi[24] = 1;
	sig_bi[25] = 1 - pPMacTxInfo->NDP_sound;
	sig_bi[26] = 1;
	sig_bi[27] = 0;
	if (pPMacTxInfo->bSTBC) {
		sig_bi[28] = 1;
		sig_bi[29] = 0;
	} else {
		sig_bi[28] = 0;
		sig_bi[29] = 0;
	}
	sig_bi[30] = pPMacTxInfo->bLDPC;
	sig_bi[31] = pPMacTxInfo->bSGI;
	if (pPMacTxInfo->NDP_sound == FALSE) {
		sig_bi[32] = 0;
		sig_bi[33] = 0;
	} else {
		int N_ELTF = pPMacTxInfo->Ntx - pPMacPktInfo->Nss;

		for (i = 0; i < 2; i++)
			sig_bi[32 + i] = (N_ELTF >> i) % 2;
	}
	CRC8_generator(crc8, sig_bi, 34);

	for (i = 0; i < 8; i++)
		sig_bi[34 + i] = crc8[i];

	for (i = 42; i < 48; i++)
		sig_bi[i] = 0;

	_rtw_memset(pPMacTxInfo->HT_SIG, 0, 6);
	ByteToBit(pPMacTxInfo->HT_SIG, sig_bi, 6);
}
