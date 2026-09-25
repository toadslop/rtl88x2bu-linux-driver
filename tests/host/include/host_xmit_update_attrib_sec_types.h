/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_XMIT_UPDATE_ATTRIB_SEC_TYPES_H
#define HOST_XMIT_UPDATE_ATTRIB_SEC_TYPES_H

#include "host_xmit_update_attrib_types.h"

struct dot11txpn_t {
	u64 val;
};

struct key_t {
	u8 skey[16];
};

struct security_priv_sec_test {
	u8 dot11AuthAlgrthm;
	u8 dot11PrivacyKeyIndex;
	u8 dot118021XGrpKeyid;
	u8 dot118021XGrpPrivacy;
	u8 busetkipkey;
	u8 sw_encrypt;
	u8 hw_decrypted;
	u8 dot118021x_bmc_cam_id;
};

extern struct security_priv_sec_test host_xmit_sec_cfg;
extern u32 host_xmit_sec_passing_ms;

struct sta_info_sec_ext {
	struct sta_info base;
	u8 mac_id;
	u8 ieee8021x_blocked;
	u8 dot118021XPrivacy;
	struct dot11txpn_t dot11txpn;
	struct key_t dot118021x_UncstKey;
	struct key_t dot11tkiptxmickey;
	u64 resp_nonenc_eapol_key_starttime;
};

struct pkt_attrib_sec_ext {
	struct pkt_attrib base;
	u8 ra[6];
	u8 encrypt;
	u8 key_idx;
	u8 iv[32];
	u8 iv_len;
	u8 icv_len;
	u8 bswenc;
	u8 bmc_camid;
	u16 ether_type;
	u8 mac_id;
	struct key_t dot118021x_UncstKey;
	struct key_t dot11tkiptxmickey;
};

int update_attrib_sec_info_l2(_adapter *padapter, struct pkt_attrib_sec_ext *pattrib,
			      struct sta_info_sec_ext *psta, int eapol_type);

#endif
