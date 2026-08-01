// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for continual no-rx recv helpers (W3-39 PR2).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_recv_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 128

enum recv_fn { FN_INC_CHK = 0, FN_RESET };

struct vector {
	char name[MAX_NAME];
	enum recv_fn fn;
	int tid_index;
	int initial_count;
	int expect_ret;
	int expect_count;
};

int rtw_inc_and_chk_continual_no_rx_packet(struct sta_info *sta, int tid_index);
void rtw_reset_continual_no_rx_packet(struct sta_info *sta, int tid_index);

static int parse_fn(const char *obj, size_t obj_len, enum recv_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "rtw_inc_and_chk_continual_no_rx_packet"))
		*out = FN_INC_CHK;
	else if (!strcmp(fn, "rtw_reset_continual_no_rx_packet"))
		*out = FN_RESET;
	else
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, obj_len, "tid_index", &v->tid_index);
	host_json_parse_int_in(obj, obj_len, "initial_count", &v->initial_count);
	host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, obj_len, "expect_count", &v->expect_count);
	return 0;
}

static int run_vector(struct vector *v)
{
	struct sta_info sta;
	int ret;

	memset(&sta, 0, sizeof(sta));
	sta.continual_no_rx_packet[v->tid_index] = v->initial_count;

	if (v->fn == FN_INC_CHK) {
		ret = rtw_inc_and_chk_continual_no_rx_packet(&sta, v->tid_index);
		if (ret != v->expect_ret) {
			fprintf(stderr, "FAIL %s: expect_ret %d got %d\n", v->name,
				v->expect_ret, ret);
			return -1;
		}
	} else {
		rtw_reset_continual_no_rx_packet(&sta, v->tid_index);
	}
	if (sta.continual_no_rx_packet[v->tid_index] != v->expect_count) {
		fprintf(stderr, "FAIL %s: expect_count %d got %d\n", v->name,
			v->expect_count, sta.continual_no_rx_packet[v->tid_index]);
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i;
	int failures = 0;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <vectors.json>\n", argv[0]);
		return 2;
	}
	if (host_load_vectors(argv[1], vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", argv[1]);
		return 2;
	}
	for (i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			failures++;
	printf("%zu vectors, %d failures\n", count, failures);
	return failures ? 1 : 0;
}
