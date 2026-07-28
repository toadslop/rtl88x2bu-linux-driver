/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Host L2 types for security_rest TDLS helpers (W3-16).
 */
#ifndef HOST_SECURITY_TDLS_H
#define HOST_SECURITY_TDLS_H

#include "host_security_types.h"

struct host_tdls_peer_key {
	u8 kck[16];
	u8 tk[16];
};

struct host_tdls_sta {
	u8 SNonce[32];
	u8 ANonce[32];
	u8 mac_addr[6];
	struct host_tdls_peer_key tpk;
};

struct host_tdls_mlme_priv {
	u8 bssid[6];
};

struct host_tdls_adapter {
	u8 mac_addr[6];
	struct host_tdls_mlme_priv mlmepriv;
};

void host_rest_tdls_generate_tpk(struct host_tdls_adapter *adapter,
				 struct host_tdls_sta *sta);
int host_rest_wpa_tdls_ftie_mic(u8 *kck, u8 trans_seq, u8 *lnkid, u8 *rsnie,
				u8 *timeoutie, u8 *ftie, u8 *mic);
int host_rest_wpa_tdls_teardown_ftie_mic(u8 *kck, u8 *lnkid, u16 reason,
					 u8 dialog_token, u8 trans_seq, u8 *ftie,
					 u8 *mic);
int host_rest_tdls_verify_mic(u8 *kck, u8 trans_seq, u8 *lnkid, u8 *rsnie,
			      u8 *timeoutie, u8 *ftie);

#endif /* HOST_SECURITY_TDLS_H */
