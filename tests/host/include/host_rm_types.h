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

#define RM_MASTER BIT(0)
#define RM_SLAVE 0

struct mlme_ext_info {
	u8 dialogToken;
};

struct mlme_ext_priv {
	struct mlme_ext_info mlmext_info;
};

struct cmn_sta_info {
	u16 aid;
};

struct sta_info {
	struct cmn_sta_info cmn;
};

struct rm_meas_req {
	u8 diag_token;
};

struct rm_obj {
	u32 rmid;
	struct rm_meas_req q;
	struct sta_info *psta;
};

struct rm_priv {
	u8 meas_token;
};

struct _adapter {
	struct rm_priv rmpriv;
	struct mlme_ext_priv mlmeextpriv;
};

typedef struct _adapter _adapter;

#endif /* HOST_RM_TYPES_H */
