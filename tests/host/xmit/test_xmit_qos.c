// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_xmit_qos_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128

struct vector {
	char name[MAX_NAME];
	char fn[64];
	int acm_mask, priority, tos, hdrlen, iv_len, pktlen, encrypt, bswenc, icv_len;
	int h_proto, expect_u8, expect_snap_len;
	unsigned expect_u32;
	unsigned char expect_snap[16];
};

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char snap[128];

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(obj, obj_len, "fn", v->fn, sizeof(v->fn)))
		return -1;
	host_json_parse_int_in(obj, obj_len, "acm_mask", &v->acm_mask);
	host_json_parse_int_in(obj, obj_len, "priority", &v->priority);
	host_json_parse_int_in(obj, obj_len, "tos", &v->tos);
	host_json_parse_int_in(obj, obj_len, "hdrlen", &v->hdrlen);
	host_json_parse_int_in(obj, obj_len, "iv_len", &v->iv_len);
	host_json_parse_int_in(obj, obj_len, "pktlen", &v->pktlen);
	host_json_parse_int_in(obj, obj_len, "encrypt", &v->encrypt);
	host_json_parse_int_in(obj, obj_len, "bswenc", &v->bswenc);
	host_json_parse_int_in(obj, obj_len, "icv_len", &v->icv_len);
	host_json_parse_int_in(obj, obj_len, "h_proto", &v->h_proto);
	host_json_parse_int_in(obj, obj_len, "expect_u8", &v->expect_u8);
	host_json_parse_int_in(obj, obj_len, "expect_snap_len", &v->expect_snap_len);
	{
		int tmp = 0;
		if (!host_json_parse_int_in(obj, obj_len, "expect_u32", &tmp))
			v->expect_u32 = (unsigned)tmp;
	}
	if (!host_json_parse_string_in(obj, obj_len, "expect_snap", snap, sizeof(snap))) {
		size_t n = 0;
		host_hex_decode(snap, v->expect_snap, sizeof(v->expect_snap), &n);
		if (!v->expect_snap_len)
			v->expect_snap_len = (int)n;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	if (!strcmp(v->fn, "qos_acm")) {
		u8 got = qos_acm((u8)v->acm_mask, (u8)v->priority);
		if (got != (u8)v->expect_u8)
			goto fail;
	} else if (!strcmp(v->fn, "tos_to_up")) {
		if (tos_to_up((u8)v->tos) != (u8)v->expect_u8)
			goto fail;
	} else if (!strcmp(v->fn, "rtw_calculate_wlan_pkt_size_by_attribue")) {
		struct pkt_attrib a;
		memset(&a, 0, sizeof(a));
		a.hdrlen = (u8)v->hdrlen;
		a.iv_len = (u8)v->iv_len;
		a.pktlen = (u32)v->pktlen;
		a.encrypt = (u8)v->encrypt;
		a.bswenc = (u8)v->bswenc;
		a.icv_len = (u8)v->icv_len;
		if (rtw_calculate_wlan_pkt_size_by_attribue(&a) != v->expect_u32)
			goto fail;
	} else if (!strcmp(v->fn, "rtw_put_snap")) {
		u8 buf[16];
		s32 len = rtw_put_snap(buf, (u16)v->h_proto);
		if (len != v->expect_snap_len ||
		    memcmp(buf, v->expect_snap, (size_t)v->expect_snap_len))
			goto fail;
	} else {
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
fail:
	fprintf(stderr, "FAIL %s\n", v->name);
	return -1;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i;
	int failures = 0;

	if (argc != 2)
		return 2;
	if (host_load_vectors(argv[1], vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count))
		return 2;
	for (i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			failures++;
	printf("%zu vectors, %d failures\n", count, failures);
	return failures ? 1 : 0;
}
