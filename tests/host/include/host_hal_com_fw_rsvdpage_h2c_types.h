/* SPDX-License-Identifier: GPL-2.0 */
/* Host types for W4-136 FW reserved page location H2C L2 harness. */

#ifndef HOST_HAL_COM_FW_RSVDPAGE_H2C_TYPES_H
#define HOST_HAL_COM_FW_RSVDPAGE_H2C_TYPES_H

#include <stddef.h>

typedef unsigned char u8;
typedef int s32;

struct _adapter;

typedef struct {
	u8 LocProbeRsp;
	u8 LocPsPoll;
	u8 LocNullData;
	u8 LocQosNull;
	u8 LocBTQosNull;
} RSVDPAGE_LOC;

#define H2C_RSVD_PAGE 0x00
#define H2C_RSVDPAGE_LOC_LEN 5

void rtw_hal_set_FwRsvdPage_cmd(struct _adapter *adapter, RSVDPAGE_LOC *rsvdpageloc);

#endif