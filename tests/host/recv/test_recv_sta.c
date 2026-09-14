// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_recv_sta_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_FRAME 128

enum recv_sta_fn { FN_COUNT = 0, FN_VALIDATE };

struct vector {
	char name[64];
	enum recv_sta_fn fn;
	unsigned int frame_len;
	u8 data_rate;
	u8 priority;
	u8 to_fr_ds;
	u32 fw_state;
	u8 ra[ETH_ALEN];
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	u8 sta_mac[ETH_ALEN];
	int expect_ret;
	u32 expect_rx_bytes;
	u64 expect_sta_pkts;
	u64 expect_sta_bytes;
	u32 expect_rate_cnt;
	u64 expect_qos_pkts;
};

static int parse_mac_hex(const char *hex, u8 *out)
{
	unsigned int b[ETH_ALEN];
	int i;

	if (!hex || !hex[0])
		return 0;
	if (sscanf(hex, "%02x%02x%02x%02x%02x%02x",
		   &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]) != ETH_ALEN)
		return -1;
	for (i = 0; i < ETH_ALEN; i++)
		out[i] = (u8)b[i];
	return 0;
}

#define PARSE_OPT_INT(field, key) do { \
	int _t; \
	if (!host_json_parse_int_in(obj, obj_len, key, &_t)) \
		field = (typeof(field))_t; \
} while (0)

#define PARSE_OPT_MAC(field, key) do { \
	char _mb[32]; \
	if (!host_json_parse_string_in(obj, obj_len, key, _mb, sizeof(_mb))) \
		parse_mac_hex(_mb, field); \
} while (0)

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	{
		char fn[32];

		if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
			return -1;
		if (!strcmp(fn, "count_rx_stats"))
			v->fn = FN_COUNT;
		else if (!strcmp(fn, "validate_hdr"))
			v->fn = FN_VALIDATE;
		else
			return -1;
	}
	if (host_json_parse_int_in(obj, obj_len, "frame_len", &tmp))
		return -1;
	v->frame_len = (unsigned int)tmp;
	PARSE_OPT_INT(v->data_rate, "data_rate");
	PARSE_OPT_INT(v->priority, "priority");
	PARSE_OPT_INT(v->to_fr_ds, "to_fr_ds");
	PARSE_OPT_INT(v->fw_state, "fw_state");
	PARSE_OPT_INT(v->expect_ret, "expect_ret");
	PARSE_OPT_INT(v->expect_rx_bytes, "expect_rx_bytes");
	PARSE_OPT_INT(v->expect_sta_pkts, "expect_sta_pkts");
	PARSE_OPT_INT(v->expect_sta_bytes, "expect_sta_bytes");
	PARSE_OPT_INT(v->expect_rate_cnt, "expect_rate_cnt");
	PARSE_OPT_INT(v->expect_qos_pkts, "expect_qos_pkts");
	PARSE_OPT_MAC(v->ra, "ra");
	PARSE_OPT_MAC(v->addr1, "addr1");
	PARSE_OPT_MAC(v->addr2, "addr2");
	PARSE_OPT_MAC(v->addr3, "addr3");
	PARSE_OPT_MAC(v->sta_mac, "sta_mac");

	return 0;
}

static int run_count_vector(const struct vector *v)
{
	_adapter adapter;
	struct sta_info sta;
	union recv_frame frame;
	u8 whdr[MAX_FRAME];

	host_recv_sta_reset();
	memset(&adapter, 0, sizeof(adapter));
	memset(&sta, 0, sizeof(sta));
	memset(&frame, 0, sizeof(frame));
	frame.u.hdr.rx_data = whdr;
	frame.u.hdr.rx_tail = whdr + MAX_FRAME;
	frame.u.hdr.len = v->frame_len;
	memcpy(frame.u.hdr.attrib.ra, v->ra, ETH_ALEN);
	frame.u.hdr.attrib.data_rate = v->data_rate;
	frame.u.hdr.attrib.priority = v->priority;

	host_recv_sta_register_sta(v->ra, &sta);
	count_rx_stats(&adapter, &frame, &sta);

	if ((u32)adapter.recvpriv.rx_bytes != v->expect_rx_bytes)
		return -1;
	if (sta.sta_stats.rx_data_pkts != v->expect_sta_pkts)
		return -1;
	if (sta.sta_stats.rx_bytes != v->expect_sta_bytes)
		return -1;
	if (v->expect_rate_cnt &&
	    sta.sta_stats.rxratecnt[v->data_rate] != v->expect_rate_cnt)
		return -1;
	if (v->expect_qos_pkts &&
	    sta.sta_stats.rx_data_qos_pkts[v->priority] != v->expect_qos_pkts)
		return -1;
	return 0;
}

static int run_validate_vector(const struct vector *v)
{
	_adapter adapter;
	struct sta_info sta;
	union recv_frame frame;
	u8 whdr[MAX_FRAME];
	struct sta_info *psta = NULL;
	int ret;

	host_recv_sta_reset();
	host_recv_sta_set_time(5000, 20000);
	memset(&adapter, 0, sizeof(adapter));
	memset(&sta, 0, sizeof(sta));
	memset(&frame, 0, sizeof(frame));

	adapter.mlmepriv.fw_state = v->fw_state;
	memcpy(adapter.mac_addr, v->addr1, ETH_ALEN);
	memcpy(adapter.mlmepriv.cur_network.network.MacAddress, v->addr3, ETH_ALEN);

	memset(whdr, 0, 24);
	memcpy(whdr + 4, v->addr1, ETH_ALEN);
	memcpy(whdr + 10, v->addr2, ETH_ALEN);
	memcpy(whdr + 16, v->addr3, ETH_ALEN);
	frame.u.hdr.rx_data = whdr;
	frame.u.hdr.rx_tail = whdr + MAX_FRAME;
	frame.u.hdr.len = v->frame_len;
	frame.u.hdr.attrib.to_fr_ds = v->to_fr_ds;

	if (v->sta_mac[0] || v->sta_mac[1])
		host_recv_sta_register_sta(v->sta_mac, &sta);

	ret = rtw_sta_rx_data_validate_hdr(&adapter, &frame, &psta);
	if (ret != v->expect_ret)
		return -1;
	return 0;
}

static int run_vector(const struct vector *v)
{
	if (v->fn == FN_COUNT)
		return run_count_vector(v);
	return run_validate_vector(v);
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0, i, fail = 0;

	if (argc != 2 ||
	    host_load_vectors(argv[1], vecs, sizeof(vecs[0]), MAX_VECTORS,
			      parse_vector_object, &n))
		return 2;

	for (i = 0; i < n; i++) {
		if (run_vector(&vecs[i]) != 0) {
			fprintf(stderr, "FAIL %s\n", vecs[i].name);
			fail++;
		}
	}
	printf("recv_sta: %zu/%zu passed\n", n - fail, n);
	return fail ? 1 : 0;
}
