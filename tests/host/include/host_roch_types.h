/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_ROCH_TYPES_H
#define HOST_ROCH_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define H2C_SUCCESS 0
#define RTW_CMDF_DIRECTLY 1
#define ROCH_RO_CH_WK 0
#define ROCH_CANCEL_RO_CH_WK 1
#define WIFI_UNDER_LINKING 0x80
#define WIFI_ASOC_STATE 2
#define HAL_PRIME_CHNL_OFFSET_DONT_CARE 0
#define CHANNEL_WIDTH_20 0
#define MAX_ROCH_IFACE 2

typedef s32 mlme_state_t;

struct mlme_priv { mlme_state_t fwstate; };
struct ieee80211_channel { int center_freq; };
enum nl80211_channel_type { NL80211_CHAN_NO_HT };
struct wireless_dev { int dummy; };

struct rtw_roch_parm {
	u64 cookie;
	struct wireless_dev *wdev;
	struct ieee80211_channel ch;
	enum nl80211_channel_type ch_type;
	unsigned int duration;
};

struct roch_info {
	u8 restore_channel;
	struct ieee80211_channel remain_on_ch_channel;
	enum nl80211_channel_type remain_on_ch_type;
	u64 remain_on_ch_cookie;
	u8 is_roch;
	struct wireless_dev *ro_ch_wdev;
};

struct _adapter;
struct dvobj_priv {
	u8 iface_nums;
	struct _adapter *padapters[MAX_ROCH_IFACE];
};

struct _adapter {
	struct mlme_priv mlmepriv;
	struct roch_info rochinfo;
	u8 oper_ch;
	struct dvobj_priv *dvobj;
};

typedef struct _adapter _adapter;
typedef _adapter *PADAPTER;

struct host_roch_trace {
	int set_channel, set_timer, cancel_timer, roch_expired, wk_cmd_direct, mfree;
	u8 set_channel_ch;
	unsigned int set_timer_ms;
};

void host_roch_reset(void);
struct host_roch_trace *host_roch_trace(void);
void host_roch_set_is_roch(PADAPTER a, u8 v);
void host_roch_set_oper_ch(PADAPTER a, u8 ch);
void host_roch_setup_iface(PADAPTER a, mlme_state_t st);

u8 rtw_roch_stay_in_cur_chan(PADAPTER padapter);
s32 rtw_roch_wk_hdl(PADAPTER padapter, int intCmdType, u8 *buf);
u8 rtw_roch_wk_cmd(PADAPTER padapter, int intCmdType, struct rtw_roch_parm *roch_parm, u8 flags);
void *rtw_zmalloc(u32 sz);

#endif
