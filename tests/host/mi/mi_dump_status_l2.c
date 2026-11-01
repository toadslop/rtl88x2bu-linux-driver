// SPDX-License-Identifier: GPL-2.0
/* Host L2 runner for W3-198 dump_mi_status / dump_dvobj_mi_status. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_mi_dump_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_SUBSTR 512

struct mi_dump_vector {
	char name[MAX_NAME];
	struct mi_state iface_state;
	unsigned int union_ch;
	unsigned int union_bw;
	unsigned int union_offset;
	char expect_substr[MAX_SUBSTR];
};

struct host_sel_capture {
	char buf[4096];
	size_t len;
};

extern void dump_mi_status(void *sel, struct mi_dump_dvobj *dvobj);
extern void dump_dvobj_mi_status(void *sel, const char *fun_name, _adapter *adapter);

#ifdef HOST_MI_DUMP_RUST
struct host_sel_capture host_mi_dump_sel;

void *host_mi_dump_adapter_dvobj(_adapter *a)
{
	return a->dvobj;
}
#endif

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct mi_dump_vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
#define PARSE_U8(field, member) \
	do { \
		if (host_json_parse_int_in(obj, len, field, &tmp)) \
			return -1; \
		v->member = (u8)tmp; \
	} while (0)
	PARSE_U8("sta_num", iface_state.sta_num);
	PARSE_U8("ld_sta_num", iface_state.ld_sta_num);
	PARSE_U8("lg_sta_num", iface_state.lg_sta_num);
	PARSE_U8("ld_tdls_num", iface_state.ld_tdls_num);
	PARSE_U8("ap_num", iface_state.ap_num);
	PARSE_U8("starting_ap_num", iface_state.starting_ap_num);
	PARSE_U8("ld_ap_num", iface_state.ld_ap_num);
	PARSE_U8("adhoc_num", iface_state.adhoc_num);
	PARSE_U8("ld_adhoc_num", iface_state.ld_adhoc_num);
	PARSE_U8("mesh_num", iface_state.mesh_num);
	PARSE_U8("ld_mesh_num", iface_state.ld_mesh_num);
	PARSE_U8("scan_num", iface_state.scan_num);
	PARSE_U8("uwps_num", iface_state.uwps_num);
	PARSE_U8("roch_num", iface_state.roch_num);
	PARSE_U8("mgmt_tx_num", iface_state.mgmt_tx_num);
	PARSE_U8("p2p_device_num", iface_state.p2p_device_num);
	PARSE_U8("p2p_gc", iface_state.p2p_gc);
	PARSE_U8("p2p_go", iface_state.p2p_go);
	if (host_json_parse_int_in(obj, len, "union_ch", &tmp))
		return -1;
	v->union_ch = (unsigned int)tmp;
	if (host_json_parse_int_in(obj, len, "union_bw", &tmp))
		return -1;
	v->union_bw = (unsigned int)tmp;
	if (host_json_parse_int_in(obj, len, "union_offset", &tmp))
		return -1;
	v->union_offset = (unsigned int)tmp;
	if (host_json_parse_string_in(obj, len, "expect_substr", v->expect_substr,
				      sizeof(v->expect_substr)))
		return -1;
	return 0;
}

static void fill_dvobj(struct mi_dump_dvobj *dv, const struct mi_dump_vector *v)
{
	memset(dv, 0, sizeof(*dv));
	dv->iface_state = v->iface_state;
	dv->union_ch = (u8)v->union_ch;
	dv->union_bw = (u8)v->union_bw;
	dv->union_offset = (u8)v->union_offset;
}

static int run_vector(const struct mi_dump_vector *v, int use_dvobj_wrapper)
{
	struct host_sel_capture cap;
	struct mi_dump_dvobj dv;
	struct _adapter adapter;

	memset(&cap, 0, sizeof(cap));
	fill_dvobj(&dv, v);
	adapter.dvobj = (struct dvobj_priv *)&dv;

	if (use_dvobj_wrapper)
		dump_dvobj_mi_status(&cap, "test_fn", &adapter);
	else
		dump_mi_status(&cap, &dv);

	cap.buf[cap.len] = '\0';
	if (!strstr(cap.buf, v->expect_substr)) {
		fprintf(stderr, "FAIL %s: missing substring %s\nin:\n%s\n",
			v->name, v->expect_substr, cap.buf);
		return 1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct mi_dump_vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "mi_dump_status_vectors.json";
	size_t i;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (i = 0; i < count; i++) {
		if (run_vector(&vectors[i], 0))
			return 1;
		if (run_vector(&vectors[i], 1))
			return 1;
	}

	printf("ok %zu vectors\n", count);
	return 0;
}
