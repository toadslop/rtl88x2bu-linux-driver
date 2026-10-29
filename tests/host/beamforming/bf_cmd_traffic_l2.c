// SPDX-License-Identifier: GPL-2.0
/* W3-191 L2: rtw_bf_cmd / attrib / c2h / traffic leaf (core/rtw_beamforming.c). */

#include <stdio.h>
#include <string.h>

#include "host_beamforming_bf_cmd_traffic_oracle.h"
#include "host_vector_json.h"

void bf_host_cmd_traffic_set_time(u32 t);
void bf_host_cmd_traffic_set_phydm_idx(u8 idx);

#define MAX_VECTORS 16
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	int cmd_type;
	int cmd_size;
	int cmd_enqueue;
	int expect_sync_hdl;
	int expect_enqueue;
	int expect_ec_id;
	int sta_g_id;
	int sta_p_aid;
	int expect_g_id;
	int expect_p_aid;
	int c2h_id;
	int c2h_result;
	int expect_cancel;
	int expect_end_period;
	int expect_end_res;
	int bfee_used;
	int bfee_su_cnt;
	int bfee_was_apply;
	int sta_tx_bytes;
	int time_start;
	int time_end;
	int phydm_idx;
	int expect_apply;
	int expect_timer;
	int expect_sound_state;
	int expect_delete;
};

static struct bf_host_cmd_traffic_adpt g_adpt;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->cmd_type = -1;
	v->cmd_enqueue = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "cmd_type", &v->cmd_type);
	host_json_parse_int_in(obj, len, "cmd_size", &v->cmd_size);
	host_json_parse_int_in(obj, len, "cmd_enqueue", &v->cmd_enqueue);
	host_json_parse_int_in(obj, len, "expect_sync_hdl", &v->expect_sync_hdl);
	host_json_parse_int_in(obj, len, "expect_enqueue", &v->expect_enqueue);
	host_json_parse_int_in(obj, len, "expect_ec_id", &v->expect_ec_id);
	host_json_parse_int_in(obj, len, "sta_g_id", &v->sta_g_id);
	host_json_parse_int_in(obj, len, "sta_p_aid", &v->sta_p_aid);
	host_json_parse_int_in(obj, len, "expect_g_id", &v->expect_g_id);
	host_json_parse_int_in(obj, len, "expect_p_aid", &v->expect_p_aid);
	host_json_parse_int_in(obj, len, "c2h_id", &v->c2h_id);
	host_json_parse_int_in(obj, len, "c2h_result", &v->c2h_result);
	host_json_parse_int_in(obj, len, "expect_cancel", &v->expect_cancel);
	host_json_parse_int_in(obj, len, "expect_end_period", &v->expect_end_period);
	host_json_parse_int_in(obj, len, "expect_end_res", &v->expect_end_res);
	host_json_parse_int_in(obj, len, "bfee_used", &v->bfee_used);
	host_json_parse_int_in(obj, len, "bfee_su_cnt", &v->bfee_su_cnt);
	host_json_parse_int_in(obj, len, "bfee_was_apply", &v->bfee_was_apply);
	host_json_parse_int_in(obj, len, "sta_tx_bytes", &v->sta_tx_bytes);
	host_json_parse_int_in(obj, len, "time_start", &v->time_start);
	host_json_parse_int_in(obj, len, "time_end", &v->time_end);
	host_json_parse_int_in(obj, len, "phydm_idx", &v->phydm_idx);
	host_json_parse_int_in(obj, len, "expect_apply", &v->expect_apply);
	host_json_parse_int_in(obj, len, "expect_timer", &v->expect_timer);
	host_json_parse_int_in(obj, len, "expect_sound_state", &v->expect_sound_state);
	host_json_parse_int_in(obj, len, "expect_delete", &v->expect_delete);
	return 0;
}

static void reset_tr(void)
{
	memset(&bf_host_cmd_enqueue_tr, 0, sizeof(bf_host_cmd_enqueue_tr));
	memset(&bf_host_c2h_tr, 0, sizeof(bf_host_c2h_tr));
	memset(&bf_host_traffic_tr, 0, sizeof(bf_host_traffic_tr));
	bf_host_phydm_sounding_idx = 0;
}

