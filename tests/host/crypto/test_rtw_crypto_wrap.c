// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for rtw_crypto_wrap.c (W2-06d).
 *
 * oracle: core/crypto/rtw_crypto_wrap.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drv_types.h"
#include "host_vector_json.h"
#include "rtw_crypto_wrap.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_HEX 256

enum wrap_fn {
	FN_OS_MEMCMP_CONST = 0,
	FN_OS_MEMCMP,
	FN_OS_STRLEN,
	FN_OS_MEMDUP,
	FN_FORCED_MEMZERO,
	FN_BIN_CLEAR_FREE,
	FN_RTW_REGISTRYPRIV_AMSDU_MODE,
};

struct vector {
	char name[MAX_NAME];
	enum wrap_fn fn;
	u8 a[MAX_HEX];
	size_t a_len;
	u8 b[MAX_HEX];
	size_t b_len;
	u8 src[MAX_HEX];
	size_t src_len;
	u8 input[MAX_HEX];
	size_t input_len;
	u8 expect[MAX_HEX];
	size_t expect_len;
	char string[64];
	u32 sz;
	int expect_result;
	size_t cmp_len;
	size_t expect_len_out;
	int adapter_null;
	int amsdu_mode;
	int expect_mode;
	int bin_null;
	int src_null;
};

void host_adapter_set_amsdu_mode(_adapter *padapter, enum rtw_amsdu_mode mode);
void host_rtw_wrap_enable_bin_clear_free_check(int on);
int host_rtw_wrap_zero_check_failed(void);
u8 rtw_registrypriv_amsdu_mode(const _adapter *padapter);
void *rtw_malloc(size_t sz);
void bin_clear_free(void *bin, size_t len);
int os_memcmp(const void *s1, const void *s2, size_t n);

static int parse_fn(const char *obj, size_t obj_len, enum wrap_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "os_memcmp_const") == 0)
		*out = FN_OS_MEMCMP_CONST;
	else if (strcmp(fn, "os_memcmp") == 0)
		*out = FN_OS_MEMCMP;
	else if (strcmp(fn, "os_strlen") == 0)
		*out = FN_OS_STRLEN;
	else if (strcmp(fn, "os_memdup") == 0)
		*out = FN_OS_MEMDUP;
	else if (strcmp(fn, "forced_memzero") == 0)
		*out = FN_FORCED_MEMZERO;
	else if (strcmp(fn, "bin_clear_free") == 0)
		*out = FN_BIN_CLEAR_FREE;
	else if (strcmp(fn, "rtw_registrypriv_amsdu_mode") == 0)
		*out = FN_RTW_REGISTRYPRIV_AMSDU_MODE;
	else
		return -1;
	return 0;
}

