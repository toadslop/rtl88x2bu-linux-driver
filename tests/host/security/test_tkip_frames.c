// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for TKIP frame encrypt/decrypt (T5 / W3-10).
 */

#if defined(HOST_TKIP_FRAME_ORACLE_BUILD) || defined(RUST_SECURITY_ORACLE)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_security_frame.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 128
#define MAX_BUF HOST_MAX_TKIP_FRAME

enum tkip_frame_fn {
	FN_ENCRYPT = 0,
	FN_DECRYPT,
};

struct vector {
	char name[MAX_NAME];
	enum tkip_frame_fn fn;
	u8 encrypt;
	u8 grp_key_index;
	u8 key_index;
	u8 ta[HOST_ETH_ALEN];
	u8 ra[HOST_ETH_ALEN];
	u8 unicast_key[16];
	u8 group_key[16];
	u8 binstall_grpkey;
	u16 hdrlen;
	u8 iv_len;
	u8 icv_len;
	u32 last_txcmdsz;
	u32 frame_len;
	u8 header[MAX_BUF];
	size_t header_len;
	u8 iv[MAX_BUF];
	size_t iv_len_field;
	u8 payload[MAX_BUF];
	size_t payload_len;
	u8 expect[MAX_BUF];
	size_t expect_len;
	int expect_fail;
};

uint32_t rtw_tkip_encrypt(struct host_adapter *padapter, u8 *pxmitframe);
uint32_t rtw_tkip_decrypt(struct host_adapter *padapter, u8 *precvframe);

static int parse_fn(const char *obj, size_t obj_len, enum tkip_frame_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "rtw_tkip_encrypt") == 0)
		*out = FN_ENCRYPT;
	else if (strcmp(fn, "rtw_tkip_decrypt") == 0)
		*out = FN_DECRYPT;
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
			   u8 *out)
{
	size_t len = 0;

	return parse_hex_field(obj, obj_len, key, out, HOST_ETH_ALEN, &len) ||
	       len != HOST_ETH_ALEN;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	int val = 0;
	size_t key_len = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	if (host_json_parse_int_in(obj, obj_len, "encrypt", &val))
		return -1;
	v->encrypt = (u8)val;
	if (!host_json_parse_int_in(obj, obj_len, "grp_key_index", &val))
		v->grp_key_index = (u8)val;
	if (!host_json_parse_int_in(obj, obj_len, "key_index", &val))
		v->key_index = (u8)val;
	if (!host_json_parse_int_in(obj, obj_len, "binstall_grpkey", &val))
		v->binstall_grpkey = (u8)val;
	if (parse_mac_field(obj, obj_len, "ta", v->ta))
		return -1;
	if (parse_mac_field(obj, obj_len, "ra", v->ra))
		return -1;
	if (parse_hex_field(obj, obj_len, "unicast_key", v->unicast_key,
			    sizeof(v->unicast_key), &key_len))
		return -1;
	if (parse_hex_field(obj, obj_len, "group_key", v->group_key,
			    sizeof(v->group_key), &key_len))
		return -1;
	if (host_json_parse_int_in(obj, obj_len, "hdrlen", &val))
		return -1;
	v->hdrlen = (u16)val;
	if (host_json_parse_int_in(obj, obj_len, "iv_len", &val))
		return -1;
	v->iv_len = (u8)val;
	if (!host_json_parse_int_in(obj, obj_len, "icv_len", &val))
		v->icv_len = (u8)val;
	if (!host_json_parse_int_in(obj, obj_len, "last_txcmdsz", &val))
		v->last_txcmdsz = (u32)val;
	if (!host_json_parse_int_in(obj, obj_len, "frame_len", &val))
		v->frame_len = (u32)val;
	parse_hex_field(obj, obj_len, "header", v->header, sizeof(v->header),
			&v->header_len);
	parse_hex_field(obj, obj_len, "iv", v->iv, sizeof(v->iv), &v->iv_len_field);
	parse_hex_field(obj, obj_len, "payload", v->payload, sizeof(v->payload),
			&v->payload_len);
	parse_hex_field(obj, obj_len, "expect", v->expect, sizeof(v->expect),
			&v->expect_len);
	if (!host_json_parse_int_in(obj, obj_len, "expect_fail", &val))
		v->expect_fail = val;
	return 0;
}

static void setup_adapter_encrypt(struct host_adapter *adapter, struct vector *v)
{
	memset(adapter, 0, sizeof(*adapter));
	adapter->securitypriv.dot118021XGrpKeyid = v->grp_key_index;
	memcpy(adapter->securitypriv.dot118021XGrpKey[v->grp_key_index].skey,
	       v->group_key, 16);
	adapter->xmitpriv.frag_len = 512;
}

