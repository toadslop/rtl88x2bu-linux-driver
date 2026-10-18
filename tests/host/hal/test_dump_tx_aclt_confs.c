// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for dump_tx_aclt_confs (W4-40). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_com_tx_aclt_confs_types.h"
#include "host_vector_json.h"

#ifdef RUST_HAL_COM_TX_ACLT_CONFS_ORACLE
int dump_tx_aclt_confs_format(const host_hal_com_tx_aclt_confs_fixture *fx,
			      char *buf, size_t buflen);
#endif

#define MAX_VECTORS 8
#define MAX_NAME 64
#define MAX_EXPECT 1024

struct vector {
	char name[MAX_NAME];
	host_hal_com_tx_aclt_confs_fixture fx;
	char expect[MAX_EXPECT];
};

static int parse_conf_row(const char *obj, size_t len, int idx,
			  host_hal_com_tx_aclt_conf_row *row)
{
	char key[16];
	int tmp;

	snprintf(key, sizeof(key), "en_%d", idx);
	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	row->en = (uint8_t)tmp;
	snprintf(key, sizeof(key), "vo_vi_%d", idx);
	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	row->vo_vi = (uint32_t)tmp;
	snprintf(key, sizeof(key), "be_bk_%d", idx);
	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	row->be_bk = (uint32_t)tmp;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "tx_aclt_unit_factor", &tmp))
		return -1;
	v->fx.tx_aclt_unit_factor = (uint8_t)tmp;
	for (int i = 0; i < HOST_TX_ACLT_CONF_NUM; i++) {
		if (parse_conf_row(obj, len, i, &v->fx.confs[i]))
			return -1;
	}
	if (host_json_parse_string_in(obj, len, "expect", v->expect,
				      sizeof(v->expect)))
		return -1;
	{
		char *w = v->expect;
		for (char *r = v->expect; *r; r++) {
			if (*r == '\\' && r[1] == 'n') {
				*w++ = '\n';
				r++;
			} else {
				*w++ = *r;
			}
		}
		*w = '\0';
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1]
				    : "hal_dump_tx_aclt_confs_vectors.json";
	char got[MAX_EXPECT];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		int n;

		n = dump_tx_aclt_confs_format(&vectors[i].fx, got, sizeof(got));
		if (n < 0) {
			fprintf(stderr, "FAIL %s: format returned %d\n",
				vectors[i].name, n);
			return 1;
		}
		if (strcmp(got, vectors[i].expect) != 0) {
			fprintf(stderr, "FAIL %s: output mismatch\n",
				vectors[i].name);
			fprintf(stderr, "expected:\n%s\n", vectors[i].expect);
			fprintf(stderr, "got:\n%s\n", got);
			return 1;
		}
		printf("PASS %s\n", vectors[i].name);
	}
	return 0;
}
