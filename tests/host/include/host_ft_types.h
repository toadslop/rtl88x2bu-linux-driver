/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_FT_TYPES_H
#define HOST_FT_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0

#define RTW_FT_MAX_IE_SZ 256
#define ETH_ALEN 6
#define MAX_IE_SZ 768
#define RTW_FT_EN 0x01
#define RTW_FT_OTD_EN 0x02
#define RTW_FT_PEER_EN 0x04
#define RTW_FT_PEER_OTD_EN 0x08
#define RTW_FT_BTM_ROAM 0x10
#define RTW_FT_TEST_RSSI_ROAM 0x80
#define _MDIE_ 54
#define _FTIE_ 55
#define EID_WPA2 48

typedef int sint;
typedef unsigned int uint;
typedef s32 sint32;

struct ft_event_host {
	u8 *ies;
	u16 ies_len;
	u8 *ric_ies;
	u16 ric_ies_len;
};

struct ft_roam_info {
	u16 mdid;
	u8 ft_cap;
	u8 ft_flags;
	bool ft_updated_bcn;
	u8 updated_ft_ies[RTW_FT_MAX_IE_SZ];
	u16 updated_ft_ies_len;
	struct ft_event_host ft_event;
};

typedef struct {
	u8 MacAddress[ETH_ALEN];
	s32 Rssi;
	u32 IELength;
	u8 IEs[MAX_IE_SZ];
} WLAN_BSSID_EX;

struct wlan_network {
	WLAN_BSSID_EX network;
};

struct mlme_priv {
	sint32 to_roam;
	struct ft_roam_info ft_roam;
	struct wlan_network cur_network;
	u8 *auth_rsp;
	u32 auth_rsp_len;
	u8 assoc_bssid[ETH_ALEN];
};

struct pkt_attrib {
	uint pktlen;
};

struct _adapter {
	struct mlme_priv mlmepriv;
};

typedef struct _adapter _adapter;

#define rtw_to_roam(a) ((a)->mlmepriv.to_roam)
#define rtw_ft_chk_flags(a, f) ((a)->mlmepriv.ft_roam.ft_flags & (f))
#define rtw_ft_roam(a) \
	((rtw_to_roam(a) > 0) && rtw_ft_chk_flags(a, RTW_FT_PEER_EN))
#define rtw_ft_clr_flags(a, f) \
	do { \
		(a)->mlmepriv.ft_roam.ft_flags &= ~(f); \
	} while (0)
#define rtw_ft_valid_otd_candidate(a, p) \
	((rtw_ft_chk_flags(a, RTW_FT_OTD_EN)) && \
	 ((rtw_ft_chk_flags(a, RTW_FT_PEER_OTD_EN) && (*((p) + 4) & 0x01) == 0) || \
	  ((rtw_ft_chk_flags(a, RTW_FT_PEER_OTD_EN) == 0) && (*((p) + 4) & 0x01))))
#define rtw_ft_authed_sta(a) 0
#define RTW_INFO(...) do { } while (0)

extern u8 host_ft_reassoc_called;
extern u8 host_ft_last_reassoc_mac[ETH_ALEN];

int _rtw_memcmp(const void *s1, const void *s2, size_t n);
void rtw_buf_update(u8 **pbuf, u32 *size, u8 *src, u32 len);
void rtw_ft_report_reassoc_evt(_adapter *padapter, u8 *pMacAddr);
u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit);
u8 *rtw_set_ie(u8 *pbuf, sint index, uint len, const u8 *source, uint *frlen);

void host_ft_info_init(struct ft_roam_info *pft);
u8 host_ft_update_rsnie(_adapter *padapter, u8 bwrite, struct pkt_attrib *pattrib,
			u8 **pframe);
u8 host_ft_update_mdie(_adapter *padapter, struct pkt_attrib *pattrib, u8 **pframe);
u8 host_ft_update_ftie(_adapter *padapter, struct pkt_attrib *pattrib, u8 **pframe);
void host_ft_build_auth_req_ies(_adapter *padapter, struct pkt_attrib *pattrib,
				u8 **pframe);
void host_ft_build_assoc_req_ies(_adapter *padapter, u8 is_reassoc,
				 struct pkt_attrib *pattrib, u8 **pframe);
u8 host_ft_chk_roaming_candidate(_adapter *padapter, struct wlan_network *competitor);
u8 host_ft_update_auth_rsp_ies(_adapter *padapter, u8 *pframe, u32 len);
void host_ft_test_adapter_init(_adapter *a);

#endif /* HOST_FT_TYPES_H */
