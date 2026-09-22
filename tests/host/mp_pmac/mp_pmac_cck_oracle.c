// SPDX-License-Identifier: GPL-2.0
/* C oracle — CCK_generator (W3-113 PR2). */
#include <math.h>

#include "host_mp_pmac_types.h"

void CCK_generator(PRT_PMAC_TX_INFO pPMacTxInfo, PRT_PMAC_PKT_INFO pPMacPktInfo)
{
	double ratio = 0;
	bool crc16_in[32] = {0}, crc16_out[16] = {0};
	bool LengthExtBit;
	double LengthExact;
	double LengthPSDU;
	u8 i;
	u32 PacketLength = pPMacTxInfo->PacketLength;

	if (pPMacTxInfo->bSPreamble)
		pPMacTxInfo->SFD = 0x05CF;
	else
		pPMacTxInfo->SFD = 0xF3A0;

	switch (pPMacPktInfo->MCS) {
	case 0:
		pPMacTxInfo->SignalField = 0xA;
		ratio = 8;
		crc16_in[1] = crc16_in[3] = 1;
		break;
	case 1:
		pPMacTxInfo->SignalField = 0x14;
		ratio = 4;
		crc16_in[2] = crc16_in[4] = 1;
		break;
	case 2:
		pPMacTxInfo->SignalField = 0x37;
		ratio = 8.0 / 5.5;
		crc16_in[0] = crc16_in[1] = crc16_in[2] = crc16_in[4] = crc16_in[5] = 1;
		break;
	case 3:
		pPMacTxInfo->SignalField = 0x6E;
		ratio = 8.0 / 11.0;
		crc16_in[1] = crc16_in[2] = crc16_in[3] = crc16_in[5] = crc16_in[6] = 1;
		break;
	}

	LengthExact = PacketLength * ratio;
	LengthPSDU = ceil(LengthExact);

	if ((pPMacPktInfo->MCS == 3) &&
	    ((LengthPSDU - LengthExact) >= 0.727 || (LengthPSDU - LengthExact) <= -0.727))
		LengthExtBit = 1;
	else
		LengthExtBit = 0;

	pPMacTxInfo->LENGTH = (u32)LengthPSDU;
	for (i = 0; i < 16; i++)
		crc16_in[i + 16] = (pPMacTxInfo->LENGTH >> i) & 0x1;

	if (LengthExtBit == 0) {
		pPMacTxInfo->ServiceField = 0x0;
	} else {
		pPMacTxInfo->ServiceField = 0x80;
		crc16_in[15] = 1;
	}

	CRC16_generator(crc16_out, crc16_in, 32);
	_rtw_memset(pPMacTxInfo->CRC16, 0, 2);
	ByteToBit(pPMacTxInfo->CRC16, crc16_out, 2);
}
