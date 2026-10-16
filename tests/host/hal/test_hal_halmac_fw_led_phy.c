// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for FW mem, LED mode, phy param val mappers (W4-24). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 64

struct fw_mem_vector {
	char name[MAX_NAME];
	unsigned mem;
	unsigned tx_stop;
	unsigned expect;
};

struct led_vector {
	char name[MAX_NAME];
	unsigned drv_mode;
	unsigned expect;
};

struct phy_val_vector {
	char name[MAX_NAME];
	unsigned val;
	unsigned msk_en;
	unsigned msk;
	unsigned expect;
};

unsigned fw_mem_drv2halmac(unsigned mem, unsigned tx_stop);
unsigned led_mode_drv2halmac(unsigned drv_mode);
unsigned phy_parameter_val_drv2halmac(unsigned val, unsigned msk_en, unsigned msk);

static int parse_fw_mem(const char *obj, size_t len, void *vec_void)
{
	struct fw_mem_vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "mem", &tmp))
		return -1;
	v->mem = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "tx_stop", &tmp))
		return -1;
	v->tx_stop = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->expect = (unsigned)tmp;
	return 0;
}

static int parse_led(const char *obj, size_t len, void *vec_void)
{
	struct led_vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "drv_mode", &tmp))
		return -1;
	v->drv_mode = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->expect = (unsigned)tmp;
	return 0;
}

static int parse_phy_val(const char *obj, size_t len, void *vec_void)
{
	struct phy_val_vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "val", &tmp))
		return -1;
	v->val = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "msk_en", &tmp))
		return -1;
	v->msk_en = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "msk", &tmp))
		return -1;
	v->msk = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->expect = (unsigned)tmp;
	return 0;
}

static int run_fw_mem(const char *path)
{
	struct fw_mem_vector vectors[MAX_VECTORS];
	size_t count = 0;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_fw_mem, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return -1;
	}

	for (size_t i = 0; i < count; i++) {
		unsigned got = fw_mem_drv2halmac(vectors[i].mem, vectors[i].tx_stop);

		if (got != vectors[i].expect) {
			fprintf(stderr, "FAIL fw_mem %s: got=%u expect=%u\n",
				vectors[i].name, got, vectors[i].expect);
			return -1;
		}
	}

	printf("PASS fw_mem: %zu vectors from %s\n", count, path);
	return 0;
}

static int run_led(const char *path)
{
	struct led_vector vectors[MAX_VECTORS];
	size_t count = 0;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_led, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return -1;
	}

	for (size_t i = 0; i < count; i++) {
		unsigned got = led_mode_drv2halmac(vectors[i].drv_mode);

		if (got != vectors[i].expect) {
			fprintf(stderr, "FAIL led_mode %s: got=%u expect=%u\n",
				vectors[i].name, got, vectors[i].expect);
			return -1;
		}
	}

	printf("PASS led_mode: %zu vectors from %s\n", count, path);
	return 0;
}

static int run_phy_val(const char *path)
{
	struct phy_val_vector vectors[MAX_VECTORS];
	size_t count = 0;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_phy_val, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return -1;
	}

	for (size_t i = 0; i < count; i++) {
		unsigned got = phy_parameter_val_drv2halmac(vectors[i].val,
							    vectors[i].msk_en,
							    vectors[i].msk);

		if (got != vectors[i].expect) {
			fprintf(stderr, "FAIL phy_param_val %s: got=%u expect=%u\n",
				vectors[i].name, got, vectors[i].expect);
			return -1;
		}
	}

	printf("PASS phy_param_val: %zu vectors from %s\n", count, path);
	return 0;
}

int main(int argc, char **argv)
{
	const char *fw = argc > 1 ? argv[1] :
			 "hal_halmac_fw_mem_drv2halmac_vectors.json";
	const char *led = argc > 2 ? argv[2] :
				  "hal_halmac_led_mode_drv2halmac_vectors.json";
	const char *phy = argc > 3 ? argv[3] :
				  "hal_halmac_phy_param_val_drv2halmac_vectors.json";

	if (run_fw_mem(fw) || run_led(led) || run_phy_val(phy))
		return 1;
	return 0;
}
