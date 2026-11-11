// SPDX-License-Identifier: GPL-2.0
/* Host L2 runner for W3-233 TRX ring proc getters. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"
#include "host_rtw_debug_proc_rings_types.h"

struct vector {
	char name[48];
	char get[32];
	char expect_contains[80];
	char expect_contains2[80];
	u32 rx_dma;
	int rx_idx;
	u32 tx_dma;
};

static int parse_vector(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	int t;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "get", v->get, sizeof(v->get)))
		return -1;
	host_json_parse_string_in(obj, len, "expect_contains", v->expect_contains,
				  sizeof(v->expect_contains));
	host_json_parse_string_in(obj, len, "expect_contains2", v->expect_contains2,
				  sizeof(v->expect_contains2));
	if (host_json_parse_int_in(obj, len, "rx_dma", &t) == 0)
		v->rx_dma = (u32)t;
	if (host_json_parse_int_in(obj, len, "rx_idx", &t) == 0)
		v->rx_idx = t;
	if (host_json_parse_int_in(obj, len, "tx_dma", &t) == 0)
		v->tx_dma = (u32)t;
	return 0;
}

static int run_one(const struct vector *v)
{
	struct _adapter a;
	struct net_device d;
	struct seq_file s;
	char *cap = NULL;
	size_t cap_len = 0;
	FILE *mem;

	memset(&a, 0, sizeof(a));
	memset(&d, 0, sizeof(d));
	host_rings_test_reset();

	a.recvpriv.rx_ring[RX_MPDU_QUEUE].dma = v->rx_dma ? v->rx_dma : 0x1000;
	a.recvpriv.rx_ring[RX_MPDU_QUEUE].idx = v->rx_idx ? (unsigned)v->rx_idx : 0;
	a.recvpriv.rxringcount = 0;

	a.xmitpriv.tx_ring[0].dma = v->tx_dma ? v->tx_dma : 0x2000;
	a.xmitpriv.tx_ring[0].entries = 256;
	a.xmitpriv.txringcount[0] = 0;
	a.xmitpriv.dump_txbd_desc = 0;

	d.priv = &a;
	s.private = &d;
	mem = open_memstream(&cap, &cap_len);
	if (!mem)
		return -1;
	s.out = mem;

	if (!strcmp(v->get, "rx_ring"))
		proc_get_rx_ring(&s, NULL);
	else if (!strcmp(v->get, "tx_ring"))
		proc_get_tx_ring(&s, NULL);
	else if (!strcmp(v->get, "tx_ring_ext"))
		proc_get_tx_ring_ext(&s, NULL);
	else {
		fclose(mem);
		free(cap);
		return -1;
	}

	fclose(mem);

	if (v->expect_contains[0] && (!cap || !strstr(cap, v->expect_contains)))
		goto fail;
	if (v->expect_contains2[0] && (!cap || !strstr(cap, v->expect_contains2)))
		goto fail;
	free(cap);
	return 0;

fail:
	free(cap);
	return -1;
}

int main(int argc, char **argv)
{
	struct vector vecs[8];
	size_t n = 0;
	size_t i;

	if (host_load_vectors(
		    argc > 1 ? argv[1] : "debug_proc_rings_vectors.json", vecs,
		    sizeof(vecs[0]), 8, parse_vector, &n))
		return 1;
	for (i = 0; i < n; i++) {
		if (run_one(&vecs[i])) {
			fprintf(stderr, "fail: %s\n", vecs[i].name);
			return 1;
		}
	}
	printf("ok: %zu proc ring vectors\n", n);
	return 0;
}
