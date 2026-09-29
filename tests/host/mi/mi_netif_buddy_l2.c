// SPDX-License-Identifier: GPL-2.0
/* W3-123 L2 C oracle: mi netif buddy queue/carrier leaf (host subset). */
#include <stdio.h>
#include <string.h>
#include "host_types.h"
#include "host_vector_json.h"

#define _TRUE 1
#define BIT(x) (1U << (x))

struct mock_ndev { u8 carrier_on, queue_stopped, queue_woken; };
struct net_device { struct mock_ndev *mock; };
struct dvobj_priv { u8 iface_nums; struct _adapter *padapters[4]; };
struct _adapter {
	u8 iface_id, adapter_up;
	struct net_device *pnetdev;
	struct dvobj_priv *dvobj;
};

static struct mock_ndev g_nd[4];
static struct net_device g_net[4];
static struct dvobj_priv g_dv;
static struct _adapter g_if[4];

static void n_carrier_off(struct net_device *n)
{
	if (n && n->mock)
		n->mock->carrier_on = 0;
}
static void n_carrier_on(struct net_device *n)
{
	if (n && n->mock)
		n->mock->carrier_on = 1;
}
static void n_stop_queue(struct net_device *n)
{
	if (n && n->mock)
		n->mock->queue_stopped = 1;
}
static void n_start_queue(struct net_device *n)
{
	if (n && n->mock) {
		n->mock->queue_stopped = 0;
		n->mock->queue_woken = 1;
	}
}
static void n_wake_queue(struct net_device *n)
{
	if (n && n->mock)
		n->mock->queue_woken = 1;
}

typedef u8 (*mi_op)(struct _adapter *, void *);

static u8 mi_process(struct _adapter *pad, int ex_self, mi_op op, void *data)
{
	u8 ret = 0;

	for (int i = 0; i < pad->dvobj->iface_nums; i++) {
		struct _adapter *iface = pad->dvobj->padapters[i];

		if (!iface || !iface->adapter_up)
			continue;
		if (ex_self && iface == pad)
			continue;
		if (op && op(iface, data) == _TRUE)
			ret++;
	}
	return ret;
}

static u8 op_caroff(struct _adapter *a, void *d)
{
	(void)d;
	n_carrier_off(a->pnetdev);
	n_stop_queue(a->pnetdev);
	return _TRUE;
}
static u8 op_caron(struct _adapter *a, void *d)
{
	(void)d;
	n_carrier_on(a->pnetdev);
	n_start_queue(a->pnetdev);
	return _TRUE;
}
static u8 op_stop(struct _adapter *a, void *d)
{
	(void)d;
	n_stop_queue(a->pnetdev);
	return _TRUE;
}
static u8 op_wake(struct _adapter *a, void *d)
{
	(void)d;
	if (a->pnetdev)
		n_wake_queue(a->pnetdev);
	return _TRUE;
}
static u8 op_carr_on(struct _adapter *a, void *d)
{
	(void)d;
	if (a->pnetdev)
		n_carrier_on(a->pnetdev);
	return _TRUE;
}
static u8 op_carr_off(struct _adapter *a, void *d)
{
	(void)d;
	if (a->pnetdev)
		n_carrier_off(a->pnetdev);
	return _TRUE;
}

static u8 call_mi(struct _adapter *pad, int fn)
{
	static const struct { mi_op op; int buddy; } tbl[] = {
		{op_caroff, 0},  {op_caroff, 1},  {op_caron, 0},  {op_caron, 1},
		{op_stop, 0},    {op_stop, 1},    {op_wake, 0},   {op_wake, 1},
		{op_carr_on, 0}, {op_carr_on, 1}, {op_carr_off, 0}, {op_carr_off, 1},
	};

	if (fn < 0 || fn >= (int)(sizeof(tbl) / sizeof(tbl[0])))
		return 0xff;
	return mi_process(pad, tbl[fn].buddy, tbl[fn].op, NULL);
}

struct vector {
	char name[32];
	int p, a, b, g, h, x, y, z, w;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {"p", "a", "b", "g", "h", "x", "y", "z", "w", NULL};
	int *vals[] = {&v->p, &v->a, &v->b, &v->g, &v->h, &v->x, &v->y, &v->z, &v->w};

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	for (int i = 0; keys[i]; i++)
		host_json_parse_int_in(obj, len, keys[i], vals[i]);
	return 0;
}

static void setup(struct vector *v)
{
	memset(g_nd, 0, sizeof(g_nd));
	memset(g_net, 0, sizeof(g_net));
	memset(&g_dv, 0, sizeof(g_dv));
	memset(g_if, 0, sizeof(g_if));
	g_dv.iface_nums = 4;
	for (int i = 0; i < 4; i++) {
		g_if[i].iface_id = (u8)i;
		g_if[i].dvobj = &g_dv;
		g_dv.padapters[i] = &g_if[i];
		g_if[i].adapter_up = (v->g & BIT(i)) ? 1 : 0;
		if (v->h & BIT(i)) {
			g_nd[i].carrier_on = (v->b & BIT(i)) ? 1 : 0;
			g_net[i].mock = &g_nd[i];
			g_if[i].pnetdev = &g_net[i];
		}
	}
}

static int check_state(int y, int z, int w)
{
	for (int i = 0; i < 4; i++) {
		if (!g_if[i].pnetdev)
			continue;
		if (g_nd[i].carrier_on != ((y >> i) & 1))
			return -1;
		if (g_nd[i].queue_stopped != ((z >> i) & 1))
			return -1;
		if (g_nd[i].queue_woken != ((w >> i) & 1))
			return -1;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	setup(v);
	if (call_mi(&g_if[v->a], v->p) != (u8)v->x)
		return -1;
	return check_state(v->y, v->z, v->w);
}

int main(int argc, char **argv)
{
	struct vector vecs[16];
	size_t n = 0;
	int fail = 0;
	const char *path = (argc > 1) ? argv[1] : "mi_netif_buddy_vectors.json";

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