static int parse_hex_field(const char *obj, size_t obj_len, const char *key,
			   u8 *buf, size_t buf_cap, size_t *out_len)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	if (host_json_parse_string_in(obj, obj_len, key, hex, sizeof(hex)))
		return -1;
	if (!*hex) {
		*out_len = 0;
		return 0;
	}
	return host_hex_decode(hex, buf, buf_cap, out_len);
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;

	switch (v->fn) {
	case FN_OS_MEMCMP_CONST: {
		int cmp_len = 0;

		if (parse_hex_field(obj, obj_len, "a", v->a, sizeof(v->a), &v->a_len))
			return -1;
		if (parse_hex_field(obj, obj_len, "b", v->b, sizeof(v->b), &v->b_len))
			return -1;
		if (v->a_len != v->b_len)
			return -1;
		if (host_json_parse_int_in(obj, obj_len, "expect_result", &v->expect_result))
			return -1;
		if (!host_json_parse_int_in(obj, obj_len, "len", &cmp_len)) {
			if (cmp_len < 0 || (size_t)cmp_len > v->a_len)
				return -1;
			v->cmp_len = (size_t)cmp_len;
		}
		break;
	}
	case FN_OS_MEMCMP: {
		int cmp_len = 0;

		if (parse_hex_field(obj, obj_len, "a", v->a, sizeof(v->a), &v->a_len))
			return -1;
		if (parse_hex_field(obj, obj_len, "b", v->b, sizeof(v->b), &v->b_len))
			return -1;
		if (v->a_len != v->b_len)
			return -1;
		if (host_json_parse_int_in(obj, obj_len, "expect_result", &v->expect_result))
			return -1;
		if (!host_json_parse_int_in(obj, obj_len, "len", &cmp_len)) {
			if (cmp_len < 0 || (size_t)cmp_len > v->a_len)
				return -1;
			v->cmp_len = (size_t)cmp_len;
		}
		break;
	}
	case FN_OS_STRLEN: {
		int expect_len = 0;

		if (host_json_parse_string_in(obj, obj_len, "string", v->string,
					      sizeof(v->string)))
			return -1;
		if (host_json_parse_int_in(obj, obj_len, "expect_len", &expect_len))
			return -1;
		if (expect_len < 0)
			return -1;
		v->expect_len_out = (size_t)expect_len;
		break;
	}
	case FN_OS_MEMDUP: {
		int sz = 0;

		if (host_json_parse_int_in(obj, obj_len, "src_null", &v->src_null))
			v->src_null = 0;
		if (!v->src_null &&
		    parse_hex_field(obj, obj_len, "src", v->src, sizeof(v->src), &v->src_len))
			return -1;
		if (host_json_parse_int_in(obj, obj_len, "sz", &sz))
			return -1;
		if (sz < 0)
			return -1;
		v->sz = (u32)sz;
		if (!v->src_null &&
		    parse_hex_field(obj, obj_len, "expect", v->expect, sizeof(v->expect),
				    &v->expect_len))
			return -1;
		if (v->src_null) {
			if (v->expect_len != 0)
				return -1;
		} else if (v->expect_len != v->sz || v->src_len != v->sz) {
			return -1;
		}
		break;
	}
	case FN_FORCED_MEMZERO:
		if (parse_hex_field(obj, obj_len, "input", v->input, sizeof(v->input),
				    &v->input_len))
			return -1;
		if (parse_hex_field(obj, obj_len, "expect", v->expect, sizeof(v->expect),
				    &v->expect_len))
			return -1;
		if (v->input_len != v->expect_len)
			return -1;
		break;
	case FN_BIN_CLEAR_FREE: {
		int len = 0;

		if (host_json_parse_int_in(obj, obj_len, "bin_null", &v->bin_null))
			v->bin_null = 0;
		if (host_json_parse_int_in(obj, obj_len, "len", &len))
			return -1;
		if (len < 0)
			return -1;
		v->expect_len_out = (size_t)len;
		if (!v->bin_null) {
			if (parse_hex_field(obj, obj_len, "input", v->input,
					    sizeof(v->input), &v->input_len))
				return -1;
			if (v->input_len != v->expect_len_out)
				return -1;
		}
		break;
	}
	case FN_RTW_REGISTRYPRIV_AMSDU_MODE:
		if (host_json_parse_int_in(obj, obj_len, "adapter_null", &v->adapter_null))
			v->adapter_null = 0;
		if (host_json_parse_int_in(obj, obj_len, "amsdu_mode", &v->amsdu_mode))
			v->amsdu_mode = 0;
		if (host_json_parse_int_in(obj, obj_len, "expect_mode", &v->expect_mode))
			return -1;
		break;
	default:
		return -1;
	}
	return 0;
}

