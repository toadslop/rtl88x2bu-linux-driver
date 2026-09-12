// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_ap_sta_alive_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	u64 rx_data_pkts;
	u64 last_rx_data_pkts;
	u64 rx_ctrl_pkts;
	u64 last_rx_ctrl_pkts;
	u8 expect_alive;
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_alive", &tmp))
		return -1;
	v->expect_alive = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "rx_data_pkts", &tmp))
		return -1;
	v->rx_data_pkts = (u64)tmp;
	if (host_json_parse_int_in(obj, len, "last_rx_data_pkts", &tmp))
		return -1;
	v->last_rx_data_pkts = (u64)tmp;
	if (host_json_parse_int_in(obj, len, "rx_ctrl_pkts", &tmp))
		return -1;
	v->rx_ctrl_pkts = (u64)tmp;
	if (host_json_parse_int_in(obj, len, "last_rx_ctrl_pkts", &tmp))
		return -1;
	v->last_rx_ctrl_pkts = (u64)tmp;
	return 0;
}

static int run_vector(const struct vector *v)
{
	struct sta_info sta;
	u8 ret;

	memset(&sta, 0, sizeof(sta));
	sta.sta_stats.rx_data_pkts = v->rx_data_pkts;
	sta.sta_stats.last_rx_data_pkts = v->last_rx_data_pkts;
	sta.sta_stats.rx_ctrl_pkts = v->rx_ctrl_pkts;
	sta.sta_stats.last_rx_ctrl_pkts = v->last_rx_ctrl_pkts;

	ret = chk_sta_is_alive(&sta);
	if (ret != v->expect_alive) {
		fprintf(stderr, "FAIL %s: ret=%u expect=%u\n", v->name, ret, v->expect_alive);
		return -1;
	}
	if (sta.sta_stats.last_rx_data_pkts != sta.sta_stats.rx_data_pkts ||
	    sta.sta_stats.last_rx_ctrl_pkts != sta.sta_stats.rx_ctrl_pkts) {
		fprintf(stderr, "FAIL %s: last_rx not synced after call\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector v[16];
	size_t n = 0, i, fail = 0;

	if (argc != 2 ||
	    host_load_vectors(argv[1], v, sizeof(v[0]), 16, parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++)
		if (run_vector(&v[i]))
			fail++;
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
