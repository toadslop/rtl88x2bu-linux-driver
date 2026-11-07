// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-221 11w TX mgmt inject test procs. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_tx_mgmt_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_INPUT 16
#define MAX_FIELD 32
#define MAX_EXPECT 128

struct vector {
	char name[MAX_NAME];
	u32 fw_state;
	u8 bip_installed;
	u8 bssid[ETH_ALEN];
	char set_field[MAX_FIELD];
	char set_input[MAX_INPUT];
	size_t set_count;
	int set_expect_fail;
	char get_field[MAX_FIELD];
	char expect[MAX_EXPECT];
	int issue_sa_query;
	int issue_sa_key;
	int issue_deauth;
	int issue_deauth_key;
	int issue_auth;
	int issue_assocreq;
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

static int parse_mac(const char *s, u8 *mac)
{
	unsigned a[ETH_ALEN];

	if (sscanf(s, "%02x:%02x:%02x:%02x:%02x:%02x", &a[0], &a[1], &a[2],
		   &a[3], &a[4], &a[5]) != ETH_ALEN)
		return -1;
	for (int i = 0; i < ETH_ALEN; i++)
		mac[i] = (u8)a[i];
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	{
		int fw;

		if (host_json_parse_int_in(obj, len, "fw_state", &fw))
			return -1;
		v->fw_state = (u32)fw;
	}
	if (host_json_find_key_in(obj, len, "bip_installed")) {
		int tmp;

		if (host_json_parse_int_in(obj, len, "bip_installed", &tmp))
			return -1;
		v->bip_installed = (u8)tmp;
	}
	if (host_json_find_key_in(obj, len, "bssid")) {
		char macstr[32];

		if (host_json_parse_string_in(obj, len, "bssid", macstr,
					      sizeof(macstr)))
			return -1;
		if (parse_mac(macstr, v->bssid))
			return -1;
	}
	if (host_json_find_key_in(obj, len, "set")) {
		if (host_json_parse_string_in(obj, len, "set", v->set_field,
					      sizeof(v->set_field)))
			return -1;
		if (host_json_parse_string_in(obj, len, "set_input",
					      v->set_input, sizeof(v->set_input)))
			return -1;
		if (host_json_find_key_in(obj, len, "set_count")) {
			int tmp;

			if (host_json_parse_int_in(obj, len, "set_count", &tmp))
				return -1;
			v->set_count = (size_t)tmp;
		} else {
			v->set_count = strlen(v->set_input);
		}
	}
	if (host_json_find_key_in(obj, len, "get")) {
		if (host_json_parse_string_in(obj, len, "get", v->get_field,
					      sizeof(v->get_field)))
			return -1;
	}
	if (host_json_find_key_in(obj, len, "expect")) {
		if (host_json_parse_string_in(obj, len, "expect", v->expect,
					      sizeof(v->expect)))
			return -1;
		unescape_expect(v->expect);
	}
	if (host_json_find_key_in(obj, len, "set_expect_fail")) {
		int tmp;

		if (host_json_parse_int_in(obj, len, "set_expect_fail", &tmp))
			return -1;
		v->set_expect_fail = tmp;
	}
#define PARSE_ISSUE(field, member)                                           \
	if (host_json_find_key_in(obj, len, field)) {                        \
		int tmp;                                                         \
		if (host_json_parse_int_in(obj, len, field, &tmp))               \
			return -1;                                               \
		v->member = tmp;                                                 \
	}
	PARSE_ISSUE("issue_sa_query", issue_sa_query);
	PARSE_ISSUE("issue_sa_key", issue_sa_key);
	PARSE_ISSUE("issue_deauth", issue_deauth);
	PARSE_ISSUE("issue_deauth_key", issue_deauth_key);
	PARSE_ISSUE("issue_auth", issue_auth);
	PARSE_ISSUE("issue_assocreq", issue_assocreq);
	return 0;
}

static int run_vector(const struct vector *v)
{
	struct _adapter adapter;
	struct net_device dev;
	struct seq_file seq;
	char got[MAX_EXPECT];
	FILE *capture;
	size_t nread;
	ssize_t ret;
	const struct host_tx_mgmt_issue_log *log;

	host_debug_proc_tx_mgmt_reset_log();
	memset(&adapter, 0, sizeof(adapter));
	adapter.mlmepriv.fw_state = v->fw_state;
	adapter.securitypriv.binstallBIPkey = v->bip_installed;
	memcpy(adapter.mlmeextpriv.mlmext_info.network.MacAddress, v->bssid,
	       ETH_ALEN);
	dev.priv = &adapter;
	seq.private = &dev;

	if (v->set_field[0]) {
		if (!strcmp(v->set_field, "tx_sa_query"))
			ret = proc_set_tx_sa_query(NULL, v->set_input,
						   v->set_count, NULL, &dev);
		else if (!strcmp(v->set_field, "tx_deauth"))
			ret = proc_set_tx_deauth(NULL, v->set_input,
						 v->set_count, NULL, &dev);
		else if (!strcmp(v->set_field, "tx_auth"))
			ret = proc_set_tx_auth(NULL, v->set_input, v->set_count,
					       NULL, &dev);
		else
			return -1;

		if (v->set_expect_fail) {
			if (ret >= 0) {
				fprintf(stderr, "FAIL %s: expected set fail\n",
					v->name);
				return -1;
			}
		} else if (ret < 0) {
			fprintf(stderr, "FAIL %s: set returned %zd\n", v->name,
				ret);
			return -1;
		}
	}

	if (v->get_field[0]) {
		capture = tmpfile();
		if (!capture)
			return -1;
		seq.out = capture;

		if (!strcmp(v->get_field, "tx_sa_query"))
			proc_get_tx_sa_query(&seq, NULL);
		else if (!strcmp(v->get_field, "tx_deauth"))
			proc_get_tx_deauth(&seq, NULL);
		else if (!strcmp(v->get_field, "tx_auth"))
			proc_get_tx_auth(&seq, NULL);
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
	}

	log = host_debug_proc_tx_mgmt_get_log();
	if (log->sa_query != (unsigned)v->issue_sa_query) {
		fprintf(stderr, "FAIL %s: sa_query issues %u expect %d\n",
			v->name, log->sa_query, v->issue_sa_query);
		return -1;
	}
	if (v->issue_sa_query && log->last_sa_key != (u8)v->issue_sa_key) {
		fprintf(stderr, "FAIL %s: sa_key %u expect %d\n", v->name,
			log->last_sa_key, v->issue_sa_key);
		return -1;
	}
	if (log->deauth != (unsigned)v->issue_deauth) {
		fprintf(stderr, "FAIL %s: deauth issues %u expect %d\n",
			v->name, log->deauth, v->issue_deauth);
		return -1;
	}
	if (v->issue_deauth &&
	    log->last_deauth_key != (u8)v->issue_deauth_key) {
		fprintf(stderr, "FAIL %s: deauth key %u expect %d\n", v->name,
			log->last_deauth_key, v->issue_deauth_key);
		return -1;
	}
	if (log->auth != (unsigned)v->issue_auth) {
		fprintf(stderr, "FAIL %s: auth issues %u expect %d\n", v->name,
			log->auth, v->issue_auth);
		return -1;
	}
	if (log->assocreq != (unsigned)v->issue_assocreq) {
		fprintf(stderr, "FAIL %s: assocreq issues %u expect %d\n",
			v->name, log->assocreq, v->issue_assocreq);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = argc > 1 ? argv[1]
				    : "debug_proc_tx_mgmt_vectors.json";
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

	printf("ok: %zu proc tx mgmt vectors\n", count);
	return 0;
}
