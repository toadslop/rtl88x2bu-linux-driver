// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-206 tx AMPDU misc procs. */

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
#define MAX_PROC 32
#define MAX_EXPECT 256
#define MAX_INPUT 32

struct get_vector {
	char name[MAX_NAME];
	char proc[MAX_PROC];
	u8 tx_quick_addba_req;
	u8 tx_aval_int_thr_mode;
	u8 tx_aval_int_thr_value;
	char expect[MAX_EXPECT];
};

struct set_vector {
	char name[MAX_NAME];
	char proc[MAX_PROC];
	char input[MAX_INPUT];
	u8 expect_tx_quick_addba_req;
	u8 expect_tx_aval_int_thr_mode;
	u8 expect_tx_aval_int_thr_value;
};

static void unescape_expect(char *s)
{
	(void)s;
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

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "proc", v->proc, sizeof(v->proc)))
		return -1;
	(void)parse_u8_field(obj, len, "tx_quick_addba_req", &v->tx_quick_addba_req);
	(void)parse_u8_field(obj, len, "tx_aval_int_thr_mode", &v->tx_aval_int_thr_mode);
	(void)parse_u8_field(obj, len, "tx_aval_int_thr_value", &v->tx_aval_int_thr_value);
	if (host_json_parse_string_in(obj, len, "expect", v->expect, sizeof(v->expect)))
		return -1;
	return 0;
}

static int parse_set_object(const char *obj, size_t len, void *vec_void)
{
	struct set_vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "proc", v->proc, sizeof(v->proc)))
		return -1;
	if (host_json_parse_string_in(obj, len, "input", v->input, sizeof(v->input)))
		return -1;
	(void)parse_u8_field(obj, len, "expect_tx_quick_addba_req",
			     &v->expect_tx_quick_addba_req);
	(void)parse_u8_field(obj, len, "expect_tx_aval_int_thr_mode",
			     &v->expect_tx_aval_int_thr_mode);
	(void)parse_u8_field(obj, len, "expect_tx_aval_int_thr_value",
			     &v->expect_tx_aval_int_thr_value);
	return 0;
}

#ifdef RUST_RTW_DEBUG_PROC_AMPDU_ORACLE
long host_proc_set_tx_quick_addba_req_from_buf(void *adapter, const unsigned char *buffer,
					       size_t count);
long host_proc_set_tx_aval_th_from_buf(void *adapter, const unsigned char *buffer,
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

static void strip_trailing_newline(char *s)
{
	size_t n = strlen(s);

	if (n && s[n - 1] == '\n')
		s[n - 1] = '\0';
}

static int run_get_vector(const struct get_vector *v)
{
	struct _adapter adapter;
	struct host_proc_seq ctx;
	char got[MAX_EXPECT];
	FILE *f;
	size_t nread;

	memset(&adapter, 0, sizeof(adapter));
	adapter.registrypriv.tx_quick_addba_req = v->tx_quick_addba_req;
	adapter.dvobj.tx_aval_int_thr_mode = v->tx_aval_int_thr_mode;
	adapter.dvobj.tx_aval_int_thr_value = v->tx_aval_int_thr_value;

	f = tmpfile();
	if (!f)
		return -1;
	ctx.adapter = &adapter;
	ctx.out = f;

	if (strcmp(v->proc, "quick") == 0)
		proc_get_tx_quick_addba_req((struct seq_file *)&ctx, NULL);
	else if (strcmp(v->proc, "aval") == 0)
		proc_get_tx_aval_th((struct seq_file *)&ctx, NULL);
	else
		return -1;

	rewind(f);
	nread = fread(got, 1, sizeof(got) - 1, f);
	fclose(f);
	got[nread] = '\0';
	{
		char expect_trim[MAX_EXPECT];

		strcpy(expect_trim, v->expect);
		normalize_proc_output(got);
		normalize_proc_output(expect_trim);
		strip_trailing_newline(got);
		strip_trailing_newline(expect_trim);
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

#ifdef RUST_RTW_DEBUG_PROC_AMPDU_ORACLE
	if (strcmp(v->proc, "quick") == 0)
		ret = host_proc_set_tx_quick_addba_req_from_buf(
			&adapter, (const unsigned char *)v->input, strlen(v->input));
	else if (strcmp(v->proc, "aval") == 0)
		ret = host_proc_set_tx_aval_th_from_buf(
			&adapter, (const unsigned char *)v->input, strlen(v->input));
	else
		return -1;
#else
	if (strcmp(v->proc, "quick") == 0)
		ret = proc_set_tx_quick_addba_req(NULL, v->input, strlen(v->input), NULL,
						  &adapter);
	else if (strcmp(v->proc, "aval") == 0)
		ret = proc_set_tx_aval_th(NULL, v->input, strlen(v->input), NULL, &adapter);
	else
		return -1;
#endif
	if (ret != (long)strlen(v->input))
		return -1;
	if (strcmp(v->proc, "quick") == 0) {
		if (adapter.registrypriv.tx_quick_addba_req != v->expect_tx_quick_addba_req)
			return -1;
	} else if (adapter.dvobj.tx_aval_int_thr_mode != v->expect_tx_aval_int_thr_mode ||
		   adapter.dvobj.tx_aval_int_thr_value != v->expect_tx_aval_int_thr_value)
		return -1;
	return 0;
}

int main(int argc, char **argv)
{
	const char *get_path = argc > 1 ? argv[1]
					: "debug_proc_tx_ampdu_misc_get_vectors.json";
	const char *set_path = argc > 2 ? argv[2]
					: "debug_proc_tx_ampdu_misc_set_vectors.json";
	struct get_vector get_vecs[MAX_VECTORS];
	struct set_vector set_vecs[MAX_VECTORS];
	size_t nget = 0, nset = 0;
	size_t i;

	if (host_load_vectors(get_path, get_vecs, sizeof(get_vecs[0]), MAX_VECTORS,
			      parse_get_object, &nget))
		return 1;
	if (host_load_vectors(set_path, set_vecs, sizeof(set_vecs[0]), MAX_VECTORS,
			      parse_set_object, &nset))
		return 1;

	for (i = 0; i < nget; i++) {
		if (run_get_vector(&get_vecs[i]))
			return 1;
	}
	for (i = 0; i < nset; i++) {
		if (run_set_vector(&set_vecs[i]))
			return 1;
	}
	printf("PASS %zu get + %zu set vectors\n", nget, nset);
	return 0;
}
