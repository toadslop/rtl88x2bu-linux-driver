// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for security_rest TDLS helpers (W3-16 PR1).
 */

#if defined(HOST_REST_TDLS_ORACLE_BUILD) || defined(RUST_SECURITY_REST_ORACLE)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_security_types.h"
#include "host_security_tdls.h"
#include "host_vector_json.h"

typedef int sint;

#define MAX_VECTORS 16
#define MAX_NAME 128
#define MAX_BUF 128
#define MAX_MIC 16
#define MAX_TPK 32

enum rest_tdls_fn {
	FN_GENERATE_TPK = 0,
	FN_FTIE_MIC,
	FN_TEARDOWN_FTIE_MIC,
	FN_VERIFY_MIC,
};

struct vector {
	char name[MAX_NAME];
	enum rest_tdls_fn fn;
	u8 own_addr[6];
	u8 peer_addr[6];
	u8 bssid[6];
	u8 snonce[32];
	u8 anonce[32];
	u8 expect_tpk[MAX_TPK];
	size_t expect_tpk_len;
	u8 kck[16];
	size_t kck_len;
	u8 trans_seq;
	u8 lnkid[MAX_BUF];
	size_t lnkid_len;
	u8 rsnie[MAX_BUF];
	size_t rsnie_len;
	u8 timeoutie[MAX_BUF];
	size_t timeoutie_len;
	u8 ftie[MAX_BUF];
	size_t ftie_len;
	u16 reason;
	u8 dialog_token;
	u8 expect_mic[MAX_MIC];
	size_t expect_mic_len;
	int expect_ret;
	char null_ie[16];
};

#ifdef RUST_SECURITY_REST_ORACLE
extern void host_rest_tdls_generate_tpk(struct host_tdls_adapter *adapter,
					struct host_tdls_sta *sta);
extern int host_rest_wpa_tdls_ftie_mic(u8 *kck, u8 trans_seq, u8 *lnkid,
				       u8 *rsnie, u8 *timeoutie, u8 *ftie,
				       u8 *mic);
extern int host_rest_wpa_tdls_teardown_ftie_mic(u8 *kck, u8 *lnkid, u16 reason,
						u8 dialog_token, u8 trans_seq,
						u8 *ftie, u8 *mic);
extern int host_rest_tdls_verify_mic(u8 *kck, u8 trans_seq, u8 *lnkid,
				     u8 *rsnie, u8 *timeoutie, u8 *ftie);
#endif

static int parse_fn(const char *obj, size_t obj_len, enum rest_tdls_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "host_rest_tdls_generate_tpk") == 0)
		*out = FN_GENERATE_TPK;
	else if (strcmp(fn, "host_rest_wpa_tdls_ftie_mic") == 0)
		*out = FN_FTIE_MIC;
	else if (strcmp(fn, "host_rest_wpa_tdls_teardown_ftie_mic") == 0)
		*out = FN_TEARDOWN_FTIE_MIC;
	else if (strcmp(fn, "host_rest_tdls_verify_mic") == 0)
		*out = FN_VERIFY_MIC;
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

static int parse_mac_field(const char *obj, size_t obj_len, const char *key,
			   u8 out[6])
{
	size_t len = 0;

	return parse_hex_field(obj, obj_len, key, out, 6, &len) || len != 6;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	int val = 0;
	size_t len = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	parse_mac_field(obj, obj_len, "own_addr", v->own_addr);
	parse_mac_field(obj, obj_len, "peer_addr", v->peer_addr);
	parse_mac_field(obj, obj_len, "bssid", v->bssid);
	parse_hex_field(obj, obj_len, "snonce", v->snonce, sizeof(v->snonce), &len);
	parse_hex_field(obj, obj_len, "anonce", v->anonce, sizeof(v->anonce), &len);
	parse_hex_field(obj, obj_len, "expect_tpk", v->expect_tpk,
			sizeof(v->expect_tpk), &v->expect_tpk_len);
	parse_hex_field(obj, obj_len, "kck", v->kck, sizeof(v->kck), &v->kck_len);
	if (!host_json_parse_int_in(obj, obj_len, "trans_seq", &val))
		v->trans_seq = (u8)val;
	parse_hex_field(obj, obj_len, "lnkid", v->lnkid, sizeof(v->lnkid),
			&v->lnkid_len);
	parse_hex_field(obj, obj_len, "rsnie", v->rsnie, sizeof(v->rsnie),
			&v->rsnie_len);
	parse_hex_field(obj, obj_len, "timeoutie", v->timeoutie,
			sizeof(v->timeoutie), &v->timeoutie_len);
	parse_hex_field(obj, obj_len, "ftie", v->ftie, sizeof(v->ftie),
			&v->ftie_len);
	if (!host_json_parse_int_in(obj, obj_len, "reason", &val))
		v->reason = (u16)val;
	if (!host_json_parse_int_in(obj, obj_len, "dialog_token", &val))
		v->dialog_token = (u8)val;
	parse_hex_field(obj, obj_len, "expect_mic", v->expect_mic,
			sizeof(v->expect_mic), &v->expect_mic_len);
	if (host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret))
		v->expect_ret = 0;
	host_json_parse_string_in(obj, obj_len, "null_ie", v->null_ie, sizeof(v->null_ie));
	return 0;
}

