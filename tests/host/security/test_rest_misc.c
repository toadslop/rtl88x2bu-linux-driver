// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for security_rest misc helpers (W3-15 PR1).
 */

#if defined(HOST_REST_MISC_ORACLE_BUILD) || defined(RUST_SECURITY_REST_ORACLE)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_security_types.h"
#include "host_vector_json.h"

typedef int sint;

#define MAX_VECTORS 16
#define MAX_NAME 128
#define MAX_BUF 256
#define MAX_ELEMENTS 4
#define MAX_ELEM_LEN 64
#define MAX_PW_LEN 64
#define MAX_OUT_LEN 256
#define MAX_CALLS 8

enum rest_misc_fn {
	FN_CALC_CRC32 = 0,
	FN_AES_SIV_ENCRYPT,
	FN_AES_SIV_DECRYPT,
	FN_RESTORE_WEP_KEY,
};

struct restore_call {
	int keyid;
	u8 set_tx;
};

struct vector {
	char name[MAX_NAME];
	enum rest_misc_fn fn;
	u8 data[MAX_BUF];
	size_t data_len;
	u32 expect_crc;
	size_t key_len;
	u8 key[MAX_BUF];
	size_t num_elem;
	u8 elem[MAX_ELEMENTS][MAX_ELEM_LEN];
	size_t elem_len[MAX_ELEMENTS];
	u8 pw[MAX_PW_LEN];
	size_t pw_len;
	u8 iv_crypt[MAX_OUT_LEN];
	size_t iv_crypt_len;
	u8 expected_out[MAX_OUT_LEN];
	size_t expected_out_len;
	int expect_ret;
	u32 privacy_alg;
	u32 privacy_key_index;
	u8 key_mask;
	size_t expect_call_count;
	struct restore_call expect_calls[MAX_CALLS];
};

#ifdef RUST_SECURITY_REST_ORACLE
extern u32 host_rest_calc_crc32(u8 *data, size_t len);
extern int host_rest_aes_siv_encrypt(const u8 *key, size_t key_len, const u8 *pw,
				     size_t pwlen, size_t num_elem,
				     const u8 *addr[], const size_t *len, u8 *out);
extern int host_rest_aes_siv_decrypt(const u8 *key, size_t key_len,
				     const u8 *iv_crypt, size_t iv_c_len,
				     size_t num_elem, const u8 *addr[],
				     const size_t *len, u8 *out);
extern void host_rest_sec_restore_wep_key(void *adapter);
extern void host_restore_wep_reset_calls(void);
extern size_t host_restore_wep_get_call_count(void);
extern sint host_restore_wep_get_call_keyid(size_t idx);
extern u8 host_restore_wep_get_call_set_tx(size_t idx);
#else
u32 host_rest_calc_crc32(u8 *data, size_t len);
int host_rest_aes_siv_encrypt(const u8 *key, size_t key_len, const u8 *pw,
			      size_t pwlen, size_t num_elem, const u8 *addr[],
			      const size_t *len, u8 *out);
int host_rest_aes_siv_decrypt(const u8 *key, size_t key_len,
			      const u8 *iv_crypt, size_t iv_c_len,
			      size_t num_elem, const u8 *addr[], const size_t *len,
			      u8 *out);
struct host_restore_wep_security_priv {
	u32 dot11PrivacyAlgrthm;
	u32 dot11PrivacyKeyIndex;
	u8 key_mask;
};

struct host_restore_wep_adapter {
	struct host_restore_wep_security_priv securitypriv;
};

void host_rest_sec_restore_wep_key(struct host_restore_wep_adapter *adapter);
void host_restore_wep_reset_calls(void);
size_t host_restore_wep_get_call_count(void);
sint host_restore_wep_get_call_keyid(size_t idx);
u8 host_restore_wep_get_call_set_tx(size_t idx);
#endif

