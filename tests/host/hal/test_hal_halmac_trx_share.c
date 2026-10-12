// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for TRX share mode mapper + dump (W4-12). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_EXPECT 128

struct mapper_vector {
	char name[MAX_NAME];
	unsigned mode;
	unsigned expect;
};

struct dump_vector {
	char name[MAX_NAME];
	unsigned mode;
	char expect[MAX_EXPECT];
};

#ifdef RUST_HAL_HALMAC_ORACLE
unsigned trx_share_mode_drv2halmac(uint8_t mode);
int dump_trx_share_mode_format(uint8_t mode, char *buf, size_t buflen);
#else
/* C oracle linked from hal_halmac_trx_share_c_oracle.c */
unsigned trx_share_mode_drv2halmac(uint8_t mode);
int dump_trx_share_mode_format(uint8_t mode, char *buf, size_t buflen);
#endif

static int parse_mapper(const char *obj, size_t len, void *vec_void)
{
	struct mapper_vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "mode", &tmp))
		return -1;
	v->mode = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->expect = (unsigned)tmp;
	return 0;
}

static int parse_dump(const char *obj, size_t len, void *vec_void)
{
	struct dump_vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "mode", &tmp))
		return -1;
	v->mode = (unsigned)tmp;
	if (host_json_parse_string_in(obj, len, "expect", v->expect,
				      sizeof(v->expect)))
		return -1;
	return 0;
}

static int run_mapper_file(const char *path)
{
	struct mapper_vector vectors[MAX_VECTORS];
	size_t count = 0;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_mapper, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return -1;
	}

	for (size_t i = 0; i < count; i++) {
		unsigned got = trx_share_mode_drv2halmac((uint8_t)vectors[i].mode);

		if (got != vectors[i].expect) {
			fprintf(stderr, "FAIL mapper %s: got=%u expect=%u\n",
				vectors[i].name, got, vectors[i].expect);
			return -1;
		}
	}

	printf("PASS mapper: %zu vectors from %s\n", count, path);
	return 0;
}

static int run_dump_file(const char *path)
{
	struct dump_vector vectors[MAX_VECTORS];
	size_t count = 0;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_dump, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return -1;
	}

	for (size_t i = 0; i < count; i++) {
		char got[256];
		int n;

		n = dump_trx_share_mode_format((uint8_t)vectors[i].mode, got,
					       sizeof(got));
		if (n < 0) {
			fprintf(stderr, "FAIL dump %s: format returned %d\n",
				vectors[i].name, n);
			return -1;
		}
		if (n < 1 || got[n - 1] != '\n') {
			fprintf(stderr, "FAIL dump %s: missing trailing newline\n",
				vectors[i].name);
			return -1;
		}
		got[n - 1] = '\0';
		if (strcmp(got, vectors[i].expect) != 0) {
			fprintf(stderr, "FAIL dump %s:\n  got:    %s\n  expect: %s\n",
				vectors[i].name, got, vectors[i].expect);
			return -1;
		}
	}

	printf("PASS dump: %zu vectors from %s\n", count, path);
	return 0;
}

int main(int argc, char **argv)
{
	const char *mapper = argc > 1 ? argv[1] :
				    "hal_halmac_trx_share_drv2halmac_vectors.json";
	const char *dump = argc > 2 ? argv[2] :
				      "hal_halmac_trx_share_dump_vectors.json";

	if (run_mapper_file(mapper) || run_dump_file(dump))
		return 1;
	return 0;
}
