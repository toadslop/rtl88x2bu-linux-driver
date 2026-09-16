// SPDX-License-Identifier: GPL-2.0
/* Host shims for update_bmc_sta L2 (W3-80 PR11). */
#include "host_ap_bmc_update_types.h"
#include <string.h>

static u8 host_media_rpt_count;

void host_ap_bmc_sta_reset_hooks(void)
{
	host_media_rpt_count = 0;
}

u8 host_ap_bmc_sta_media_rpt_count(void)
{
	return host_media_rpt_count;
}

void rtw_hal_update_sta_ra_info(struct _adapter *padapter, struct sta_info *psta)
{
	(void)padapter;
	(void)psta;
}

void rtw_sta_media_status_rpt(struct _adapter *padapter, struct sta_info *psta, u8 connected)
{
	(void)padapter;
	(void)psta;
	(void)connected;
	host_media_rpt_count++;
}

static const u8 rtw_basic_rate_cck[4] = {0x02, 0x04, 0x0b, 0x16};
static const u8 rtw_basic_rate_ofdm[3] = {0x0c, 0x12, 0x18};

void update_sta_basic_rate(struct sta_info *psta, u8 wireless_mode)
{
	if (IsSupportedTxCCK(wireless_mode)) {
		memcpy(psta->bssrateset, rtw_basic_rate_cck, 4);
		psta->bssratelen = 4;
	} else {
		memcpy(psta->bssrateset, rtw_basic_rate_ofdm, 3);
		psta->bssratelen = 3;
	}
}
