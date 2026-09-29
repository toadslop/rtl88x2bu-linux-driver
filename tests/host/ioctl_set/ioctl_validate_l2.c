// SPDX-License-Identifier: GPL-2.0
/* W3-124 L2 C oracle: validate_bssid / validate_ssid. */
#include <stdio.h>
#include <string.h>

#include "host_types.h"
#include "host_vector_json.h"

#define _TRUE 1
#define _FALSE 0
#define ETH_ALEN 6

typedef struct _NDIS_802_11_SSID {
	u32 SsidLength;
	u8 Ssid[32];
} NDIS_802_11_SSID;

#define is_zero_mac_addr(Addr) \
	((Addr)[0] == 0x00 && (Addr)[1] == 0x00 && (Addr)[2] == 0x00 && \
	 (Addr)[3] == 0x00 && (Addr)[4] == 0x00 && (Addr)[5] == 0x00)
#define is_broadcast_mac_addr(Addr) \
	(((Addr)[0] == 0xff) && ((Addr)[1] == 0xff) && ((Addr)[2] == 0xff) && \
	 ((Addr)[3] == 0xff) && ((Addr)[4] == 0xff) && ((Addr)[5] == 0xff))
#define is_multicast_mac_addr(Addr) \
	((((Addr)[0]) & 0x01) == 0x01 && ((Addr)[0]) != 0xff)

static u8 rtw_validate_bssid(u8 *bssid)
{
	u8 ret = _TRUE;

	if (is_zero_mac_addr(bssid) || is_broadcast_mac_addr(bssid) ||
	    is_multicast_mac_addr(bssid))
		ret = _FALSE;

	return ret;
}

static u8 rtw_validate_ssid(NDIS_802_11_SSID *ssid)
{
	u8 ret = _TRUE;

	if (ssid->SsidLength > 32) {
		ret = _FALSE;
		goto exit;
	}

exit:
	return ret;
}

enum ioctl_validate_fn {
	FN_VALIDATE_BSSID = 0,
	FN_VALIDATE_SSID,
};

struct vector {
	char name[64];
	char fn[32];
	char mac[13];
	int ssid_len;
	char ssid_hex[128];
	int expect;
};

static int hex_nibble(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

static int decode_hex(const char *hex, u8 *out, size_t out_len, size_t *written)
{
	size_t i, n = strlen(hex) / 2;

	if (n > out_len)
		return -1;
	for (i = 0; i < n; i++) {
		int hi = hex_nibble(hex[i * 2]);
		int lo = hex_nibble(hex[i * 2 + 1]);

		if (hi < 0 || lo < 0)
			return -1;
		out[i] = (u8)((hi << 4) | lo);
	}
	*written = n;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "fn", v->fn, sizeof(v->fn)))
		return -1;
	host_json_parse_string_in(obj, len, "mac", v->mac, sizeof(v->mac));
	host_json_parse_int_in(obj, len, "ssid_len", &v->ssid_len);
	host_json_parse_string_in(obj, len, "ssid_hex", v->ssid_hex, sizeof(v->ssid_hex));
	host_json_parse_int_in(obj, len, "expect", &v->expect);
	return 0;
}

static enum ioctl_validate_fn parse_fn(const char *fn)
{
	if (strcmp(fn, "validate_bssid") == 0)
		return FN_VALIDATE_BSSID;
	if (strcmp(fn, "validate_ssid") == 0)
		return FN_VALIDATE_SSID;
	return -1;
}

static int run_vector(struct vector *v)
{
	enum ioctl_validate_fn fn = parse_fn(v->fn);
	u8 got;

	if (fn < 0) {
		fprintf(stderr, "FAIL %s unknown fn %s\n", v->name, v->fn);
		return 1;
	}

	if (fn == FN_VALIDATE_BSSID) {
		u8 mac[ETH_ALEN];
		size_t n = 0;

		if (!v->mac[0] || decode_hex(v->mac, mac, sizeof(mac), &n) ||
		    n != ETH_ALEN) {
			fprintf(stderr, "FAIL %s bad mac hex\n", v->name);
			return 1;
		}
		got = rtw_validate_bssid(mac);
	} else {
		NDIS_802_11_SSID ssid;
		size_t n = 0;

		memset(&ssid, 0, sizeof(ssid));
		ssid.SsidLength = (u32)v->ssid_len;
		if (v->ssid_hex[0] &&
		    decode_hex(v->ssid_hex, ssid.Ssid, sizeof(ssid.Ssid), &n))
			return 1;
		got = rtw_validate_ssid(&ssid);
	}

	if (got == (u8)v->expect) {
		printf("PASS %s\n", v->name);
		return 0;
	}
	fprintf(stderr, "FAIL %s got=%u expect=%d\n", v->name, got, v->expect);
	return 1;
}

int main(int argc, char **argv)
{
	struct vector vecs[32];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "ioctl_validate_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 32, parse_vector_object, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vector(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
