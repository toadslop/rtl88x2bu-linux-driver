// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_hal_dm_dyntxpwr_types.h"
#include "host_vector_json.h"

struct vector {
	char name[48];
	char op[16];
	int chip_type, dpt_lv, expect_offset, expect_err;
	int mac_id, dyn_tx_power, support_ability, sta_active, expect_odm_calls;
};

static int parse_ints(const char *obj, size_t len, struct vector *v)
{
	const char *k[] = {"chip_type", "dpt_lv", "expect_offset", "expect_err",
			   "mac_id", "dyn_tx_power", "support_ability", "sta_active",
			   "expect_odm_calls"};
	int *d[] = {&v->chip_type, &v->dpt_lv, &v->expect_offset, &v->expect_err,
		    &v->mac_id, &v->dyn_tx_power, &v->support_ability,
		    &v->sta_active, &v->expect_odm_calls};
	size_t i;

	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op)))
		return -1;
	for (i = 0; i < sizeof(k) / sizeof(k[0]); i++)
		if (host_json_parse_int_in(obj, len, k[i], d[i]))
			return -1;
	return 0;
}

static int run_vector(struct vector *v)
{
	host_dyntxpwr_adapter adapter;
	struct host_cmn_sta_dyntxpwr sta;
	u8 desc[32];

	memset(&adapter, 0, sizeof(adapter));
	memset(&sta, 0, sizeof(sta));
	memset(desc, 0, sizeof(desc));
	adapter.dvobj.chip_type = (u8)v->chip_type;
	adapter.odmpriv.adapter = &adapter;

	host_hal_dm_dyntxpwr_reset();
	if (!strcmp(v->op, "fill_desc")) {
		rtw_phydm_fill_desc_dpt(&adapter.odmpriv, desc, (u8)v->dpt_lv);
	} else if (!strcmp(v->op, "set_dyntxpwr")) {
		adapter.odmpriv.support_ability = (u64)v->support_ability;
		adapter.odmpriv.phydm_sta_info[v->mac_id] = &sta;
		adapter.odmpriv.fill_desc_dyntxpwr = rtw_phydm_fill_desc_dpt;
		if (v->sta_active)
			sta.dm_ctrl = HOST_STA_DM_CTRL_ACTIVE;
		sta.dtp_stat.dyn_tx_power = (u8)v->dyn_tx_power;
		rtw_phydm_set_dyntxpwr(&adapter, desc, (u8)v->mac_id);
		if (host_hal_dm_dyntxpwr_trace.odm_set_calls != (u8)v->expect_odm_calls)
			return -1;
	} else {
		return -1;
	}
	if (!!host_hal_dm_dyntxpwr_trace.err_logged != !!v->expect_err)
		return -1;
	if (host_get_tx_desc_txpwr_ofset_8822b(desc) != (u8)v->expect_offset)
		return -1;
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector v[8];
	size_t n = 0;
	const char *path = argc > 1 ? argv[1] : "hal_dm_dyntxpwr_vectors.json";

	if (host_load_vectors(path, v, sizeof(v[0]), 8,
			      (int (*)(const char *, size_t, void *))parse_ints, &n))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < n; i++)
		if (run_vector(&v[i]))
			return fprintf(stderr, "FAIL %s\n", v[i].name), 1;
	return 0;
}