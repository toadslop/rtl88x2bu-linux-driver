// SPDX-License-Identifier: GPL-2.0
/* Host L2 runner for W3-232 PCI conf space proc getters. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_pci_types.h"
#include "host_vector_json.h"

#define MAX_V 4

struct vector {
	char name[48];
	char get[32];
	char expect_contains[80];
	char expect_contains2[80];
	u32 pci_conf_dword;
	int expect_target_id, has_expect_target_id;
	int has_pciehdr;
	u16 pciehdr_offset;
	u32 linkcap;
	u16 linkctl;
	int l1off_cap;
};

static int parse_vector(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	int t;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "get", v->get, sizeof(v->get)))
		return -1;
	host_json_parse_string_in(obj, len, "expect_contains", v->expect_contains,
				  sizeof(v->expect_contains));
	host_json_parse_string_in(obj, len, "expect_contains2", v->expect_contains2,
				  sizeof(v->expect_contains2));
	if (host_json_parse_int_in(obj, len, "pci_conf_dword", &t) == 0)
		v->pci_conf_dword = (u32)t;
	if (host_json_parse_int_in(obj, len, "expect_target_id", &t) == 0) {
		v->expect_target_id = t;
		v->has_expect_target_id = 1;
	}
	if (host_json_parse_int_in(obj, len, "pciehdr_offset", &t) == 0) {
		v->pciehdr_offset = (u16)t;
		v->has_pciehdr = 1;
	}
	if (host_json_parse_int_in(obj, len, "linkcap", &t) == 0)
		v->linkcap = (u32)t;
	if (host_json_parse_int_in(obj, len, "linkctl", &t) == 0)
		v->linkctl = (u16)t;
	if (host_json_parse_int_in(obj, len, "l1off_cap", &t) == 0)
		v->l1off_cap = t;
	return 0;
}

static int run_one(const struct vector *v)
{
	struct _adapter a;
	struct net_device d;
	struct pci_bus bus;
	struct pci_dev bridge, pdev;
	struct seq_file s;
	char *cap = NULL;
	size_t cap_len = 0;
	FILE *mem;
	struct pci_dev *rdev;
	int ret;

	memset(&a, 0, sizeof(a));
	memset(&d, 0, sizeof(d));
	memset(&bus, 0, sizeof(bus));
	memset(&bridge, 0, sizeof(bridge));
	memset(&pdev, 0, sizeof(pdev));

	host_pci_get_test_reset();
	host_pci_conf_dword = v->pci_conf_dword ? v->pci_conf_dword : host_pci_conf_dword;
	if (v->has_pciehdr)
		a.dvobjpriv.pcipriv.pciehdr_offset = v->pciehdr_offset;
	host_pci_linkcap = v->linkcap;
	host_pci_linkctl = v->linkctl;
	host_pci_l1off_cap = (u8)v->l1off_cap;

	bridge.id = 2;
	pdev.id = 1;
	pdev.bus = &bus;
	bus.self = &bridge;
	a.dvobjpriv.ppcidev = &pdev;
	d.priv = &a;

	mem = open_memstream(&cap, &cap_len);
	if (!mem)
		return -1;
	s.private = &d;
	s.out = mem;

	if (!strcmp(v->get, "pci_conf_space"))
		ret = proc_get_pci_conf_space(&s, NULL);
	else if (!strcmp(v->get, "pci_bridge_conf_space"))
		ret = proc_get_pci_bridge_conf_space(&s, NULL);
	else if (!strcmp(v->get, "pci_aspm"))
		ret = proc_get_pci_aspm(&s, NULL);
	else
		ret = -1;
	fclose(mem);

	if (ret != 0 || !cap)
		goto fail;
	if (v->expect_contains[0] && !strstr(cap, v->expect_contains))
		goto fail;
	if (v->expect_contains2[0] && !strstr(cap, v->expect_contains2))
		goto fail;
	host_pci_read_get(&rdev);
	if (v->has_expect_target_id &&
	    (!rdev || (int)rdev->id != v->expect_target_id)) {
		fprintf(stderr, "%s: pci read target id mismatch\n", v->name);
		goto fail;
	}
	free(cap);
	return 0;

fail:
	fprintf(stderr, "%s: get output mismatch\n", v->name);
	free(cap);
	return -1;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_V];
	size_t n = 0;

	if (host_load_vectors(
		    argc > 1 ? argv[1] : "debug_proc_pci_get_vectors.json", vecs,
		    sizeof(vecs[0]), MAX_V, parse_vector, &n))
		return 1;
	for (size_t i = 0; i < n; i++) {
		if (run_one(&vecs[i])) {
			fprintf(stderr, "fail: %s\n", vecs[i].name);
			return 1;
		}
	}
	printf("ok: %zu proc pci get vectors\n", n);
	return 0;
}
