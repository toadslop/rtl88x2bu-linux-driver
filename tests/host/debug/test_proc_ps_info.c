// SPDX-License-Identifier: GPL-2.0
/* Host L2 runner for W3-219 proc ps_info / wmmps / new_bcn_max. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_ps_info_types.h"
#include "host_vector_json.h"

int new_bcn_max = 3;

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_INPUT 64
#define MAX_FIELD 48
#define MAX_EXPECT 2048

struct vector {
	char name[MAX_NAME];
	char get_field[MAX_FIELD];
	char set_field[MAX_FIELD];
	char set_input[MAX_INPUT];
	char expect[MAX_EXPECT];
	int ips_mode_req;
	unsigned ips_enter_cnts;
	unsigned ips_leave_cnts;
	unsigned lps_enter_cnts;
	unsigned lps_leave_cnts;
	int power_mgnt;
	int dtim;
	int lps_level;
	int uapsd_max_sp_len;
	int uapsd_ac_enable;
	int new_bcn_before;
};

#ifdef PROC_PS_INFO_RUST_TEST
int proc_get_new_bcn_max(struct seq_file *m, void *v);
ssize_t proc_set_new_bcn_max(void *file, const void *buffer, size_t count,
			     long *pos, void *data);
int proc_get_ps_info(struct seq_file *m, void *v);
ssize_t proc_set_ps_info(void *file, const void *buffer, size_t count,
			 long *pos, void *data);
int proc_get_wmmps_info(struct seq_file *m, void *v);
ssize_t proc_set_wmmps_info(void *file, const void *buffer, size_t count,
			     long *pos, void *data);
#else
#define proc_get_new_bcn_max debug_proc_ps_info_c_ref_get_new_bcn_max
#define proc_set_new_bcn_max debug_proc_ps_info_c_ref_set_new_bcn_max
#define proc_get_ps_info debug_proc_ps_info_c_ref_get_ps_info
#define proc_set_ps_info debug_proc_ps_info_c_ref_set_ps_info
#define proc_get_wmmps_info debug_proc_ps_info_c_ref_get_wmmps_info
#define proc_set_wmmps_info debug_proc_ps_info_c_ref_set_wmmps_info
#endif

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
	if (host_json_find_key_in(obj, len, "ips_mode_req")) {
		if (host_json_parse_int_in(obj, len, "ips_mode_req", &tmp))
			return -1;
		v->ips_mode_req = tmp;
	}
	if (host_json_find_key_in(obj, len, "ips_enter_cnts")) {
		if (host_json_parse_int_in(obj, len, "ips_enter_cnts", &tmp))
			return -1;
		v->ips_enter_cnts = (unsigned)tmp;
	}
	if (host_json_find_key_in(obj, len, "ips_leave_cnts")) {
		if (host_json_parse_int_in(obj, len, "ips_leave_cnts", &tmp))
			return -1;
		v->ips_leave_cnts = (unsigned)tmp;
	}
	if (host_json_find_key_in(obj, len, "lps_enter_cnts")) {
		if (host_json_parse_int_in(obj, len, "lps_enter_cnts", &tmp))
			return -1;
		v->lps_enter_cnts = (unsigned)tmp;
	}
	if (host_json_find_key_in(obj, len, "lps_leave_cnts")) {
		if (host_json_parse_int_in(obj, len, "lps_leave_cnts", &tmp))
			return -1;
		v->lps_leave_cnts = (unsigned)tmp;
	}
	if (host_json_find_key_in(obj, len, "power_mgnt")) {
		if (host_json_parse_int_in(obj, len, "power_mgnt", &tmp))
			return -1;
		v->power_mgnt = tmp;
	}
	if (host_json_find_key_in(obj, len, "dtim")) {
		if (host_json_parse_int_in(obj, len, "dtim", &tmp))
			return -1;
		v->dtim = tmp;
	}
	if (host_json_find_key_in(obj, len, "lps_level")) {
		if (host_json_parse_int_in(obj, len, "lps_level", &tmp))
			return -1;
		v->lps_level = tmp;
	}
	if (host_json_find_key_in(obj, len, "uapsd_max_sp_len")) {
		if (host_json_parse_int_in(obj, len, "uapsd_max_sp_len", &tmp))
			return -1;
		v->uapsd_max_sp_len = tmp;
	}
	if (host_json_find_key_in(obj, len, "uapsd_ac_enable")) {
		if (host_json_parse_int_in(obj, len, "uapsd_ac_enable", &tmp))
			return -1;
		v->uapsd_ac_enable = tmp;
	}
	if (host_json_find_key_in(obj, len, "new_bcn_before")) {
		if (host_json_parse_int_in(obj, len, "new_bcn_before", &tmp))
			return -1;
		v->new_bcn_before = tmp;
	}
	if (host_json_find_key_in(obj, len, "set")) {
		if (host_json_parse_string_in(obj, len, "set", v->set_field,
					      sizeof(v->set_field)))
			return -1;
		if (host_json_parse_string_in(obj, len, "set_input", v->set_input,
					      sizeof(v->set_input)))
			return -1;
	}
	if (host_json_find_key_in(obj, len, "get")) {
		if (host_json_parse_string_in(obj, len, "get", v->get_field,
					      sizeof(v->get_field)))
			return -1;
	}
	if (host_json_parse_string_in(obj, len, "expect", v->expect,
				      sizeof(v->expect)))
		return -1;
	unescape_expect(v->expect);
	return 0;
}

static void apply_fixture(const struct vector *v, struct _adapter *adapter)
{
	struct pwrctrl_priv *p = &adapter->pwrctrlpriv;
	struct registry_priv *r = &adapter->registrypriv;

	p->ips_mode_req = (u8)v->ips_mode_req;
	p->ips_enter_cnts = v->ips_enter_cnts;
	p->ips_leave_cnts = v->ips_leave_cnts;
	p->lps_enter_cnts = v->lps_enter_cnts;
	p->lps_leave_cnts = v->lps_leave_cnts;
	p->power_mgnt = (u8)v->power_mgnt;
	p->dtim = (u8)v->dtim;
	p->lps_level = (u8)v->lps_level;
	r->uapsd_max_sp_len = (u8)v->uapsd_max_sp_len;
	r->uapsd_ac_enable = (u8)v->uapsd_ac_enable;
	r->power_mgnt = PS_MODE_DTIM;
	r->ips_mode = IPS_NORMAL;
	r->wow_power_mgnt = PS_MODE_ACTIVE;
}

static int dispatch_set(const struct vector *v, struct net_device *dev)
{
	if (!strcmp(v->set_field, "wmmps_info"))
		return proc_set_wmmps_info(NULL, v->set_input, strlen(v->set_input),
					   NULL, dev) < 0;
	if (!strcmp(v->set_field, "new_bcn_max"))
		return proc_set_new_bcn_max(NULL, v->set_input,
					    strlen(v->set_input), NULL, dev) < 0;
	return 1;
}

static int dispatch_get(const char *field, struct seq_file *seq)
{
	if (!strcmp(field, "ps_info"))
		return proc_get_ps_info(seq, NULL);
	if (!strcmp(field, "wmmps_info") || !strcmp(field, "wmmps_info_after_set"))
		return proc_get_wmmps_info(seq, NULL);
	if (!strcmp(field, "new_bcn_max"))
		return proc_get_new_bcn_max(seq, NULL);
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
	if (v->new_bcn_before)
		new_bcn_max = v->new_bcn_before;
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

	if (dispatch_get(v->get_field, &seq)) {
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
	const char *path =
		argc > 1 ? argv[1] : "debug_proc_ps_info_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	size_t i;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load vectors from %s\n", path);
		return 1;
	}

	for (i = 0; i < count; i++) {
		if (run_vector(&vectors[i]))
			return 1;
	}

	printf("ok: %zu proc ps_info vectors\n", count);
	return 0;
}
