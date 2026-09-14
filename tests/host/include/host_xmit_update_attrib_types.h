/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_XMIT_UPDATE_ATTRIB_TYPES_H
#define HOST_XMIT_UPDATE_ATTRIB_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0

#define NONE_VCS 0
#define RTS_CTS 1
#define CTS_TO_SELF 2

#define DISABLE_VCS 0
#define ENABLE_VCS 1
#define AUTO_VCS 2

#define WIRELESS_11_24N (1U << 3)
#define HT_IOT_PEER_ATHEROS 5
#define _AES_ 0x04

#define IS_HARDWARE_TYPE_8812(a) 0

struct registry_priv {
	u8 wifi_spec;
	u16 rts_thresh;
	u8 vrtl_carrier_sense;
	u8 vcs_type;
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

struct pkt_attrib {
	u8 nr_frags;
	u32 last_txcmdsz;
	u8 rtsen;
	u8 cts2self;
	u8 ht_en;
	u8 ampdu_en;
	u8 vcs_mode;
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
};

typedef struct _adapter _adapter;

void update_attrib_vcs_info(_adapter *padapter, struct xmit_frame *pxmitframe);

#endif /* HOST_XMIT_UPDATE_ATTRIB_TYPES_H */
