// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-174 traffic_check_for_leave_lps_by_tp. */

#include <stdio.h>
#include <string.h>

#include "host_pwrctrl_lps_tp.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	int tx;
	int tx_bytes;
	int acc_tx_bytes;
	int rx_bytes;
	int acc_rx_bytes;
	int lps_tx_tp_th;
	int lps_rx_tp_th;
	int lps_tx_pkts;
	int lps_rx_pkts;
	int num_tx_ok_in_period;
	int num_rx_unicast_ok_in_period;
	int b_leisure_ps;
	int pwr_mode;
	int lps_chk_cnt_th;
	int lps_chk_cnt;
	int bt_control_lps;
	int expect_lps_ctrl;
	int expect_lps_ctrl_type;
	int has_expect_lps_ctrl_type;
	int expect_lps_chk_cnt;
	int has_expect_lps_chk_cnt;
};

static _adapter g_adapter;
static struct sta_info g_sta;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "tx", &v->tx);
	host_json_parse_int_in(obj, len, "tx_bytes", &v->tx_bytes);
	host_json_parse_int_in(obj, len, "acc_tx_bytes", &v->acc_tx_bytes);
	host_json_parse_int_in(obj, len, "rx_bytes", &v->rx_bytes);
	host_json_parse_int_in(obj, len, "acc_rx_bytes", &v->acc_rx_bytes);
	host_json_parse_int_in(obj, len, "lps_tx_tp_th", &v->lps_tx_tp_th);
	host_json_parse_int_in(obj, len, "lps_rx_tp_th", &v->lps_rx_tp_th);
	host_json_parse_int_in(obj, len, "lps_tx_pkts", &v->lps_tx_pkts);
	host_json_parse_int_in(obj, len, "lps_rx_pkts", &v->lps_rx_pkts);
	host_json_parse_int_in(obj, len, "num_tx_ok_in_period", &v->num_tx_ok_in_period);
	host_json_parse_int_in(obj, len, "num_rx_unicast_ok_in_period",
			      &v->num_rx_unicast_ok_in_period);
	host_json_parse_int_in(obj, len, "b_leisure_ps", &v->b_leisure_ps);
	host_json_parse_int_in(obj, len, "pwr_mode", &v->pwr_mode);
	host_json_parse_int_in(obj, len, "lps_chk_cnt_th", &v->lps_chk_cnt_th);
	host_json_parse_int_in(obj, len, "lps_chk_cnt", &v->lps_chk_cnt);
	host_json_parse_int_in(obj, len, "bt_control_lps", &v->bt_control_lps);
	host_json_parse_int_in(obj, len, "expect_lps_ctrl", &v->expect_lps_ctrl);
	if (host_json_parse_int_in(obj, len, "expect_lps_ctrl_type",
				   &v->expect_lps_ctrl_type) == 0)
		v->has_expect_lps_ctrl_type = 1;
	if (host_json_parse_int_in(obj, len, "expect_lps_chk_cnt", &v->expect_lps_chk_cnt) ==
	    0)
		v->has_expect_lps_chk_cnt = 1;
	return 0;
}

static int run_vec(struct vector *v)
{
	struct pwrctrl_priv *p;
	struct host_pwrctrl_lps_tp_trace *tr;

	memset(&g_adapter, 0, sizeof(g_adapter));
	memset(&g_sta, 0, sizeof(g_sta));
	host_pwrctrl_lps_tp_reset_trace();
	host_pwrctrl_lps_tp_set_bt_control_lps((u8)v->bt_control_lps);

	p = adapter_to_pwrctl(&g_adapter);
	p->bLeisurePs = (u8)v->b_leisure_ps;
	p->pwr_mode = (u8)v->pwr_mode;
	p->lps_tx_tp_th = (u16)v->lps_tx_tp_th;
	p->lps_rx_tp_th = (u16)v->lps_rx_tp_th;
	p->lps_tx_pkts = (u32)v->lps_tx_pkts;
	p->lps_rx_pkts = (u32)v->lps_rx_pkts;
	p->lps_chk_cnt_th = v->lps_chk_cnt_th;
	p->lps_chk_cnt = v->lps_chk_cnt;
	g_adapter.mlmepriv.LinkDetectInfo.NumTxOkInPeriod = (u32)v->num_tx_ok_in_period;
	g_adapter.mlmepriv.LinkDetectInfo.NumRxUnicastOkInPeriod =
		(u32)v->num_rx_unicast_ok_in_period;
	g_sta.sta_stats.tx_bytes = (u64)v->tx_bytes;
	g_sta.sta_stats.acc_tx_bytes = (u64)v->acc_tx_bytes;
	g_sta.sta_stats.rx_bytes = (u64)v->rx_bytes;
	g_sta.sta_stats.acc_rx_bytes = (u64)v->acc_rx_bytes;

	traffic_check_for_leave_lps_by_tp(&g_adapter, (u8)v->tx, &g_sta);

	tr = host_pwrctrl_lps_tp_get_trace();
	if ((int)tr->lps_ctrl_calls != v->expect_lps_ctrl) {
		fprintf(stderr, "FAIL %s lps_ctrl_calls=%u expect %d\n", v->name,
			tr->lps_ctrl_calls, v->expect_lps_ctrl);
		return 1;
	}
	if (v->has_expect_lps_ctrl_type &&
	    (int)tr->last_lps_ctrl_type != v->expect_lps_ctrl_type) {
		fprintf(stderr, "FAIL %s lps_ctrl_type=%u expect %d\n", v->name,
			tr->last_lps_ctrl_type, v->expect_lps_ctrl_type);
		return 1;
	}
	if (v->has_expect_lps_chk_cnt && p->lps_chk_cnt != v->expect_lps_chk_cnt) {
		fprintf(stderr, "FAIL %s lps_chk_cnt=%d expect %d\n", v->name, p->lps_chk_cnt,
			v->expect_lps_chk_cnt);
		return 1;
	}

	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "lps_tp_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
