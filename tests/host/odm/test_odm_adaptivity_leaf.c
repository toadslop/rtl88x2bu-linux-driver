// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_odm_adaptivity_types.h"
#include "host_vector_json.h"

struct vector {
	char name[48];
	char op[24];
	char expect[512];
	char expect_contains[128];
	int adaptivity_en;
	int adaptivity_mode;
	int th_l2h_ini;
	int th_edcca_hl_diff;
	int rx_rate;
	int rssi_a;
	int rssi_b;
	int expect_bool;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_string_in(obj, len, "expect", v->expect, sizeof(v->expect));
	host_json_parse_string_in(obj, len, "expect_contains", v->expect_contains,
				  sizeof(v->expect_contains));
	host_json_parse_int_in(obj, len, "adaptivity_en", &v->adaptivity_en);
	host_json_parse_int_in(obj, len, "adaptivity_mode", &v->adaptivity_mode);
	host_json_parse_int_in(obj, len, "th_l2h_ini", &v->th_l2h_ini);
	host_json_parse_int_in(obj, len, "th_edcca_hl_diff", &v->th_edcca_hl_diff);
	host_json_parse_int_in(obj, len, "rx_rate", &v->rx_rate);
	host_json_parse_int_in(obj, len, "rssi_a", &v->rssi_a);
	host_json_parse_int_in(obj, len, "rssi_b", &v->rssi_b);
	host_json_parse_int_in(obj, len, "expect_bool", &v->expect_bool);
	return 0;
}

static int output_matches(struct vector *v)
{
	if (v->expect[0])
		return strcmp(host_sel_out.buf, v->expect) != 0 ? -1 : 0;
	if (v->expect_contains[0])
		return strstr(host_sel_out.buf, v->expect_contains) ? 0 : -1;
	return -1;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	void *sel = (void *)1;

	host_odm_adaptivity_reset(&adapter);
	adapter.registrypriv.adaptivity_en = (u8)v->adaptivity_en;
	adapter.registrypriv.adaptivity_mode = (u8)v->adaptivity_mode;
	{
		struct dm_struct *odm = adapter_to_phydm(&adapter);

		odm->th_l2h_ini = (s8)v->th_l2h_ini;
		odm->th_edcca_hl_diff = (s8)v->th_edcca_hl_diff;
		odm->rx_rate = (u8)v->rx_rate;
		odm->rssi_a = (u8)v->rssi_a;
		odm->rssi_b = (u8)v->rssi_b;
	}

	if (!strcmp(v->op, "needed")) {
		if ((int)rtw_odm_adaptivity_needed(&adapter) != v->expect_bool)
			goto fail;
	} else if (!strcmp(v->op, "en_msg")) {
		rtw_odm_adaptivity_en_msg(sel, &adapter);
		if (output_matches(v))
			goto fail;
	} else if (!strcmp(v->op, "mode_msg")) {
		rtw_odm_adaptivity_mode_msg(sel, &adapter);
		if (output_matches(v))
			goto fail;
	} else if (!strcmp(v->op, "config_msg")) {
		rtw_odm_adaptivity_config_msg(sel, &adapter);
		if (output_matches(v))
			goto fail;
	} else if (!strcmp(v->op, "parm_msg")) {
		rtw_odm_adaptivity_parm_set(&adapter, (s8)v->th_l2h_ini,
					    (s8)v->th_edcca_hl_diff);
		host_sel_reset();
		rtw_odm_adaptivity_parm_msg(sel, &adapter);
		if (output_matches(v))
			goto fail;
	} else if (!strcmp(v->op, "perpkt_rssi")) {
		rtw_odm_get_perpkt_rssi(sel, &adapter);
		if (output_matches(v))
			goto fail;
	} else {
		goto fail;
	}

	printf("PASS %s\n", v->name);
	return 0;
fail:
	fprintf(stderr, "FAIL %s\n", v->name);
	fprintf(stderr, "  got: %s\n", host_sel_out.buf);
	return -1;
}

int main(int argc, char **argv)
{
	struct vector vectors[16];
	size_t nvec = 0;
	int failed = 0;
	const char *path = (argc > 1) ? argv[1] : "odm_adaptivity_leaf_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 16,
			      parse_vector_object, &nvec))
		return 2;
	for (size_t i = 0; i < nvec; i++)
		failed += run_vector(&vectors[i]) != 0;
	if (!failed)
		printf("PASS %zu vectors (%s)\n", nvec, path);
	return failed ? 1 : 0;
}
