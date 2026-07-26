/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_SWCRYPTO_VECTOR_H
#define HOST_SWCRYPTO_VECTOR_H

#include "host_types.h"

#define HOST_SWCRYPTO_MAX_VECTORS 48
#define HOST_SWCRYPTO_MAX_FRAME 512
#define HOST_SWCRYPTO_MAX_DATA 256
#define HOST_SWCRYPTO_MAX_ELEMENTS 6
#define HOST_SWCRYPTO_MAX_ELEMENT_HEX 64
#define HOST_SWCRYPTO_MAX_MIC 16
#define HOST_SWCRYPTO_MAX_OUT 64

enum host_swcrypto_fn {
	HOST_SWCRYPTO_FN_CCMP_ENCRYPT = 0,
	HOST_SWCRYPTO_FN_CCMP_DECRYPT,
	HOST_SWCRYPTO_FN_GCMP_ENCRYPT,
	HOST_SWCRYPTO_FN_GCMP_DECRYPT,
	HOST_SWCRYPTO_FN_BIP_CCMP_PROTECT,
	HOST_SWCRYPTO_FN_BIP_GCMP_PROTECT,
	HOST_SWCRYPTO_FN_AES_SIV_ENCRYPT,
	HOST_SWCRYPTO_FN_AES_SIV_DECRYPT,
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
	u8 data[HOST_SWCRYPTO_MAX_DATA];
	size_t data_len;
	u8 expected_mic[HOST_SWCRYPTO_MAX_MIC];
	size_t expected_mic_len;
	u8 pw[HOST_SWCRYPTO_MAX_DATA];
	size_t pw_len;
	u8 elements[HOST_SWCRYPTO_MAX_ELEMENTS][HOST_SWCRYPTO_MAX_ELEMENT_HEX];
	size_t element_lens[HOST_SWCRYPTO_MAX_ELEMENTS];
	size_t num_elements;
	u8 iv_crypt[HOST_SWCRYPTO_MAX_OUT];
	size_t iv_crypt_len;
	u8 expected_out[HOST_SWCRYPTO_MAX_OUT];
	size_t expected_out_len;
	int expect_ret;
};

int host_swcrypto_parse_vector_object(const char *obj, size_t obj_len, void *vec_void);
int host_swcrypto_run_vector(const struct host_swcrypto_vector *v);

#endif /* HOST_SWCRYPTO_VECTOR_H */
