// SPDX-License-Identifier: GPL-2.0
/* W3-199 L2 runner: mi scan abort + drv thread buddy control. */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "host_types.h"
#include "host_vector_json.h"

#define BIT(x) (1U << (x))

struct mi_scan_drv_trace {
	u8 scan_wait[4];
	u8 scan_nowait[4];
	u8 start_drv[4];
	u8 stop_drv[4];
};

extern struct mi_scan_drv_trace g_mi_scan_drv_trace;
extern void mi_scan_drv_trace_reset(void);
extern void mi_scan_drv_test_setup(unsigned int up_bm, unsigned int start_fail_bm);
extern struct _adapter *mi_scan_drv_test_adapter(int idx);
extern u8 mi_oracle_scan_abort(struct _adapter *adapter, bool buddy, bool bwait);
extern u32 rtw_mi_start_drv_threads(struct _adapter *adapter);
extern u32 rtw_mi_buddy_start_drv_threads(struct _adapter *adapter);
extern void rtw_mi_stop_drv_threads(struct _adapter *adapter);
extern void rtw_mi_buddy_stop_drv_threads(struct _adapter *adapter);

#ifdef HOST_MI_SCAN_DRV_RUST
extern u32 mi_rust_call_scan_drv(struct _adapter *pad, int fn_id, int bwait);
#endif

struct vector {
	char name[48];
	int p, a, g, b, h, x, y, z, w, t;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {"p", "a", "g", "b", "h", "x", "y", "z", "w", "t", NULL};
	int *vals[] = {&v->p, &v->a, &v->g, &v->b, &v->h, &v->x,
		       &v->y, &v->z, &v->w, &v->t};

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	for (int i = 0; keys[i]; i++)
		host_json_parse_int_in(obj, len, keys[i], vals[i]);
	return 0;
}

static int trace_bm(const u8 *arr)
{
	int bm = 0;

	for (int i = 0; i < 4; i++)
		if (arr[i])
			bm |= BIT(i);
	return bm;
}

#ifndef HOST_MI_SCAN_DRV_RUST
static u32 call_mi(struct _adapter *pad, int fn, int bwait)
{
	switch (fn) {
	case 0:
		return mi_oracle_scan_abort(pad, false, bwait ? true : false);
	case 1:
		return mi_oracle_scan_abort(pad, true, bwait ? true : false);
	case 2:
		return rtw_mi_start_drv_threads(pad);
	case 3:
		return rtw_mi_buddy_start_drv_threads(pad);
	case 4:
		rtw_mi_stop_drv_threads(pad);
		return 0;
	case 5:
		rtw_mi_buddy_stop_drv_threads(pad);
		return 0;
	default:
		return 0xff;
	}
}
#endif

static int run_vector(struct vector *v)
{
	struct _adapter *pad;
	u32 ret;

	mi_scan_drv_trace_reset();
	mi_scan_drv_test_setup((unsigned int)v->g, (unsigned int)v->h);
	pad = mi_scan_drv_test_adapter(v->a);
	if (!pad)
		return -1;

#ifdef HOST_MI_SCAN_DRV_RUST
	ret = mi_rust_call_scan_drv(pad, v->p, v->b);
#else
	ret = call_mi(pad, v->p, v->b);
#endif

	if (v->p <= 1) {
		if (ret != (u32)v->x)
			return -1;
	} else if (v->p <= 3) {
		if (ret != (u32)v->x)
			return -1;
	}

	if (trace_bm(g_mi_scan_drv_trace.scan_wait) != v->y)
		return -1;
	if (trace_bm(g_mi_scan_drv_trace.scan_nowait) != v->z)
		return -1;
	if (trace_bm(g_mi_scan_drv_trace.start_drv) != v->w)
		return -1;
	if (trace_bm(g_mi_scan_drv_trace.stop_drv) != v->t)
		return -1;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[16];
	size_t n = 0;
	int fail = 0;
	const char *path = (argc > 1) ? argv[1] : "mi_scan_drv_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 16, parse_vector_object, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		fail += run_vector(&vecs[i]) != 0;
	if (fail)
		fprintf(stderr, "FAIL %d/%zu (%s)\n", fail, n, path);
	else
		printf("PASS %zu vectors (%s)\n", n, path);
	return fail ? 1 : 0;
}
