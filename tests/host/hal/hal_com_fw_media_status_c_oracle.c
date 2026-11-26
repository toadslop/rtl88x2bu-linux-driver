// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-139 rtw_hal_set_FwMediaStatusRpt_* (hal_com_rest.c layout). */

#include <string.h>
#include "host_hal_com_fw_media_status_types.h"

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

#define SET_H2CCMD_MSRRPT_PARM_OPMODE(__p, __v) SET_BITS_TO_LE_1BYTE(__p, 0, 1, __v)
#define SET_H2CCMD_MSRRPT_PARM_MACID_IND(__p, __v) SET_BITS_TO_LE_1BYTE(__p, 1, 1, __v)
#define SET_H2CCMD_MSRRPT_PARM_MIRACAST(__p, __v) SET_BITS_TO_LE_1BYTE(__p, 2, 1, __v)
#define SET_H2CCMD_MSRRPT_PARM_MIRACAST_SINK(__p, __v) SET_BITS_TO_LE_1BYTE(__p, 3, 1, __v)
#define SET_H2CCMD_MSRRPT_PARM_ROLE(__p, __v) SET_BITS_TO_LE_1BYTE(__p, 4, 4, __v)
#define SET_H2CCMD_MSRRPT_PARM_MACID(__p, __v) SET_BITS_TO_LE_1BYTE((__p) + 1, 0, 8, __v)
#define SET_H2CCMD_MSRRPT_PARM_MACID_END(__p, __v) SET_BITS_TO_LE_1BYTE((__p) + 2, 0, 8, __v)
#define SET_H2CCMD_MSRRPT_PARM_PORT_NUM(__p, __v) SET_BITS_TO_LE_1BYTE((__p) + 3, 0, 3, __v)

s32 rtw_hal_set_FwMediaStatusRpt_cmd_c_oracle(struct _adapter *adapter,
					       bool opmode, bool miracast,
					       bool miracast_sink, u8 role,
					       u8 macid, bool macid_ind,
					       u8 macid_end)
{
	struct macid_ctl_t *macid_ctl = &adapter->dvobj.macid_ctl;
	u8 parm[H2C_MEDIA_STATUS_RPT_LEN] = {0};
	int i;
	s32 ret;
	u8 op_num_change_bmp = 0;

	SET_H2CCMD_MSRRPT_PARM_OPMODE(parm, opmode);
	SET_H2CCMD_MSRRPT_PARM_MACID_IND(parm, macid_ind);
	SET_H2CCMD_MSRRPT_PARM_MIRACAST(parm, miracast);
	SET_H2CCMD_MSRRPT_PARM_MIRACAST_SINK(parm, miracast_sink);
	SET_H2CCMD_MSRRPT_PARM_ROLE(parm, role);
	SET_H2CCMD_MSRRPT_PARM_MACID(parm, macid);
	SET_H2CCMD_MSRRPT_PARM_MACID_END(parm, macid_end);
#ifdef CONFIG_FW_MULTI_PORT_SUPPORT
	{
		u8 hw_port = rtw_hal_get_port(adapter);

		SET_H2CCMD_MSRRPT_PARM_PORT_NUM(parm, hw_port);
	}
#endif

	ret = rtw_hal_fill_h2c_cmd(adapter, H2C_MEDIA_STATUS_RPT,
				   H2C_MEDIA_STATUS_RPT_LEN, parm);
	if (ret != _SUCCESS)
		return ret;

	SET_H2CCMD_MSRRPT_PARM_MACID_IND(parm, 0);
	if (macid_ind == 0)
		macid_end = macid;

	for (i = macid; i <= macid_end; i++) {
		op_num_change_bmp |= rtw_macid_ctl_set_h2c_msr(macid_ctl, i, parm[0]);
		if (!opmode) {
			rtw_macid_ctl_set_bw(macid_ctl, i, CHANNEL_WIDTH_20);
			rtw_macid_ctl_set_vht_en(macid_ctl, i, 0);
			rtw_macid_ctl_set_rate_bmp0(macid_ctl, i, 0);
			rtw_macid_ctl_set_rate_bmp1(macid_ctl, i, 0);
		}
	}

#if CONFIG_TX_AC_LIFETIME
	if (op_num_change_bmp)
		rtw_hal_update_tx_aclt(adapter);
#endif

	if (!opmode)
		rtw_update_tx_rate_bmp(adapter_to_dvobj(adapter));

	return ret;
}

#ifndef RUST_HAL_COM_FW_MEDIA_STATUS_ORACLE
s32 rtw_hal_set_FwMediaStatusRpt_cmd(struct _adapter *adapter, bool opmode,
				     bool miracast, bool miracast_sink, u8 role,
				     u8 macid, bool macid_ind, u8 macid_end)
{
	return rtw_hal_set_FwMediaStatusRpt_cmd_c_oracle(
		adapter, opmode, miracast, miracast_sink, role, macid, macid_ind,
		macid_end);
}

s32 rtw_hal_set_FwMediaStatusRpt_single_cmd(struct _adapter *adapter,
					    bool opmode, bool miracast,
					    bool miracast_sink, u8 role,
					    u8 macid)
{
	return rtw_hal_set_FwMediaStatusRpt_cmd(adapter, opmode, miracast,
						miracast_sink, role, macid, 0,
						0);
}

s32 rtw_hal_set_FwMediaStatusRpt_range_cmd(struct _adapter *adapter,
					   bool opmode, bool miracast,
					   bool miracast_sink, u8 role,
					   u8 macid, u8 macid_end)
{
	return rtw_hal_set_FwMediaStatusRpt_cmd(adapter, opmode, miracast,
						miracast_sink, role, macid, 1,
						macid_end);
}
#endif