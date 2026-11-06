// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-214 proc malloc cnt + best channel. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_misc_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64
#define MAX_INPUT 32
#define MAX_FIELD 32
#define MAX_EXPECT 512
#define MAX_CH 8

struct ch_entry {
	u8 num;
	u32 rx_count;
};

struct vector {
	char name[MAX_NAME];
	int malloc_cnt;
	int malloc_size;
	u8 max_chan_nums;
	struct ch_entry ch[MAX_CH];
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

static int parse_ch_field(const char *obj, size_t len, int idx, struct ch_entry *ch)
{
	char key_num[16];
	char key_rx[16];
	int tmp;

	snprintf(key_num, sizeof(key_num), "ch%d_num", idx);
	snprintf(key_rx, sizeof(key_rx), "ch%d_rx", idx);
	if (!host_json_find_key_in(obj, len, key_num)) {
		ch->num = 0;
		ch->rx_count = 0;
		return 0;
	}
	if (host_json_parse_int_in(obj, len, key_num, &tmp))
		return -1;
	ch->num = (u8)tmp;
	if (host_json_parse_int_in(obj, len, key_rx, &tmp))
		return -1;
	ch->rx_count = (u32)tmp;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;
	int i;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_find_key_in(obj, len, "malloc_cnt")) {
		if (host_json_parse_int_in(obj, len, "malloc_cnt", &tmp))
			return -1;
		v->malloc_cnt = tmp;
	}
	if (host_json_find_key_in(obj, len, "malloc_size")) {
		if (host_json_parse_int_in(obj, len, "malloc_size", &tmp))
			return -1;
		v->malloc_size = tmp;
	}
	if (host_json_find_key_in(obj, len, "max_chan_nums")) {
		if (host_json_parse_int_in(obj, len, "max_chan_nums", &tmp))
			return -1;
		v->max_chan_nums = (u8)tmp;
	}
	for (i = 0; i < MAX_CH; i++) {
		if (parse_ch_field(obj, len, i, &v->ch[i]))
			return -1;
	}
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

static void apply_fixture(const struct vector *v, struct _adapter *adapter)
{
	u8 i;

	adapter->rf_ctl.max_chan_nums = v->max_chan_nums ? v->max_chan_nums : MAX_CHANNEL_NUM;
	for (i = 0; i < MAX_CH; i++) {
		if (v->ch[i].num)
			adapter->rf_ctl.channel_set[i].ChannelNum = v->ch[i].num;
		adapter->rf_ctl.channel_set[i].rx_count = v->ch[i].rx_count;
	}
}

static int dispatch_set(const struct vector *v, struct net_device *dev)
{
	if (!strcmp(v->set_field, "best_channel")) {
		return proc_set_best_channel(NULL, v->set_input, strlen(v->set_input),
					     NULL, dev) < 0;
	}
	return 1;
}

static int dispatch_get(const struct vector *v, struct seq_file *seq)
{
	if (!strcmp(v->get_field, "malloc_cnt"))
		return proc_get_malloc_cnt(seq, NULL);
	if (!strcmp(v->get_field, "best_channel"))
		return proc_get_best_channel(seq, NULL);
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
	apply_fixture(v, &adapter);
	_malloc_cnt = v->malloc_cnt;
	_malloc_size = v->malloc_size;
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
	const char *path = argc > 1 ? argv[1] : "debug_proc_misc_vectors.json";
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

	printf("ok: %zu proc misc vectors\n", count);
	return 0;
}
