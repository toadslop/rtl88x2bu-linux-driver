/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_VHT_REST_H
#define HOST_VHT_REST_H

#include "host_types.h"
#include "host_autoconf.h"
#include <stdbool.h>
#include <string.h>

typedef unsigned int uint;
typedef int sint;
#define _TRUE 1
#define _FALSE 0
#define BOOLEAN u8
#define rtw_min(a, b) ((a) < (b) ? (a) : (b))
#define RTW_ERR(...) do { } while (0)
#define rtw_warn_on(c) ((void)(c))
#define FUNC_ADPT_FMT "%p"
#define FUNC_ADPT_ARG(a) (void *)(a)

enum channel_width { CHANNEL_WIDTH_20 = 0, CHANNEL_WIDTH_40 = 1, CHANNEL_WIDTH_80 = 2 };
#define HAL_PRIME_CHNL_OFFSET_DONT_CARE 0
#define HAL_PRIME_CHNL_OFFSET_LOWER 1
#define HAL_PRIME_CHNL_OFFSET_UPPER 2
#define WLAN_EID_HT_OPERATION 61
#define EID_VHTCapability 191
#define EID_VHTOperation 192
#define EID_OpModeNotification 199
#define HT_OP_IE_LEN 22
#define VHT_CAP_IE_LEN 12
#define VHT_OP_IE_LEN 5
#define SCA 1
#define SCB 3
#define MAX_CHANNEL_NUM 59
#define LE_BITS_TO_1BYTE(p, o, l) (((*((u8 *)(p)) >> (o)) & ((1U << (l)) - 1)))
#define SET_BITS_TO_LE_1BYTE(p, o, l, v) do { u8 *__x = (u8 *)(p); u8 __m = ((1U << (l)) - 1) << (o); *__x = (*__x & ~__m) | (((v) & ((1U << (l)) - 1)) << (o)); } while (0)
#define GET_HT_OP_ELE_PRI_CHL(p) LE_BITS_TO_1BYTE(p, 0, 8)
#define GET_HT_OP_ELE_2ND_CHL_OFFSET(p) LE_BITS_TO_1BYTE((p) + 1, 0, 2)
#define GET_HT_OP_ELE_STA_CHL_WIDTH(p) LE_BITS_TO_1BYTE((p) + 1, 2, 1)
#define GET_VHT_OPERATION_ELE_CHL_WIDTH(p) LE_BITS_TO_1BYTE(p, 0, 8)
#define SET_VHT_OPERATION_ELE_CHL_WIDTH(p, v) SET_BITS_TO_LE_1BYTE(p, 0, 8, v)
#define SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ1(p, v) SET_BITS_TO_LE_1BYTE((p) + 1, 0, 8, v)
#define SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ2(p, v) SET_BITS_TO_LE_1BYTE((p) + 2, 0, 8, v)

typedef struct _RT_CHANNEL_INFO { u8 ChannelNum; u8 flags; } RT_CHANNEL_INFO;
struct registry_priv { u8 bw_mode; };
struct vht_priv { u8 vht_option; };
struct mlme_priv { struct vht_priv vhtpriv; };
struct rf_ctl_t { u8 dfs_slave_with_rd; RT_CHANNEL_INFO channel_set[MAX_CHANNEL_NUM]; };
struct host_vht_fixture {
	u8 hal_max_bw;
	u8 chset_max_bw;
	u8 dfs_slave_with_rd;
	u8 dfs_domain_unknown;
	u8 chbw_non_ocp_at_80;
	u8 cap_ie[VHT_CAP_IE_LEN];
	u8 opmode_notify;
};
struct _adapter { struct registry_priv registrypriv; struct mlme_priv mlmepriv; struct rf_ctl_t rf_ctl; struct host_vht_fixture host_fixture; };
typedef struct _adapter _adapter;
#define adapter_to_rfctl(a) (&(a)->rf_ctl)
#define BW_MODE_5G(bw_mode) ((bw_mode) >> 4)
#define REGSTY_BW_5G(r) BW_MODE_5G((r)->bw_mode)
#define IS_DFS_SLAVE_WITH_RD(r) ((r)->dfs_slave_with_rd)

extern _adapter *host_vht_rest_adapter;
u8 *rtw_get_ie(const u8 *pbuf, sint index, uint *len, sint limit);
u8 *rtw_set_ie(u8 *pbuf, sint index, uint len, const u8 *source, uint *frlen);
void rtw_vht_use_default_setting(_adapter *padapter);
u32 rtw_build_vht_cap_ie(_adapter *padapter, u8 *pbuf);
u32 rtw_build_vht_op_mode_notify_ie(_adapter *padapter, u8 *pbuf, u8 bw);
u8 hal_largest_bw(_adapter *padapter, u8 bw_cap);
bool rtw_chset_is_chbw_valid(RT_CHANNEL_INFO *cs, u8 ch, u8 bw, u8 off, u8 a, u8 b);
bool rtw_chset_is_chbw_non_ocp(RT_CHANNEL_INFO *cs, u8 ch, u8 bw, u8 off);
u8 rtw_rfctl_dfs_domain_unknown(struct rf_ctl_t *rfctl);
u8 rtw_get_center_ch(u8 ch, u8 bw, u8 offset);

#endif /* HOST_VHT_REST_H */
