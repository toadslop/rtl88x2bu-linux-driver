// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for regd_exc list CRUD/search (W3-51).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rf_regd_exc_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_REGD_NAME 32

enum regd_exc_fn {
	FN_ADD_SEARCH = 0,
	FN_SEARCH,
	FN_ADD_FREE,
	FN_MULTI_SEARCH,
};

struct vector {
	char name[MAX_NAME];
	enum regd_exc_fn fn;
	char country_add[3];
	u8 domain_add;
	char regd_name[MAX_REGD_NAME];
	char country_search[3];
	u8 domain_search;
	int expect_found;
	int expect_num;
	int expect_num_after_add;
	int expect_num_after_free;
	char expect_name[MAX_REGD_NAME];
	char entry1_country[3];
	u8 entry1_domain;
	char entry1_name[MAX_REGD_NAME];
	char entry2_country[3];
	u8 entry2_domain;
	char entry2_name[MAX_REGD_NAME];
};

static const char *country_ptr(const char *country)
{
	if (!country || !country[0])
		return NULL;
	return country;
}

static int parse_fn(const char *obj, size_t len, enum regd_exc_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "add_search"))
		*out = FN_ADD_SEARCH;
	else if (!strcmp(fn, "search"))
		*out = FN_SEARCH;
	else if (!strcmp(fn, "add_free"))
		*out = FN_ADD_FREE;
	else if (!strcmp(fn, "multi_search"))
		*out = FN_MULTI_SEARCH;
	else
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, len, &v->fn))
		return -1;
	host_json_parse_string_in(obj, len, "country_add", v->country_add,
				  sizeof(v->country_add));
	host_json_parse_string_in(obj, len, "regd_name", v->regd_name,
				  sizeof(v->regd_name));
	host_json_parse_string_in(obj, len, "country_search", v->country_search,
				  sizeof(v->country_search));
	host_json_parse_string_in(obj, len, "expect_name", v->expect_name,
				  sizeof(v->expect_name));
	host_json_parse_string_in(obj, len, "entry1_country", v->entry1_country,
				  sizeof(v->entry1_country));
	host_json_parse_string_in(obj, len, "entry1_name", v->entry1_name,
				  sizeof(v->entry1_name));
	host_json_parse_string_in(obj, len, "entry2_country", v->entry2_country,
				  sizeof(v->entry2_country));
	host_json_parse_string_in(obj, len, "entry2_name", v->entry2_name,
				  sizeof(v->entry2_name));
	if (!host_json_parse_int_in(obj, len, "domain_add", &tmp))
		v->domain_add = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "domain_search", &tmp))
		v->domain_search = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "entry1_domain", &tmp))
		v->entry1_domain = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "entry2_domain", &tmp))
		v->entry2_domain = (u8)tmp;
	host_json_parse_int_in(obj, len, "expect_found", &v->expect_found);
	host_json_parse_int_in(obj, len, "expect_num", &v->expect_num);
	host_json_parse_int_in(obj, len, "expect_num_after_add",
			       &v->expect_num_after_add);
	host_json_parse_int_in(obj, len, "expect_num_after_free",
			       &v->expect_num_after_free);
	return 0;
}

static int check_search(struct vector *v, struct rf_ctl_t *rfctl)
{
	struct regd_exc_ent *ent;
	const char *country = country_ptr(v->country_search);

	ent = rtw_regd_exc_search(rfctl, country, v->domain_search);
	if (v->expect_found) {
		if (!ent) {
			fprintf(stderr, "%s: expected match, got NULL\n", v->name);
			return -1;
		}
		if (v->expect_name[0] &&
		    strcmp(ent->regd_name, v->expect_name) != 0) {
			fprintf(stderr, "%s: got name '%s' expect '%s'\n",
				v->name, ent->regd_name, v->expect_name);
			return -1;
		}
		if (!v->expect_name[0] && v->regd_name[0] &&
		    strcmp(ent->regd_name, v->regd_name) != 0) {
			fprintf(stderr, "%s: got name '%s' expect '%s'\n",
				v->name, ent->regd_name, v->regd_name);
			return -1;
		}
	} else if (ent) {
		fprintf(stderr, "%s: expected miss, got '%s'\n", v->name,
			ent->regd_name);
		return -1;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	struct rf_ctl_t rfctl;

	host_rf_regd_exc_reset(&rfctl);

	switch (v->fn) {
	case FN_ADD_SEARCH:
		rtw_regd_exc_add(&rfctl, country_ptr(v->country_add), v->domain_add,
				 v->regd_name);
		if ((int)rfctl.regd_exc_num != v->expect_num) {
			fprintf(stderr, "%s: regd_exc_num=%u expect=%d\n",
				v->name, rfctl.regd_exc_num, v->expect_num);
			return -1;
		}
		return check_search(v, &rfctl);
	case FN_SEARCH:
		if ((int)rfctl.regd_exc_num != v->expect_num) {
			fprintf(stderr, "%s: regd_exc_num=%u expect=%d\n",
				v->name, rfctl.regd_exc_num, v->expect_num);
			return -1;
		}
		return check_search(v, &rfctl);
	case FN_ADD_FREE:
		rtw_regd_exc_add(&rfctl, country_ptr(v->country_add), v->domain_add,
				 v->regd_name);
		if ((int)rfctl.regd_exc_num != v->expect_num_after_add) {
			fprintf(stderr, "%s: after add num=%u expect=%d\n",
				v->name, rfctl.regd_exc_num, v->expect_num_after_add);
			return -1;
		}
		rtw_regd_exc_list_free(&rfctl);
		if ((int)rfctl.regd_exc_num != v->expect_num_after_free) {
			fprintf(stderr, "%s: after free num=%u expect=%d\n",
				v->name, rfctl.regd_exc_num,
				v->expect_num_after_free);
			return -1;
		}
		break;
	case FN_MULTI_SEARCH:
		rtw_regd_exc_add(&rfctl, country_ptr(v->entry1_country),
				 v->entry1_domain, v->entry1_name);
		rtw_regd_exc_add(&rfctl, country_ptr(v->entry2_country),
				 v->entry2_domain, v->entry2_name);
		if ((int)rfctl.regd_exc_num != v->expect_num) {
			fprintf(stderr, "%s: regd_exc_num=%u expect=%d\n",
				v->name, rfctl.regd_exc_num, v->expect_num);
			return -1;
		}
		return check_search(v, &rfctl);
	default:
		fprintf(stderr, "%s: unknown fn\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t count = 0;
	size_t executed = 0;
	size_t i;
	const char *path = "regd_exc_vectors.json";

	if (argc > 1)
		path = argv[1];
	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}
	for (i = 0; i < count; i++) {
		executed++;
		if (run_vector(&vecs[i])) {
			fprintf(stderr, "FAIL %s\n", vecs[i].name);
			return 1;
		}
	}
#ifdef RUST_RF_REGD_EXC_ORACLE
	printf("PASS %zu vectors (oracle: rust/rtw_rf_rest.rs regd_exc) (%s)\n",
	       executed, path);
#else
	printf("PASS %zu vectors (%s)\n", executed, path);
#endif
	return 0;
}
