/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_RSON_TYPES_H
#define HOST_RSON_TYPES_H

#include "host_types.h"
#include <string.h>

#define _TRUE 1
#define _FALSE 0
#define ETH_ALEN 6
#define MAX_IE_SZ 768

#define RTW_RSON_VER 1
#define RTW_RSON_SCORE_NOTCNNT 0x1
#define RTW_RSON_SCORE_MAX 0xFF
#define RTW_RSON_HC_NOTREADY 0xFF
#define RTW_RSON_HC_ROOT 0x0
#define RTW_RSON_ALLOWCONNECT 0x1
#define RTW_RSON_DENYCONNECT 0x0

#define CONFIG_RTW_REPEATER_SON_ID 0x02040608
#define _BEACON_IE_OFFSET_ 12
#define _VENDOR_SPECIFIC_IE_ 221
#define cpu_to_le32(x) ((u32)(x))
#define le32_to_cpup(p) (*(const u32 *)(p))

typedef long NDIS_802_11_RSSI;
typedef unsigned char NDIS_802_11_MAC_ADDRESS[ETH_ALEN];
typedef int sint;
typedef u32 __le32;

struct rtw_rson_struct {
	u8 ver;
	u32 id;
	u8 hopcnt;
	u8 connectible;
	u8 loading;
	u8 res[16];
} __attribute__((__packed__));

struct dvobj_priv {
	struct rtw_rson_struct rson_data;
};

typedef struct _WLAN_BSSID_EX {
	u32 Length;
	NDIS_802_11_MAC_ADDRESS MacAddress;
	u8 Reserved[2];
	u32 Privacy;
	NDIS_802_11_RSSI Rssi;
	u32 IELength;
	u8 IEs[MAX_IE_SZ];
} WLAN_BSSID_EX;

static inline int _rtw_memcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1, s2, n) == 0 ? _TRUE : _FALSE;
}

u8 key_2char2num(u8 hch, u8 lch);
u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit);

u8 rtw_cal_rson_score(struct rtw_rson_struct *cand_rson_data, NDIS_802_11_RSSI rssi);
int is_match_bssid(u8 *mac, u8 bssid_array[][6], int num);
void init_rtw_rson_data(struct dvobj_priv *dvobj);
int str2hexbuf(char *str, u8 *hexbuf, int len);
u8 rtw_rson_varify_ie(u8 *p);
struct wlan_network {
	WLAN_BSSID_EX network;
};

typedef struct _adapter {
	struct dvobj_priv dvobj;
} _adapter;

int rtw_get_rson_struct(WLAN_BSSID_EX *bssid, struct rtw_rson_struct *rson_data);
int rtw_rson_choose(struct wlan_network **candidate, struct wlan_network *competitor);
u32 rtw_rson_append_ie(_adapter *padapter, unsigned char *pframe, u32 *len);

void host_rson_set_block_bssid_count(u8 n);
void host_rson_set_block_bssid(u8 idx, const u8 mac[ETH_ALEN]);
void host_rson_set_root_bssid_count(u8 n);
void host_rson_set_root_bssid(u8 idx, const u8 mac[ETH_ALEN]);

#endif /* HOST_RSON_TYPES_H */
