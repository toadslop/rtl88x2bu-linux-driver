// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle: W4-18 Read/Write/Refresh; W4-19 flag helpers (second target). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64

enum rf_path { RF_PATH_A = 0, RF_PATH_B = 1 };

struct host_rf_shadow_adapter {
	uint8_t is_8188e;
};

#ifndef HOST_HAL_PHY_RF_SHADOW_FLAGS_TEST

struct vector {
	char name[MAX_NAME];
	int rf_path;
	int offset;
	unsigned write_data;
	unsigned expect_read;
};

uint32_t PHY_RFShadowRead(void *adapter, enum rf_path path, uint32_t offset);
void PHY_RFShadowWrite(void *adapter, enum rf_path path, uint32_t offset,
		       uint32_t data);
void PHY_RFShadowRefresh(void *adapter);

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "rf_path", &tmp))
		return -1;
	v->rf_path = tmp;
	if (host_json_parse_int_in(obj, len, "offset", &tmp))
		return -1;
	v->offset = tmp;
	if (host_json_parse_int_in(obj, len, "write_data", &tmp))
		return -1;
	v->write_data = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect_read", &tmp))
		return -1;
	v->expect_read = (unsigned)tmp;
	return 0;
}

static int run_refresh_self_test(void)
{
	struct host_rf_shadow_adapter adapter = {0};

	PHY_RFShadowWrite(NULL, RF_PATH_A, 5, 42);
	PHY_RFShadowRefresh(&adapter);
	if (PHY_RFShadowRead(NULL, RF_PATH_A, 5) != 0) {
		fprintf(stderr, "FAIL refresh_clears_seeded_entry\n");
		return -1;
	}
	printf("PASS refresh_clears_seeded_entry\n");
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path =
		argc > 1 ? argv[1] : "hal_phy_rf_shadow_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		PHY_RFShadowWrite(NULL, (enum rf_path)vectors[i].rf_path,
				  (uint32_t)vectors[i].offset,
				  (uint32_t)vectors[i].write_data);
		uint32_t got = PHY_RFShadowRead(
			NULL, (enum rf_path)vectors[i].rf_path,
			(uint32_t)vectors[i].offset);
		if (got != (uint32_t)vectors[i].expect_read) {
			fprintf(stderr, "FAIL %s: read got=0x%x expect=0x%x\n",
				vectors[i].name, got, vectors[i].expect_read);
			return 1;
		}
		printf("PASS %s\n", vectors[i].name);
	}

	if (run_refresh_self_test())
		return 1;

	printf("PASS: %zu vectors + refresh from %s\n", count, path);
	return 0;
}

#else /* HOST_HAL_PHY_RF_SHADOW_FLAGS_TEST */

struct flag_vector {
	char name[MAX_NAME];
	char op[32];
	int rf_path;
	int offset;
	int flag_type;
	int expect_compare;
	int expect_recover;
};

void PHY_RFShadowRefresh(void *adapter);
void PHY_RFShadowCompareFlagSet(void *adapter, enum rf_path path,
				uint32_t offset, uint8_t type);
void PHY_RFShadowRecorverFlagSet(void *adapter, enum rf_path path,
				 uint32_t offset, uint8_t type);
void PHY_RFShadowCompareFlagSetAll(void *adapter);
void PHY_RFShadowRecorverFlagSetAll(void *adapter);
uint8_t PHY_RFShadowTestPeekCompare(enum rf_path path, uint32_t offset);
uint8_t PHY_RFShadowTestPeekRecorver(enum rf_path path, uint32_t offset);

static int parse_flag_vector(const char *obj, size_t len, void *vec_void)
{
	struct flag_vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op)))
		return -1;
	if (host_json_parse_int_in(obj, len, "rf_path", &tmp))
		return -1;
	v->rf_path = tmp;
	if (host_json_parse_int_in(obj, len, "offset", &tmp))
		return -1;
	v->offset = tmp;
	if (host_json_parse_int_in(obj, len, "flag_type", &tmp))
		return -1;
	v->flag_type = tmp;
	if (!strcmp(v->op, "compare_set")) {
		if (host_json_parse_int_in(obj, len, "expect_compare", &tmp))
			return -1;
		v->expect_compare = tmp;
	} else if (!strcmp(v->op, "recover_set")) {
		if (host_json_parse_int_in(obj, len, "expect_recover", &tmp))
			return -1;
		v->expect_recover = tmp;
	} else {
		return -1;
	}
	return 0;
}

static int run_flag_set_all_self_test(void)
{
	struct host_rf_shadow_adapter adapter = {0};

	PHY_RFShadowRefresh(&adapter);
	PHY_RFShadowCompareFlagSetAll(&adapter);
	if (PHY_RFShadowTestPeekCompare(RF_PATH_A, 0x25) != 0 ||
	    PHY_RFShadowTestPeekCompare(RF_PATH_A, 0x26) != 1 ||
	    PHY_RFShadowTestPeekCompare(RF_PATH_B, 0x26) != 1) {
		fprintf(stderr, "FAIL compare_flag_set_all_92c\n");
		return -1;
	}
	printf("PASS compare_flag_set_all_92c\n");

	PHY_RFShadowRefresh(&adapter);
	PHY_RFShadowRecorverFlagSetAll(&adapter);
	if (PHY_RFShadowTestPeekRecorver(RF_PATH_A, 0x26) != 1 ||
	    PHY_RFShadowTestPeekRecorver(RF_PATH_B, 0x27) != 1) {
		fprintf(stderr, "FAIL recover_flag_set_all_92c\n");
		return -1;
	}
	printf("PASS recover_flag_set_all_92c\n");
	return 0;
}

int main(int argc, char **argv)
{
	struct flag_vector vectors[MAX_VECTORS];
	size_t count = 0;
	static struct host_rf_shadow_adapter zero_adapter;
	const char *path = argc > 1 ? argv[1] :
					   "hal_phy_rf_shadow_flags_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_flag_vector, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		PHY_RFShadowRefresh(&zero_adapter);
		if (!strcmp(vectors[i].op, "compare_set")) {
			PHY_RFShadowCompareFlagSet(
				NULL, (enum rf_path)vectors[i].rf_path,
				(uint32_t)vectors[i].offset,
				(uint8_t)vectors[i].flag_type);
			if (PHY_RFShadowTestPeekCompare(
				    (enum rf_path)vectors[i].rf_path,
				    (uint32_t)vectors[i].offset) !=
			    (uint8_t)vectors[i].expect_compare) {
				fprintf(stderr, "FAIL %s\n", vectors[i].name);
				return 1;
			}
		} else {
			PHY_RFShadowRecorverFlagSet(
				NULL, (enum rf_path)vectors[i].rf_path,
				(uint32_t)vectors[i].offset,
				(uint8_t)vectors[i].flag_type);
			if (PHY_RFShadowTestPeekRecorver(
				    (enum rf_path)vectors[i].rf_path,
				    (uint32_t)vectors[i].offset) !=
			    (uint8_t)vectors[i].expect_recover) {
				fprintf(stderr, "FAIL %s\n", vectors[i].name);
				return 1;
			}
		}
		printf("PASS %s\n", vectors[i].name);
	}

	if (run_flag_set_all_self_test())
		return 1;

	printf("PASS: %zu vectors + flag_set_all from %s\n", count, path);
	return 0;
}

#endif
