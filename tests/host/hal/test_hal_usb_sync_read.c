// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for usb_read8/16/32 (W4-33). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 48

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

struct intf_hdl {
	void *pintf_dev;
};

struct vector {
	char name[MAX_NAME];
	unsigned addr;
	unsigned width;
	unsigned expect_wvalue;
	unsigned payload;
	unsigned expect;
};

static struct {
	u8 request;
	u16 value;
	u16 index;
	u16 len;
	u8 requesttype;
	unsigned call_count;
} g_vreq;

/* Set by runner before each vector invocation. */
static unsigned g_mock_payload;

int usbctrl_vendorreq(struct intf_hdl *pintfhdl, u8 request, u16 value, u16 index,
		      void *pdata, u16 len, u8 requesttype)
{
	(void)pintfhdl;
	g_vreq.request = request;
	g_vreq.value = value;
	g_vreq.index = index;
	g_vreq.len = len;
	g_vreq.requesttype = requesttype;
	g_vreq.call_count++;

	if (!pdata)
		return -1;

	if (len == 1)
		*(u8 *)pdata = (u8)g_mock_payload;
	else if (len == 2)
		*(u16 *)pdata = (u16)g_mock_payload;
	else if (len == 4)
		*(u32 *)pdata = (u32)g_mock_payload;

	return 0;
}

u8 usb_read8(struct intf_hdl *pintfhdl, u32 addr);
u16 usb_read16(struct intf_hdl *pintfhdl, u32 addr);
u32 usb_read32(struct intf_hdl *pintfhdl, u32 addr);

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "addr", &tmp))
		return -1;
	v->addr = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "width", &tmp))
		return -1;
	v->width = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect_wvalue", &tmp))
		return -1;
	v->expect_wvalue = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "payload", &tmp))
		return -1;
	v->payload = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->expect = (unsigned)tmp;
	return 0;
}

static int run_vector(const struct vector *v)
{
	struct intf_hdl hdl = {0};
	unsigned got;

	memset(&g_vreq, 0, sizeof(g_vreq));
	g_mock_payload = v->payload;

	if (v->width == 8)
		got = usb_read8(&hdl, (u32)v->addr);
	else if (v->width == 16)
		got = usb_read16(&hdl, (u32)v->addr);
	else if (v->width == 32)
		got = usb_read32(&hdl, (u32)v->addr);
	else
		return -1;

	if (g_vreq.call_count != 1) {
		fprintf(stderr, "FAIL %s: expected 1 vendorreq call, got %u\n",
			v->name, g_vreq.call_count);
		return -1;
	}
	if (g_vreq.request != 0x05) {
		fprintf(stderr, "FAIL %s: request=0x%02x expect 0x05\n", v->name,
			g_vreq.request);
		return -1;
	}
	if (g_vreq.requesttype != 0x01) {
		fprintf(stderr, "FAIL %s: requesttype=0x%02x expect 0x01\n",
			v->name, g_vreq.requesttype);
		return -1;
	}
	if (g_vreq.index != 0) {
		fprintf(stderr, "FAIL %s: index=0x%04x expect 0\n", v->name,
			g_vreq.index);
		return -1;
	}
	if (g_vreq.value != (u16)v->expect_wvalue) {
		fprintf(stderr,
			"FAIL %s: wvalue=0x%04x expect_wvalue=0x%04x\n",
			v->name, g_vreq.value, v->expect_wvalue);
		return -1;
	}
	if (g_vreq.len != (u16)(v->width / 8)) {
		fprintf(stderr, "FAIL %s: len=%u expect %u bytes for width %u\n",
			v->name, g_vreq.len, v->width / 8, v->width);
		return -1;
	}
	if (got != v->expect) {
		fprintf(stderr,
			"FAIL %s: return=0x%x expect=0x%x\n",
			v->name, got, v->expect);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_usb_sync_read_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		if (run_vector(&vectors[i]))
			return 1;
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
