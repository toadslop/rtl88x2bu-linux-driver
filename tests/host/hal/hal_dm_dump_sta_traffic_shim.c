// SPDX-License-Identifier: GPL-2.0
/* Host stubs for W4-32 dump_sta_traffic (BCN/AP, rate, TP formatters). */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "host_hal_dm_dump_sta_traffic_types.h"
#include "host_rf_types.h"

typedef uint8_t u8;
typedef uint32_t u32;

#define _FALSE 0
#define _TRUE 1

struct rssi_info {
	int8_t rssi;
};

struct cmn_sta_info {
	u8 mac_id;
	u8 mac_addr[6];
	struct rssi_info rssi_stat;
};

struct stainfo_stats {
	u32 tx_tp_kbits;
	u32 rx_tp_kbits;
	u32 smooth_tx_tp_kbits;
	u32 smooth_rx_tp_kbits;
};

struct sta_info {
	struct cmn_sta_info cmn;
	u8 curr_rx_rate;
	u8 curr_rx_rate_bmc;
	struct stainfo_stats sta_stats;
	void *padapter;
};

struct mlme_ext_priv {
	u8 cur_bcn_cnt;
	u8 dtim;
};

struct _adapter {
	struct mlme_ext_priv mlmeextpriv;
};

static u8 host_stub_assoc;
static u8 host_stub_tx_rate;
static u8 host_stub_tx_sgi;
static struct _adapter host_stub_adapter;

void host_hal_dm_dump_sta_traffic_set_assoc(u8 assoc, u8 bcn_cnt, u8 dtim)
{
	host_stub_assoc = assoc;
	host_stub_adapter.mlmeextpriv.cur_bcn_cnt = bcn_cnt;
	host_stub_adapter.mlmeextpriv.dtim = dtim;
}

void host_hal_dm_dump_sta_traffic_set_rate_sgi(u8 rate, u8 sgi)
{
	host_stub_tx_rate = rate;
	host_stub_tx_sgi = sgi;
}

int is_client_associated_to_ap(void *padapter)
{
	(void)padapter;
	return host_stub_assoc ? _TRUE : _FALSE;
}

u8 rtw_get_bcn_cnt(void *adapter)
{
	return ((struct _adapter *)adapter)->mlmeextpriv.cur_bcn_cnt;
}

