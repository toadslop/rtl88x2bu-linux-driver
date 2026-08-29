/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MLME_WMM_RSN_TYPES_H
#define HOST_MLME_WMM_RSN_TYPES_H

#include <stddef.h>
#include <string.h>

#include "host_ieee80211_types.h"
#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define NUM_PMKID_CACHE 16

#define RTW_INFO(...) do { } while (0)
#define RTW_WARN(...) do { } while (0)
#define RTW_PUT_LE16(a, val) \
	do { \
		(a)[0] = (u8)((u16)(val) & 0xff); \
		(a)[1] = (u8)(((u16)(val) >> 8) & 0xff); \
	} while (0)
#define FUNC_ADPT_FMT "%p"
#define FUNC_ADPT_ARG(adapter) (adapter)
#define KEY_FMT "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
#define KEY_ARG(key) \
	(key)[0], (key)[1], (key)[2], (key)[3], (key)[4], (key)[5], (key)[6], (key)[7], \
	(key)[8], (key)[9], (key)[10], (key)[11], (key)[12], (key)[13], (key)[14], (key)[15]

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
#define WMM_TID7 BIT(7)

enum UAPSD_MAX_SP { NO_LIMIT, TWO_MSDU, FOUR_MSDU, SIX_MSDU };

typedef struct _RT_PMKID_LIST {
	u8 bUsed;
	u8 Bssid[ETH_ALEN];
	u8 PMKID[16];
} RT_PMKID_LIST;

struct qos_priv { u8 uapsd_max_sp_len; u16 uapsd_tid; };
struct security_priv { RT_PMKID_LIST PMKIDList[NUM_PMKID_CACHE]; };
struct mlme_priv { struct qos_priv qospriv; };

struct _adapter {
	struct mlme_priv mlmepriv;
	struct security_priv securitypriv;
	u8 scratch[256];
};

typedef struct _adapter _adapter;
typedef unsigned int uint;

#endif /* HOST_MLME_WMM_RSN_TYPES_H */