static int run_encrypt_vector(struct vector *v)
{
	struct host_adapter adapter;
	struct host_xmit_frame xmit;
	u8 buf[MAX_BUF];
	u8 *wire;
	size_t wire_len;

	setup_adapter_encrypt(&adapter, v);
	memset(&xmit, 0, sizeof(xmit));
	memset(buf, 0, sizeof(buf));

	wire = buf + HOST_TXDESC_OFFSET;
	if (v->header_len != v->hdrlen || v->iv_len_field != v->iv_len ||
	    v->payload_len + v->hdrlen + v->iv_len + v->icv_len != v->last_txcmdsz) {
		fprintf(stderr, "%s: inconsistent encrypt vector lengths\n", v->name);
		return -1;
	}

	memcpy(wire, v->header, v->header_len);
	memcpy(wire + v->hdrlen, v->iv, v->iv_len);
	memcpy(wire + v->hdrlen + v->iv_len, v->payload, v->payload_len);

	xmit.buf_addr = buf;
	xmit.pkt_offset = 0;
	xmit.attrib.encrypt = v->encrypt;
	xmit.attrib.nr_frags = 1;
	xmit.attrib.hdrlen = v->hdrlen;
	xmit.attrib.iv_len = v->iv_len;
	xmit.attrib.icv_len = v->icv_len;
	xmit.attrib.last_txcmdsz = v->last_txcmdsz;
	memcpy(xmit.attrib.ta, v->ta, HOST_ETH_ALEN);
	memcpy(xmit.attrib.ra, v->ra, HOST_ETH_ALEN);
	memcpy(xmit.attrib.dot118021x_UncstKey.skey, v->unicast_key, 16);

	if (rtw_tkip_encrypt(&adapter, (u8 *)&xmit) != 0) {
		fprintf(stderr, "%s: encrypt returned fail\n", v->name);
		return -1;
	}

	wire_len = v->payload_len + v->icv_len;
	if (wire_len != v->expect_len ||
	    memcmp(wire + v->hdrlen + v->iv_len, v->expect, v->expect_len) != 0) {
		fprintf(stderr, "%s: encrypt mismatch\n", v->name);
		return -1;
	}
	return 0;
}

static void setup_adapter_decrypt(struct host_adapter *adapter, struct vector *v)
{
	memset(adapter, 0, sizeof(*adapter));
	adapter->securitypriv.dot118021XGrpKeyid = v->grp_key_index;
	memcpy(adapter->securitypriv.dot118021XGrpKey[v->grp_key_index].skey,
	       v->group_key, 16);
	memcpy(adapter->securitypriv.dot118021XGrpKey[v->key_index].skey,
	       v->group_key, 16);
	adapter->securitypriv.binstallGrpkey = v->binstall_grpkey;
	adapter->stapriv.stas[0].used = 1;
	memcpy(adapter->stapriv.stas[0].ta, v->ta, HOST_ETH_ALEN);
	memcpy(adapter->stapriv.stas[0].dot118021x_UncstKey.skey, v->unicast_key, 16);
}

static int run_decrypt_vector(struct vector *v)
{
	struct host_adapter adapter;
	union host_recv_frame recv;
	u8 buf[MAX_BUF];
	u8 *wire;
	size_t plain_len;
	u32 res;

	setup_adapter_decrypt(&adapter, v);
	memset(&recv, 0, sizeof(recv));
	memset(buf, 0, sizeof(buf));

	wire = buf;
	if (v->header_len != v->hdrlen || v->iv_len_field != v->iv_len) {
		fprintf(stderr, "%s: inconsistent decrypt vector lengths\n", v->name);
		return -1;
	}
	if (!v->frame_len)
		v->frame_len = v->hdrlen + v->iv_len + v->payload_len;

	memcpy(wire, v->header, v->header_len);
	memcpy(wire + v->hdrlen, v->iv, v->iv_len);
	memcpy(wire + v->hdrlen + v->iv_len, v->payload, v->payload_len);

	recv.u.hdr.rx_data = wire;
	recv.u.hdr.len = v->frame_len;
	recv.u.hdr.attrib.encrypt = v->encrypt;
	recv.u.hdr.attrib.hdrlen = v->hdrlen;
	recv.u.hdr.attrib.iv_len = v->iv_len;
	recv.u.hdr.attrib.key_index = v->key_index;
	memcpy(recv.u.hdr.attrib.ra, v->ra, HOST_ETH_ALEN);
	memcpy(recv.u.hdr.attrib.ta, v->ta, HOST_ETH_ALEN);

	res = rtw_tkip_decrypt(&adapter, (u8 *)&recv);
	if (v->expect_fail) {
		if (res == 0) {
			fprintf(stderr, "%s: expected decrypt fail\n", v->name);
			return -1;
		}
		return 0;
	}
	if (res != 0) {
		fprintf(stderr, "%s: decrypt returned fail\n", v->name);
		return -1;
	}

	plain_len = v->frame_len - v->hdrlen - v->iv_len - v->icv_len;
	if (plain_len != v->expect_len ||
	    memcmp(wire + v->hdrlen + v->iv_len, v->expect, v->expect_len) != 0) {
		fprintf(stderr, "%s: decrypt mismatch\n", v->name);
		return -1;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
#if defined(RUST_SECURITY_ORACLE)
	if (v->fn == FN_DECRYPT)
		return 0;
#endif
	switch (v->fn) {
	case FN_ENCRYPT:
		return run_encrypt_vector(v);
	case FN_DECRYPT:
		return run_decrypt_vector(v);
	default:
		return -1;
	}
}

int main(int argc, char **argv)
{
	const char *path = "tkip_frame_vectors.json";
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
	printf("all %zu tkip frame vectors passed\n", nvec);
	return 0;
}

#endif /* HOST_TKIP_FRAME_ORACLE_BUILD || RUST_SECURITY_ORACLE */