static int run_vector(struct vector *v)
{
	switch (v->fn) {
	case FN_GENERATE_TPK: {
		struct host_tdls_adapter adapter;
		struct host_tdls_sta sta;

		memset(&adapter, 0, sizeof(adapter));
		memset(&sta, 0, sizeof(sta));
		memcpy(adapter.mac_addr, v->own_addr, 6);
		memcpy(adapter.mlmepriv.bssid, v->bssid, 6);
		memcpy(sta.mac_addr, v->peer_addr, 6);
		memcpy(sta.SNonce, v->snonce, 32);
		memcpy(sta.ANonce, v->anonce, 32);
		host_rest_tdls_generate_tpk(&adapter, &sta);
		if (v->expect_tpk_len &&
		    memcmp(&sta.tpk, v->expect_tpk, v->expect_tpk_len) != 0) {
			fprintf(stderr, "%s: tpk mismatch\n", v->name);
			return -1;
		}
		break;
	}
	case FN_FTIE_MIC: {
		u8 mic[16];
		int ret;

		ret = host_rest_wpa_tdls_ftie_mic(v->kck, v->trans_seq, v->lnkid,
						  v->rsnie, v->timeoutie, v->ftie,
						  mic);
		if (ret != v->expect_ret) {
			fprintf(stderr, "%s: ftie_mic ret=%d expect=%d\n", v->name,
				ret, v->expect_ret);
			return -1;
		}
		if (v->expect_mic_len && memcmp(mic, v->expect_mic, v->expect_mic_len) != 0) {
			fprintf(stderr, "%s: ftie_mic output mismatch\n", v->name);
			return -1;
		}
		break;
	}
	case FN_TEARDOWN_FTIE_MIC: {
		u8 mic[16];
		int ret;

		ret = host_rest_wpa_tdls_teardown_ftie_mic(v->kck, v->lnkid,
							   v->reason,
							   v->dialog_token,
							   v->trans_seq, v->ftie,
							   mic);
		if (ret != v->expect_ret) {
			fprintf(stderr, "%s: teardown_mic ret=%d expect=%d\n",
				v->name, ret, v->expect_ret);
			return -1;
		}
		if (v->expect_mic_len && memcmp(mic, v->expect_mic, v->expect_mic_len) != 0) {
			fprintf(stderr, "%s: teardown_mic output mismatch\n", v->name);
			return -1;
		}
		break;
	}
	case FN_VERIFY_MIC: {
		int ret;
		u8 *lnkid = v->lnkid_len ? v->lnkid : NULL;
		u8 *rsnie = v->rsnie_len ? v->rsnie : NULL;
		u8 *timeoutie = v->timeoutie_len ? v->timeoutie : NULL;
		u8 *ftie = v->ftie_len ? v->ftie : NULL;

		if (v->null_ie[0]) {
			if (strcmp(v->null_ie, "lnkid") == 0)
				lnkid = NULL;
			else if (strcmp(v->null_ie, "rsnie") == 0)
				rsnie = NULL;
			else if (strcmp(v->null_ie, "timeoutie") == 0)
				timeoutie = NULL;
			else if (strcmp(v->null_ie, "ftie") == 0)
				ftie = NULL;
			else
				return -1;
		}

		ret = host_rest_tdls_verify_mic(v->kck, v->trans_seq, lnkid,
						rsnie, timeoutie, ftie);
		if (ret != v->expect_ret) {
			fprintf(stderr, "%s: verify_mic ret=%d expect=%d\n", v->name,
				ret, v->expect_ret);
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
	const char *path = "tdls_vectors.json";
	static struct vector vectors[MAX_VECTORS];
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
	printf("all %zu rest tdls vectors passed\n", nvec);
	return 0;
}

#endif /* HOST_REST_TDLS_ORACLE_BUILD || RUST_SECURITY_REST_ORACLE */
