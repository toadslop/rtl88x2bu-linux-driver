// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for rtw_iol_rest append encoders (W3-50). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_iol_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 128
#define XMIT_BUF_SZ (MAX_XMITBUF_SZ + TXDESC_OFFSET + 64)

enum iol_fn {
	FN_WB, FN_WW, FN_WD, FN_DELAY_US, FN_DELAY_MS, FN_END, FN_LLT, FN_CHAIN,
};

struct vector {
	char name[MAX_NAME];
	enum iol_fn fn;
	int addr, value8, value16;
	unsigned int value32;
	int delay_us, delay_ms, page_boundary, initial_pktlen;
	int expect_ret;
	unsigned int expect_pktlen;
	char expect_hex[64];
};

static int parse_fn(const char *obj, size_t len, enum iol_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "_rtw_IOL_append_WB_cmd")) *out = FN_WB;
	else if (!strcmp(fn, "_rtw_IOL_append_WW_cmd")) *out = FN_WW;
	else if (!strcmp(fn, "_rtw_IOL_append_WD_cmd")) *out = FN_WD;
	else if (!strcmp(fn, "rtw_IOL_append_DELAY_US_cmd")) *out = FN_DELAY_US;
	else if (!strcmp(fn, "rtw_IOL_append_DELAY_MS_cmd")) *out = FN_DELAY_MS;
	else if (!strcmp(fn, "rtw_IOL_append_END_cmd")) *out = FN_END;
	else if (!strcmp(fn, "rtw_IOL_append_LLT_cmd")) *out = FN_LLT;
	else if (!strcmp(fn, "chain_wb_end")) *out = FN_CHAIN;
	else return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    parse_fn(obj, len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, len, "addr", &v->addr);
	host_json_parse_int_in(obj, len, "value8", &v->value8);
	host_json_parse_int_in(obj, len, "value16", &v->value16);
	host_json_parse_int_in(obj, len, "value32", (int *)&v->value32);
	host_json_parse_int_in(obj, len, "delay_us", &v->delay_us);
	host_json_parse_int_in(obj, len, "delay_ms", &v->delay_ms);
	host_json_parse_int_in(obj, len, "page_boundary", &v->page_boundary);
	host_json_parse_int_in(obj, len, "initial_pktlen", &v->initial_pktlen);
	host_json_parse_int_in(obj, len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, len, "expect_pktlen", (int *)&v->expect_pktlen);
	host_json_parse_string_in(obj, len, "expect_hex", v->expect_hex, sizeof(v->expect_hex));
	return 0;
}

static int hex_eq(const u8 *got, const char *hex, size_t off)
{
	size_t i = 0, j = 0;

	while (hex[i]) {
		int hi, lo, b;

		while (hex[i] == ' ') i++;
		if (!hex[i]) break;
		hi = hex[i] >= 'a' ? hex[i] - 'a' + 10 :
		     hex[i] >= 'A' ? hex[i] - 'A' + 10 : hex[i] - '0';
		i++;
		while (hex[i] == ' ') i++;
		lo = hex[i] >= 'a' ? hex[i] - 'a' + 10 :
		     hex[i] >= 'A' ? hex[i] - 'A' + 10 : hex[i] - '0';
		i++;
		b = (hi << 4) | lo;
		if (got[off + j++] != (u8)b)
			return -1;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	struct xmit_frame frame;
	u8 buf[XMIT_BUF_SZ];
	int ret = _FAIL;

	memset(&frame, 0, sizeof(frame));
	memset(buf, 0, sizeof(buf));
	frame.buf_addr = buf;
	frame.attrib.pktlen = (u32)v->initial_pktlen;
	frame.attrib.last_txcmdsz = (u32)v->initial_pktlen;

	switch (v->fn) {
	case FN_WB:
		ret = _rtw_IOL_append_WB_cmd(&frame, (u16)v->addr, (u8)v->value8);
		break;
	case FN_WW:
		ret = _rtw_IOL_append_WW_cmd(&frame, (u16)v->addr, (u16)v->value16);
		break;
	case FN_WD:
		ret = _rtw_IOL_append_WD_cmd(&frame, (u16)v->addr, (u32)v->value32);
		break;
	case FN_DELAY_US:
		ret = rtw_IOL_append_DELAY_US_cmd(&frame, (u16)v->delay_us);
		break;
	case FN_DELAY_MS:
		ret = rtw_IOL_append_DELAY_MS_cmd(&frame, (u16)v->delay_ms);
		break;
	case FN_END:
		ret = rtw_IOL_append_END_cmd(&frame);
		break;
	case FN_LLT:
		ret = rtw_IOL_append_LLT_cmd(&frame, (u8)v->page_boundary);
		break;
	case FN_CHAIN:
		ret = _rtw_IOL_append_WB_cmd(&frame, (u16)v->addr, (u8)v->value8);
		if (ret == _SUCCESS)
			ret = rtw_IOL_append_END_cmd(&frame);
		break;
	default:
		return -1;
	}
	if (ret != v->expect_ret || frame.attrib.pktlen != v->expect_pktlen) {
		fprintf(stderr, "%s: ret/pktlen mismatch\n", v->name);
		return -1;
	}
	if (v->expect_hex[0] &&
	    hex_eq(buf + TXDESC_OFFSET + v->initial_pktlen, v->expect_hex, 0)) {
		fprintf(stderr, "%s: bytes mismatch\n", v->name);
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i, failures = 0;

	if (argc != 2)
		return 2;
	if (host_load_vectors(argv[1], vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count))
		return 2;
	for (i = 0; i < count; i++)
		failures += run_vector(&vectors[i]) ? 1 : 0;
	if (failures)
		return 1;
	printf("all %zu iol vectors passed (oracle: core/rtw_iol_rest.c)\n", count);
	return 0;
}
