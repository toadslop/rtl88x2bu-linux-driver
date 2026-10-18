// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for usb_async_write8/16/32 (W4-37). */

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

struct usb_device {
	int dummy;
};

struct dvobj_priv {
	struct usb_device *pusbdev;
};

struct intf_hdl {
	void *pintf_dev;
};

struct vector {
	char name[MAX_NAME];
	unsigned addr;
	unsigned width;
	unsigned val;
	unsigned expect_wvalue;
};

static struct {
	u8 request;
	u16 value;
	u16 index;
	u16 len;
	u8 requesttype;
	unsigned call_count;
	u8 buf[4];
} g_vreq;

int _usbctrl_vendorreq_async_write(struct usb_device *udev, u8 request,
				   u16 value, u16 index, void *pdata, u16 len,
				   u8 requesttype)
{
	(void)udev;
	g_vreq.request = request;
	g_vreq.value = value;
	g_vreq.index = index;
	g_vreq.len = len;
	g_vreq.requesttype = requesttype;
	g_vreq.call_count++;

	if (!pdata)
		return -1;

	if (len > sizeof(g_vreq.buf))
		return -1;

	memcpy(g_vreq.buf, pdata, len);
	return 0;
}

int usb_async_write8(struct intf_hdl *pintfhdl, u32 addr, u8 val);
int usb_async_write16(struct intf_hdl *pintfhdl, u32 addr, u16 val);
int usb_async_write32(struct intf_hdl *pintfhdl, u32 addr, u32 val);

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
	if (host_json_parse_int_in(obj, len, "val", &tmp))
		return -1;
	v->val = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect_wvalue", &tmp))
		return -1;
	v->expect_wvalue = (unsigned)tmp;
	return 0;
}

static int check_vendorreq_common(const struct vector *v, u16 expect_len)
{
	if (g_vreq.call_count != 1) {
		fprintf(stderr, "FAIL %s: expected 1 async vendorreq call, got %u\n",
			v->name, g_vreq.call_count);
		return -1;
	}
	if (g_vreq.request != 0x05) {
		fprintf(stderr, "FAIL %s: request=0x%02x expect 0x05\n", v->name,
			g_vreq.request);
		return -1;
	}
	if (g_vreq.requesttype != 0x00) {
		fprintf(stderr, "FAIL %s: requesttype=0x%02x expect 0x00\n",
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
	if (g_vreq.len != expect_len) {
		fprintf(stderr, "FAIL %s: len=%u expect %u\n", v->name,
			g_vreq.len, expect_len);
		return -1;
	}
	return 0;
}

static int run_vector(const struct vector *v)
{
	struct usb_device udev = {0};
	struct dvobj_priv dvobj = {.pusbdev = &udev};
	struct intf_hdl hdl = {.pintf_dev = &dvobj};
	int ret;

	memset(&g_vreq, 0, sizeof(g_vreq));

	if (v->width == 8) {
		ret = usb_async_write8(&hdl, (u32)v->addr, (u8)v->val);
		if (ret != 0)
			return -1;
		if (check_vendorreq_common(v, 1))
			return -1;
		if (g_vreq.buf[0] != (u8)v->val) {
			fprintf(stderr, "FAIL %s: pdata byte=0x%02x expect 0x%02x\n",
				v->name, g_vreq.buf[0], (u8)v->val);
			return -1;
		}
	} else if (v->width == 16) {
		ret = usb_async_write16(&hdl, (u32)v->addr, (u16)v->val);
		if (ret != 0)
			return -1;
		if (check_vendorreq_common(v, 2))
			return -1;
		if (*(u16 *)g_vreq.buf != (u16)v->val) {
			fprintf(stderr, "FAIL %s: pdata u16=0x%04x expect 0x%04x\n",
				v->name, *(u16 *)g_vreq.buf, (u16)v->val);
			return -1;
		}
	} else if (v->width == 32) {
		ret = usb_async_write32(&hdl, (u32)v->addr, (u32)v->val);
		if (ret != 0)
			return -1;
		if (check_vendorreq_common(v, 4))
			return -1;
		if (*(u32 *)g_vreq.buf != (u32)v->val) {
			fprintf(stderr, "FAIL %s: pdata u32=0x%08x expect 0x%08x\n",
				v->name, *(u32 *)g_vreq.buf, (u32)v->val);
			return -1;
		}
	} else {
		fprintf(stderr, "FAIL %s: invalid width\n", v->name);
		return -1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path =
		argc > 1 ? argv[1] : "hal_usb_async_write_vectors.json";

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
