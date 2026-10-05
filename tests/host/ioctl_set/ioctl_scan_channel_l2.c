// SPDX-License-Identifier: GPL-2.0
/* W3-125 L2 C oracle: scan/auth/channel setter leaf helpers. */
#include <stdio.h>
#include <string.h>

#include "host_types.h"
#include "host_vector_json.h"

#define _SUCCESS 1
#define _FAIL 0
#define _TRUE 1
#define _FALSE 0
#define SCAN_PASSIVE 0
#define SCAN_ACTIVE 1
#define WIFI_FREQUENCY_BAND_2GHZ 2
#define dot11AuthAlgrthm_8021X 2
#define _WEP40_ 0x01
#define _WEP104_ 0x05
#define _NO_PRIVACY_ 0x00

typedef struct { int scan_mode; } mock_mlme_priv;
typedef struct { mock_mlme_priv mlmepriv; u8 setband; } mock_adapter;
typedef struct { u32 ndisauthtype; u32 dot11AuthAlgrthm; } mock_security_priv;

#ifdef HOST_IOCTL_SCAN_CHANNEL_RUST
extern int rtw_set_scan_mode_rust(mock_adapter *, int);
extern int rtw_set_band_rust(mock_adapter *, u8);
extern u8 rtw_add_wep_privacy_rust(u32, u32, u32 *);
extern u8 rtw_auth_mode_map_rust(mock_security_priv *, u32, u32 *);
#define O_SET_SCAN rtw_set_scan_mode_rust
#define O_SET_BAND rtw_set_band_rust
#define O_WEP_PRIV rtw_add_wep_privacy_rust
#define O_AUTH_MAP rtw_auth_mode_map_rust
#else
static int O_SET_SCAN(mock_adapter *a, int mode)
{
	if (mode != SCAN_ACTIVE && mode != SCAN_PASSIVE)
		return _FAIL;
	a->mlmepriv.scan_mode = mode;
	return _SUCCESS;
}

static int O_SET_BAND(mock_adapter *a, u8 band)
{
	if (band > WIFI_FREQUENCY_BAND_2GHZ)
		return _FAIL;
	a->setband = band;
	return _SUCCESS;
}

static u8 O_WEP_PRIV(u32 key_index, u32 key_length, u32 *privacy_out)
{
	u32 keyid = key_index & 0x3fffffff;
	u32 privacy = _NO_PRIVACY_;

	if (keyid >= 4)
		return _FALSE;
	if (key_length == 5)
		privacy = _WEP40_;
	else if (key_length == 13)
		privacy = _WEP104_;
	if (privacy_out)
		*privacy_out = privacy;
	return _TRUE;
}

static u8 O_AUTH_MAP(mock_security_priv *sec, u32 authmode, u32 *dot11_out)
{
	sec->ndisauthtype = authmode;
	if (sec->ndisauthtype > 3)
		sec->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
	if (dot11_out)
		*dot11_out = sec->dot11AuthAlgrthm;
	return _TRUE;
}
#endif

struct vector {
	char name[64];
	char fn[32];
	int scan_mode, band, expect_ret, expect_u32;
	u32 key_index, key_length, authmode, sec_dot11;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(obj, len, "fn", v->fn, sizeof(v->fn)))
		return -1;
	host_json_parse_int_in(obj, len, "scan_mode", &v->scan_mode);
	host_json_parse_int_in(obj, len, "band", &v->band);
	host_json_parse_int_in(obj, len, "key_index", (int *)&v->key_index);
	host_json_parse_int_in(obj, len, "key_length", (int *)&v->key_length);
	host_json_parse_int_in(obj, len, "authmode", (int *)&v->authmode);
	host_json_parse_int_in(obj, len, "sec_dot11", (int *)&v->sec_dot11);
	host_json_parse_int_in(obj, len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, len, "expect_u32", &v->expect_u32);
	return 0;
}

static int run_vector(struct vector *v)
{
	if (!strcmp(v->fn, "set_scan_mode")) {
		mock_adapter a = {0};
		int got = O_SET_SCAN(&a, v->scan_mode);

		if (got != v->expect_ret || (got == _SUCCESS && a.mlmepriv.scan_mode != v->expect_u32))
			goto fail;
	} else if (!strcmp(v->fn, "set_band")) {
		mock_adapter a = {0};
		int got = O_SET_BAND(&a, (u8)v->band);

		if (got != v->expect_ret || (got == _SUCCESS && a.setband != (u8)v->expect_u32))
			goto fail;
	} else if (!strcmp(v->fn, "add_wep_privacy")) {
		u32 privacy = 0;
		u8 ok = O_WEP_PRIV(v->key_index, v->key_length, &privacy);

		if (ok != (u8)v->expect_ret || (ok && privacy != (u32)v->expect_u32))
			goto fail;
	} else if (!strcmp(v->fn, "auth_mode_map")) {
		mock_security_priv sec = {0};

		sec.dot11AuthAlgrthm = v->sec_dot11;
		u32 dot11 = 0;

		O_AUTH_MAP(&sec, v->authmode, &dot11);
		if (dot11 != (u32)v->expect_u32)
			goto fail;
	} else {
		fprintf(stderr, "FAIL %s unknown fn\n", v->name);
		return 1;
	}
	printf("PASS %s\n", v->name);
	return 0;
fail:
	fprintf(stderr, "FAIL %s\n", v->name);
	return 1;
}

int main(int argc, char **argv)
{
	struct vector vecs[32];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "ioctl_scan_channel_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 32, parse_vector_object, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vector(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