static int run_vector(const struct vector *v)
{
	switch (v->fn) {
	case FN_OS_MEMCMP_CONST: {
		int rc;
		size_t len = v->cmp_len ? v->cmp_len : v->a_len;

		if (v->a_len != v->b_len) {
			fprintf(stderr, "%s: os_memcmp_const a_len (%zu) != b_len (%zu)\n",
				v->name, v->a_len, v->b_len);
			return -1;
		}
		rc = os_memcmp_const(v->a, v->b, len);

		if (rc != v->expect_result) {
			fprintf(stderr, "%s: os_memcmp_const returned %d, expected %d\n",
				v->name, rc, v->expect_result);
			return -1;
		}
		return 0;
	}
	case FN_OS_MEMCMP: {
		int rc;
		size_t len = v->cmp_len ? v->cmp_len : v->a_len;

		if (v->a_len != v->b_len) {
			fprintf(stderr, "%s: os_memcmp a_len (%zu) != b_len (%zu)\n",
				v->name, v->a_len, v->b_len);
			return -1;
		}
		rc = os_memcmp(v->a, v->b, len);

		if (rc != v->expect_result) {
			fprintf(stderr, "%s: os_memcmp returned %d, expected %d\n",
				v->name, rc, v->expect_result);
			return -1;
		}
		return 0;
	}
	case FN_OS_STRLEN: {
		size_t rc = os_strlen(v->string);

		if (rc != v->expect_len_out) {
			fprintf(stderr, "%s: os_strlen returned %zu, expected %zu\n", v->name,
				rc, v->expect_len_out);
			return -1;
		}
		return 0;
	}
	case FN_OS_MEMDUP: {
		u8 *dup;

		if (!v->src_null) {
			if (v->expect_len != v->sz) {
				fprintf(stderr, "%s: os_memdup expect_len (%zu) != sz (%u)\n",
					v->name, v->expect_len, v->sz);
				return -1;
			}
			if (v->src_len != v->sz) {
				fprintf(stderr, "%s: os_memdup src_len (%zu) != sz (%u)\n",
					v->name, v->src_len, v->sz);
				return -1;
			}
		}
		dup = os_memdup(v->src_null ? NULL : (v->src_len ? v->src : NULL), v->sz);

		if (!dup && v->sz > 0) {
			fprintf(stderr, "%s: os_memdup returned NULL\n", v->name);
			return -1;
		}
		if (!v->src_null && v->expect_len > 0 &&
		    memcmp(dup, v->expect, v->expect_len) != 0) {
			fprintf(stderr, "%s: os_memdup content mismatch\n", v->name);
			bin_clear_free(dup, v->sz);
			return -1;
		}
		bin_clear_free(dup, v->sz);
		return 0;
	}
	case FN_FORCED_MEMZERO: {
		u8 buf[MAX_HEX];

		if (v->input_len != v->expect_len) {
			fprintf(stderr, "%s: forced_memzero input_len (%zu) != expect_len (%zu)\n",
				v->name, v->input_len, v->expect_len);
			return -1;
		}
		if (v->input_len > sizeof(buf))
			return -1;
		memcpy(buf, v->input, v->input_len);
		forced_memzero(buf, v->input_len);
		if (memcmp(buf, v->expect, v->expect_len) != 0) {
			fprintf(stderr, "%s: forced_memzero left non-zero bytes\n", v->name);
			return -1;
		}
		return 0;
	}
	case FN_BIN_CLEAR_FREE: {
		u8 *buf;

		if (v->bin_null) {
			bin_clear_free(NULL, v->expect_len_out);
			return 0;
		}
		if (v->input_len != v->expect_len_out) {
			fprintf(stderr,
				"%s: bin_clear_free input_len (%zu) != len (%zu)\n",
				v->name, v->input_len, v->expect_len_out);
			return -1;
		}
		buf = rtw_malloc(v->expect_len_out);
		if (!buf && v->expect_len_out > 0) {
			fprintf(stderr, "%s: rtw_malloc returned NULL\n", v->name);
			return -1;
		}
		if (v->input_len)
			memcpy(buf, v->input, v->input_len);
		host_rtw_wrap_enable_bin_clear_free_check(1);
		bin_clear_free(buf, v->expect_len_out);
		host_rtw_wrap_enable_bin_clear_free_check(0);
		if (host_rtw_wrap_zero_check_failed()) {
			fprintf(stderr, "%s: bin_clear_free left non-zero bytes\n",
				v->name);
			return -1;
		}
		return 0;
	}
	case FN_RTW_REGISTRYPRIV_AMSDU_MODE: {
		_adapter adapter;
		_adapter *padapter = v->adapter_null ? NULL : &adapter;
		u8 rc;

		if (!v->adapter_null)
			host_adapter_set_amsdu_mode(&adapter, (enum rtw_amsdu_mode)v->amsdu_mode);
		rc = rtw_registrypriv_amsdu_mode(padapter);
		if (rc != (u8)v->expect_mode) {
			fprintf(stderr, "%s: rtw_registrypriv_amsdu_mode returned %u, expected %d\n",
				v->name, rc, v->expect_mode);
			return -1;
		}
		return 0;
	}
	default:
		return -1;
	}
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = "rtw_crypto_wrap_vectors.json";
	int failures = 0;

	if (argc > 1)
		path = argv[1];
	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}
	for (size_t i = 0; i < count; i++) {
		if (run_vector(&vectors[i]) != 0)
			failures++;
		else
			printf("ok %s\n", vectors[i].name);
	}
	if (failures) {
		fprintf(stderr, "%d/%zu vectors failed\n", failures, count);
		return 1;
	}
#ifndef RUST_RTW_CRYPTO_WRAP_ORACLE
	printf("all %zu rtw-crypto-wrap vectors passed (oracle: core/crypto/rtw_crypto_wrap.c)\n",
	       count);
#else
	printf("all %zu rtw-crypto-wrap vectors passed (oracle: rust/rtw_crypto_wrap.rs)\n",
	       count);
#endif
	return 0;
}
