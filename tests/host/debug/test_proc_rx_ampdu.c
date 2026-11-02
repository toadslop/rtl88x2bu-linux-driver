// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-204 proc rx_ampdu get/set. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_ampdu_types.h"
#include "host_vector_json.h"

struct host_proc_seq {
	struct _adapter *adapter;
	FILE *out;
};

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_EXPECT 1024
#define MAX_SET 32

struct get_vector {
	char name[MAX_NAME];
	u8 fix_accept;
	u8 fix_size;
	u8 auto_accept;
	u8 auto_size;
	u8 table[16];
	char expect[MAX_EXPECT];
};

struct set_vector {
	char name[MAX_NAME];
	char input[MAX_SET];
	u8 expect_accept;
	u8 expect_size;
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
	int v;

	if (host_json_parse_int_in(obj, len, key, &v))
		return -1;
	*out = (u8)v;
	return 0;
}

static int parse_get_object(const char *obj, size_t len, void *vec_void)
{
	struct get_vector *v = vec_void;
	char hex[64];
	size_t hex_len;

	memset(v, 0, sizeof(*v));
	v->fix_accept = RX_AMPDU_ACCEPT_INVALID;
	v->fix_size = RX_AMPDU_SIZE_INVALID;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	parse_u8_field(obj, len, "fix_accept", &v->fix_accept);
	parse_u8_field(obj, len, "fix_size", &v->fix_size);
	parse_u8_field(obj, len, "auto_accept", &v->auto_accept);
	parse_u8_field(obj, len, "auto_size", &v->auto_size);
	if (host_json_parse_string_in(obj, len, "limits_hex", hex, sizeof(hex)) == 0) {
		if (host_hex_decode(hex, v->table, sizeof(v->table), &hex_len) ||
		    hex_len != sizeof(v->table))
			return -1;
	}
	if (host_json_parse_string_in(obj, len, "expect", v->expect, sizeof(v->expect)))
		return -1;
	unescape_expect(v->expect);
	return 0;
}

static int parse_set_object(const char *obj, size_t len, void *vec_void)
{
	struct set_vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "input", v->input, sizeof(v->input)))
		return -1;
	if (parse_u8_field(obj, len, "expect_accept", &v->expect_accept))
		return -1;
	if (parse_u8_field(obj, len, "expect_size", &v->expect_size))
		return -1;
	return 0;
}

#ifdef RUST_RTW_DEBUG_PROC_AMPDU_ORACLE
long host_proc_set_rx_ampdu_from_buf(void *adapter, const unsigned char *buffer,
				     size_t count);
#endif

static void normalize_proc_output(char *s)
{
	size_t i = 0;

	while (s[i]) {
		size_t start = i;

		while (s[i] && s[i] != '\n')
			i++;
		while (i > start && (s[i - 1] == ' ' || s[i - 1] == '\t'))
			i--;
		if (s[i] == '\n') {
			memmove(s + start + (i - start), s + i, strlen(s + i) + 1);
			i = start + (i - start) + 1;
		} else {
			s[i] = '\0';
			break;
		}
	}
}

static int run_get_vector(const struct get_vector *v)
{
	struct _adapter adapter;
	struct host_proc_seq ctx;
	char got[MAX_EXPECT];
	FILE *f;
	size_t nread;

	memset(&adapter, 0, sizeof(adapter));
	adapter.fix_rx_ampdu_accept = v->fix_accept;
	adapter.fix_rx_ampdu_size = v->fix_size;
	adapter.host_auto_accept = v->auto_accept;
	adapter.host_auto_size = v->auto_size;
	memcpy(adapter.registrypriv.rx_ampdu_sz_limit_by_nss_bw, v->table,
	       sizeof(v->table));

	f = tmpfile();
	if (!f)
		return -1;
	ctx.adapter = &adapter;
	ctx.out = f;
	proc_get_rx_ampdu((struct seq_file *)&ctx, NULL);
	rewind(f);
	nread = fread(got, 1, sizeof(got) - 1, f);
	fclose(f);
	got[nread] = '\0';
	normalize_proc_output(got);
	{
		char expect_trim[MAX_EXPECT];

		strcpy(expect_trim, v->expect);
		normalize_proc_output(expect_trim);
		if (strcmp(got, expect_trim) != 0) {
			fprintf(stderr, "FAIL get %s:\n  got:    %s\n  expect: %s\n",
				v->name, got, expect_trim);
			return -1;
		}
	}
	return 0;
}

static int run_set_vector(const struct set_vector *v)
{
	struct _adapter adapter;
	long ret;

	memset(&adapter, 0, sizeof(adapter));
	adapter.fix_rx_ampdu_accept = RX_AMPDU_ACCEPT_INVALID;
	adapter.fix_rx_ampdu_size = RX_AMPDU_SIZE_INVALID;

#ifdef RUST_RTW_DEBUG_PROC_AMPDU_ORACLE
	ret = host_proc_set_rx_ampdu_from_buf(&adapter, (const unsigned char *)v->input,
					      strlen(v->input));
#else
	ret = proc_set_rx_ampdu(NULL, v->input, strlen(v->input), NULL, &adapter);
#endif
	if (ret != (long)strlen(v->input))
		return -1;
	if (adapter.fix_rx_ampdu_accept != v->expect_accept ||
	    adapter.fix_rx_ampdu_size != v->expect_size)
		return -1;
	return 0;
}

int main(int argc, char **argv)
{
	const char *get_path = argc > 1 ? argv[1] : "debug_proc_rx_ampdu_get_vectors.json";
	const char *set_path = argc > 2 ? argv[2] : "debug_proc_rx_ampdu_set_vectors.json";
	struct get_vector get_vectors[MAX_VECTORS];
	struct set_vector set_vectors[MAX_VECTORS];
	size_t get_count = 0;
	size_t set_count = 0;

	if (host_load_vectors(get_path, get_vectors, sizeof(get_vectors[0]), MAX_VECTORS,
			      parse_get_object, &get_count))
		return 1;
	if (host_load_vectors(set_path, set_vectors, sizeof(set_vectors[0]), MAX_VECTORS,
			      parse_set_object, &set_count))
		return 1;

	for (size_t i = 0; i < get_count; i++) {
		if (run_get_vector(&get_vectors[i]))
			return 1;
	}
	for (size_t i = 0; i < set_count; i++) {
		if (run_set_vector(&set_vectors[i]))
			return 1;
	}

	printf("ok: %zu get + %zu set proc rx_ampdu vectors\n", get_count, set_count);
	return 0;
}
