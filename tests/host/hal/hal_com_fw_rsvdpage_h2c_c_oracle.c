// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-136 rtw_hal_set_FwRsvdPage_cmd (hal_com_rest.c layout). */

#include <string.h>
#include "host_hal_com_fw_rsvdpage_h2c_types.h"

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

#define SET_H2CCMD_RSVDPAGE_LOC_PROBE_RSP(__pH2CCmd, __Value) \
	SET_BITS_TO_LE_1BYTE(__pH2CCmd, 0, 8, __Value)
#define SET_H2CCMD_RSVDPAGE_LOC_PSPOLL(__pH2CCmd, __Value) \
	SET_BITS_TO_LE_1BYTE((__pH2CCmd) + 1, 0, 8, __Value)
#define SET_H2CCMD_RSVDPAGE_LOC_NULL_DATA(__pH2CCmd, __Value) \
	SET_BITS_TO_LE_1BYTE((__pH2CCmd) + 2, 0, 8, __Value)
#define SET_H2CCMD_RSVDPAGE_LOC_QOS_NULL_DATA(__pH2CCmd, __Value) \
	SET_BITS_TO_LE_1BYTE((__pH2CCmd) + 3, 0, 8, __Value)
#define SET_H2CCMD_RSVDPAGE_LOC_BT_QOS_NULL_DATA(__pH2CCmd, __Value) \
	SET_BITS_TO_LE_1BYTE((__pH2CCmd) + 4, 0, 8, __Value)

extern s32 rtw_hal_fill_h2c_cmd(struct _adapter *adapter, u8 cmd, unsigned int len, u8 *buf);

void rtw_hal_set_FwRsvdPage_cmd_c_oracle(struct _adapter *adapter, RSVDPAGE_LOC *rsvdpageloc)
{
	u8 u1H2CRsvdPageParm[H2C_RSVDPAGE_LOC_LEN] = {0};

	(void)adapter;

	SET_H2CCMD_RSVDPAGE_LOC_PROBE_RSP(u1H2CRsvdPageParm, rsvdpageloc->LocProbeRsp);
	SET_H2CCMD_RSVDPAGE_LOC_PSPOLL(u1H2CRsvdPageParm, rsvdpageloc->LocPsPoll);
	SET_H2CCMD_RSVDPAGE_LOC_NULL_DATA(u1H2CRsvdPageParm, rsvdpageloc->LocNullData);
	SET_H2CCMD_RSVDPAGE_LOC_QOS_NULL_DATA(u1H2CRsvdPageParm, rsvdpageloc->LocQosNull);
	SET_H2CCMD_RSVDPAGE_LOC_BT_QOS_NULL_DATA(u1H2CRsvdPageParm,
						rsvdpageloc->LocBTQosNull);

	rtw_hal_fill_h2c_cmd(adapter, H2C_RSVD_PAGE, H2C_RSVDPAGE_LOC_LEN,
			     u1H2CRsvdPageParm);
}