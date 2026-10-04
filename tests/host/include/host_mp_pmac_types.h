/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MP_PMAC_TYPES_H
#define HOST_MP_PMAC_TYPES_H

#include <stdbool.h>

#include "host_mp_pmac_rate.h"
#include "host_types.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

typedef struct _RT_PMAC_PKT_INFO {
	u8 MCS;
	u8 Nss;
	u8 Nsts;
	u32 N_sym;
	u8 SIGA2B3;
} RT_PMAC_PKT_INFO, *PRT_PMAC_PKT_INFO;

typedef struct _RT_PMAC_TX_INFO {
	u8 bEnPMacTx : 1;
	u8 Mode : 3;
	u8 Ntx : 4;
	u8 TX_RATE;
	u8 TX_RATE_HEX;
	u8 TX_SC;
	u8 bSGI : 1;
	u8 bSPreamble : 1;
	u8 bSTBC : 1;
	u8 bLDPC : 1;
	u8 NDP_sound : 1;
	u8 BandWidth : 3;
	u8 m_STBC;
	u16 PacketPeriod;
	u32 PacketCount;
	u32 PacketLength;
	u8 PacketPattern;
	u16 SFD;
	u8 SignalField;
	u8 ServiceField;
	u16 LENGTH;
	u8 CRC16[2];
	u8 LSIG[3];
	u8 HT_SIG[6];
	u8 VHT_SIG_A[6];
	u8 VHT_SIG_B[4];
	u8 VHT_SIG_B_CRC;
	u8 VHT_Delimiter[4];
	u8 MacAddress[6];
} RT_PMAC_TX_INFO, *PRT_PMAC_TX_INFO;

void ByteToBit(u8 *out, bool *in, u8 in_size);
void CRC16_generator(bool *out, bool *in, u8 in_size);
void CRC8_generator(bool *out, bool *in, u8 in_size);
void CCK_generator(PRT_PMAC_TX_INFO pPMacTxInfo, PRT_PMAC_PKT_INFO pPMacPktInfo);
void PMAC_Get_Pkt_Param(PRT_PMAC_TX_INFO pPMacTxInfo, PRT_PMAC_PKT_INFO pPMacPktInfo);

#endif /* HOST_MP_PMAC_TYPES_H */
