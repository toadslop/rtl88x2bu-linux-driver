// SPDX-License-Identifier: GPL-2.0
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 48
#define HW_QUEUE_ENTRY 8
#define RT_PIPE_LEN 4

struct host_hal_mapping_adapter {
	uint8_t wifi_spec;
	int rt_out_pipe[RT_PIPE_LEN];
	uint8_t queue2pipe[HW_QUEUE_ENTRY];
};

typedef uint8_t BOOLEAN;

struct vector {
	char name[MAX_NAME];
	uint8_t num_out_pipe;
	uint8_t wifi_spec;
	uint8_t rt_out_pipe[RT_PIPE_LEN];
	int expect_result;
	uint8_t expect_queue2pipe[HW_QUEUE_ENTRY];
};

BOOLEAN Hal_MappingOutPipe(void *adapter, uint8_t num_out_pipe);

static int parse_u8_array_in(const char *obj, size_t obj_len, const char *key,
			     uint8_t *out, size_t out_cap, size_t *count_out)
{
	const char *p = host_json_find_key_in(obj, obj_len, key);
	size_t n = 0;

	if (!p || *p != '[')
		return -1;
	p++;
	for (;;) {
		int val;

		p = host_json_skip_ws(p);
		if (*p == ']') {
			p++;
			*count_out = n;
			return 0;
		}
		if (n >= out_cap)
			return -1;
		if (n > 0) {
			if (*p != ',')
				return -1;
			p++;
			p = host_json_skip_ws(p);
		}
		val = 0;
		while (*p >= '0' && *p <= '9')
			val = val * 10 + (*p++ - '0');
		out[n++] = (uint8_t)val;
	}
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;
	size_t n;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "num_out_pipe", &tmp))
		return -1;
	v->num_out_pipe = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "wifi_spec", &tmp))
		return -1;
	v->wifi_spec = (uint8_t)tmp;
	if (parse_u8_array_in(obj, len, "rt_out_pipe", v->rt_out_pipe, RT_PIPE_LEN, &n))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_result", &tmp))
		return -1;
	v->expect_result = tmp;
	if (parse_u8_array_in(obj, len, "expect_queue2pipe", v->expect_queue2pipe,
			      HW_QUEUE_ENTRY, &n))
		return -1;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_mapping_out_pipe_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		struct host_hal_mapping_adapter adapter;

		memset(&adapter, 0, sizeof(adapter));
		adapter.wifi_spec = vectors[i].wifi_spec;
		for (int p = 0; p < RT_PIPE_LEN; p++)
			adapter.rt_out_pipe[p] = vectors[i].rt_out_pipe[p];

		BOOLEAN got = Hal_MappingOutPipe(&adapter, vectors[i].num_out_pipe);

		if ((int)got != vectors[i].expect_result) {
			fprintf(stderr, "FAIL %s: result got=%d expect=%d\n",
				vectors[i].name, (int)got, vectors[i].expect_result);
			return 1;
		}
		if (!vectors[i].expect_result)
			continue;

		for (int q = 0; q < HW_QUEUE_ENTRY; q++) {
			if (adapter.queue2pipe[q] != vectors[i].expect_queue2pipe[q]) {
				fprintf(stderr,
					"FAIL %s: queue2pipe[%d] got=%u expect=%u\n",
					vectors[i].name, q, adapter.queue2pipe[q],
					vectors[i].expect_queue2pipe[q]);
				return 1;
			}
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
