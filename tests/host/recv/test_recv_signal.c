// SPDX-License-Identifier: GPL-2.0
/* L2 harness for W3-178 recv signal/SNAP static tables. */

#include <stdio.h>
#include <string.h>

#include "host_recv_signal.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_SYMBOL 64

struct vector {
	char name[MAX_NAME];
	char symbol[MAX_SYMBOL];
	int offset;
	int len;
	unsigned char expect[HOST_SNAP_HEADER_LEN];
	size_t expect_len;
	int requires_tdls;
	int requires_signal_stat;
};

static int bytes_match(const unsigned char *got, const unsigned char *exp, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++) {
		if (got[i] != exp[i])
			return 0;
	}
	return 1;
}

#ifdef HOST_RECV_SIGNAL_RUST
extern unsigned char rtw_bridge_tunnel_header[];
extern unsigned char rtw_rfc1042_header[];
extern unsigned char SNAP_ETH_TYPE_IPX[];
extern unsigned char SNAP_ETH_TYPE_APPLETALK_AARP[];
#ifdef CONFIG_TDLS
extern unsigned char SNAP_ETH_TYPE_TDLS[];
#endif
#ifdef CONFIG_NEW_SIGNAL_STAT_PROCESS
extern unsigned char signal_stat_calc_profile[];
#endif
#endif

static unsigned char *resolve_table(const char *symbol, unsigned offset)
{
#ifdef HOST_RECV_SIGNAL_RUST
	if (!strcmp(symbol, "host_rtw_bridge_tunnel_header"))
		return rtw_bridge_tunnel_header + offset;
	if (!strcmp(symbol, "host_rtw_rfc1042_header"))
		return rtw_rfc1042_header + offset;
	if (!strcmp(symbol, "host_snap_eth_type_ipx"))
		return SNAP_ETH_TYPE_IPX + offset;
	if (!strcmp(symbol, "host_snap_eth_type_appletalk_aarp"))
		return SNAP_ETH_TYPE_APPLETALK_AARP + offset;
#ifdef CONFIG_TDLS
	if (!strcmp(symbol, "host_snap_eth_type_tdls"))
		return SNAP_ETH_TYPE_TDLS + offset;
#endif
#ifdef CONFIG_NEW_SIGNAL_STAT_PROCESS
	if (!strcmp(symbol, "host_signal_stat_calc_profile"))
		return signal_stat_calc_profile + offset;
#endif
#endif
	if (!strcmp(symbol, "host_rtw_bridge_tunnel_header"))
		return host_rtw_bridge_tunnel_header + offset;
	if (!strcmp(symbol, "host_rtw_rfc1042_header"))
		return host_rtw_rfc1042_header + offset;
	if (!strcmp(symbol, "host_snap_eth_type_ipx"))
		return host_snap_eth_type_ipx + offset;
	if (!strcmp(symbol, "host_snap_eth_type_appletalk_aarp"))
		return host_snap_eth_type_appletalk_aarp + offset;
#ifdef CONFIG_TDLS
	if (!strcmp(symbol, "host_snap_eth_type_tdls"))
		return host_snap_eth_type_tdls + offset;
#endif
#ifdef CONFIG_NEW_SIGNAL_STAT_PROCESS
	if (!strcmp(symbol, "host_signal_stat_calc_profile"))
		return (unsigned char *)host_signal_stat_calc_profile + offset;
#endif
	return NULL;
}

static int skip_vector(const struct vector *v)
{
	if (v->requires_tdls) {
#ifdef CONFIG_TDLS
		return 0;
#else
		return 1;
#endif
	}
	if (v->requires_signal_stat) {
#ifdef CONFIG_NEW_SIGNAL_STAT_PROCESS
		return 0;
#else
		return 1;
#endif
	}
	return 0;
}

static int parse_vector(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[32];

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "symbol", v->symbol, sizeof(v->symbol)))
		return -1;
	if (host_json_parse_int_in(obj, len, "offset", &v->offset))
		return -1;
	if (host_json_parse_int_in(obj, len, "len", &v->len))
		return -1;
	host_json_parse_bool_in(obj, len, "requires_tdls", &v->requires_tdls);
	host_json_parse_bool_in(obj, len, "requires_signal_stat", &v->requires_signal_stat);
	if (host_json_parse_string_in(obj, len, "expect_hex", hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, v->expect, sizeof(v->expect), &v->expect_len))
		return -1;
	return 0;
}

static int run_vector(struct vector *v)
{
	unsigned char *got;

	if (skip_vector(v))
		return 0;

	got = resolve_table(v->symbol, (unsigned)v->offset);
	if (!got) {
		fprintf(stderr, "FAIL %s: unknown symbol %s\n", v->name, v->symbol);
		return 1;
	}
	if ((size_t)v->len != v->expect_len) {
		fprintf(stderr, "FAIL %s: length mismatch\n", v->name);
		return 1;
	}
	if (!bytes_match(got, v->expect, v->expect_len)) {
		fprintf(stderr, "FAIL %s: byte mismatch\n", v->name);
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
	const char *path = argc > 1 ? argv[1] : "recv_signal_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vector, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vector(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
