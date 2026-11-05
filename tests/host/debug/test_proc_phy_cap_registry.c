// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-211 proc PHY cap registry get/set. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_phy_cap_registry_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_INPUT 32
#define MAX_FIELD 32
#define MAX_EXPECT 32

struct vector {
	char name[MAX_NAME];
	u8 stbc_cap;
	u8 rx_stbc;
	u8 ldpc_cap;
	u8 beamform_cap;
	char set_field[MAX_FIELD];
	char set_input[MAX_INPUT];
	char get_field[MAX_FIELD];
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

static int parse_u8_field(const char *obj, size_t len, const char *key, u8 *out)
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

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_u8_field(obj, len, "stbc_cap", &v->stbc_cap))
		return -1;
	if (parse_u8_field(obj, len, "rx_stbc", &v->rx_stbc))
		return -1;
	if (parse_u8_field(obj, len, "ldpc_cap", &v->ldpc_cap))
		return -1;
	if (parse_u8_field(obj, len, "beamform_cap", &v->beamform_cap))
		return -1;
	if (host_json_find_key_in(obj, len, "set")) {
		if (host_json_parse_string_in(obj, len, "set", v->set_field,
					      sizeof(v->set_field)))
			return -1;
		if (host_json_parse_string_in(obj, len, "set_input", v->set_input,
					      sizeof(v->set_input)))
			return -1;
	}
	if (host_json_parse_string_in(obj, len, "get", v->get_field,
				      sizeof(v->get_field)))
		return -1;
	if (host_json_parse_string_in(obj, len, "expect", v->expect,
				      sizeof(v->expect)))
		return -1;
	unescape_expect(v->expect);
	return 0;
}

static int dispatch_set(const struct vector *v, struct net_device *dev)
{
	if (!strcmp(v->set_field, "stbc_cap")) {
		return proc_set_stbc_cap(NULL, v->set_input, strlen(v->set_input),
					 NULL, dev) < 0;
	}
	if (!strcmp(v->set_field, "rx_stbc")) {
		return proc_set_rx_stbc(NULL, v->set_input, strlen(v->set_input),
					NULL, dev) < 0;
	}
	if (!strcmp(v->set_field, "ldpc_cap")) {
		return proc_set_ldpc_cap(NULL, v->set_input, strlen(v->set_input),
					 NULL, dev) < 0;
	}
#ifdef CONFIG_BEAMFORMING
	if (!strcmp(v->set_field, "txbf_cap")) {
		return proc_set_txbf_cap(NULL, v->set_input, strlen(v->set_input),
					 NULL, dev) < 0;
	}
#endif
	return 1;
}

static int dispatch_get(const struct vector *v, struct seq_file *seq)
{
	if (!strcmp(v->get_field, "stbc_cap"))
		return proc_get_stbc_cap(seq, NULL);
	if (!strcmp(v->get_field, "rx_stbc"))
		return proc_get_rx_stbc(seq, NULL);
	if (!strcmp(v->get_field, "ldpc_cap"))
		return proc_get_ldpc_cap(seq, NULL);
#ifdef CONFIG_BEAMFORMING
	if (!strcmp(v->get_field, "txbf_cap"))
		return proc_get_txbf_cap(seq, NULL);
#endif
	return 1;
}

static int run_vector(const struct vector *v)
{
	struct _adapter adapter;
	struct net_device dev;
	struct seq_file seq;
	char got[MAX_EXPECT];
	FILE *capture;
	size_t nread;

	memset(&adapter, 0, sizeof(adapter));
	adapter.registrypriv.stbc_cap = v->stbc_cap;
	adapter.registrypriv.rx_stbc = v->rx_stbc;
	adapter.registrypriv.ldpc_cap = v->ldpc_cap;
	adapter.registrypriv.beamform_cap = v->beamform_cap;
	dev.priv = &adapter;
	seq.private = &dev;

	capture = tmpfile();
	if (!capture)
		return -1;
	seq.out = capture;

	if (v->set_field[0] && dispatch_set(v, &dev)) {
		fclose(capture);
		return -1;
	}

	if (dispatch_get(v, &seq)) {
		fclose(capture);
		return -1;
	}

	rewind(capture);
	nread = fread(got, 1, sizeof(got) - 1, capture);
	fclose(capture);
	got[nread] = '\0';

	if (strcmp(got, v->expect) != 0) {
		fprintf(stderr, "FAIL %s:\n  got:    %s\n  expect: %s\n", v->name,
			got, v->expect);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = argc > 1 ? argv[1]
				    : "debug_proc_phy_cap_registry_vectors.json";
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

	printf("ok: %zu proc phy cap registry vectors\n", count);
	return 0;
}
