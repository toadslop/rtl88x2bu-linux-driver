// SPDX-License-Identifier: GPL-2.0
/* W3-202 L2 runner: mi timer cancel + intf lifecycle buddy control. */

#include <stdio.h>
#include <string.h>

#include "host_types.h"
#include "host_vector_json.h"

#define BIT(x) (1U << (x))

struct mi_timer_intf_trace {
	u8 cancel_timer[4];
	u8 reset_drv[4];
	u8 intf_start[4];
	u8 intf_stop[4];
};

extern struct mi_timer_intf_trace g_mi_timer_intf_trace;
extern void mi_timer_intf_trace_reset(void);
extern void mi_timer_intf_test_setup(unsigned int up_bm);
extern struct _adapter *mi_timer_intf_test_adapter(int idx);
extern u8 mi_oracle_call_timer_intf(struct _adapter *pad, int fn_id);

#ifdef HOST_MI_TIMER_INTF_RUST
extern u8 mi_rust_call_timer_intf(struct _adapter *pad, int fn_id);
#endif

struct vector {
	char name[48];
	int p, a, g, x, y, z, w, t;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {"p", "a", "g", "x", "y", "z", "w", "t", NULL};
	int *vals[] = {&v->p, &v->a, &v->g, &v->x, &v->y, &v->z, &v->w, &v->t};

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

#ifndef HOST_MI_TIMER_INTF_RUST
static u8 call_mi(struct _adapter *pad, int fn)
{
	return mi_oracle_call_timer_intf(pad, fn);
}
#endif

static int run_vector(struct vector *v)
{
	struct _adapter *pad;
	u8 ret;

	mi_timer_intf_trace_reset();
	mi_timer_intf_test_setup((unsigned int)v->g);
	pad = mi_timer_intf_test_adapter(v->a);
	if (!pad)
		return -1;

#ifdef HOST_MI_TIMER_INTF_RUST
	ret = mi_rust_call_timer_intf(pad, v->p);
#else
	ret = call_mi(pad, v->p);
#endif

	if (ret != (u8)v->x)
		return -1;
	if (trace_bm(g_mi_timer_intf_trace.cancel_timer) != v->y)
		return -1;
	if (trace_bm(g_mi_timer_intf_trace.reset_drv) != v->z)
		return -1;
	if (trace_bm(g_mi_timer_intf_trace.intf_start) != v->w)
		return -1;
	if (trace_bm(g_mi_timer_intf_trace.intf_stop) != v->t)
		return -1;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[16];
	size_t n = 0;
	int fail = 0;
	const char *path = (argc > 1) ? argv[1] : "mi_timer_intf_vectors.json";

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
