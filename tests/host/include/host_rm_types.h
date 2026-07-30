/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal userspace types for host L2 RM util tests (W3-33).
 */
#ifndef HOST_RM_TYPES_H
#define HOST_RM_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1

#define RTW_INFO(fmt, ...) ((void)0)

struct rtw_ieee80211_channel {
	u16 hw_value;
	u32 flags;
};

#define MAX_CH_NUM_IN_OP_CLASS 11
typedef struct _RT_OPERATING_CLASS {
	int global_op_class;
	int Len;
	u8 Channel[MAX_CH_NUM_IN_OP_CLASS];
} RT_OPERATING_CLASS, *PRT_OPERATING_CLASS;

#endif /* HOST_RM_TYPES_H */
