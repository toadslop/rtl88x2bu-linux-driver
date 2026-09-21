/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_TDLS_HT_TYPES_H
#define HOST_TDLS_HT_TYPES_H

#include "host_types.h"
#include <string.h>

#define _TRUE 1
#define _FALSE 0
#define WLAN_STA_WME (1U << 9)
#define WLAN_STA_HT (1U << 11)
#define WIRELESS_11_24N (1U << 3)
#define WIRELESS_11_5N (1U << 4)
#define CHANNEL_WIDTH_40 1
#define IEEE80211_HT_CAP_SUP_WIDTH 0x0002
#define IEEE80211_HT_CAP_SGI_20 0x0020
#define HT_CAP_LEN 26

static inline u16 cpu_to_le16(u16 x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return x;
#else
	return (u16)((x >> 8) | (x << 8));
#endif
}

#define _rtw_memset(p, c, n) memset((p), (c), (n))
#define _rtw_memcpy(d, s, n) memcpy((d), (s), (n))

struct ht_priv {
	u8 ht_option;
	u8 ampdu_enable;
	u8 rx_ampdu_min_spacing;
	u8 ch_offset;
	u8 sgi_20m;
	u8 sgi_40m;
	u8 ht_cap[HT_CAP_LEN];
};

struct registry_priv {
	u8 ht_enable;
	u8 wireless_mode;
	u8 ampu_enable;
};

struct mlme_ext_info {
	u32 state;
	u8 ap_ampdu_para;
};

struct mlme_ext_priv {
	u8 cur_bwmode;
	u8 cur_ch_offset;
	struct mlme_ext_info mlmext_info;
};

struct mlme_priv {
	struct ht_priv htpriv;
};

struct sta_info {
	int flags;
	u32 qos_option;
	struct ht_priv htpriv;
	u8 bw_mode;
};

struct _adapter {
	struct registry_priv registrypriv;
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
};

typedef struct _adapter _adapter;

static inline u8 is_supported_ht(u8 mode)
{
	return (mode & (WIRELESS_11_24N | WIRELESS_11_5N)) ? _TRUE : _FALSE;
}

static inline u16 ht_cap_info(const u8 *cap)
{
	u16 v;

	memcpy(&v, cap, sizeof(v));
	return v;
}

void rtw_tdls_process_ht_cap(_adapter *padapter, struct sta_info *ptdls_sta, u8 *data,
			     u8 length);

#endif /* HOST_TDLS_HT_TYPES_H */
