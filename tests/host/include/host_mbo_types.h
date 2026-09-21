/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MBO_TYPES_H
#define HOST_MBO_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _VENDOR_SPECIFIC_IE_ 221
#define WLAN_EID_VENDOR_SPECIFIC 221
#define RTW_MBO_EID WLAN_EID_VENDOR_SPECIFIC
#define RTW_MBO_ATTR_NPREF_CH_RPT_ID 0x2
#define RTW_MBO_ATTR_CELL_DATA_CAP_ID 0x3
#define RTW_MBO_ATTR_ASSOC_DISABLED_ID 0x4
#define RTW_MBO_ATTR_TRANS_REJ_ID 0x7
#define RTW_MBO_MAX_CH_LIST_NUM 64
#define RTW_MBO_MAX_CH_RPT_NUM 32
#define MAX_IE_SZ 768
#define RTW_INFO(...) do { } while (0)
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(x) (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5]

struct npref_ch {
	u8 op_class;
	u8 chs[RTW_MBO_MAX_CH_LIST_NUM];
	size_t nm_of_ch;
	u8 preference;
	u8 reason;
};

struct npref_ch_rtp {
	struct npref_ch ch_rpt[RTW_MBO_MAX_CH_RPT_NUM];
	size_t nm_of_rpt;
};

struct rf_ctl_t {
	struct npref_ch_rtp ch_rtp;
};

struct pkt_attrib {
	u32 pktlen;
};

typedef struct {
	u8 MacAddress[6];
	u32 IELength;
	u8 IEs[MAX_IE_SZ];
} WLAN_BSSID_EX;

struct wlan_network {
	WLAN_BSSID_EX network;
};

struct _adapter {
	struct rf_ctl_t rf_ctl;
};

typedef struct _adapter _adapter;

#define adapter_to_rfctl(adapter) (&(adapter)->rf_ctl)

int _rtw_memcmp(const void *s1, const void *s2, size_t n);
u8 *rtw_get_ie(const u8 *pbuf, s32 index, s32 *len, s32 limit);
u8 *rtw_set_fixed_ie(u8 *pbuf, unsigned int len, u8 *source, unsigned int *frlen);

u8 *host_mbo_ie_get(u8 *pie, u32 *plen, u32 limit);
u8 *host_mbo_attrs_get(u8 *pie, u32 limit, u8 attr_id, u32 *attr_len);
u32 host_mbo_attr_sz_get(_adapter *padapter, u8 id);
void host_mbo_build_mbo_ie_hdr(u8 **pframe, struct pkt_attrib *pattrib,
			       u8 payload_len);
u8 host_mbo_disallowed_network(struct wlan_network *pnetwork);
u8 host_mbo_non_pref_chan_exist(struct npref_ch *pch, u8 ch);

void host_mbo_adapter_clear(_adapter *a);
void host_mbo_seed_npref(_adapter *a, u8 rpt_idx, u8 op_class, u8 ch_count,
			 const u8 *chs, u8 preference, u8 reason);

#endif /* HOST_MBO_TYPES_H */
