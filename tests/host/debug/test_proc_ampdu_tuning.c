// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-205 driver AMPDU tuning procs. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_ampdu_types.h"
#include "host_vector_json.h"

struct host_proc_seq {
	struct _adapter *adapter;
	FILE *out;
};

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_FIELD 32
#define MAX_EXPECT 128
#define MAX_INPUT 32

struct get_vector {
	char name[MAX_NAME];
	char field[MAX_FIELD];
	u8 value;
	char expect[MAX_EXPECT];
};

struct set_vector {
	char name[MAX_NAME];
	char field[MAX_FIELD];
	char input[MAX_INPUT];
	u8 expect;
};

static int parse_u8_field(const char *obj, size_t len, const char *key, u8 *out)
{
	int v;

	if (host_json_parse_int_in(obj, len, key, &v))
		return -1;
	*out = (u8)v;
	return 0;
}

static int parse_get_object(const char *obj, size_t len, void *vec_void)
{
	struct get_vector *v = vec_void;
	char *p;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "field", v->field, sizeof(v->field)))
		return -1;
	if (parse_u8_field(obj, len, "value", &v->value))
		return -1;
	if (host_json_parse_string_in(obj, len, "expect", v->expect, sizeof(v->expect)))
		return -1;
	for (p = v->expect; *p; p++) {
		if (p[0] == '\\' && p[1] == 'n') {
			p[0] = '\n';
			memmove(p + 1, p + 2, strlen(p + 2) + 1);
		}
	}
	return 0;
}

static int parse_set_object(const char *obj, size_t len, void *vec_void)
{
	struct set_vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "field", v->field, sizeof(v->field)))
		return -1;
	if (host_json_parse_string_in(obj, len, "input", v->input, sizeof(v->input)))
		return -1;
	if (parse_u8_field(obj, len, "expect", &v->expect))
		return -1;
	return 0;
}

static int run_get(const struct get_vector *v)
{
	struct _adapter adapter;
	struct host_proc_seq ctx;
	char got[MAX_EXPECT];
	size_t nread;
	FILE *f;

	memset(&adapter, 0, sizeof(adapter));
	if (!strcmp(v->field, "rx_factor"))
		adapter.driver_rx_ampdu_factor = v->value;
	else if (!strcmp(v->field, "rx_density"))
		adapter.driver_rx_ampdu_spacing = v->value;
	else if (!strcmp(v->field, "tx_density"))
		adapter.driver_ampdu_spacing = v->value;
	else if (!strcmp(v->field, "tx_agg"))
		adapter.driver_tx_max_agg_num = v->value;
	else
		return -1;

	f = tmpfile();
	if (!f)
		return -1;
	ctx.adapter = &adapter;
	ctx.out = f;

	if (!strcmp(v->field, "rx_factor"))
		proc_get_rx_ampdu_factor((struct seq_file *)&ctx, NULL);
	else if (!strcmp(v->field, "rx_density"))
		proc_get_rx_ampdu_density((struct seq_file *)&ctx, NULL);
	else if (!strcmp(v->field, "tx_density"))
		proc_get_tx_ampdu_density((struct seq_file *)&ctx, NULL);
	else
		proc_get_tx_max_agg_num((struct seq_file *)&ctx, NULL);

	rewind(f);
	nread = fread(got, 1, sizeof(got) - 1, f);
	fclose(f);
	got[nread] = '\0';
	if (strcmp(got, v->expect) != 0) {
		fprintf(stderr, "FAIL get %s:\n  got:    %s\n  expect: %s\n", v->name,
			got, v->expect);
		return -1;
	}
	return 0;
}

static u8 read_field(const struct _adapter *a, const char *field)
{
	if (!strcmp(field, "rx_factor"))
		return a->driver_rx_ampdu_factor;
	if (!strcmp(field, "rx_density"))
		return a->driver_rx_ampdu_spacing;
	if (!strcmp(field, "tx_density"))
		return a->driver_ampdu_spacing;
	return a->driver_tx_max_agg_num;
}

static int run_set(const struct set_vector *v)
{
	struct _adapter adapter;
	long ret;
	size_t in_len = strlen(v->input);

	memset(&adapter, 0, sizeof(adapter));
	if (!strcmp(v->field, "rx_factor"))
		ret = proc_set_rx_ampdu_factor(NULL, v->input, in_len, NULL, &adapter);
	else if (!strcmp(v->field, "rx_density"))
		ret = proc_set_rx_ampdu_density(NULL, v->input, in_len, NULL, &adapter);
	else if (!strcmp(v->field, "tx_density"))
		ret = proc_set_tx_ampdu_density(NULL, v->input, in_len, NULL, &adapter);
	else
		ret = proc_set_tx_max_agg_num(NULL, v->input, in_len, NULL, &adapter);

	if (ret != (long)in_len)
		return -1;
	if (read_field(&adapter, v->field) != v->expect) {
		fprintf(stderr, "FAIL set %s: field %s got %u expect %u\n", v->name,
			v->field, read_field(&adapter, v->field), v->expect);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *get_path = argc > 1 ? argv[1] :
		"debug_proc_ampdu_tuning_get_vectors.json";
	const char *set_path = argc > 2 ? argv[2] :
		"debug_proc_ampdu_tuning_set_vectors.json";
	struct get_vector get_vectors[MAX_VECTORS];
	struct set_vector set_vectors[MAX_VECTORS];
	size_t get_count = 0;
	size_t set_count = 0;

	if (host_load_vectors(get_path, get_vectors, sizeof(get_vectors[0]), MAX_VECTORS,
			      parse_get_object, &get_count))
		return 1;
	if (host_load_vectors(set_path, set_vectors, sizeof(set_vectors[0]), MAX_VECTORS,
			      parse_set_object, &set_count))
		return 1;

	for (size_t i = 0; i < get_count; i++) {
		if (run_get(&get_vectors[i]))
			return 1;
	}
	for (size_t i = 0; i < set_count; i++) {
		if (run_set(&set_vectors[i]))
			return 1;
	}

	printf("ok: %zu get + %zu set proc ampdu tuning vectors\n", get_count,
	       set_count);
	return 0;
}
