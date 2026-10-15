// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle: W4-18 Read/Write/Refresh; W4-19 flags; W4-20 compare/recover. */

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

#if defined(HOST_HAL_PHY_RF_SHADOW_COMPARE_TEST)

struct compare_vector {
	char name[MAX_NAME];
	char op[32];
	int rf_path;
	int offset;
	unsigned shadow_value;
	unsigned hw_value;
	int compare_flag;
	int recover_flag;
	int prime_compare_mismatch;
	int expect_result;
	int expect_error;
	unsigned expect_hw_value;
};

void PHY_RFShadowRefresh(void *adapter);
void PHY_RFShadowWrite(void *adapter, enum rf_path path, uint32_t offset,
		       uint32_t data);
void PHY_RFShadowCompareFlagSet(void *adapter, enum rf_path path,
				uint32_t offset, uint8_t type);
void PHY_RFShadowRecorverFlagSet(void *adapter, enum rf_path path,
				 uint32_t offset, uint8_t type);
uint8_t PHY_RFShadowCompare(void *adapter, enum rf_path path,
			    uint32_t offset);
void PHY_RFShadowRecorver(void *adapter, enum rf_path path, uint32_t offset);
void PHY_RFShadowCompareAll(void *adapter);
void PHY_RFShadowRecorverAll(void *adapter);
uint8_t PHY_RFShadowTestPeekErrorOrNot(enum rf_path path, uint32_t offset);

void host_rf_shadow_hw_reset(void);
void host_rf_shadow_hw_set(enum rf_path path, uint32_t offset, uint32_t value);
uint32_t host_rf_shadow_hw_get(enum rf_path path, uint32_t offset);

static int parse_compare_vector(const char *obj, size_t len, void *vec_void)
{
	struct compare_vector *v = vec_void;
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
	if (host_json_parse_int_in(obj, len, "shadow_value", &tmp))
		return -1;
	v->shadow_value = (unsigned)tmp;
	if (!strcmp(v->op, "compare")) {
		if (host_json_parse_int_in(obj, len, "hw_value", &tmp))
			return -1;
		v->hw_value = (unsigned)tmp;
		if (host_json_parse_int_in(obj, len, "compare_flag", &tmp))
			return -1;
		v->compare_flag = tmp;
		if (host_json_parse_int_in(obj, len, "expect_result", &tmp))
			return -1;
		v->expect_result = tmp;
		if (host_json_parse_int_in(obj, len, "expect_error", &tmp))
			return -1;
		v->expect_error = tmp;
	} else if (!strcmp(v->op, "recover")) {
		if (host_json_parse_int_in(obj, len, "recover_flag", &tmp))
			return -1;
		v->recover_flag = tmp;
		if (host_json_parse_int_in(obj, len, "prime_compare_mismatch", &tmp))
			v->prime_compare_mismatch = 1;
		else
			v->prime_compare_mismatch = tmp;
		if (host_json_parse_int_in(obj, len, "expect_hw_value", &tmp))
			return -1;
		v->expect_hw_value = (unsigned)tmp;
	} else {
		return -1;
	}
	return 0;
}

static int run_compare_all_self_test(void)
{
	struct host_rf_shadow_adapter adapter = {0};

	host_rf_shadow_hw_reset();
	PHY_RFShadowRefresh(&adapter);
	PHY_RFShadowWrite(NULL, RF_PATH_A, 0x26, 0x11);
	PHY_RFShadowCompareFlagSet(NULL, RF_PATH_A, 0x26, 1);
	host_rf_shadow_hw_set(RF_PATH_A, 0x26, 0x22);
	PHY_RFShadowCompareAll(&adapter);
	if (PHY_RFShadowTestPeekErrorOrNot(RF_PATH_A, 0x26) != 1) {
		fprintf(stderr, "FAIL compare_all_mismatch\n");
		return -1;
	}
	printf("PASS compare_all_mismatch\n");

	host_rf_shadow_hw_reset();
	PHY_RFShadowRefresh(&adapter);
	PHY_RFShadowWrite(NULL, RF_PATH_B, 0x27, 0x33);
	PHY_RFShadowCompareFlagSet(NULL, RF_PATH_B, 0x27, 1);
	host_rf_shadow_hw_set(RF_PATH_B, 0x27, 0);
	PHY_RFShadowCompare(NULL, RF_PATH_B, 0x27);
	PHY_RFShadowRecorverFlagSet(NULL, RF_PATH_B, 0x27, 1);
	PHY_RFShadowRecorverAll(&adapter);
	if (host_rf_shadow_hw_get(RF_PATH_B, 0x27) != 0x33) {
		fprintf(stderr, "FAIL recover_all_writes_hw\n");
		return -1;
	}
	printf("PASS recover_all_writes_hw\n");
	return 0;
}

