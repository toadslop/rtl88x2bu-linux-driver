// SPDX-License-Identifier: GPL-2.0
#include <string.h>
#include "host_hal_com_fw_bcn_offload_types.h"

#define BIT_LEN_MASK_8(__BitLen) (0xFFu >> (8 - (__BitLen)))
#define BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen) \
	(BIT_LEN_MASK_8(__BitLen) << (__BitOffset))

#define LE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	((*(u8 *)(__pStart)) & (~BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen)))

#define SET_BITS_TO_LE_1BYTE(__pStart, __BitOffset, __BitLen, __Value)       \
	do {                                                               \
		if (__BitOffset == 0 && __BitLen == 8)                     \
			*(u8 *)(__pStart) = (u8)(__Value);                 \
		else {                                                     \
			*(u8 *)(__pStart) =                                \
				LE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) | \
				((((u8)(__Value)) & BIT_LEN_MASK_8(__BitLen)) << (__BitOffset)); \
		}                                                          \
	} while (0)

#define SET_H2CCMD_BCN_OFFLOAD_EN(__pH2CCmd, __Value) \
	SET_BITS_TO_LE_1BYTE(__pH2CCmd, 0, 1, __Value)
#define SET_H2CCMD_BCN_ROOT_TBTT_RPT(__pH2CCmd, __Value) \
	SET_BITS_TO_LE_1BYTE(__pH2CCmd, 1, 1, __Value)
#define SET_H2CCMD_BCN_VAP1_TBTT_RPT(__pH2CCmd, __Value) \
	SET_BITS_TO_LE_1BYTE(__pH2CCmd, 2, 1, __Value)
#define SET_H2CCMD_BCN_VAP2_TBTT_RPT(__pH2CCmd, __Value) \
	SET_BITS_TO_LE_1BYTE(__pH2CCmd, 3, 1, __Value)
#define SET_H2CCMD_BCN_VAP3_TBTT_RPT(__pH2CCmd, __Value) \
	SET_BITS_TO_LE_1BYTE(__pH2CCmd, 4, 1, __Value)

void rtw_hal_set_fw_ap_bcn_offload_cmd_c_oracle(struct _adapter *adapter,
						  bool fw_bcn_en, u8 tbtt_rpt_map)
{
	u8 fw_bcn_offload[H2C_BCN_OFFLOAD_LEN] = {0};
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	if (fw_bcn_en)
		SET_H2CCMD_BCN_OFFLOAD_EN(fw_bcn_offload, 1);
	if (tbtt_rpt_map & (1u << 0))
		SET_H2CCMD_BCN_ROOT_TBTT_RPT(fw_bcn_offload, 1);
	if (tbtt_rpt_map & (1u << 1))
		SET_H2CCMD_BCN_VAP1_TBTT_RPT(fw_bcn_offload, 1);
	if (tbtt_rpt_map & (1u << 2))
		SET_H2CCMD_BCN_VAP2_TBTT_RPT(fw_bcn_offload, 1);
	if (tbtt_rpt_map & (1u << 3))
		SET_H2CCMD_BCN_VAP3_TBTT_RPT(fw_bcn_offload, 1);
	dvobj->vap_tbtt_rpt_map = tbtt_rpt_map;
	dvobj->fw_bcn_offload = fw_bcn_en ? 1u : 0u;

	rtw_hal_fill_h2c_cmd(adapter, H2C_FW_BCN_OFFLOAD, H2C_BCN_OFFLOAD_LEN,
			     fw_bcn_offload);
}

#ifndef RUST_HAL_COM_FW_BCN_OFFLOAD_ORACLE
void rtw_hal_set_fw_ap_bcn_offload_cmd(struct _adapter *adapter, bool fw_bcn_en,
				       u8 tbtt_rpt_map)
{
	rtw_hal_set_fw_ap_bcn_offload_cmd_c_oracle(adapter, fw_bcn_en, tbtt_rpt_map);
}
#endif