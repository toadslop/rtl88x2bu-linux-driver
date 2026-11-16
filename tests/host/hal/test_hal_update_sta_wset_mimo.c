// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_hal_update_sta_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define KIND_WSET 0
#define KIND_MIMO 1

extern int host_hal_update_sta_warn_on_called(void);

struct vector {
	char name[48];
	int kind;
	u8 wireless_mode;
	s8 tx_nss, rx_nss;
	u8 expect_wset, expect_mimo;
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "kind", &tmp))
		return -1;
	v->kind = tmp;
	if (host_json_parse_int_in(obj, len, "wireless_mode", &tmp))
		return -1;
	v->wireless_mode = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "tx_nss", &tmp))
		return -1;
	v->tx_nss = (s8)tmp;
	if (host_json_parse_int_in(obj, len, "rx_nss", &tmp))
		return -1;
	v->rx_nss = (s8)tmp;
	if (host_json_parse_int_in(obj, len, "expect_wset", &tmp))
		return -1;
	v->expect_wset = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "expect_mimo", &tmp))
		return -1;
	v->expect_mimo = (u8)tmp;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	struct _adapter adp;
	const char *path = argc > 1 ? argv[1] : "hal_update_sta_wset_mimo_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	memset(&adp, 0, sizeof(adp));
	for (size_t i = 0; i < count; i++) {
		struct host_sta_info sta;

		memset(&sta, 0, sizeof(sta));
		host_hal_update_sta_reset_nss();
		host_hal_update_sta_set_nss(vectors[i].tx_nss, vectors[i].rx_nss);
		sta.wireless_mode = vectors[i].wireless_mode;

		if (vectors[i].kind == KIND_WSET) {
			rtw_hal_update_sta_wset(&adp, &sta);
			if (sta.cmn.support_wireless_set != vectors[i].expect_wset) {
				fprintf(stderr, "FAIL %s wset %u != %u\n", vectors[i].name,
					sta.cmn.support_wireless_set, vectors[i].expect_wset);
				return 1;
			}
		} else {
			rtw_hal_update_sta_mimo_type(&adp, &sta);
			if ((u8)sta.cmn.mimo_type != vectors[i].expect_mimo) {
				fprintf(stderr, "FAIL %s mimo %u != %u\n", vectors[i].name,
					(unsigned)sta.cmn.mimo_type, vectors[i].expect_mimo);
				return 1;
			}
			if (host_hal_update_sta_warn_on_called()) {
				fprintf(stderr, "FAIL %s warn_on\n", vectors[i].name);
				return 1;
			}
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}