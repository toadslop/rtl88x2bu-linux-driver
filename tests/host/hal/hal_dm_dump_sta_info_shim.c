// SPDX-License-Identifier: GPL-2.0
/* Host stubs for W4-31 dump_sta_info (ch_width_str, HDATA_RATE, rate/SGI shims). */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "host_hal_dm_dump_sta_info_types.h"
#include "host_rf_types.h"

typedef uint8_t u8;

#define SM_PS_STATIC 0
#define _FALSE 0
#define _TRUE 1

struct ra_sta_info {
	u8 rate_id;
	u8 rssi_level;
	u8 is_first_connect : 1;
	u8 is_support_sgi : 1;
	u8 is_vht_enable : 2;
	u8 disable_ra : 1;
	u8 disable_pt : 1;
	u8 txrx_state : 2;
	u8 is_noisy : 1;
	u8 curr_tx_rate;
	u8 ra_bw_mode;
	u8 curr_tx_bw;
	u8 curr_retry_ratio;
	uint64_t ramask;
};

struct rssi_info {
	int8_t rssi;
};

struct cmn_sta_info {
	u8 bw_mode;
	u8 mac_id;
	u8 mac_addr[6];
	u8 mimo_type;
	struct rssi_info rssi_stat;
	struct ra_sta_info ra_info;
	u8 sm_ps : 2;
};

struct sta_info {
	struct cmn_sta_info cmn;
	u8 wireless_mode;
	void *padapter;
};

const char *const _ch_width_str[CHANNEL_WIDTH_MAX] = {
	"20MHz", "40MHz", "80MHz", "160MHz", "80_80MHz", "5MHz", "10MHz",
};

extern const char *rtw_rust_hal_com_hdata_rate(u8 hw_rate);
#define HDATA_RATE(rate) rtw_rust_hal_com_hdata_rate((u8)(rate))

static u8 host_stub_tx_rate;
static u8 host_stub_tx_sgi;

void host_hal_dm_dump_sta_set_rate_sgi(u8 rate, u8 sgi)
{
	host_stub_tx_rate = rate;
	host_stub_tx_sgi = sgi;
}

u8 rtw_get_current_tx_rate(void *padapter, struct sta_info *psta)
{
	(void)padapter;
	(void)psta;
	return host_stub_tx_rate;
}

u8 rtw_get_current_tx_sgi(void *padapter, struct sta_info *psta)
{
	(void)padapter;
	(void)psta;
	return host_stub_tx_sgi;
}

struct host_sel_capture {
	char buf[4096];
	size_t len;
};

struct host_sel_capture host_sel_out;

void host_sel_reset(void)
{
	host_sel_out.len = 0;
	host_sel_out.buf[0] = '\0';
}

