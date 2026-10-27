// SPDX-License-Identifier: GPL-2.0
/* Host L2 runner for W3-177 AP pwrbit / WMM PS data hooks. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_recv_ap_ps_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	char fn[16];
	u16 fc;
	u32 sta_state;
	u8 priority;
	u8 qos_option;
	u8 qos_info;
	u8 uapsd_bk;
	u8 uapsd_be;
	u8 uapsd_vi;
	u8 uapsd_vo;
	unsigned int sleepq_len;
	int expect_stop;
	int has_expect_stop;
	int expect_wakeup;
	int has_expect_wakeup;
	int expect_delivery;
	int has_expect_delivery;
	int expect_null;
	int has_expect_null;
	int expect_null_tid;
	int has_expect_null_tid;
};

static int parse_u8_field(const char *obj, size_t len, const char *key, u8 *out)
{
	int v;

	if (host_json_parse_int_in(obj, len, key, &v))
		return -1;
	*out = (u8)v;
	return 0;
}

static int parse_vec(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	char fc_hex[16];

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "fn", v->fn, sizeof(v->fn)))
		return -1;
	if (host_json_parse_string_in(obj, len, "fc", fc_hex, sizeof(fc_hex)) == 0)
		v->fc = (u16)strtoul(fc_hex, NULL, 16);
	host_json_parse_int_in(obj, len, "sta_state", (int *)&v->sta_state);
	parse_u8_field(obj, len, "priority", &v->priority);
	parse_u8_field(obj, len, "qos_option", &v->qos_option);
	parse_u8_field(obj, len, "qos_info", &v->qos_info);
	parse_u8_field(obj, len, "uapsd_bk", &v->uapsd_bk);
	parse_u8_field(obj, len, "uapsd_be", &v->uapsd_be);
	parse_u8_field(obj, len, "uapsd_vi", &v->uapsd_vi);
	parse_u8_field(obj, len, "uapsd_vo", &v->uapsd_vo);
	host_json_parse_int_in(obj, len, "sleepq_len", (int *)&v->sleepq_len);
	if (host_json_parse_int_in(obj, len, "expect_stop", &v->expect_stop) == 0)
		v->has_expect_stop = 1;
	if (host_json_parse_int_in(obj, len, "expect_wakeup", &v->expect_wakeup) == 0)
		v->has_expect_wakeup = 1;
	if (host_json_parse_int_in(obj, len, "expect_delivery", &v->expect_delivery) == 0)
		v->has_expect_delivery = 1;
	if (host_json_parse_int_in(obj, len, "expect_null", &v->expect_null) == 0)
		v->has_expect_null = 1;
	if (host_json_parse_int_in(obj, len, "expect_null_tid", &v->expect_null_tid) == 0)
		v->has_expect_null_tid = 1;
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	union recv_frame_host frame;
	struct sta_info_host sta;
	u8 rx_buf[32];
	struct host_recv_ap_ps_trace *tr;

	memset(&adapter, 0, sizeof(adapter));
	memset(&frame, 0, sizeof(frame));
	memset(&sta, 0, sizeof(sta));
	memset(rx_buf, 0, sizeof(rx_buf));

	host_recv_ap_ps_reset_trace();
	*(u16 *)rx_buf = v->fc;
	frame.hdr.rx_data = rx_buf;
	frame.hdr.attrib.priority = v->priority;
	sta.state = v->sta_state;
	sta.qos_option = v->qos_option;
	sta.qos_info = v->qos_info;
	sta.uapsd_bk = v->uapsd_bk;
	sta.uapsd_be = v->uapsd_be;
	sta.uapsd_vi = v->uapsd_vi;
	sta.uapsd_vo = v->uapsd_vo;
	sta.sleepq_ac_len = v->sleepq_len;

	if (strcmp(v->fn, "pwrbit") == 0)
		process_pwrbit_data(&adapter, &frame, &sta);
	else if (strcmp(v->fn, "wmmps") == 0)
		process_wmmps_data(&adapter, &frame, &sta);
	else
		return -1;

	tr = host_recv_ap_ps_get_trace();
	if (v->has_expect_stop && (int)tr->stop_sta_xmit != v->expect_stop) {
		fprintf(stderr, "%s: stop_sta_xmit %u expect %d\n", v->name,
			tr->stop_sta_xmit, v->expect_stop);
		return -1;
	}
	if (v->has_expect_wakeup && (int)tr->wakeup_sta_to_xmit != v->expect_wakeup) {
		fprintf(stderr, "%s: wakeup %u expect %d\n", v->name,
			tr->wakeup_sta_to_xmit, v->expect_wakeup);
		return -1;
	}
	if (v->has_expect_delivery && (int)tr->xmit_delivery != v->expect_delivery) {
		fprintf(stderr, "%s: delivery %u expect %d\n", v->name,
			tr->xmit_delivery, v->expect_delivery);
		return -1;
	}
	if (v->has_expect_null && (int)tr->issue_qos_null != v->expect_null) {
		fprintf(stderr, "%s: qos_null %u expect %d\n", v->name,
			tr->issue_qos_null, v->expect_null);
		return -1;
	}
	if (v->has_expect_null_tid && (int)tr->last_qos_tid != v->expect_null_tid) {
		fprintf(stderr, "%s: null tid %u expect %d\n", v->name,
			tr->last_qos_tid, v->expect_null_tid);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path;
	struct vector vectors[MAX_VECTORS];
	size_t n;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <vectors.json>\n", argv[0]);
		return 1;
	}
	path = argv[1];
	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vec, &n)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}
	for (size_t i = 0; i < (size_t)n; i++) {
		if (run_vector(&vectors[i]) != 0)
			return 1;
	}
	printf("ok %zu vectors\n", (size_t)n);
	return 0;
}