int main(int argc, char **argv)
{
	struct compare_vector vectors[MAX_VECTORS];
	size_t count = 0;
	static struct host_rf_shadow_adapter zero_adapter;
	const char *path = argc > 1 ? argv[1] :
					   "hal_phy_rf_shadow_compare_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_compare_vector, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		host_rf_shadow_hw_reset();
		PHY_RFShadowRefresh(&zero_adapter);
		if (!strcmp(vectors[i].op, "compare")) {
			PHY_RFShadowWrite(NULL, (enum rf_path)vectors[i].rf_path,
					  (uint32_t)vectors[i].offset,
					  (uint32_t)vectors[i].shadow_value);
			PHY_RFShadowCompareFlagSet(
				NULL, (enum rf_path)vectors[i].rf_path,
				(uint32_t)vectors[i].offset,
				(uint8_t)vectors[i].compare_flag);
			host_rf_shadow_hw_set((enum rf_path)vectors[i].rf_path,
					      (uint32_t)vectors[i].offset,
					      (uint32_t)vectors[i].hw_value);
			uint8_t got = PHY_RFShadowCompare(
				NULL, (enum rf_path)vectors[i].rf_path,
				(uint32_t)vectors[i].offset);
			if (got != (uint8_t)vectors[i].expect_result ||
			    PHY_RFShadowTestPeekErrorOrNot(
				    (enum rf_path)vectors[i].rf_path,
				    (uint32_t)vectors[i].offset) !=
				    (uint8_t)vectors[i].expect_error) {
				fprintf(stderr, "FAIL %s\n", vectors[i].name);
				return 1;
			}
		} else {
			PHY_RFShadowWrite(NULL, (enum rf_path)vectors[i].rf_path,
					  (uint32_t)vectors[i].offset,
					  (uint32_t)vectors[i].shadow_value);
			if (vectors[i].prime_compare_mismatch) {
				PHY_RFShadowCompareFlagSet(
					NULL, (enum rf_path)vectors[i].rf_path,
					(uint32_t)vectors[i].offset, 1);
				host_rf_shadow_hw_set(
					(enum rf_path)vectors[i].rf_path,
					(uint32_t)vectors[i].offset, 0);
				PHY_RFShadowCompare(
					NULL, (enum rf_path)vectors[i].rf_path,
					(uint32_t)vectors[i].offset);
			}
			PHY_RFShadowRecorverFlagSet(
				NULL, (enum rf_path)vectors[i].rf_path,
				(uint32_t)vectors[i].offset,
				(uint8_t)vectors[i].recover_flag);
			PHY_RFShadowRecorver(NULL, (enum rf_path)vectors[i].rf_path,
					     (uint32_t)vectors[i].offset);
			uint32_t hw = host_rf_shadow_hw_get(
				(enum rf_path)vectors[i].rf_path,
				(uint32_t)vectors[i].offset);
			if (hw != vectors[i].expect_hw_value) {
				fprintf(stderr, "FAIL %s: hw=0x%x expect=0x%x\n",
					vectors[i].name, hw,
					vectors[i].expect_hw_value);
				return 1;
			}
		}
		printf("PASS %s\n", vectors[i].name);
	}

	if (run_compare_all_self_test())
		return 1;

	printf("PASS: %zu vectors + compare_all from %s\n", count, path);
	return 0;
}

#elif !defined(HOST_HAL_PHY_RF_SHADOW_FLAGS_TEST)

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