#define RTW_PRINT_SEL(sel, fmt, ...) \
	do { \
		(void)(sel); \
		size_t _rem = sizeof(host_sel_out.buf) - host_sel_out.len; \
		int _n = snprintf(host_sel_out.buf + host_sel_out.len, _rem, \
				  fmt, ##__VA_ARGS__); \
		if (_n > 0) { \
			if ((size_t)_n >= _rem) \
				host_sel_out.len = sizeof(host_sel_out.buf) - 1; \
			else \
				host_sel_out.len += (size_t)_n; \
			host_sel_out.buf[host_sel_out.len] = '\0'; \
		} \
	} while (0)

void dump_sta_info(void *sel, void *psta);

void host_hal_dm_dump_sta_apply_fixture(struct sta_info *psta,
					const struct host_hal_dm_dump_sta_fixture *fx)
{
	memset(psta, 0, sizeof(*psta));
	memcpy(psta->cmn.mac_addr, fx->mac_addr, sizeof(psta->cmn.mac_addr));
	psta->cmn.mac_id = fx->mac_id;
	psta->wireless_mode = fx->wireless_mode;
	psta->cmn.mimo_type = fx->mimo_type;
	psta->cmn.sm_ps = fx->sm_ps;
	psta->cmn.bw_mode = fx->bw_mode;
	psta->cmn.rssi_stat.rssi = fx->rssi;
	psta->cmn.ra_info.rate_id = fx->rate_id;
	psta->cmn.ra_info.rssi_level = fx->rssi_level;
	psta->cmn.ra_info.is_support_sgi = fx->is_support_sgi ? 1 : 0;
	psta->cmn.ra_info.is_vht_enable = fx->is_vht_enable ? 1 : 0;
	psta->cmn.ra_info.disable_ra = fx->disable_ra ? 1 : 0;
	psta->cmn.ra_info.disable_pt = fx->disable_pt ? 1 : 0;
	psta->cmn.ra_info.is_noisy = fx->is_noisy ? 1 : 0;
	psta->cmn.ra_info.txrx_state = fx->txrx_state;
	psta->cmn.ra_info.ra_bw_mode = fx->ra_bw_mode;
	psta->cmn.ra_info.curr_tx_bw = fx->curr_tx_bw;
	psta->cmn.ra_info.curr_retry_ratio = fx->curr_retry_ratio;
	psta->cmn.ra_info.ramask = fx->ramask;
	host_hal_dm_dump_sta_set_rate_sgi(fx->stub_tx_rate, fx->stub_tx_sgi);
}

#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(x) \
	((u8 *)(x))[0], ((u8 *)(x))[1], ((u8 *)(x))[2], ((u8 *)(x))[3], \
		((u8 *)(x))[4], ((u8 *)(x))[5]

void dump_sta_info(void *sel, void *psta_void)
{
	struct sta_info *psta = (struct sta_info *)psta_void;
	struct ra_sta_info *ra_info;
	u8 curr_tx_sgi = _FALSE;
	u8 curr_tx_rate = 0;

	if (!psta)
		return;

	ra_info = &psta->cmn.ra_info;

	RTW_PRINT_SEL(sel, "============ STA [" MAC_FMT "]  ===================\n",
		      MAC_ARG(psta->cmn.mac_addr));
	RTW_PRINT_SEL(sel, "mac_id : %d\n", psta->cmn.mac_id);
	RTW_PRINT_SEL(sel, "wireless_mode : 0x%02x\n", psta->wireless_mode);
	RTW_PRINT_SEL(sel, "mimo_type : %d\n", psta->cmn.mimo_type);
	RTW_PRINT_SEL(sel, "static smps : %s\n",
		      (psta->cmn.sm_ps == SM_PS_STATIC) ? "Y" : "N");
	RTW_PRINT_SEL(sel, "bw_mode : %s, ra_bw_mode : %s\n",
		      ch_width_str(psta->cmn.bw_mode),
		      ch_width_str(ra_info->ra_bw_mode));
	RTW_PRINT_SEL(sel, "rate_id : %d\n", ra_info->rate_id);
	RTW_PRINT_SEL(sel, "rssi : %d (%%), rssi_level : %d\n",
		      psta->cmn.rssi_stat.rssi, ra_info->rssi_level);
	RTW_PRINT_SEL(sel, "is_support_sgi : %s, is_vht_enable : %s\n",
		      (ra_info->is_support_sgi) ? "Y" : "N",
		      (ra_info->is_vht_enable) ? "Y" : "N");
	RTW_PRINT_SEL(sel, "disable_ra : %s, disable_pt : %s\n",
		      (ra_info->disable_ra) ? "Y" : "N",
		      (ra_info->disable_pt) ? "Y" : "N");
	RTW_PRINT_SEL(sel, "is_noisy : %s\n", (ra_info->is_noisy) ? "Y" : "N");
	RTW_PRINT_SEL(sel, "txrx_state : %d\n", ra_info->txrx_state);

	curr_tx_sgi = rtw_get_current_tx_sgi(psta->padapter, psta);
	curr_tx_rate = rtw_get_current_tx_rate(psta->padapter, psta);
	RTW_PRINT_SEL(sel, "curr_tx_rate : %s (%s)\n", HDATA_RATE(curr_tx_rate),
		      (curr_tx_sgi) ? "S" : "L");
	RTW_PRINT_SEL(sel, "curr_tx_bw : %s\n", ch_width_str(ra_info->curr_tx_bw));
	RTW_PRINT_SEL(sel, "curr_retry_ratio : %d\n", ra_info->curr_retry_ratio);
	RTW_PRINT_SEL(sel, "ra_mask : 0x%016llx\n\n", (unsigned long long)ra_info->ramask);
}

int dump_sta_info_format(const struct host_hal_dm_dump_sta_fixture *fx, char *buf,
			 size_t buflen)
{
	struct sta_info psta;

	if (!fx || !buf || buflen == 0)
		return -1;

	host_sel_reset();
	host_hal_dm_dump_sta_apply_fixture(&psta, fx);
	dump_sta_info((void *)1, &psta);
	if (host_sel_out.len == 0) {
		buf[0] = '\0';
		return 0;
	}
	if (host_sel_out.len >= buflen)
		return -1;
	memcpy(buf, host_sel_out.buf, host_sel_out.len + 1);
	return (int)host_sel_out.len;
}