static int run_vec(struct vector *v)
{
	u8 ra[6] = {0xaa, 0, 0, 0, 0, 1};
	u8 c2h_buf[4];
	struct bf_host_pkt_attrib attrib;
	struct bf_host_sta *sta;
	struct bf_host_cmd_traffic_info *info;

	memset(&g_adpt, 0, sizeof(g_adpt));
	reset_tr();
	o_bf_init(&g_adpt.base);

	if (v->cmd_type >= 0) {
		o_bf_cmd(&g_adpt, v->cmd_type, ra, v->cmd_size > 0 ? v->cmd_size : 6, (u8)v->cmd_enqueue);
		if ((int)bf_host_cmd_enqueue_tr.sync_cmd_hdl != v->expect_sync_hdl ||
		    (int)bf_host_cmd_enqueue_tr.count != v->expect_enqueue ||
		    (v->expect_ec_id && (int)bf_host_cmd_enqueue_tr.last_ec_id != v->expect_ec_id)) {
			fprintf(stderr, "FAIL %s cmd\n", v->name);
			return 1;
		}
		printf("PASS %s\n", v->name);
		return 0;
	}

	if (v->expect_g_id || v->expect_p_aid) {
		memset(&attrib, 0, sizeof(attrib));
		g_adpt.stapriv.stas[0].used = 1;
		g_adpt.stapriv.stas[0].cmn.g_id = (u8)v->sta_g_id;
		g_adpt.stapriv.stas[0].cmn.p_aid = (u16)v->sta_p_aid;
		sta = &g_adpt.stapriv.stas[0];
		o_bf_update_attrib(&g_adpt, &attrib, sta);
		if ((int)attrib.txbf_g_id != v->expect_g_id || (int)attrib.txbf_p_aid != v->expect_p_aid) {
			fprintf(stderr, "FAIL %s attrib\n", v->name);
			return 1;
		}
		printf("PASS %s\n", v->name);
		return 0;
	}

	if (v->c2h_id) {
		memset(c2h_buf, 0, sizeof(c2h_buf));
		c2h_buf[0] = (u8)v->c2h_result;
		o_bf_c2h_handler(&g_adpt, (u8)v->c2h_id, c2h_buf, 4);
		if ((int)bf_host_c2h_tr.cancel_timer != v->expect_cancel ||
		    (int)bf_host_c2h_tr.end_period_cmd != v->expect_end_period ||
		    (int)bf_host_c2h_tr.end_period_res != v->expect_end_res) {
			fprintf(stderr, "FAIL %s c2h\n", v->name);
			return 1;
		}
		printf("PASS %s\n", v->name);
		return 0;
	}

	info = bf_host_traffic_info(&g_adpt);
	info->beamformee_su_cnt = (u8)v->bfee_su_cnt;
	info->bfee_entry[0].used = (u8)v->bfee_used;
	info->bfee_entry[0].mac_id = 1;
	memcpy(info->bfee_entry[0].mac_addr, ra, BF_HOST_ETH_ALEN);
	info->bfee_entry[0].bApplySounding = (u8)v->bfee_was_apply;
	g_adpt.stapriv.stas[0].used = 1;
	memcpy(g_adpt.stapriv.stas[0].mac_addr, ra, BF_HOST_ETH_ALEN);
	g_adpt.stapriv.stas[0].sta_stats.tx_bytes = (u64)v->sta_tx_bytes;
	bf_host_cmd_traffic_set_phydm_idx((u8)v->phydm_idx);
	bf_host_cmd_traffic_set_time((u32)v->time_start);
	info->bfee_entry[0].tx_timestamp = (u32)v->time_start;
	info->bfee_entry[0].tx_bytes = 0;
	bf_host_cmd_traffic_set_time((u32)v->time_end);
	o_bf_update_traffic(&g_adpt);
	if ((int)bf_host_traffic_tr.bfee0_apply != v->expect_apply ||
	    (int)bf_host_traffic_tr.timer_set != v->expect_timer ||
	    (v->expect_sound_state &&
	     info->sounding_info.state != BF_HOST_SOUNDING_STATE_INIT) ||
	    (int)bf_host_traffic_tr.bfee0_delete != v->expect_delete) {
		fprintf(stderr, "FAIL %s traffic\n", v->name);
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
	const char *path = argc > 1 ? argv[1] : "bf_cmd_traffic_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(struct vector), MAX_VECTORS, parse_vec, &n))
		return 1;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors\n", n);
	return bad ? 1 : 0;
}
