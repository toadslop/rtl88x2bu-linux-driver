// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-118 rtw_hal_set_req_per_rpt_cmd (hal_com_rest.c layout). */

#include <string.h>
#include "host_hal_com_req_per_rpt_types.h"

#define _FAIL 0
#define H2C_REQ_PER_RPT 0x4e
#define H2C_REQ_PER_RPT_LEN 5

#define BIT_LEN_MASK_8(__BitLen)		(0xFF >> (8 - (__BitLen)))
#define BIT_LEN_MASK_32(__BitLen)	(0xFFFFFFFF >> (32 - (__BitLen)))
#define BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen) \
	(BIT_LEN_MASK_8(__BitLen) << (__BitOffset))
#define BIT_OFFSET_LEN_MASK_32(__BitOffset, __BitLen) \
	(((u32)BIT_LEN_MASK_32(__BitLen)) << (__BitOffset))

#define LE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	((*(unsigned char *)(__pStart)) & \
	 (~BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen)))

#define LE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
	((*(unsigned int *)(__pStart)) & \
	 (~BIT_OFFSET_LEN_MASK_32(__BitOffset, __BitLen)))

#define WriteLE1Byte(__pStart, __Value)	(*(unsigned char *)(__pStart) = (unsigned char)(__Value))
#define WriteLE4Byte(__pStart, __Value)	(*(unsigned int *)(__pStart) = (unsigned int)(__Value))

#define SET_BITS_TO_LE_1BYTE(__pStart, __BitOffset, __BitLen, __Value)       \
	do {                                                               \
		if (__BitOffset == 0 && __BitLen == 8)                     \
			WriteLE1Byte(__pStart, __Value);                   \
		else {                                                     \
			WriteLE1Byte(__pStart,                                \
				     LE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) | \
					     ((((unsigned char)(__Value)) & BIT_LEN_MASK_8(__BitLen)) << (__BitOffset))); \
		}                                                          \
	} while (0)

#define SET_BITS_TO_LE_4BYTE(__pStart, __BitOffset, __BitLen, __Value)       \
	do {                                                               \
		if (__BitOffset == 0 && __BitLen == 32)                    \
			WriteLE4Byte(__pStart, __Value);                   \
		else {                                                     \
			WriteLE4Byte(__pStart,                                \
				     LE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) | \
					     ((((unsigned int)(__Value)) & BIT_LEN_MASK_32(__BitLen)) << (__BitOffset))); \
		}                                                          \
	} while (0)

typedef unsigned int u32;

extern long rtw_hal_fill_h2c_cmd(struct _adapter *adapter, unsigned char cmd, unsigned int len,
				 unsigned char *buf);

unsigned char rtw_hal_set_req_per_rpt_cmd_c_oracle(struct _adapter *adapter, unsigned char group_macid,
						   unsigned char rpt_type, u32 macid_bitmap)
{
	unsigned char ret = _FAIL;
	unsigned char cmd_buf[H2C_REQ_PER_RPT_LEN] = {0};

	SET_BITS_TO_LE_1BYTE(cmd_buf, 0, 4, group_macid);
	SET_BITS_TO_LE_1BYTE(cmd_buf, 4, 4, rpt_type);
	SET_BITS_TO_LE_4BYTE(cmd_buf + 1, 0, 32, macid_bitmap);

	ret = (unsigned char)rtw_hal_fill_h2c_cmd(adapter, H2C_REQ_PER_RPT, H2C_REQ_PER_RPT_LEN, cmd_buf);
	return ret;
}