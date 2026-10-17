// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for rtw_rust_hal_phy_query_rf_max_reg (W4-35). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_phy_rf_max_reg_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	int adapter_null;
	int hw_type;
	unsigned expect;
};

uint32_t rtw_rust_hal_phy_query_rf_max_reg(void *adapter);

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (!host_json_parse_int_in(obj, len, "adapter_null", &tmp))
		v->adapter_null = tmp;
	if (!host_json_parse_int_in(obj, len, "hw_type", &tmp))
		v->hw_type = tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->expect = (unsigned)tmp;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path =
		argc > 1 ? argv[1] : "hal_phy_rf_max_reg_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		struct host_hal_phy_rf_max_reg_adapter adapter;
		void *padapter = NULL;
		uint32_t got;

		if (!vectors[i].adapter_null) {
			adapter.hw_type = (uint8_t)vectors[i].hw_type;
			padapter = &adapter;
		}
		got = rtw_rust_hal_phy_query_rf_max_reg(padapter);
		if (got != (uint32_t)vectors[i].expect) {
			fprintf(stderr,
				"FAIL %s: got=%u expect=%u hw_type=%d null=%d\n",
				vectors[i].name, got, vectors[i].expect,
				vectors[i].hw_type, vectors[i].adapter_null);
			return 1;
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
