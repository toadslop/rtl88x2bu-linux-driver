// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_cmd_traffic_lps_types.h"
#include "host_vector_json.h"

static _adapter g_adapter;

struct vector {
	char name[64];
	int test_fn;
	int lps_ctrl_type, adhoc, from_timer, assoc, lps_chk_by_tp;
	int tx_tp_mbits, rx_tp_mbits, lps_chk_cnt, lps_chk_cnt_th;
	int num_tx, num_rx_unicast;
	int expect_enter_ps;
	int expect_lps_enter, expect_lps_leave, expect_hw_rpt, expect_deny, expect_lps_ctrl_wk;
};

static void setup_watchdog(struct vector *v)
{
	struct pwrctrl_priv *pwr = adapter_to_pwrctl(&g_adapter);

	host_traffic_lps_set_sta(&g_adapter);
	pwr->bLeisurePs = 1;
	pwr->lps_chk_by_tp = (u8)v->lps_chk_by_tp;
	pwr->lps_chk_cnt = v->lps_chk_cnt;
	pwr->lps_chk_cnt_th = v->lps_chk_cnt_th ? v->lps_chk_cnt_th : 2;
	pwr->lps_bi_tp_th = pwr->lps_tx_tp_th = pwr->lps_rx_tp_th = 2;
	g_adapter.stapriv.sta->sta_stats.tx_tp_kbits = (u32)v->tx_tp_mbits << 10;
	g_adapter.stapriv.sta->sta_stats.rx_tp_kbits = (u32)v->rx_tp_mbits << 10;
	g_adapter.mlmepriv.LinkDetectInfo.NumTxOkInPeriod = (u32)v->num_tx;
	g_adapter.mlmepriv.LinkDetectInfo.NumRxUnicastOkInPeriod = (u32)v->num_rx_unicast;
	if (v->assoc)
		g_adapter.mlmepriv.fw_state = WIFI_ASOC_STATE | WIFI_STATION_STATE;
}

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)))
		return -1;
#define I(k, fld) host_json_parse_int_in(o, l, k, &v->fld)
	I("test_fn", test_fn);
	I("lps_ctrl_type", lps_ctrl_type);
	I("adhoc", adhoc);
	I("from_timer", from_timer);
	I("assoc", assoc);
	I("lps_chk_by_tp", lps_chk_by_tp);
	I("tx_tp_mbits", tx_tp_mbits);
	I("rx_tp_mbits", rx_tp_mbits);
	I("lps_chk_cnt", lps_chk_cnt);
	I("lps_chk_cnt_th", lps_chk_cnt_th);
	I("num_tx", num_tx);
	I("num_rx_unicast", num_rx_unicast);
	I("expect_enter_ps", expect_enter_ps);
	I("expect_lps_enter", expect_lps_enter);
	I("expect_lps_leave", expect_lps_leave);
	I("expect_hw_rpt", expect_hw_rpt);
	I("expect_deny", expect_deny);
	I("expect_lps_ctrl_wk", expect_lps_ctrl_wk);
#undef I
	return 0;
}

static int run_vec(struct vector *v)
{
	struct host_traffic_lps_trace *tr;
	u8 ret = 0;

	host_traffic_lps_reset();
	memset(&g_adapter, 0, sizeof(g_adapter));
	tr = host_traffic_lps_get_trace();

	if (v->test_fn == 0) {
		if (v->adhoc)
			g_adapter.mlmepriv.fw_state = WIFI_ADHOC_STATE;
		lps_ctrl_wk_hdl(&g_adapter, (u8)v->lps_ctrl_type, NULL);
		if (tr->lps_enter == v->expect_lps_enter && tr->lps_leave == v->expect_lps_leave &&
		    tr->hw_joinbss_rpt == v->expect_hw_rpt && tr->set_lps_deny == v->expect_deny)
			goto pass;
		goto fail;
	}

	setup_watchdog(v);
	switch (v->test_fn) {
	case 1:
		ret = _lps_chk_by_pkt_cnts(&g_adapter, (u8)v->from_timer, _FALSE);
		break;
	case 2:
		ret = _lps_chk_by_tp(&g_adapter, (u8)v->from_timer);
		break;
	case 3:
		ret = traffic_status_watchdog(&g_adapter, (u8)v->from_timer);
		break;
	default:
		goto fail;
	}
	if ((int)ret == v->expect_enter_ps && tr->lps_enter == v->expect_lps_enter &&
	    tr->lps_leave == v->expect_lps_leave &&
	    tr->lps_ctrl_wk_cmd == v->expect_lps_ctrl_wk)
		goto pass;
fail:
	fprintf(stderr, "FAIL %s\n", v->name);
	return -1;
pass:
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector v[16];
	size_t n = 0;
	int bad = 0;
	const char *p = argc > 1 ? argv[1] : "traffic_lps_vectors.json";

	if (host_load_vectors(p, v, sizeof(v[0]), 16, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&v[i]) != 0;
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, p);
	return bad ? 1 : 0;
}
