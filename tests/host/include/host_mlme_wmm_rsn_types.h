/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MLME_WMM_RSN_TYPES_H
#define HOST_MLME_WMM_RSN_TYPES_H

#include "host_types.h"

#define BIT0 (1 << 0)
#define BIT1 (1 << 1)
#define BIT2 (1 << 2)
#define BIT3 (1 << 3)
#define BIT4 (1 << 4)
#define BIT5 (1 << 5)
#define BIT6 (1 << 6)
#define BIT7 (1 << 7)

enum UAPSD_MAX_SP {
	NO_LIMIT,
	TWO_MSDU,
	FOUR_MSDU,
	SIX_MSDU
};

#define TEST_FLAG(__Flag, __testFlag) (((__Flag) & (__testFlag)) != 0)
#define SET_FLAG(__Flag, __setFlag) ((__Flag) |= (__setFlag))

#define WMM_IE_UAPSD_VO BIT0
#define WMM_IE_UAPSD_VI BIT1
#define WMM_IE_UAPSD_BK BIT2
#define WMM_IE_UAPSD_BE BIT3

#define WMM_TID0 BIT0
#define WMM_TID1 BIT1
#define WMM_TID2 BIT2
#define WMM_TID3 BIT3
#define WMM_TID4 BIT4
#define WMM_TID5 BIT5
#define WMM_TID6 BIT6
#define WMM_TID7 BIT7

struct qos_priv {
	u8 uapsd_max_sp_len;
	u16 uapsd_tid;
};

struct mlme_priv {
	struct qos_priv qospriv;
};

struct _adapter {
	struct mlme_priv mlmepriv;
};

typedef struct _adapter _adapter;
typedef unsigned int uint;

#endif