static int parse_fn(const char *obj, size_t obj_len, enum rest_misc_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "host_rest_calc_crc32") == 0)
		*out = FN_CALC_CRC32;
	else if (strcmp(fn, "host_rest_aes_siv_encrypt") == 0)
		*out = FN_AES_SIV_ENCRYPT;
	else if (strcmp(fn, "host_rest_aes_siv_decrypt") == 0)
		*out = FN_AES_SIV_DECRYPT;
	else if (strcmp(fn, "host_rest_sec_restore_wep_key") == 0)
		*out = FN_RESTORE_WEP_KEY;
	else
		return -1;
	return 0;
}

static int parse_hex_field(const char *obj, size_t obj_len, const char *key,
			   u8 *out, size_t out_cap, size_t *out_len)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	if (host_json_parse_string_in(obj, obj_len, key, hex, sizeof(hex)))
		return -1;
	return host_hex_decode(hex, out, out_cap, out_len);
}

static int parse_elements(const char *obj, size_t obj_len, struct vector *v)
{
	const char *p = host_json_find_key_in(obj, obj_len, "elements");
	size_t count = 0;

	if (!p || p >= obj + obj_len || *p != '[')
		return 0;
	p++;
	while (count < MAX_ELEMENTS) {
		char hex[HOST_VECTOR_MAX_HEX_BUF];

		p = host_json_skip_ws(p);
		if (*p == ']')
			break;
		if (*p != '"')
			return -1;
		{
			const char *start = p + 1;
			const char *end = strchr(start, '"');

			if (!end || (size_t)(end - start) + 1 >= sizeof(hex))
				return -1;
			memcpy(hex, start, (size_t)(end - start));
			hex[end - start] = '\0';
			p = end + 1;
			if (host_hex_decode(hex, v->elem[count], MAX_ELEM_LEN,
					    &v->elem_len[count]))
				return -1;
		}
		count++;
		p = host_json_skip_ws(p);
		if (*p == ',')
			p++;
	}
	v->num_elem = count;
	return 0;
}

static int parse_expect_calls(const char *obj, size_t obj_len, struct vector *v)
{
	const char *p = host_json_find_key_in(obj, obj_len, "expect_calls");
	size_t count = 0;

	if (!p || p >= obj + obj_len || *p != '[')
		return 0;
	p++;
	while (count < MAX_CALLS) {
		const char *start;
		const char *end;
		size_t call_len;
		int keyid = 0;
		int set_tx = 0;

		p = host_json_skip_ws(p);
		if (*p == ']')
			break;
		if (*p != '{')
			return -1;
		start = p;
		end = strchr(start, '}');
		if (!end)
			return -1;
		call_len = (size_t)(end - start + 1);
		host_json_parse_int_in(start, call_len, "keyid", &keyid);
		host_json_parse_int_in(start, call_len, "set_tx", &set_tx);
		v->expect_calls[count].keyid = keyid;
		v->expect_calls[count].set_tx = (u8)set_tx;
		count++;
		p = end + 1;
		p = host_json_skip_ws(p);
		if (*p == ',')
			p++;
	}
	v->expect_call_count = count;
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	int val = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	parse_hex_field(obj, obj_len, "data", v->data, sizeof(v->data), &v->data_len);
	if (!host_json_parse_int_in(obj, obj_len, "expect_crc", &val))
		v->expect_crc = (u32)val;
	if (!host_json_parse_int_in(obj, obj_len, "key_len", &val))
		v->key_len = (size_t)val;
	parse_hex_field(obj, obj_len, "key", v->key, sizeof(v->key), &v->key_len);
	parse_elements(obj, obj_len, v);
	parse_hex_field(obj, obj_len, "pw", v->pw, sizeof(v->pw), &v->pw_len);
	parse_hex_field(obj, obj_len, "iv_crypt", v->iv_crypt, sizeof(v->iv_crypt),
			&v->iv_crypt_len);
	parse_hex_field(obj, obj_len, "expected_out", v->expected_out,
			sizeof(v->expected_out), &v->expected_out_len);
	if (!host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret))
		v->expect_ret = 0;
	if (!host_json_parse_int_in(obj, obj_len, "privacy_alg", &val))
		v->privacy_alg = (u32)val;
	if (!host_json_parse_int_in(obj, obj_len, "privacy_key_index", &val))
		v->privacy_key_index = (u32)val;
	if (!host_json_parse_int_in(obj, obj_len, "key_mask", &val))
		v->key_mask = (u8)val;
	{
		int call_count = 0;

		if (!host_json_parse_int_in(obj, obj_len, "expect_call_count",
					    &call_count))
			v->expect_call_count = (size_t)call_count;
	}
	parse_expect_calls(obj, obj_len, v);
	return 0;
}