u8 rtw_get_bcn_dtim_period(void *adapter)
{
	return ((struct _adapter *)adapter)->mlmeextpriv.dtim;
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

extern const char *rtw_rust_hal_com_hdata_rate(u8 hw_rate);
#define HDATA_RATE(rate) rtw_rust_hal_com_hdata_rate((u8)(rate))

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

#define _RTW_PRINT_SEL RTW_PRINT_SEL

#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(x) \
	((u8 *)(x))[0], ((u8 *)(x))[1], ((u8 *)(x))[2], ((u8 *)(x))[3], \
		((u8 *)(x))[4], ((u8 *)(x))[5]

void dump_sta_traffic(void *sel, void *adapter, void *psta_void)
{
	struct sta_info *psta = (struct sta_info *)psta_void;
	u8 curr_sgi = _FALSE;
	u32 tx_tp_mbips, rx_tp_mbips, bi_tp_mbips;

	if (!psta)
		return;

	RTW_PRINT_SEL(sel, "\n");
	RTW_PRINT_SEL(sel, "====== mac_id : %d [" MAC_FMT "] ======\n",
		      psta->cmn.mac_id, MAC_ARG(psta->cmn.mac_addr));

	if (is_client_associated_to_ap(psta->padapter))
		RTW_PRINT_SEL(sel,
			      "BCN counts : %d (per-%d second), DTIM Period:%d\n",
			      rtw_get_bcn_cnt(psta->padapter) / 2, 1,
			      rtw_get_bcn_dtim_period(psta->padapter));

	curr_sgi = rtw_get_current_tx_sgi(adapter, psta);
	RTW_PRINT_SEL(sel,
		      "tx_rate : %s(%s)  rx_rate : %s, rx_rate_bmc : %s, rssi : %d %%\n",
		      HDATA_RATE(rtw_get_current_tx_rate(adapter, psta)),
		      (curr_sgi) ? "S" : "L",
		      HDATA_RATE((psta->curr_rx_rate & 0x7F)),
		      HDATA_RATE((psta->curr_rx_rate_bmc & 0x7F)),
		      psta->cmn.rssi_stat.rssi);

	_RTW_PRINT_SEL(sel, "RTW: [TP] ");
	tx_tp_mbips = psta->sta_stats.tx_tp_kbits >> 10;
	rx_tp_mbips = psta->sta_stats.rx_tp_kbits >> 10;
	bi_tp_mbips = tx_tp_mbips + rx_tp_mbips;

	if (tx_tp_mbips)
		_RTW_PRINT_SEL(sel, "Tx : %d(Mbps) ", tx_tp_mbips);
	else
		_RTW_PRINT_SEL(sel, "Tx : %d(Kbps) ", psta->sta_stats.tx_tp_kbits);

	if (rx_tp_mbips)
		_RTW_PRINT_SEL(sel, "Rx : %d(Mbps) ", rx_tp_mbips);
	else
		_RTW_PRINT_SEL(sel, "Rx : %d(Kbps) ", psta->sta_stats.rx_tp_kbits);

	if (bi_tp_mbips)
		_RTW_PRINT_SEL(sel, "Total : %d(Mbps)\n", bi_tp_mbips);
	else
		_RTW_PRINT_SEL(sel, "Total : %d(Kbps)\n",
			       psta->sta_stats.tx_tp_kbits +
				       psta->sta_stats.rx_tp_kbits);

	_RTW_PRINT_SEL(sel, "RTW: [Smooth TP] ");
	tx_tp_mbips = psta->sta_stats.smooth_tx_tp_kbits >> 10;
	rx_tp_mbips = psta->sta_stats.smooth_rx_tp_kbits >> 10;
	bi_tp_mbips = tx_tp_mbips + rx_tp_mbips;
	if (tx_tp_mbips)
		_RTW_PRINT_SEL(sel, "Tx : %d(Mbps) ", tx_tp_mbips);
	else
		_RTW_PRINT_SEL(sel, "Tx : %d(Kbps) ",
			       psta->sta_stats.smooth_tx_tp_kbits);

	if (rx_tp_mbips)
		_RTW_PRINT_SEL(sel, "Rx : %d(Mbps) ", rx_tp_mbips);
	else
		_RTW_PRINT_SEL(sel, "Rx : %d(Kbps) ",
			       psta->sta_stats.smooth_rx_tp_kbits);

	if (bi_tp_mbips)
		_RTW_PRINT_SEL(sel, "Total : %d(Mbps)\n", bi_tp_mbips);
	else
		_RTW_PRINT_SEL(sel, "Total : %d(Kbps)\n",
			       psta->sta_stats.smooth_tx_tp_kbits +
				       psta->sta_stats.smooth_rx_tp_kbits);
}

void host_hal_dm_dump_sta_traffic_apply_fixture(struct sta_info *psta,
						const struct host_hal_dm_dump_sta_traffic_fixture *fx)
{
	memset(psta, 0, sizeof(*psta));
	memcpy(psta->cmn.mac_addr, fx->mac_addr, sizeof(psta->cmn.mac_addr));
	psta->cmn.mac_id = fx->mac_id;
	psta->cmn.rssi_stat.rssi = fx->rssi;
	psta->curr_rx_rate = fx->curr_rx_rate;
	psta->curr_rx_rate_bmc = fx->curr_rx_rate_bmc;
	psta->sta_stats.tx_tp_kbits = fx->tx_tp_kbits;
	psta->sta_stats.rx_tp_kbits = fx->rx_tp_kbits;
	psta->sta_stats.smooth_tx_tp_kbits = fx->smooth_tx_tp_kbits;
	psta->sta_stats.smooth_rx_tp_kbits = fx->smooth_rx_tp_kbits;
	psta->padapter = &host_stub_adapter;
	host_hal_dm_dump_sta_traffic_set_assoc(fx->associated_to_ap, fx->bcn_cnt,
					       fx->bcn_dtim);
	host_hal_dm_dump_sta_traffic_set_rate_sgi(fx->stub_tx_rate, fx->stub_tx_sgi);
}

int dump_sta_traffic_format(const struct host_hal_dm_dump_sta_traffic_fixture *fx,
			    char *buf, size_t buflen)
{
	struct sta_info psta;

	if (!fx || !buf || buflen == 0)
		return -1;

	host_sel_reset();
	host_hal_dm_dump_sta_traffic_apply_fixture(&psta, fx);
	dump_sta_traffic((void *)1, &host_stub_adapter, &psta);
	if (host_sel_out.len == 0) {
		buf[0] = '\0';
		return 0;
	}
	if (host_sel_out.len >= buflen)
		return -1;
	memcpy(buf, host_sel_out.buf, host_sel_out.len + 1);
	return (int)host_sel_out.len;
}
