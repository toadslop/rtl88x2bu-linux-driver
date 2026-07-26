// SPDX-License-Identifier: GPL-2.0
/*
 * swcrypto wrapper vector parse + oracle runner (W3-01/W3-02).
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
u8 _bip_ccmp_protect(const u8 *key, size_t key_len, const u8 *data, size_t data_len,
		     u8 *mic);
u8 _bip_gcmp_protect(u8 *whdr_pos, size_t len, const u8 *key, size_t key_len,
		     const u8 *data, size_t data_len, u8 *mic);
int _aes_siv_encrypt(const u8 *key, size_t key_len, const u8 *pw, size_t pwlen,
		     size_t num_elem, const u8 *addr[], const size_t *len, u8 *out);
int _aes_siv_decrypt(const u8 *key, size_t key_len, const u8 *iv_crypt, size_t iv_c_len,
		     size_t num_elem, const u8 *addr[], const size_t *len, u8 *out);

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
	if (strcmp(fn, "_bip_ccmp_protect") == 0) {
		*out = HOST_SWCRYPTO_FN_BIP_CCMP_PROTECT;
		return 0;
	}
	if (strcmp(fn, "_bip_gcmp_protect") == 0) {
		*out = HOST_SWCRYPTO_FN_BIP_GCMP_PROTECT;
		return 0;
	}
	if (strcmp(fn, "_aes_siv_encrypt") == 0) {
		*out = HOST_SWCRYPTO_FN_AES_SIV_ENCRYPT;
		return 0;
	}
	if (strcmp(fn, "_aes_siv_decrypt") == 0) {
		*out = HOST_SWCRYPTO_FN_AES_SIV_DECRYPT;
		return 0;
	}
	return -1;
}

static int parse_hex_field(const char *obj, size_t obj_len, const char *key,
			   u8 *buf, size_t buf_cap, size_t *out_len)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	if (host_json_parse_string_in(obj, obj_len, key, hex, sizeof(hex)))
		return 0;
	if (!*hex) {
		*out_len = 0;
		return 0;
	}
	return host_hex_decode(hex, buf, buf_cap, out_len);
}

static int parse_elements_in(const char *obj, size_t obj_len, const char *key,
			     struct host_swcrypto_vector *v)
{
	const char *p = host_json_find_key_in(obj, obj_len, key);
	size_t count = 0;

	if (!p || p >= obj + obj_len || *p != '[')
		return 0;
	p++;
	while (count < HOST_SWCRYPTO_MAX_ELEMENTS) {
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
		}
		if (host_hex_decode(hex, v->elements[count],
				    sizeof(v->elements[count]),
				    &v->element_lens[count]))
			return -1;
		count++;
		p = host_json_skip_ws(p);
		if (*p == ',')
			p++;
	}
	v->num_elements = count;
	return 0;
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
	if (host_json_parse_int_in(obj, obj_len, "hdrlen", &hdrlen) == 0)
		v->hdrlen = (u32)hdrlen;
	if (host_json_parse_int_in(obj, obj_len, "plen", &plen) == 0)
		v->plen = (u32)plen;
	if (parse_hex_field(obj, obj_len, "frame", v->frame, sizeof(v->frame), &v->frame_len))
		return -1;
	if (parse_hex_field(obj, obj_len, "data", v->data, sizeof(v->data), &v->data_len))
		return -1;
	if (parse_hex_field(obj, obj_len, "expected_mic", v->expected_mic,
			    sizeof(v->expected_mic), &v->expected_mic_len))
		return -1;
	if (parse_hex_field(obj, obj_len, "pw", v->pw, sizeof(v->pw), &v->pw_len))
		return -1;
	if (parse_hex_field(obj, obj_len, "iv_crypt", v->iv_crypt, sizeof(v->iv_crypt),
			    &v->iv_crypt_len))
		return -1;
	if (parse_hex_field(obj, obj_len, "expected_out", v->expected_out,
			    sizeof(v->expected_out), &v->expected_out_len))
		return -1;
	if (parse_elements_in(obj, obj_len, "elements", v))
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

static int round_trip_aes_siv(const struct host_swcrypto_vector *v)
{
	const u8 *addr[HOST_SWCRYPTO_MAX_ELEMENTS];
	u8 out[HOST_SWCRYPTO_MAX_OUT];
	u8 plain[HOST_SWCRYPTO_MAX_DATA];
	size_t i;
	int ret;

	for (i = 0; i < v->num_elements; i++)
		addr[i] = v->elements[i];

	ret = _aes_siv_encrypt(v->key, v->key_len, v->pw, v->pw_len, v->num_elements,
			       addr, v->element_lens, out);
	if (ret != 0)
		return ret;

	ret = _aes_siv_decrypt(v->key, v->key_len, out, v->pw_len + 16, v->num_elements,
			       addr, v->element_lens, plain);
	if (ret != 0)
		return ret;

	if (memcmp(plain, v->pw, v->pw_len) != 0)
		return -1;
	return 0;
}

int host_swcrypto_run_vector(const struct host_swcrypto_vector *v)
{
	_adapter adapter = {0};
	u8 frame[HOST_SWCRYPTO_MAX_FRAME];
	u8 key[32];
	u8 mic[16];
	const u8 *addr[HOST_SWCRYPTO_MAX_ELEMENTS];
	u8 out[HOST_SWCRYPTO_MAX_OUT];
	size_t i;
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
	case HOST_SWCRYPTO_FN_BIP_CCMP_PROTECT:
		ret = _bip_ccmp_protect(key, v->key_len, v->data, v->data_len, mic);
		if (ret == (u8)_SUCCESS && v->expected_mic_len &&
		    memcmp(mic, v->expected_mic, v->expected_mic_len) != 0)
			ret = _FAIL;
		break;
	case HOST_SWCRYPTO_FN_BIP_GCMP_PROTECT:
		ret = _bip_gcmp_protect(frame, v->frame_len, key, v->key_len, v->data,
					v->data_len, mic);
		if (ret == (u8)_SUCCESS && v->expected_mic_len &&
		    memcmp(mic, v->expected_mic, v->expected_mic_len) != 0)
			ret = _FAIL;
		break;
	case HOST_SWCRYPTO_FN_AES_SIV_ENCRYPT:
		if (v->expect_ret == _SUCCESS)
			ret = round_trip_aes_siv(v);
		else {
			for (i = 0; i < v->num_elements; i++)
				addr[i] = v->elements[i];
			ret = _aes_siv_encrypt(v->key, v->key_len, v->pw, v->pw_len,
					       v->num_elements, addr, v->element_lens, out);
		}
		break;
	case HOST_SWCRYPTO_FN_AES_SIV_DECRYPT:
		for (i = 0; i < v->num_elements; i++)
			addr[i] = v->elements[i];
		ret = _aes_siv_decrypt(v->key, v->key_len, v->iv_crypt, v->iv_crypt_len,
				       v->num_elements, addr, v->element_lens, out);
		if (ret == 0 && v->expected_out_len &&
		    memcmp(out, v->expected_out, v->expected_out_len) != 0)
			ret = -1;
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
