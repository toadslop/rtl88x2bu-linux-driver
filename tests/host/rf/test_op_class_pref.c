// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-56 op_class_pref lifecycle helpers. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rf_op_class_pref_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 24
#define MAX_NAME 128
#define MAX_CHSET 16

enum opc_pref_fn {
	FN_INIT,
	FN_DEINIT,
	FN_APPLY,
};

struct chset_entry {
	u8 ch;
	u8 flags;
};

struct vector {
	char name[MAX_NAME];
	enum opc_pref_fn fn;
	u8 wireless_mode;
	u8 bw_mode;
	u8 vht_enable;
	u8 band_cap;
	u8 hal_bw_cap;
	u8 reason;
	u8 class_id;
	u8 country_en_11ac;
	u8 dfs_unknown;
	s16 txpwr_mbm;
	int expect_ret;
	int expect_cap_num;
	int expect_reg_num;
	int expect_cur_num;
	int expect_op_ch_num;
	int expect_ir_ch_num;
	int expect_static_non_op;
	int expect_no_ir;
	int expect_max_txpwr;
	size_t chset_len;
	struct chset_entry chset[MAX_CHSET];
};

static int parse_fn(const char *obj, size_t len, enum opc_pref_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "op_class_pref_init"))
		*out = FN_INIT;
	else if (!strcmp(fn, "op_class_pref_deinit"))
		*out = FN_DEINIT;
	else if (!strcmp(fn, "op_class_pref_apply_regulatory"))
		*out = FN_APPLY;
	else
		return -1;
	return 0;
}

static int parse_chset_array(const char *obj, size_t len, struct vector *v)
{
	char arr[512];
	size_t i = 0;
	size_t pos = 0;

	if (host_json_parse_string_in(obj, len, "chset", arr, sizeof(arr)))
		return 0;

	while (arr[pos] && v->chset_len < MAX_CHSET) {
		unsigned ch = 0;
		unsigned flags = 0;

		if (sscanf(arr + pos, "{%u,%u}", &ch, &flags) != 2)
			break;
		v->chset[v->chset_len].ch = (u8)ch;
		v->chset[v->chset_len].flags = (u8)flags;
		v->chset_len++;
		while (arr[pos] && arr[pos] != '}')
			pos++;
		if (arr[pos] == '}')
			pos++;
		while (arr[pos] == ',' || arr[pos] == ' ')
			pos++;
		i++;
		(void)i;
	}
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	v->expect_static_non_op = -1;
	v->expect_no_ir = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, len, "expect_cap_num", &v->expect_cap_num);
	host_json_parse_int_in(obj, len, "expect_reg_num", &v->expect_reg_num);
	host_json_parse_int_in(obj, len, "expect_cur_num", &v->expect_cur_num);
	host_json_parse_int_in(obj, len, "expect_op_ch_num", &v->expect_op_ch_num);
	host_json_parse_int_in(obj, len, "expect_ir_ch_num", &v->expect_ir_ch_num);
	host_json_parse_int_in(obj, len, "expect_static_non_op", &v->expect_static_non_op);
	host_json_parse_int_in(obj, len, "expect_no_ir", &v->expect_no_ir);
	host_json_parse_int_in(obj, len, "expect_max_txpwr", &v->expect_max_txpwr);
	host_json_parse_int_in(obj, len, "wireless_mode", (int *)&v->wireless_mode);
	host_json_parse_int_in(obj, len, "bw_mode", (int *)&v->bw_mode);
	host_json_parse_int_in(obj, len, "vht_enable", (int *)&v->vht_enable);
	host_json_parse_int_in(obj, len, "band_cap", (int *)&v->band_cap);
	host_json_parse_int_in(obj, len, "hal_bw_cap", (int *)&v->hal_bw_cap);
	host_json_parse_int_in(obj, len, "reason", (int *)&v->reason);
	host_json_parse_int_in(obj, len, "class_id", (int *)&v->class_id);
	host_json_parse_int_in(obj, len, "country_en_11ac", (int *)&v->country_en_11ac);
	host_json_parse_int_in(obj, len, "dfs_unknown", (int *)&v->dfs_unknown);
	host_json_parse_int_in(obj, len, "txpwr_mbm", (int *)&v->txpwr_mbm);
	parse_chset_array(obj, len, v);
	return 0;
}

static void setup_adapter(struct vector *v, _adapter *adapter)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	struct registry_priv *regsty = adapter_to_regsty(adapter);
	size_t i;
	static struct country_chplan country;

	host_rf_op_class_pref_reset(adapter);
	if (v->wireless_mode)
		regsty->wireless_mode = v->wireless_mode;
	if (v->bw_mode)
		regsty->bw_mode = v->bw_mode;
	if (v->vht_enable)
		regsty->vht_enable = v->vht_enable;
	if (v->band_cap)
		host_rf_op_class_pref_set_hal(v->band_cap, v->hal_bw_cap ? v->hal_bw_cap :
					    (BW_CAP_20M | BW_CAP_40M | BW_CAP_80M | BW_CAP_160M));
	else if (v->hal_bw_cap)
		host_rf_op_class_pref_set_hal(BAND_CAP_2G | BAND_CAP_5G, v->hal_bw_cap);
	if (v->txpwr_mbm)
		host_rf_op_class_pref_set_txpwr(v->txpwr_mbm);
	host_rf_op_class_pref_set_dfs_unknown(v->dfs_unknown ? _TRUE : _FALSE);

	memset(&country, 0, sizeof(country));
	country.en_11ac = v->country_en_11ac;
	rfctl->country_ent = &country;

	for (i = 0; i < v->chset_len && i < MAX_CHSET; i++) {
		rfctl->channel_set[i].ChannelNum = v->chset[i].ch;
		rfctl->channel_set[i].flags = v->chset[i].flags;
	}
}

