// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-220 monitor / simple_config / xmit_block. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_monitor_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_INPUT 32
#define MAX_FIELD 32
#define MAX_EXPECT 512

struct vector {
	char name[MAX_NAME];
	u32 fw_state;
	u16 dev_type;
	u8 oper_ch;
	u8 oper_offset;
	u8 oper_bw;
	u8 rtw_simple_config;
	u8 xmit_block;
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

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "fw_state", &tmp))
		return -1;
	v->fw_state = (u32)tmp;
	if (host_json_parse_int_in(obj, len, "dev_type", &tmp))
		return -1;
	v->dev_type = (u16)tmp;
	if (host_json_parse_int_in(obj, len, "oper_ch", &tmp))
		return -1;
	v->oper_ch = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "oper_offset", &tmp))
		return -1;
	v->oper_offset = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "oper_bw", &tmp))
		return -1;
	v->oper_bw = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "rtw_simple_config", &tmp))
		return -1;
	v->rtw_simple_config = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "xmit_block", &tmp))
		return -1;
	v->xmit_block = (u8)tmp;
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

static int run_vector(const struct vector *v)
{
	struct _adapter adapter;
	struct dvobj_priv dvobj;
	struct net_device dev;
	struct seq_file seq;
	char got[MAX_EXPECT];
	FILE *capture;
	size_t nread;
	long ret;

	memset(&adapter, 0, sizeof(adapter));
	memset(&dvobj, 0, sizeof(dvobj));
	adapter.dvobj = &dvobj;
	adapter.mlmepriv.fw_state = v->fw_state;
	adapter.mlmeextpriv.cur_channel = v->oper_ch;
	adapter.mlmeextpriv.cur_ch_offset = v->oper_offset;
	adapter.mlmeextpriv.cur_bwmode = v->oper_bw;
	adapter.rtw_simple_config = v->rtw_simple_config;
	dvobj.xmit_block = v->xmit_block;
	dvobj.oper_channel = v->oper_ch;
	dvobj.oper_bwmode = v->oper_bw;
	dvobj.oper_ch_offset = v->oper_offset;
	dev.priv = &adapter;
	dev.type = v->dev_type;
	seq.private = &dev;

	capture = tmpfile();
	if (!capture)
		return -1;
	seq.out = capture;

	if (v->set_field[0]) {
		if (!strcmp(v->set_field, "monitor")) {
			ret = proc_set_monitor(NULL, v->set_input,
					       strlen(v->set_input), NULL, &dev);
		} else if (!strcmp(v->set_field, "simple_config")) {
			ret = proc_set_simple_config(
				NULL, v->set_input, strlen(v->set_input), NULL,
				&dev);
		} else if (!strcmp(v->set_field, "xmit_block")) {
			ret = proc_set_xmit_block(
				NULL, v->set_input, strlen(v->set_input), NULL,
				&dev);
		} else {
			fclose(capture);
			return -1;
		}

		if (ret < 0) {
			fprintf(stderr, "FAIL %s: set returned %ld\n", v->name,
				ret);
			fclose(capture);
			return -1;
		}
	}

	if (!strcmp(v->get_field, "monitor"))
		proc_get_monitor(&seq, NULL);
	else if (!strcmp(v->get_field, "simple_config"))
		proc_get_simple_config(&seq, NULL);
	else if (!strcmp(v->get_field, "xmit_block"))
		proc_get_xmit_block(&seq, NULL);
	else {
		fclose(capture);
		return -1;
	}

	rewind(capture);
	nread = fread(got, 1, sizeof(got) - 1, capture);
	fclose(capture);
	got[nread] = '\0';

	if (strcmp(got, v->expect) != 0) {
		fprintf(stderr, "FAIL %s:\n  got:    %s\n  expect: %s\n",
			v->name, got, v->expect);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = argc > 1 ? argv[1]
				    : "debug_proc_monitor_vectors.json";
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

	printf("ok: %zu proc monitor + xmit_block vectors\n", count);
	return 0;
}
