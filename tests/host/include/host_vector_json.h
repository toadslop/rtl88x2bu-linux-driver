/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Shared hex + hand-rolled JSON helpers for host L2 crypto vector fixtures.
 */
#ifndef HOST_VECTOR_JSON_H
#define HOST_VECTOR_JSON_H

#include <stddef.h>

#define HOST_VECTOR_MAX_HEX_BUF 4096

int host_hex_decode(const char *hex, unsigned char *out, size_t out_cap,
		    size_t *out_len);

const char *host_json_skip_ws(const char *p);
const char *host_json_find_key_in(const char *obj, size_t obj_len,
				  const char *key);
int host_json_parse_string_in(const char *obj, size_t obj_len, const char *key,
			      char *out, size_t out_cap);
int host_json_parse_int_in(const char *obj, size_t obj_len, const char *key,
			   int *out);
int host_json_parse_bool_in(const char *obj, size_t obj_len, const char *key,
			    int *out);

typedef int (*host_vector_parse_fn)(const char *obj, size_t obj_len, void *vec);

int host_load_vectors(const char *path, void *vecs, size_t vec_size, size_t cap,
		      host_vector_parse_fn parse_fn, size_t *count_out);

#endif /* HOST_VECTOR_JSON_H */
