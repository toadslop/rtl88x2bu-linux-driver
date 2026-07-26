/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_SWCRYPTO_VECTOR_H
#define HOST_SWCRYPTO_VECTOR_H

#include "host_types.h"

#define HOST_SWCRYPTO_MAX_VECTORS 32
#define HOST_SWCRYPTO_MAX_FRAME 512

enum host_swcrypto_fn {
	HOST_SWCRYPTO_FN_CCMP_ENCRYPT = 0,
	HOST_SWCRYPTO_FN_CCMP_DECRYPT,
	HOST_SWCRYPTO_FN_GCMP_ENCRYPT,
	HOST_SWCRYPTO_FN_GCMP_DECRYPT,
};

struct host_swcrypto_vector {
	char name[128];
	enum host_swcrypto_fn fn;
	u32 key_len;
	u8 key[32];
	u32 hdrlen;
	u32 plen;
	u8 frame[HOST_SWCRYPTO_MAX_FRAME];
	size_t frame_len;
	int expect_ret;
};

int host_swcrypto_parse_vector_object(const char *obj, size_t obj_len, void *vec_void);
int host_swcrypto_run_vector(const struct host_swcrypto_vector *v);

#endif /* HOST_SWCRYPTO_VECTOR_H */
