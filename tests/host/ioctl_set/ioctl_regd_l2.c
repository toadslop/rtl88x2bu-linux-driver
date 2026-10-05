// SPDX-License-Identifier: GPL-2.0
/* W3-125 follow-up (#982) PR4: L2 C oracle regd leaf for chplan/country setters. */
#include <stdio.h>
#include <string.h>

#include "host_vector_json.h"

#define _SUCCESS 1
#define _FAIL 0
#define REGD_SRC_RTK_PRIV 0
#define REGD_SRC_OS 1

typedef unsigned char u8;

typedef struct {
	u8 regd_src;
} mock_regsty;

#ifdef HOST_IOCTL_REGD_RUST
extern int ioctl_chplan_leaf_rust(mock_regsty *, int, u8, int, int *);
extern int ioctl_country_leaf_rust(mock_regsty *, int, int, int, int *);
#define O_CHPLAN ioctl_chplan_leaf_rust
#define O_COUNTRY ioctl_country_leaf_rust
#else
static int regd_from_os(mock_regsty *reg, int cfg)
{
	if (!cfg)
		return 0;
	return reg->regd_src == REGD_SRC_OS;
}

static int O_CHPLAN(mock_regsty *reg, int regd_cfg, u8 chplan __attribute__((unused)),
		    int cmd_ret, int *cmd_invoked)
{
	if (cmd_invoked)
		*cmd_invoked = 0;
	if (!regd_from_os(reg, regd_cfg)) {
		if (cmd_invoked)
			*cmd_invoked = 1;
		return cmd_ret;
	}
	return _SUCCESS;
}

static int O_COUNTRY(mock_regsty *reg, int regd_cfg, int country_enabled, int cmd_ret,
		     int *cmd_invoked)
{
	if (cmd_invoked)
		*cmd_invoked = 0;
	if (!country_enabled)
		return _SUCCESS;
	if (!regd_from_os(reg, regd_cfg)) {
		if (cmd_invoked)
			*cmd_invoked = 1;
		return cmd_ret;
	}
	return _SUCCESS;
}
#endif

struct vector {
	char name[64];
	char fn[16];
	int regd_src;
	int regd_from_os;
	int chplan;
	int country_enabled;
	int cmd_ret;
	int expect_ret;
	int expect_cmd;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(obj, len, "fn", v->fn, sizeof(v->fn)))
		return -1;
	host_json_parse_int_in(obj, len, "regd_src", &v->regd_src);
	host_json_parse_int_in(obj, len, "regd_from_os", &v->regd_from_os);
	host_json_parse_int_in(obj, len, "chplan", &v->chplan);
	host_json_parse_int_in(obj, len, "country_enabled", &v->country_enabled);
	host_json_parse_int_in(obj, len, "cmd_ret", &v->cmd_ret);
	host_json_parse_int_in(obj, len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, len, "expect_cmd", &v->expect_cmd);
	return 0;
}

static int run_vector(struct vector *v)
{
	mock_regsty reg = { .regd_src = (u8)v->regd_src };
	int cmd = -1;
	int got;

	if (!strcmp(v->fn, "chplan"))
		got = O_CHPLAN(&reg, v->regd_from_os, (u8)v->chplan, v->cmd_ret, &cmd);
	else if (!strcmp(v->fn, "country"))
		got = O_COUNTRY(&reg, v->regd_from_os, v->country_enabled, v->cmd_ret, &cmd);
	else {
		fprintf(stderr, "FAIL %s unknown fn\n", v->name);
		return 1;
	}

	if (got != v->expect_ret || cmd != v->expect_cmd)
		goto fail;
	printf("PASS %s\n", v->name);
	return 0;
fail:
	fprintf(stderr, "FAIL %s (got ret=%d cmd=%d)\n", v->name, got, cmd);
	return 1;
}

int main(int argc, char **argv)
{
	struct vector vecs[32];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "ioctl_regd_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 32, parse_vector_object, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vector(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
