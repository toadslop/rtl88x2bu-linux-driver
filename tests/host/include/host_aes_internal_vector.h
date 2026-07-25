/* SPDX-License-Identifier: GPL-2.0 */
/*
 * AES-internal vector fixture types + runner for host L2 tests (W2-11a).
 */
#ifndef HOST_AES_INTERNAL_VECTOR_H
#define HOST_AES_INTERNAL_VECTOR_H

#include <stddef.h>

#include "host_types.h"
#include "host_vector_json.h"

#define HOST_AES_INTERNAL_MAX_VECTORS 16
#define HOST_AES_INTERNAL_MAX_RK_BYTES (4 * 4 * 15)

enum host_aes_internal_fn {
	HOST_AES_INTERNAL_FN_KEY_SETUP_ENC = 0,
	HOST_AES_INTERNAL_FN_ENCRYPT,
};

struct host_aes_internal_vector {
	char name[128];
	enum host_aes_internal_fn fn;
	int key_bits;
	u8 key[32];
	size_t key_len;
	u8 plaintext[16];
	size_t plaintext_len;
	int expect_rounds;
	u8 expected_rk[HOST_AES_INTERNAL_MAX_RK_BYTES];
	size_t expected_rk_len;
	u8 expected[16];
	size_t expected_len;
	int expect_ret;
	int rust_only;
};

int host_aes_internal_parse_vector_object(const char *obj, size_t obj_len,
					  void *vec_void);
int host_aes_internal_run_vector(const struct host_aes_internal_vector *v);

#endif /* HOST_AES_INTERNAL_VECTOR_H */
