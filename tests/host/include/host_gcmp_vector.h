/* SPDX-License-Identifier: GPL-2.0 */
/*
 * GCMP vector fixture types + runner for host L2 tests (W2-02b).
 */
#ifndef HOST_GCMP_VECTOR_H
#define HOST_GCMP_VECTOR_H

#include <stddef.h>

#include "host_types.h"
#include "host_vector_json.h"
#include "host_wifi_types.h"

#define HOST_GCMP_MAX_VECTORS 32

enum host_gcmp_fn {
	HOST_GCMP_FN_ENCRYPT = 0,
	HOST_GCMP_FN_DECRYPT,
};

struct host_gcmp_vector {
	char name[128];
	enum host_gcmp_fn fn;
	enum rtw_amsdu_mode amsdu_mode;
	size_t key_len;
	u8 key[32];
	u8 frame[HOST_VECTOR_MAX_HEX_BUF / 2];
	size_t frame_len;
	u8 hdr[32];
	size_t hdr_len;
	u8 data[HOST_VECTOR_MAX_HEX_BUF / 2];
	size_t data_len;
	size_t hdrlen;
	u8 pn[6];
	int null_pn;
	int keyid;
	u8 expected[HOST_VECTOR_MAX_HEX_BUF / 2];
	size_t expected_len;
	/* 1 = gcmp_* returns non-NULL, 0 = returns NULL */
	int expect_ret;
	/* skipped by C oracle runners; consumed by test_gcmp.c */
	int rust_only;
};

int host_gcmp_parse_vector_object(const char *obj, size_t obj_len, void *vec_void);
int host_gcmp_run_vector(const struct host_gcmp_vector *v);

#endif /* HOST_GCMP_VECTOR_H */
