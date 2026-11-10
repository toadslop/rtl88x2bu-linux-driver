// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-228 fw_offload + W3-229 fw_tbtt_rpt. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_fw_offload_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 20
#define MAX_NAME 64
#define MAX_INPUT 32
#define MAX_FIELD 32
#define MAX_EXPECT 128

struct vector {
	char name[MAX_NAME];
	u8 iqk_offload;
	u8 ch_switch_offload;
	u8 fw_bcn_offload;
	u8 vap_tbtt_rpt_map;
	unsigned expect_hal_cmd;
	int has_expect_hal_cmd;
	char set_field[MAX_FIELD];
	char set_input[MAX_INPUT];
	size_t set_count;
	long set_expect_ret;
	int has_set_expect_ret;
	unsigned expect_thread_cmd;
	int has_expect_thread_cmd;
	char get_field[MAX_FIELD];
	char expect[MAX_EXPECT];
	int has_get;
};

void host_fw_offload_reset_thread_cmd_count(void);

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
	int t;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	parse_u8_field(obj, len, "iqk_offload", &v->iqk_offload);
	parse_u8_field(obj, len, "ch_switch_offload", &v->ch_switch_offload);
	parse_u8_field(obj, len, "fw_bcn_offload", &v->fw_bcn_offload);
	parse_u8_field(obj, len, "vap_tbtt_rpt_map", &v->vap_tbtt_rpt_map);
	if (host_json_parse_string_in(obj, len, "set", v->set_field,
				      sizeof(v->set_field)) == 0) {
		if (host_json_parse_string_in(obj, len, "set_input", v->set_input,
					      sizeof(v->set_input)))
			return -1;
		v->set_count = strlen(v->set_input);
		if (host_json_parse_int_in(obj, len, "set_count", &t) == 0)
			v->set_count = (size_t)t;
		if (host_json_parse_int_in(obj, len, "set_expect_ret", &t) == 0) {
			v->set_expect_ret = t;
			v->has_set_expect_ret = 1;
		}
	}
	if (host_json_parse_int_in(obj, len, "expect_thread_cmd", &t) == 0) {
		v->expect_thread_cmd = (unsigned)t;
		v->has_expect_thread_cmd = 1;
	}
	if (host_json_parse_int_in(obj, len, "expect_hal_cmd", &t) == 0) {
		v->expect_hal_cmd = (unsigned)t;
		v->has_expect_hal_cmd = 1;
	}
	if (host_json_parse_string_in(obj, len, "get", v->get_field,
				      sizeof(v->get_field)) == 0) {
		v->has_get = 1;
		if (host_json_parse_string_in(obj, len, "expect", v->expect,
					      sizeof(v->expect)))
			return -1;
		unescape_expect(v->expect);
	}
	return 0;
}

static void seed_adapter(struct _adapter *adapter)
{
	memset(adapter, 0, sizeof(*adapter));
	adapter->dvobj = &adapter->dvobj_storage;
	adapter->dvobj->padapters[0] = adapter;
}

static int run_vector(const struct vector *v)
{
	struct _adapter adapter;
	struct net_device dev;
	struct seq_file seq;
	char got[MAX_EXPECT];
	FILE *capture;
	size_t nread;
	ssize_t set_ret;

	host_fw_offload_reset_thread_cmd_count();
	host_fw_tbtt_reset_hal_cmd_count();
	seed_adapter(&adapter);
	adapter.hal_data.RegIQKFWOffload = v->iqk_offload;
	adapter.hal_data.ch_switch_offload = v->ch_switch_offload;
	adapter.dvobj->fw_bcn_offload = v->fw_bcn_offload;
	adapter.dvobj->vap_tbtt_rpt_map = v->vap_tbtt_rpt_map;
	dev.priv = &adapter;
	seq.private = &dev;

	if (v->set_field[0]) {
		if (!strcmp(v->set_field, "fw_offload")) {
			set_ret = proc_set_fw_offload(NULL, v->set_input,
						      v->set_count,
						      NULL, &dev);
			if (v->has_set_expect_ret) {
				if (set_ret != v->set_expect_ret) {
					fprintf(stderr,
						"FAIL %s set ret %zd expect %ld\n",
						v->name, set_ret,
						v->set_expect_ret);
					return -1;
				}
				if (!v->has_get)
					return 0;
			} else if (set_ret < 0) {
				fprintf(stderr, "FAIL %s set ret %zd\n", v->name,
					set_ret);
				return -1;
			}
			if (v->has_expect_thread_cmd &&
			    host_fw_offload_thread_cmd_count() !=
				    v->expect_thread_cmd) {
				fprintf(stderr,
					"FAIL %s thread_cmd %u expect %u\n",
					v->name,
					host_fw_offload_thread_cmd_count(),
					v->expect_thread_cmd);
				return -1;
			}
		} else if (!strcmp(v->set_field, "fw_tbtt_rpt")) {
			set_ret = proc_set_fw_tbtt_rpt(NULL, v->set_input,
						      v->set_count,
						      NULL, &dev);
			if (v->has_set_expect_ret) {
				if (set_ret != v->set_expect_ret) {
					fprintf(stderr,
						"FAIL %s set ret %zd expect %ld\n",
						v->name, set_ret,
						v->set_expect_ret);
					return -1;
				}
				if (!v->has_get && !v->has_expect_hal_cmd)
					return 0;
			} else if (set_ret < 0) {
				fprintf(stderr, "FAIL %s set ret %zd\n", v->name,
					set_ret);
				return -1;
			}
			if (v->has_expect_hal_cmd &&
			    host_fw_tbtt_hal_cmd_count() != v->expect_hal_cmd) {
				fprintf(stderr,
					"FAIL %s hal_cmd %u expect %u\n",
					v->name, host_fw_tbtt_hal_cmd_count(),
					v->expect_hal_cmd);
				return -1;
			}
			if (!v->has_get)
				return 0;
		} else {
			return -1;
		}
	}

	if (!v->has_get)
		return 0;

	capture = tmpfile();
	if (!capture)
		return -1;
	seq.out = capture;

	if (!strcmp(v->get_field, "fw_offload"))
		proc_get_fw_offload(&seq, NULL);
	else if (!strcmp(v->get_field, "fw_tbtt_rpt"))
		proc_get_fw_tbtt_rpt(&seq, NULL);
	else {
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
	const char *path =
		argc > 1 ? argv[1] : "debug_proc_fw_offload_vectors.json";
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

	printf("ok: %zu proc fw_offload/tbtt vectors\n", count);
	return 0;
}