static struct op_ch_t *find_op_ch(struct op_class_pref_t *opc, u8 ch)
{
	size_t i;

	if (!opc)
		return NULL;
	for (i = 0; i < MAX_CHANNEL_NUM_OF_BAND && opc->chs[i].ch; i++) {
		if (opc->chs[i].ch == ch)
			return &opc->chs[i];
	}
	return NULL;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	struct rf_ctl_t *rfctl;
	struct op_class_pref_t *opc;
	int ret;

	setup_adapter(v, &adapter);
	rfctl = adapter_to_rfctl(&adapter);

	switch (v->fn) {
	case FN_INIT:
		ret = op_class_pref_init(&adapter);
		if (ret != v->expect_ret) {
			fprintf(stderr, "%s: init ret got %d expect %d\n", v->name, ret,
				v->expect_ret);
			return -1;
		}
		if (v->expect_cap_num &&
		    (int)rfctl->cap_spt_op_class_num != v->expect_cap_num) {
			fprintf(stderr, "%s: cap_spt_op_class_num got %u expect %d\n",
				v->name, rfctl->cap_spt_op_class_num, v->expect_cap_num);
			return -1;
		}
		break;
	case FN_DEINIT:
		op_class_pref_init(&adapter);
		op_class_pref_deinit(&adapter);
		if (rfctl->spt_op_class_ch != NULL) {
			fprintf(stderr, "%s: spt_op_class_ch not NULL after deinit\n",
				v->name);
			return -1;
		}
		break;
	case FN_APPLY:
		ret = op_class_pref_init(&adapter);
		if (ret != _SUCCESS) {
			fprintf(stderr, "%s: init failed before apply\n", v->name);
			return -1;
		}
		op_class_pref_apply_regulatory(&adapter, v->reason);
		if (v->expect_reg_num &&
		    (int)rfctl->reg_spt_op_class_num != v->expect_reg_num) {
			fprintf(stderr, "%s: reg_spt_op_class_num got %u expect %d\n",
				v->name, rfctl->reg_spt_op_class_num, v->expect_reg_num);
			return -1;
		}
		if (v->expect_cur_num &&
		    (int)rfctl->cur_spt_op_class_num != v->expect_cur_num) {
			fprintf(stderr, "%s: cur_spt_op_class_num got %u expect %d\n",
				v->name, rfctl->cur_spt_op_class_num, v->expect_cur_num);
			return -1;
		}
		opc = host_rf_op_class_pref_by_class_id(&adapter, v->class_id);
		if (v->class_id && !opc) {
			fprintf(stderr, "%s: class_id %u not found\n", v->name, v->class_id);
			return -1;
		}
		if (opc && v->expect_op_ch_num &&
		    (int)opc->op_ch_num != v->expect_op_ch_num) {
			fprintf(stderr, "%s: op_ch_num got %u expect %d\n", v->name,
				opc->op_ch_num, v->expect_op_ch_num);
			return -1;
		}
		if (opc && v->expect_ir_ch_num &&
		    (int)opc->ir_ch_num != v->expect_ir_ch_num) {
			fprintf(stderr, "%s: ir_ch_num got %u expect %d\n", v->name,
				opc->ir_ch_num, v->expect_ir_ch_num);
			return -1;
		}
		if (opc && v->chset_len) {
			struct op_ch_t *ch_ent = find_op_ch(opc, v->chset[0].ch);

			if (!ch_ent) {
				fprintf(stderr, "%s: ch %u missing in class %u\n", v->name,
					v->chset[0].ch, v->class_id);
				return -1;
			}
			if (v->expect_static_non_op >= 0 &&
			    (int)ch_ent->static_non_op != v->expect_static_non_op) {
				fprintf(stderr, "%s: ch%u static_non_op got %u expect %d\n",
					v->name, v->chset[0].ch, ch_ent->static_non_op,
					v->expect_static_non_op);
				return -1;
			}
			if (v->expect_no_ir >= 0 && (int)ch_ent->no_ir != v->expect_no_ir) {
				fprintf(stderr, "%s: ch%u no_ir got %u expect %d\n", v->name,
					v->chset[0].ch, ch_ent->no_ir, v->expect_no_ir);
				return -1;
			}
			if (v->expect_max_txpwr &&
			    ch_ent->max_txpwr != v->expect_max_txpwr) {
				fprintf(stderr, "%s: ch%u max_txpwr got %d expect %d\n",
					v->name, v->chset[0].ch, ch_ent->max_txpwr,
					v->expect_max_txpwr);
				return -1;
			}
		}
		op_class_pref_deinit(&adapter);
		break;
	default:
		return -1;
	}

	if (v->fn != FN_APPLY)
		op_class_pref_deinit(&adapter);
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "op_class_pref_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t nvec = 0;
	size_t i;
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
		if (run_vector(&vectors[i]) != 0)
			failed++;
	}

	printf("op_class_pref: %zu vectors, %d failures\n", nvec, failed);
	return failed ? 1 : 0;
}
