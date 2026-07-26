// SPDX-License-Identifier: GPL-2.0
/*
 * swcrypto wrapper vector parse + oracle runner (W3-01).
 */

#include <stdio.h>
#include <string.h>

#include "host_swcrypto_vector.h"
#include "host_vector_json.h"
#include "host_wifi_types.h"

#define _SUCCESS 1
#define _FAIL 0

#define CCMP_ENC_OVERHEAD 16
#define GCMP_ENC_OVERHEAD 24

int _rtw_ccmp_encrypt(_adapter *padapter, u8 *key, u32 key_len, unsigned int hdrlen,
		      u8 *frame, unsigned int plen);
int _rtw_ccmp_decrypt(_adapter *padapter, u8 *key, u32 key_len, unsigned int hdrlen,
		      u8 *frame, unsigned int plen);
int _rtw_gcmp_encrypt(_adapter *padapter, u8 *key, u32 key_len, unsigned int hdrlen,
		      u8 *frame, unsigned int plen);
int _rtw_gcmp_decrypt(_adapter *padapter, u8 *key, u32 key_len, unsigned int hdrlen,
		      u8 *frame, unsigned int plen);

static int parse_fn(const char *obj, size_t obj_len, enum host_swcrypto_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "_rtw_ccmp_encrypt") == 0) {
		*out = HOST_SWCRYPTO_FN_CCMP_ENCRYPT;
		return 0;
	}
	if (strcmp(fn, "_rtw_ccmp_decrypt") == 0) {
		*out = HOST_SWCRYPTO_FN_CCMP_DECRYPT;
		return 0;
	}
	if (strcmp(fn, "_rtw_gcmp_encrypt") == 0) {
		*out = HOST_SWCRYPTO_FN_GCMP_ENCRYPT;
		return 0;
	}
	if (strcmp(fn, "_rtw_gcmp_decrypt") == 0) {
		*out = HOST_SWCRYPTO_FN_GCMP_DECRYPT;
		return 0;
	}
	return -1;
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

int host_swcrypto_parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct host_swcrypto_vector *v = vec_void;
	int key_len = 0;
	int hdrlen = 0;
	int plen = 0;
	int expect_ret = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	if (host_json_parse_int_in(obj, obj_len, "key_len", &key_len))
		return -1;
	v->key_len = (u32)key_len;
	{
		size_t key_hex_len = 0;

		if (parse_hex_field(obj, obj_len, "key", v->key, sizeof(v->key), &key_hex_len))
			return -1;
	}
	if (host_json_parse_int_in(obj, obj_len, "hdrlen", &hdrlen))
		return -1;
	v->hdrlen = (u32)hdrlen;
	if (host_json_parse_int_in(obj, obj_len, "plen", &plen))
		return -1;
	v->plen = (u32)plen;
	if (parse_hex_field(obj, obj_len, "frame", v->frame, sizeof(v->frame), &v->frame_len))
		return -1;
	if (host_json_parse_int_in(obj, obj_len, "expect_ret", &expect_ret))
		return -1;
	v->expect_ret = expect_ret;
	return 0;
}

static u32 ccmp_enc_total(u32 key_len, u32 hdrlen, u32 plen)
{
	u32 overhead = (key_len == 32) ? 24 : 16;

	return hdrlen + plen + overhead;
}

static int round_trip_ccmp(_adapter *adapter, u8 *key, u32 key_len, u32 hdrlen,
			   u8 *frame, u32 plen)
{
	u8 payload[HOST_SWCRYPTO_MAX_FRAME];
	u32 enc_plen;
	int ret;

	if (hdrlen + 8 + plen > sizeof(payload))
		return -1;
	memcpy(payload, frame + hdrlen + 8, plen);

	ret = _rtw_ccmp_encrypt(adapter, key, key_len, hdrlen, frame, plen);
	if (ret != _SUCCESS)
		return ret;

	enc_plen = ccmp_enc_total(key_len, hdrlen, plen);
	ret = _rtw_ccmp_decrypt(adapter, key, key_len, hdrlen, frame, enc_plen);
	if (ret != _SUCCESS)
		return ret;

	if (memcmp(frame + hdrlen + 8, payload, plen) != 0)
		return -1;
	return _SUCCESS;
}

static int round_trip_gcmp(_adapter *adapter, u8 *key, u32 key_len, u32 hdrlen,
			   u8 *frame, u32 plen)
{
	u8 payload[HOST_SWCRYPTO_MAX_FRAME];
	u32 enc_plen;
	int ret;

	if (hdrlen + 8 + plen > sizeof(payload))
		return -1;
	memcpy(payload, frame + hdrlen + 8, plen);

	ret = _rtw_gcmp_encrypt(adapter, key, key_len, hdrlen, frame, plen);
	if (ret != _SUCCESS)
		return ret;

	enc_plen = hdrlen + plen + GCMP_ENC_OVERHEAD;
	ret = _rtw_gcmp_decrypt(adapter, key, key_len, hdrlen, frame, enc_plen);
	if (ret != _SUCCESS)
		return ret;

	if (memcmp(frame + hdrlen + 8, payload, plen) != 0)
		return -1;
	return _SUCCESS;
}

int host_swcrypto_run_vector(const struct host_swcrypto_vector *v)
{
	_adapter adapter = {0};
	u8 frame[HOST_SWCRYPTO_MAX_FRAME];
	u8 key[32];
	int ret;

	if (v->frame_len > sizeof(frame)) {
		fprintf(stderr, "%s: frame too large\n", v->name);
		return -1;
	}
	memcpy(frame, v->frame, v->frame_len);
	memcpy(key, v->key, sizeof(key));

	switch (v->fn) {
	case HOST_SWCRYPTO_FN_CCMP_ENCRYPT:
		if (v->expect_ret == _SUCCESS)
			ret = round_trip_ccmp(&adapter, key, v->key_len, v->hdrlen, frame,
					      v->plen);
		else
			ret = _rtw_ccmp_encrypt(&adapter, key, v->key_len, v->hdrlen, frame,
						v->plen);
		break;
	case HOST_SWCRYPTO_FN_CCMP_DECRYPT:
		ret = round_trip_ccmp(&adapter, key, v->key_len, v->hdrlen, frame, v->plen);
		break;
	case HOST_SWCRYPTO_FN_GCMP_ENCRYPT:
		if (v->expect_ret == _SUCCESS)
			ret = round_trip_gcmp(&adapter, key, v->key_len, v->hdrlen, frame,
					      v->plen);
		else
			ret = _rtw_gcmp_encrypt(&adapter, key, v->key_len, v->hdrlen, frame,
						v->plen);
		break;
	case HOST_SWCRYPTO_FN_GCMP_DECRYPT:
		ret = round_trip_gcmp(&adapter, key, v->key_len, v->hdrlen, frame, v->plen);
		break;
	default:
		fprintf(stderr, "%s: unknown fn\n", v->name);
		return -1;
	}

	if (ret != v->expect_ret) {
		fprintf(stderr, "%s: ret %d expected %d\n", v->name, ret, v->expect_ret);
		return -1;
	}
	return 0;
}
