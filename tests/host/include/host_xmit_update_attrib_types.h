/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_XMIT_UPDATE_ATTRIB_TYPES_H
#define HOST_XMIT_UPDATE_ATTRIB_TYPES_H

#include "host_rf_types.h"
#include "host_types.h"

#define _TRUE 1
#define _FALSE 0

#ifndef BIT
#define BIT(x) (1U << (x))
#endif

#define NONE_VCS 0
#define RTS_CTS 1
#define CTS_TO_SELF 2

#define DISABLE_VCS 0
#define ENABLE_VCS 1
#define AUTO_VCS 2

#define WIRELESS_11_24N (1U << 3)
#define WIRELESS_11_5N (1U << 4)
#define HT_IOT_PEER_ATHEROS 5
#define is_supported_ht(NetType) \
	(((NetType) & (WIRELESS_11_24N | WIRELESS_11_5N)) ? _TRUE : _FALSE)

#define _AES_ 0x04

#define IS_HARDWARE_TYPE_8812(a) 0
#define rtw_min(a, b) ((a) > (b) ? (b) : (a))

struct registry_priv {
	u8 wifi_spec;
	u16 rts_thresh;
	u8 vrtl_carrier_sense;
	u8 vcs_type;
	u8 ht_enable;
	u8 wireless_mode;
};

struct security_priv {
	u8 dot11PrivacyAlgrthm;
};

struct mlme_ext_info {
	u8 assoc_AP_vendor;
	u8 HT_protection;
};

struct mlme_ext_priv {
	u8 cur_wireless_mode;
	u8 cur_bwmode;
	struct mlme_ext_info mlmext_info;
};

struct xmit_priv {
	u32 frag_len;
};

struct ra_info {
	u8 rate_id;
};

struct sta_cmn_info {
	struct ra_info ra_info;
	u8 ldpc_en;
	u8 stbc_en;
	u8 bw_mode;
};

struct ht_priv {
	u8 ht_option;
	u8 ch_offset;
	u8 ampdu_enable;
	u8 agg_enable_bitmap;
	u8 rx_ampdu_min_spacing;
	u8 tx_amsdu_enable;
	u8 sgi_20m;
	u8 sgi_40m;
};

struct vht_priv {
	u8 vht_option;
	u8 sgi_80m;
};

struct sta_info {
	u8 rtsen;
	u8 cts2self;
	struct sta_cmn_info cmn;
	struct ht_priv htpriv;
	struct vht_priv vhtpriv;
};

struct pkt_attrib {
	u8 nr_frags;
	u32 last_txcmdsz;
	u8 rtsen;
	u8 cts2self;
	u8 ht_en;
	u8 ampdu_en;
	u8 vcs_mode;
	u8 mdata;
	u8 eosp;
	u8 triggered;
	u8 ampdu_spacing;
	u8 raid;
	u8 bwmode;
	u8 sgi;
	u8 ldpc;
	u8 stbc;
	u8 ch_offset;
	u8 amsdu_ampdu_en;
	u8 priority;
	u8 retry_ctrl;
};

struct xmit_frame {
	struct pkt_attrib attrib;
};

struct _adapter {
	struct mlme_ext_priv mlmeextpriv;
	struct registry_priv registrypriv;
	struct security_priv securitypriv;
	struct xmit_priv xmitpriv;
	u8 driver_vcs_en;
	u8 driver_vcs_type;
	u8 driver_ampdu_spacing;
};

typedef struct _adapter _adapter;

void update_attrib_vcs_info(_adapter *padapter, struct xmit_frame *pxmitframe);
void update_attrib_phy_info(_adapter *padapter, struct pkt_attrib *pattrib,
			    struct sta_info *psta);

#endif /* HOST_XMIT_UPDATE_ATTRIB_TYPES_H */
