// SPDX-License-Identifier: GPL-2.0
/* C oracle — L_SIG_generator / HT_SIG_generator (W3-113 PR4). */
#include <math.h>

#include "host_mp_pmac_types.h"

void L_SIG_generator(u32 N_SYM, PRT_PMAC_TX_INFO pPMacTxInfo,
		     PRT_PMAC_PKT_INFO pPMacPktInfo)
{
	u8 sig_bi[24] = {0};
	u32 mode, LENGTH;
	int i;

	if (MPT_IS_OFDM_RATE(pPMacTxInfo->TX_RATE)) {
		mode = pPMacPktInfo->MCS;
		LENGTH = pPMacTxInfo->PacketLength;
	} else {
		u8 N_LTF;
		double T_data;
		u32 OFDM_symbol;

		mode = 0;

		if (pPMacPktInfo->Nsts <= 2)
			N_LTF = pPMacPktInfo->Nsts;
		else
			N_LTF = 4;

		if (pPMacTxInfo->bSGI)
			T_data = 3.6;
		else
			T_data = 4.0;

		if (MPT_IS_VHT_RATE(pPMacTxInfo->TX_RATE))
			OFDM_symbol = (u32)ceil((double)(8 + 4 + N_LTF * 4 + N_SYM * T_data + 4) / 4.);
		else
			OFDM_symbol = (u32)ceil((double)(8 + 4 + N_LTF * 4 + N_SYM * T_data) / 4.);

		LENGTH = OFDM_symbol * 3 - 3;
	}

	switch (mode) {
	case 0:
		sig_bi[0] = 1;
		sig_bi[1] = 1;
		sig_bi[2] = 0;
		sig_bi[3] = 1;
		break;
	case 1:
		sig_bi[0] = 1;
		sig_bi[1] = 1;
		sig_bi[2] = 1;
		sig_bi[3] = 1;
		break;
	case 2:
		sig_bi[0] = 0;
		sig_bi[1] = 1;
		sig_bi[2] = 0;
		sig_bi[3] = 1;
		break;
	case 3:
		sig_bi[0] = 0;
		sig_bi[1] = 1;
		sig_bi[2] = 1;
		sig_bi[3] = 1;
		break;
	case 4:
		sig_bi[0] = 1;
		sig_bi[1] = 0;
		sig_bi[2] = 0;
		sig_bi[3] = 1;
		break;
	case 5:
		sig_bi[0] = 1;
		sig_bi[1] = 0;
		sig_bi[2] = 1;
		sig_bi[3] = 1;
		break;
	case 6:
		sig_bi[0] = 0;
		sig_bi[1] = 0;
		sig_bi[2] = 0;
		sig_bi[3] = 1;
		break;
	case 7:
		sig_bi[0] = 0;
		sig_bi[1] = 0;
		sig_bi[2] = 1;
		sig_bi[3] = 1;
		break;
	}
	sig_bi[4] = 0;

	for (i = 0; i < 12; i++)
		sig_bi[i + 5] = (LENGTH >> i) & 1;

	sig_bi[17] = 0;
	for (i = 0; i < 17; i++)
		sig_bi[17] = sig_bi[17] + sig_bi[i];

	sig_bi[17] %= 2;

	for (i = 18; i < 24; i++)
		sig_bi[i] = 0;

	_rtw_memset(pPMacTxInfo->LSIG, 0, 3);
	ByteToBit(pPMacTxInfo->LSIG, (bool *)sig_bi, 3);
}
