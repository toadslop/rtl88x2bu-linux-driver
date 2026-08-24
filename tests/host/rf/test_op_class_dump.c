// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for W3-57 op-class dump formatters.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rf_op_class_dump_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 24
#define MAX_NAME 128
#define MAX_EXPECT 512

enum dump_fn {
	FN_VALIDATE = 0,
	FN_DUMP_GLOBAL,
	FN_DUMP_CAP,
	FN_DUMP_REG,
	FN_DUMP_CUR,
};

struct vector {
	char name[MAX_NAME];
	enum dump_fn fn;
	int gid;
	u8 class_id;
	u8 op_ch_num;
	u8 ir_ch_num;
	u8 cap_num;
	u8 reg_num;
	u8 cur_num;
	u8 detail;
	int expect_valid;
	u8 has_expect_valid;
	char expect_contains[MAX_EXPECT];
};

static int parse_fn(const char *obj, size_t len, enum dump_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "dbg_global_op_class_validate"))
		*out = FN_VALIDATE;
	else if (!strcmp(fn, "dump_global_op_class"))
		*out = FN_DUMP_GLOBAL;
	else if (!strcmp(fn, "dump_cap_spt_op_class_ch"))
		*out = FN_DUMP_CAP;
	else if (!strcmp(fn, "dump_reg_spt_op_class_ch"))
		*out = FN_DUMP_REG;
	else if (!strcmp(fn, "dump_cur_spt_op_class_ch"))
		*out = FN_DUMP_CUR;
	else
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, len, &v->fn))
		return -1;
	host_json_parse_string_in(obj, len, "expect_contains", v->expect_contains,
				  sizeof(v->expect_contains));
	if (!host_json_parse_int_in(obj, len, "gid", &tmp))
		v->gid = tmp;
	if (!host_json_parse_int_in(obj, len, "class_id", &tmp))
		v->class_id = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "op_ch_num", &tmp))
		v->op_ch_num = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "ir_ch_num", &tmp))
		v->ir_ch_num = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "cap_num", &tmp))
		v->cap_num = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "reg_num", &tmp))
		v->reg_num = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "cur_num", &tmp))
		v->cur_num = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "detail", &tmp))
		v->detail = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "expect_valid", &tmp)) {
		v->expect_valid = tmp;
		v->has_expect_valid = 1;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	struct rf_ctl_t rfctl = {0};

	host_rf_op_class_dump_reset(&rfctl);
	host_sel_reset();

	switch (v->fn) {
	case FN_VALIDATE: {
		bool ok = dbg_global_op_class_validate((u8)v->gid);

		if (v->has_expect_valid && ok != (bool)v->expect_valid) {
			fprintf(stderr, "%s: validate gid=%d got %d expect %d\n",
				v->name, v->gid, ok, v->expect_valid);
			return -1;
		}
		break;
	}
	case FN_DUMP_GLOBAL:
		dump_global_op_class(NULL);
		break;
	case FN_DUMP_CAP:
		host_rf_op_class_dump_add_pref(&rfctl, v->class_id, v->op_ch_num,
					       v->ir_ch_num);
		rfctl.cap_spt_op_class_num = v->cap_num;
		dump_cap_spt_op_class_ch(NULL, &rfctl, v->detail);
		break;
	case FN_DUMP_REG:
		host_rf_op_class_dump_add_pref(&rfctl, v->class_id, v->op_ch_num,
					       v->ir_ch_num);
		rfctl.reg_spt_op_class_num = v->reg_num;
		dump_reg_spt_op_class_ch(NULL, &rfctl, v->detail);
		break;
	case FN_DUMP_CUR:
		host_rf_op_class_dump_add_pref(&rfctl, v->class_id, v->op_ch_num,
					       v->ir_ch_num);
		rfctl.cur_spt_op_class_num = v->cur_num;
		dump_cur_spt_op_class_ch(NULL, &rfctl, v->detail);
		break;
	default:
		return -1;
	}

	if (v->expect_contains[0] &&
	    !strstr(host_sel_out.buf, v->expect_contains)) {
		fprintf(stderr, "%s: output missing %s\ngot:\n%s\n",
			v->name, v->expect_contains, host_sel_out.buf);
		return -1;
	}

	host_rf_op_class_dump_reset(&rfctl);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t nvec = 0;
	int failed = 0;
	const char *path = "op_class_dump_vectors.json";

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "load %s failed\n", path);
		return 1;
	}

	for (size_t i = 0; i < nvec; i++) {
		if (run_vector(&vectors[i]))
			failed++;
	}

	printf("op_class_dump: %zu vectors, %d failures\n", nvec, failed);
	return failed ? 1 : 0;
}
