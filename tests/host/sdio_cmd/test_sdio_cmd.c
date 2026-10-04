// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_sdio_cmd_types.h"
#include "host_vector_json.h"

struct vector {
	char name[40], op[16];
	int addr, len, expect_ret, expect_addr, expect_io;
	int io_fail_remaining, io_mock_err, io_mock_err_set, expect_surprise;
};

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->expect_io = -1;
	v->expect_surprise = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_int_in(obj, len, "addr", &v->addr);
	host_json_parse_int_in(obj, len, "len", &v->len);
	host_json_parse_int_in(obj, len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, len, "expect_addr", &v->expect_addr);
	host_json_parse_int_in(obj, len, "expect_io", &v->expect_io);
	host_json_parse_int_in(obj, len, "io_fail_remaining", &v->io_fail_remaining);
	if (host_json_find_key_in(obj, len, "io_mock_err")) {
		host_json_parse_int_in(obj, len, "io_mock_err", &v->io_mock_err);
		v->io_mock_err_set = 1;
	}
	host_json_parse_int_in(obj, len, "expect_surprise", &v->expect_surprise);
	if (!v->len)
		v->len = 4;
	return 0;
}

static int run_vec(struct vector *v)
{
	struct host_dvobj *d = host_sdio_cmd_dvobj();
	u8 buf[8], ret = _FAIL;

	if (!strcmp(v->op, "reset")) {
		host_sdio_cmd_reset();
	} else if (!strcmp(v->op, "surprise")) {
		host_sdio_cmd_reset();
		host_sdio_cmd_set_surprise(1);
	} else {
		if (v->io_fail_remaining)
			d->io_fail_remaining = v->io_fail_remaining;
		if (v->io_mock_err_set)
			d->io_mock_err = v->io_mock_err;

		if (!strcmp(v->op, "read52"))
			ret = rtw_sdio_read_cmd52(d, (u32)v->addr, buf, (size_t)v->len);
		else if (!strcmp(v->op, "read53"))
			ret = rtw_sdio_read_cmd53(d, (u32)v->addr, buf, (size_t)v->len);
		else if (!strcmp(v->op, "write52"))
			ret = rtw_sdio_write_cmd52(d, (u32)v->addr, buf, (size_t)v->len);
		else if (!strcmp(v->op, "write53"))
			ret = rtw_sdio_write_cmd53(d, (u32)v->addr, buf, (size_t)v->len);
		else if (!strcmp(v->op, "f0"))
			ret = rtw_sdio_f0_read(d, (u32)v->addr, buf, (size_t)v->len);
		else
			goto bad;

		if (ret != (u8)v->expect_ret)
			goto bad;
		if (v->expect_addr && (int)host_sdio_cmd_last_addr() != v->expect_addr)
			goto bad;
		if (v->expect_io >= 0 && host_sdio_cmd_io_count() != v->expect_io)
			goto bad;
		if (v->expect_surprise >= 0 &&
		    (int)host_sdio_cmd_surprise() != v->expect_surprise)
			goto bad;
	}
	printf("PASS %s\n", v->name);
	return 0;
bad:
	fprintf(stderr, "FAIL %s\n", v->name);
	return 1;
}

int main(int argc, char **argv)
{
	struct vector vecs[24];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "sdio_cmd_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 24, parse_vec, &n))
		return 1;
	for (size_t i = 0; i < n; i++)
		bad |= run_vec(&vecs[i]);
	return bad ? 1 : 0;
}
