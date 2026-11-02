// SPDX-License-Identifier: GPL-2.0
/* W3-201 PR1: host L2 runner for txvector + STA beamform cap leaf. */

#include <stdio.h>
#include <string.h>

#include "host_beamforming_bf_txvector_cap_oracle.h"

#define WIRELESS_11_24N (1u << 3)
#define WIRELESS_11AC (1u << 6)

#ifndef HOST_BF_TXVECTOR_CAP_RUST
extern void o_rtw_bf_get_txvector_parameter(bf_host_txvec_padpt adapter, struct bf_host_txvec_sta *sta,
					    u8 *g_id, u16 *p_aid);
extern void o_rtw_bf_get_sta_beamform_cap(bf_host_txvec_padpt adapter, struct bf_host_txvec_sta *sta,
					  u8 *sta_bf_cap, u8 *sounding_dim, u8 *comp_steering);
#else
void o_rtw_bf_get_txvector_parameter(bf_host_txvec_padpt adapter, struct bf_host_txvec_sta *sta,
				     u8 *g_id, u16 *p_aid);
void o_rtw_bf_get_sta_beamform_cap(bf_host_txvec_padpt adapter, struct bf_host_txvec_sta *sta,
				   u8 *sta_bf_cap, u8 *sounding_dim, u8 *comp_steering);
#endif

static int expect_txvec(bf_host_txvec_padpt adpt, u8 exp_gid, u16 exp_paid, const char *name)
{
	u8 gid;
	u16 paid;
	int bad = 0;

	o_rtw_bf_get_txvector_parameter(adpt, &adpt->sta, &gid, &paid);
	if (gid != exp_gid || paid != exp_paid) {
		fprintf(stderr, "FAIL %s txvec gid=0x%x paid=0x%x (want 0x%x 0x%x)\n", name, gid,
			paid, exp_gid, exp_paid);
		bad++;
	} else {
		printf("PASS %s txvec\n", name);
	}
	return bad;
}

static int expect_cap(bf_host_txvec_padpt adpt, u8 exp_cap, u8 exp_dim, u8 exp_steer,
		      u8 exp_info_cap, const char *name)
{
	u8 cap, dim, steer;
	int bad = 0;

	o_rtw_bf_get_sta_beamform_cap(adpt, &adpt->sta, &cap, &dim, &steer);
	if (cap != exp_cap || dim != exp_dim || steer != exp_steer ||
	    adpt->beamforming_cap != exp_info_cap) {
		fprintf(stderr,
			"FAIL %s cap=0x%x dim=%u steer=%u info=0x%x (want cap=0x%x dim=%u steer=%u info=0x%x)\n",
			name, cap, dim, steer, adpt->beamforming_cap,
			exp_cap, exp_dim, exp_steer, exp_info_cap);
		bad++;
	} else {
		printf("PASS %s cap\n", name);
	}
	return bad;
}

int main(void)
{
	struct bf_host_txvec_adpt adpt;
	int bad = 0;

	memset(&adpt, 0, sizeof(adpt));
	adpt.mlmepriv.fw_state = BF_HOST_TXVEC_WIFI_AP_STATE;
	adpt.mlmepriv.mac_addr[5] = 0x55;
	adpt.sta.cmn.aid = 0x123;
	adpt.sta.wireless_mode = WIRELESS_11_24N;
	bad += expect_txvec(&adpt, 63, 0x123, "ap_partial_aid");

	memset(&adpt, 0, sizeof(adpt));
	adpt.mlmepriv.fw_state = BF_HOST_TXVEC_WIFI_ADHOC_STATE;
	adpt.sta.wireless_mode = WIRELESS_11_24N;
	bad += expect_txvec(&adpt, 63, 0, "adhoc");

	memset(&adpt, 0, sizeof(adpt));
	adpt.sta.cmn.mac_addr[4] = 0xee;
	adpt.sta.cmn.mac_addr[5] = 0xff;
	adpt.sta.wireless_mode = WIRELESS_11_24N;
	bad += expect_txvec(&adpt, 0, 0x1ff, "infra_sta");

	memset(&adpt, 0, sizeof(adpt));
	adpt.mlmepriv.fw_state = BF_HOST_TXVEC_WIFI_AP_STATE;
	adpt.sta.wireless_mode = WIRELESS_11_24N;
	adpt.sta.htpriv.beamform_cap = BF_HOST_TXVEC_HT_BFEE_EN | (2u << 4);
	bad += expect_cap(&adpt, BF_HOST_TXVEC_BFEE_HT_EXPLICIT, 0, 2,
			   BF_HOST_TXVEC_BFER_HT_EXPLICIT, "ap_ht_bfee");

	memset(&adpt, 0, sizeof(adpt));
	adpt.mlmepriv.ht_beamform_cap = BF_HOST_TXVEC_HT_BFER_EN | (3u << 6);
	adpt.sta.wireless_mode = WIRELESS_11_24N | WIRELESS_11AC;
	adpt.mlmepriv.vht_beamform_cap = BF_HOST_TXVEC_VHT_BFEE_EN | (1u << 12);
	bad += expect_cap(&adpt, BF_HOST_TXVEC_BFEE_HT_EXPLICIT | BF_HOST_TXVEC_BFER_VHT_SU, 1,
			   0, BF_HOST_TXVEC_BFER_HT_EXPLICIT | BF_HOST_TXVEC_BFEE_VHT_SU,
			   "infra_ht_vht");

	if (!bad)
		printf("PASS 5 vectors (builtin)\n");
	return bad ? 1 : 0;
}
