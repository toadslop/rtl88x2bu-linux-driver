// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-210 PHY cap dump helpers. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_phy_cap_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64
#define MAX_EXPECT 2048

typedef void (*phy_cap_fn)(void *sel, _adapter *adapter);

struct vector {
	char name[MAX_NAME];
	char fn[32];
	u8 ht_stbc;
	u8 ht_ldpc;
	u8 ht_bf;
	u8 vht_stbc;
	u8 vht_ldpc;
	u16 vht_bf;
	u8 reg_stbc;
	u8 reg_ldpc;
	u8 reg_bf;
	u8 reg_bfer_rf;
	u8 reg_bfee_rf;
	char expect[MAX_EXPECT];
};

static void unescape_expect(char *s)
{
	char *r = s;
	char *w = s;

	while (*r) {
		if (r[0] == '\\' && r[1] == 'n') {
			*w++ = '\n';
			r += 2;
		} else {
			*w++ = *r++;
		}
	}
	*w = '\0';
}

static int parse_u8(const char *obj, size_t len, const char *key, u8 *out)
{
	int tmp;

	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (u8)tmp;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "fn", v->fn, sizeof(v->fn)))
		return -1;
	if (parse_u8(obj, len, "ht_stbc", &v->ht_stbc) ||
	    parse_u8(obj, len, "ht_ldpc", &v->ht_ldpc) ||
	    parse_u8(obj, len, "ht_bf", &v->ht_bf) ||
	    parse_u8(obj, len, "vht_stbc", &v->vht_stbc) ||
	    parse_u8(obj, len, "vht_ldpc", &v->vht_ldpc))
		return -1;
	if (host_json_parse_int_in(obj, len, "vht_bf", &tmp))
		return -1;
	v->vht_bf = (u16)tmp;
	if (parse_u8(obj, len, "reg_stbc", &v->reg_stbc) ||
	    parse_u8(obj, len, "reg_ldpc", &v->reg_ldpc) ||
	    parse_u8(obj, len, "reg_bf", &v->reg_bf) ||
	    parse_u8(obj, len, "reg_bfer_rf", &v->reg_bfer_rf) ||
	    parse_u8(obj, len, "reg_bfee_rf", &v->reg_bfee_rf))
		return -1;
	if (host_json_parse_string_in(obj, len, "expect", v->expect, sizeof(v->expect)))
		return -1;
	unescape_expect(v->expect);
	return 0;
}

static phy_cap_fn resolve_fn(const char *name)
{
	if (!strcmp(name, "dump_drv"))
		return rtw_dump_drv_phy_cap;
	if (!strcmp(name, "dump_dft"))
		return rtw_dump_dft_phy_cap;
	if (!strcmp(name, "get_dft"))
		return rtw_get_dft_phy_cap;
	return NULL;
}

static int run_vector(const struct vector *v)
{
	_adapter adapter;
	phy_cap_fn fn = resolve_fn(v->fn);
	char got[MAX_EXPECT];
	FILE *f;
	size_t nread;

	if (!fn) {
		fprintf(stderr, "unknown fn %s\n", v->fn);
		return -1;
	}

	memset(&adapter, 0, sizeof(adapter));
	adapter.mlmepriv.htpriv.stbc_cap = v->ht_stbc;
	adapter.mlmepriv.htpriv.ldpc_cap = v->ht_ldpc;
	adapter.mlmepriv.htpriv.beamform_cap = v->ht_bf;
	adapter.mlmepriv.vhtpriv.stbc_cap = v->vht_stbc;
	adapter.mlmepriv.vhtpriv.ldpc_cap = v->vht_ldpc;
	adapter.mlmepriv.vhtpriv.beamform_cap = v->vht_bf;
	adapter.registrypriv.stbc_cap = v->reg_stbc;
	adapter.registrypriv.ldpc_cap = v->reg_ldpc;
	adapter.registrypriv.beamform_cap = v->reg_bf;
	adapter.registrypriv.beamformer_rf_num = v->reg_bfer_rf;
	adapter.registrypriv.beamformee_rf_num = v->reg_bfee_rf;

	f = tmpfile();
	if (!f) {
		fprintf(stderr, "tmpfile failed\n");
		return -1;
	}
	fn(f, &adapter);
	rewind(f);
	nread = fread(got, 1, sizeof(got) - 1, f);
	fclose(f);
	got[nread] = '\0';

	if (strcmp(got, v->expect) != 0) {
		fprintf(stderr, "FAIL %s:\n  got:    %s\n  expect: %s\n", v->name, got,
			v->expect);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = argc > 1 ? argv[1] : "debug_phy_cap_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load vectors from %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		if (run_vector(&vectors[i]))
			return 1;
	}

	printf("ok: %zu phy_cap vectors\n", count);
	return 0;
}
