// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for WPA/RSN IE parse helpers (W3-28).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_ieee80211_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_IE 256

enum wpa_rsn_parse_fn {
	FN_PARSE_WPA_IE = 0,
	FN_RSNE_INFO_PARSE,
	FN_PARSE_WPA2_IE,
};

struct vector {
	char name[MAX_NAME];
	enum wpa_rsn_parse_fn fn;
	u8 ie[MAX_IE];
	size_t ie_len;
	int expect_ret;
	int expect_err;
	int expect_pcs_cnt;
	int expect_akm_cnt;
	int expect_group_cipher;
	int expect_pairwise_cipher;
	int expect_gmcs;
	u32 expect_akm;
	int expect_mfp_opt;
	int expect_spp_opt;
	int with_akm;
};

static int parse_fn(const char *obj, size_t obj_len, enum wpa_rsn_parse_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "rtw_parse_wpa_ie") == 0)
		*out = FN_PARSE_WPA_IE;
	else if (strcmp(fn, "rtw_rsne_info_parse") == 0)
		*out = FN_RSNE_INFO_PARSE;
	else if (strcmp(fn, "rtw_parse_wpa2_ie") == 0)
		*out = FN_PARSE_WPA2_IE;
	else
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded_len = 0;
	int json_ie_len = -1;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	if (!host_json_parse_int_in(obj, obj_len, "ie_len", &json_ie_len))
		v->ie_len = (size_t)json_ie_len;
	host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, obj_len, "expect_err", &v->expect_err);
	host_json_parse_int_in(obj, obj_len, "expect_pcs_cnt", &v->expect_pcs_cnt);
	host_json_parse_int_in(obj, obj_len, "expect_akm_cnt", &v->expect_akm_cnt);
	host_json_parse_int_in(obj, obj_len, "expect_group_cipher", &v->expect_group_cipher);
	host_json_parse_int_in(obj, obj_len, "expect_pairwise_cipher",
			       &v->expect_pairwise_cipher);
	host_json_parse_int_in(obj, obj_len, "expect_gmcs", &v->expect_gmcs);
	host_json_parse_int_in(obj, obj_len, "expect_akm", (int *)&v->expect_akm);
	host_json_parse_int_in(obj, obj_len, "expect_mfp_opt", &v->expect_mfp_opt);
	host_json_parse_int_in(obj, obj_len, "expect_spp_opt", &v->expect_spp_opt);
	host_json_parse_int_in(obj, obj_len, "with_akm", &v->with_akm);
	if (host_json_parse_string_in(obj, obj_len, "ie", hex, sizeof(hex)))
		return -1;
	if (hex[0] != '\0') {
		if (host_hex_decode(hex, v->ie, sizeof(v->ie), &decoded_len))
			return -1;
		if (json_ie_len < 0)
			v->ie_len = decoded_len;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	switch (v->fn) {
	case FN_PARSE_WPA_IE: {
		int group_cipher = 0;
		int pairwise_cipher = 0;
		u32 akm = 0;
		u32 *akm_ptr = v->with_akm ? &akm : NULL;
		int ret = rtw_parse_wpa_ie(v->ie, (int)v->ie_len, &group_cipher,
					   &pairwise_cipher, akm_ptr);

		if (ret != v->expect_ret) {
			fprintf(stderr, "%s: ret got %d expect %d\n", v->name, ret,
				v->expect_ret);
			return -1;
		}
		if (ret != _SUCCESS)
			break;
		if (group_cipher != v->expect_group_cipher) {
			fprintf(stderr, "%s: group_cipher got %d expect %d\n", v->name,
				group_cipher, v->expect_group_cipher);
			return -1;
		}
		if (pairwise_cipher != v->expect_pairwise_cipher) {
			fprintf(stderr, "%s: pairwise_cipher got %d expect %d\n", v->name,
				pairwise_cipher, v->expect_pairwise_cipher);
			return -1;
		}
		if (v->with_akm && akm != v->expect_akm) {
			fprintf(stderr, "%s: akm got %u expect %u\n", v->name, akm,
				v->expect_akm);
			return -1;
		}
		break;
	}
	case FN_RSNE_INFO_PARSE: {
		struct rsne_info info;
		int ret = rtw_rsne_info_parse(v->ie, (unsigned int)v->ie_len, &info);

		if (ret != v->expect_ret) {
			fprintf(stderr, "%s: ret got %d expect %d\n", v->name, ret,
				v->expect_ret);
			return -1;
		}
		if (info.err != (u8)v->expect_err) {
			fprintf(stderr, "%s: err got %u expect %d\n", v->name, info.err,
				v->expect_err);
			return -1;
		}
		if (ret != _SUCCESS)
			break;
		if (info.pcs_cnt != (u16)v->expect_pcs_cnt) {
			fprintf(stderr, "%s: pcs_cnt got %u expect %d\n", v->name,
				info.pcs_cnt, v->expect_pcs_cnt);
			return -1;
		}
		if (info.akm_cnt != (u16)v->expect_akm_cnt) {
			fprintf(stderr, "%s: akm_cnt got %u expect %d\n", v->name,
				info.akm_cnt, v->expect_akm_cnt);
			return -1;
		}
		break;
	}
	case FN_PARSE_WPA2_IE: {
		int group_cipher = 0;
		int pairwise_cipher = 0;
		int gmcs = 0;
		u32 akm = 0;
		u8 mfp_opt = 0;
		u8 spp_opt = 0;
		int ret = rtw_parse_wpa2_ie(v->ie, (int)v->ie_len, &group_cipher,
					    &pairwise_cipher, &gmcs, &akm, &mfp_opt,
					    &spp_opt);

		if (ret != v->expect_ret) {
			fprintf(stderr, "%s: ret got %d expect %d\n", v->name, ret,
				v->expect_ret);
			return -1;
		}
		if (ret != _SUCCESS)
			break;
		if (group_cipher != v->expect_group_cipher) {
			fprintf(stderr, "%s: group_cipher got %d expect %d\n", v->name,
				group_cipher, v->expect_group_cipher);
			return -1;
		}
		if (pairwise_cipher != v->expect_pairwise_cipher) {
			fprintf(stderr, "%s: pairwise_cipher got %d expect %d\n", v->name,
				pairwise_cipher, v->expect_pairwise_cipher);
			return -1;
		}
		if (gmcs != v->expect_gmcs) {
			fprintf(stderr, "%s: gmcs got %d expect %d\n", v->name, gmcs,
				v->expect_gmcs);
			return -1;
		}
		if (akm != v->expect_akm) {
			fprintf(stderr, "%s: akm got %u expect %u\n", v->name, akm,
				v->expect_akm);
			return -1;
		}
		if (mfp_opt != (u8)v->expect_mfp_opt) {
			fprintf(stderr, "%s: mfp_opt got %u expect %d\n", v->name, mfp_opt,
				v->expect_mfp_opt);
			return -1;
		}
		if (spp_opt != (u8)v->expect_spp_opt) {
			fprintf(stderr, "%s: spp_opt got %u expect %d\n", v->name, spp_opt,
				v->expect_spp_opt);
			return -1;
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
	const char *path = "ie_rest_wpa_rsn_parse_vectors.json";
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
		if (run_vector(&vectors[i])) {
			fprintf(stderr, "FAIL: %s\n", vectors[i].name);
			failed++;
		}
	}

	if (failed)
		return 1;

	printf("PASS: %zu vectors\n", nvec);
	return 0;
}