static int run_vector(struct vector *v)
{
	switch (v->fn) {
	case FN_CALC_CRC32: {
		u32 got = host_rest_calc_crc32(v->data, v->data_len);

		if (got != v->expect_crc) {
			fprintf(stderr, "%s: crc32 mismatch got=%08x expect=%08x\n",
				v->name, got, v->expect_crc);
			return -1;
		}
		break;
	}
	case FN_AES_SIV_ENCRYPT: {
		const u8 *addr[MAX_ELEMENTS];
		size_t i;
		u8 out[MAX_OUT_LEN];
		int ret;

		for (i = 0; i < v->num_elem; i++)
			addr[i] = v->elem[i];
		ret = host_rest_aes_siv_encrypt(v->key, v->key_len, v->pw, v->pw_len,
						v->num_elem, addr, v->elem_len, out);
		if (ret != v->expect_ret) {
			fprintf(stderr, "%s: aes_siv_encrypt ret=%d expect=%d\n",
				v->name, ret, v->expect_ret);
			return -1;
		}
		break;
	}
	case FN_AES_SIV_DECRYPT: {
		const u8 *addr[MAX_ELEMENTS];
		size_t i;
		u8 out[MAX_OUT_LEN];
		int ret;

		for (i = 0; i < v->num_elem; i++)
			addr[i] = v->elem[i];
		ret = host_rest_aes_siv_decrypt(v->key, v->key_len, v->iv_crypt,
						v->iv_crypt_len, v->num_elem, addr,
						v->elem_len, out);
		if (ret != v->expect_ret) {
			fprintf(stderr, "%s: aes_siv_decrypt ret=%d expect=%d\n",
				v->name, ret, v->expect_ret);
			return -1;
		}
		if (v->expected_out_len &&
		    memcmp(out, v->expected_out, v->expected_out_len) != 0) {
			fprintf(stderr, "%s: aes_siv_decrypt output mismatch\n",
				v->name);
			return -1;
		}
		break;
	}
	case FN_RESTORE_WEP_KEY: {
		struct host_restore_wep_adapter adapter;
		size_t i;

		memset(&adapter, 0, sizeof(adapter));
		adapter.securitypriv.dot11PrivacyAlgrthm = v->privacy_alg;
		adapter.securitypriv.dot11PrivacyKeyIndex = v->privacy_key_index;
		adapter.securitypriv.key_mask = v->key_mask;
		host_restore_wep_reset_calls();
		host_rest_sec_restore_wep_key(&adapter);
		if (host_restore_wep_get_call_count() != v->expect_call_count) {
			fprintf(stderr,
				"%s: restore_wep call_count=%zu expect=%zu\n",
				v->name, host_restore_wep_get_call_count(),
				v->expect_call_count);
			return -1;
		}
		for (i = 0; i < v->expect_call_count; i++) {
			if (host_restore_wep_get_call_keyid(i) !=
				    v->expect_calls[i].keyid ||
			    host_restore_wep_get_call_set_tx(i) !=
				    v->expect_calls[i].set_tx) {
				fprintf(stderr,
					"%s: restore_wep call[%zu] mismatch\n",
					v->name, i);
				return -1;
			}
		}
		break;
	}
	default:
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "rest_misc_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t nvec = 0;
	size_t i;
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to parse %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
		if (run_vector(&vectors[i]) != 0)
			failed++;
		else
			printf("ok %s\n", vectors[i].name);
	}

	if (failed) {
		fprintf(stderr, "%d vector(s) failed\n", failed);
		return 1;
	}
	printf("all %zu rest misc vectors passed\n", nvec);
	return 0;
}

#endif /* HOST_REST_MISC_ORACLE_BUILD || RUST_SECURITY_REST_ORACLE */
