// SPDX-License-Identifier: GPL-2.0
/* Host L2 runner for W3-231 PCI conf space proc setters. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_pci_types.h"
#include "host_vector_json.h"

#define MAX_V 12

struct vector {
	char name[48];
	char set[32];
	char set_input[48];
	int set_count;
	int has_set_count;
	char expect_log[64];
	int expect_writes;
	int exp_reg, has_exp_reg;
	int exp_val, has_exp_val;
	int exp_target_id, has_exp_target_id;
	int exp_ret, has_exp_ret;
};

ssize_t proc_set_pci_conf_space(void *f, const void *b, size_t c, long *p, void *d);
ssize_t proc_set_pci_bridge_conf_space(void *f, const void *b, size_t c, long *p,
				      void *d);

static int parse_vector(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	int t;

	memset(v, 0, sizeof(*v));
	v->set_count = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_find_key_in(obj, len, "set")) {
		host_json_parse_string_in(obj, len, "set", v->set, sizeof(v->set));
		host_json_parse_string_in(obj, len, "set_input", v->set_input,
					  sizeof(v->set_input));
	}
	if (host_json_parse_int_in(obj, len, "set_count", &t) == 0) {
		v->set_count = t;
		v->has_set_count = 1;
	}
	if (host_json_parse_string_in(obj, len, "expect_log", v->expect_log,
				      sizeof(v->expect_log)) == 0) {
		char *r = v->expect_log, *w = v->expect_log;

		for (; *r; r++, w++) {
			if (r[0] == '\\' && r[1] == 'n') {
				*w = '\n';
				r++;
			} else {
				*w = *r;
			}
		}
		*w = '\0';
	}
	if (host_json_parse_int_in(obj, len, "expect_writes", &t) == 0)
		v->expect_writes = t;
	if (host_json_parse_int_in(obj, len, "expect_reg", &t) == 0) {
		v->exp_reg = t;
		v->has_exp_reg = 1;
	}
	if (host_json_parse_int_in(obj, len, "expect_val", &t) == 0) {
		v->exp_val = t;
		v->has_exp_val = 1;
	}
	if (host_json_parse_int_in(obj, len, "expect_target_id", &t) == 0) {
		v->exp_target_id = t;
		v->has_exp_target_id = 1;
	}
	if (host_json_parse_int_in(obj, len, "set_expect_ret", &t) == 0) {
		v->exp_ret = t;
		v->has_exp_ret = 1;
	}
	return 0;
}

static int run_one(const struct vector *v)
{
	struct _adapter a;
	struct net_device d;
	struct pci_bus bus;
	struct pci_dev bridge;
	struct pci_dev pdev;
	size_t count;
	ssize_t ret;
	u32 reg;
	u8 val;
	struct pci_dev *wdev;

	memset(&a, 0, sizeof(a));
	memset(&d, 0, sizeof(d));
	memset(&bus, 0, sizeof(bus));
	memset(&bridge, 0, sizeof(bridge));
	memset(&pdev, 0, sizeof(pdev));

	bridge.id = 2;
	pdev.id = 1;
	pdev.bus = &bus;
	bus.self = &bridge;
	a.dvobjpriv.ppcidev = &pdev;
	d.priv = &a;

	host_pci_write_reset();

	count = v->has_set_count ? (size_t)v->set_count : strlen(v->set_input);

	if (!strcmp(v->set, "pci_conf_space"))
		ret = proc_set_pci_conf_space(NULL, v->set_input, count, NULL, &d);
	else if (!strcmp(v->set, "pci_bridge_conf_space"))
		ret = proc_set_pci_bridge_conf_space(NULL, v->set_input, count, NULL,
						     &d);
	else {
		fprintf(stderr, "%s: unknown set %s\n", v->name, v->set);
		return -1;
	}

	if (v->has_exp_ret && ret != v->exp_ret) {
		fprintf(stderr, "%s: ret %zd want %d\n", v->name, ret, v->exp_ret);
		return -1;
	}
	if (host_pci_write_count() != (unsigned)v->expect_writes) {
		fprintf(stderr, "%s: writes %u want %d\n", v->name,
			host_pci_write_count(), v->expect_writes);
		return -1;
	}
	if (v->expect_log[0]) {
		const char *log = host_pci_last_log_line();
		size_t ln = strlen(log);

		if (ln > 0 && log[ln - 1] == '\n')
			ln--;
		if (ln != strlen(v->expect_log) ||
		    strncmp(log, v->expect_log, ln)) {
			fprintf(stderr, "%s: log %s want %s\n", v->name, log,
				v->expect_log);
			return -1;
		}
	}
	host_pci_write_get(&reg, &val, &wdev);
	if (v->has_exp_reg && (int)reg != v->exp_reg) {
		fprintf(stderr, "%s: reg %u want %d\n", v->name, reg, v->exp_reg);
		return -1;
	}
	if (v->has_exp_val && (int)val != v->exp_val) {
		fprintf(stderr, "%s: val %u want %d\n", v->name, val, v->exp_val);
		return -1;
	}
	if (v->has_exp_target_id && (!wdev || (int)wdev->id != v->exp_target_id)) {
		fprintf(stderr, "%s: target id mismatch\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_V];
	size_t n = 0;

	if (host_load_vectors(argc > 1 ? argv[1] : "debug_proc_pci_vectors.json",
			      vecs, sizeof(vecs[0]), MAX_V, parse_vector, &n))
		return 1;
	for (size_t i = 0; i < n; i++) {
		if (run_one(&vecs[i])) {
			fprintf(stderr, "fail: %s\n", vecs[i].name);
			return 1;
		}
	}
	printf("ok: %zu proc pci conf vectors\n", n);
	return 0;
}
