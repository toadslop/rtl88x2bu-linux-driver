/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal 802.11 / driver types for host GCMP differential tests (W2-02).
 * Layout matches linux/ieee80211.h ieee80211_hdr and a stub _adapter.
 */
#ifndef HOST_WIFI_TYPES_H
#define HOST_WIFI_TYPES_H

#include "host_types.h"

#define ETH_ALEN 6

#define WLAN_FC_PVER 0x0003
#define WLAN_FC_TODS 0x0100
#define WLAN_FC_FROMDS 0x0200
#define WLAN_FC_MOREFRAG 0x0400
#define WLAN_FC_RETRY 0x0800
#define WLAN_FC_PWRMGT 0x1000
#define WLAN_FC_MOREDATA 0x2000
#define WLAN_FC_ISWEP 0x4000
#define WLAN_FC_ORDER 0x8000

#define RTW_IEEE80211_FTYPE_MGMT 0x0000
#define RTW_IEEE80211_FTYPE_DATA 0x0008
#define RTW_IEEE80211_STYPE_QOS_DATA 0x0080
#define RTW_IEEE80211_SCTL_FRAG 0x000f
#define RTW_IEEE80211_SCTL_SEQ 0xfff0

#define WLAN_FC_TYPE_DATA RTW_IEEE80211_FTYPE_DATA
#define WLAN_FC_TYPE_MGMT RTW_IEEE80211_FTYPE_MGMT
#define WLAN_FC_STYPE_QOS_DATA RTW_IEEE80211_STYPE_QOS_DATA

#define WLAN_FC_GET_TYPE(fc) ((fc) & 0x000c)
#define WLAN_FC_GET_STYPE(fc) ((fc) & 0x00f0)
#define WLAN_GET_SEQ_FRAG(seq) ((seq) & RTW_IEEE80211_SCTL_FRAG)
#define WLAN_GET_SEQ_SEQ(seq) ((seq) & RTW_IEEE80211_SCTL_SEQ)

#define GetAddr1Ptr(pbuf) ((u8 *)((uintptr_t)(pbuf) + 4))

enum rtw_amsdu_mode {
	RTW_AMSDU_MODE_NON_SPP = 0,
	RTW_AMSDU_MODE_SPP = 1,
	RTW_AMSDU_MODE_ALL_DROP = 2,
};

struct ieee80211_hdr {
	u16 frame_control;
	u16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	u16 seq_ctrl;
	u8 addr4[ETH_ALEN];
} __attribute__((packed));

struct registry_priv {
	u8 amsdu_mode;
};

typedef struct {
	struct registry_priv registrypriv;
} _adapter;

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
static inline u16 le_to_host16(u16 val)
{
	return val;
}
#else
static inline u16 le_to_host16(u16 val)
{
	return (u16)((val & 0xff) << 8) | (val >> 8);
}
#endif

#define WPA_PUT_LE16(a, val)                     \
	do {                                     \
		(a)[1] = (u8)(((u16)(val)) >> 8); \
		(a)[0] = (u8)(((u16)(val)) & 0xff); \
	} while (0)

/* Needed by core/crypto/aes-ccm.c under HOST_CRYPTO_TEST (W2-04a). */
#define WPA_PUT_BE16(a, val)                     \
	do {                                     \
		(a)[0] = (u8)(((u16)(val)) >> 8); \
		(a)[1] = (u8)(((u16)(val)) & 0xff); \
	} while (0)

#define WPA_GET_BE32(a)                                       \
	(((u32)(((const u8 *)(a))[0]) << 24) |                \
	 ((u32)(((const u8 *)(a))[1]) << 16) |                \
	 ((u32)(((const u8 *)(a))[2]) << 8) |                 \
	 ((u32)(((const u8 *)(a))[3])))

#define WPA_PUT_BE32(a, val)                                  \
	do {                                                  \
		(a)[0] = (u8)(((u32)(val)) >> 24) & 0xff;     \
		(a)[1] = (u8)(((u32)(val)) >> 16) & 0xff;     \
		(a)[2] = (u8)(((u32)(val)) >> 8) & 0xff;      \
		(a)[3] = (u8)(((u32)(val)) & 0xff);           \
	} while (0)

#define WPA_PUT_BE64(a, val)                                  \
	do {                                                  \
		WPA_PUT_BE32((a), (u32)((u64)(val) >> 32));    \
		WPA_PUT_BE32((a) + 4, (u32)((u64)(val)));     \
	} while (0)

#define BIT(x) (1U << (x))

static inline int os_memcmp_const(const void *a, const void *b, size_t len)
{
	const u8 *aa = a;
	const u8 *bb = b;
	u8 res = 0;
	size_t i;

	for (i = 0; i < len; i++)
		res |= aa[i] ^ bb[i];
	return res;
}

#endif /* HOST_WIFI_TYPES_H */
