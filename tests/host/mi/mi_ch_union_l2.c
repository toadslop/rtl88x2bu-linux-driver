// SPDX-License-Identifier: GPL-2.0
/* W3-121 L2 C oracle: mi channel union + stay-in helpers (host subset). */
#include <stdio.h>
#include <string.h>
#include "host_types.h"
#include "host_vector_json.h"

#define _TRUE 1
#define _FALSE 0
#define BIT(x) (1U << (x))
#define ASOC 0x00000001
#define LINK 0x00000080
#define OP_SW 0x00800000

struct mlme_priv { s32 fw_state; };
struct mlme_ext_priv { u8 cur_channel, cur_bwmode, cur_ch_offset; };
struct dvobj_priv {
	u8 iface_nums, union_ch, union_bw, union_offset;
	u8 union_ch_bak, union_bw_bak, union_offset_bak;
	struct _adapter *padapters[4];
};
struct _adapter {
	u8 iface_id;
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
	u8 oper_ch, oper_bw, oper_offset;
	struct dvobj_priv *dvobj;
};

static inline s32 linked(struct mlme_priv *m, s32 st)
{
	return (st == 0 && !m->fw_state) || (m->fw_state & st) ? _TRUE : _FALSE;
}

#ifdef HOST_MI_RUST
extern void mi_rust_update_union(struct _adapter *a, u8 ch, u8 off, u8 bw);
extern u8 mi_rust_stay_ch(struct _adapter *a);
extern int mi_rust_union_ifbmp(struct dvobj_priv *d, u8 ifbmp, u8 *ch, u8 *bw,
			       u8 *off);
#define update_union mi_rust_update_union
#define stay_ch mi_rust_stay_ch
#define union_ifbmp mi_rust_union_ifbmp
#else
static void update_union(struct _adapter *a, u8 ch, u8 off, u8 bw)
{
	struct dvobj_priv *d = a->dvobj;

	if (!ch) {
		d->union_ch_bak = d->union_ch;
		d->union_bw_bak = d->union_bw;
		d->union_offset_bak = d->union_offset;
	}
	d->union_ch = ch;
	d->union_bw = bw;
	d->union_offset = off;
}

static u8 stay_ch(struct _adapter *a)
{
	struct dvobj_priv *d = a->dvobj;
	u8 uc = d->union_ch ? d->union_ch : d->union_ch_bak;
	u8 ub = d->union_ch ? d->union_bw : d->union_bw_bak;
	u8 uo = d->union_ch ? d->union_offset : d->union_offset_bak;

	return uc == a->oper_ch && ub == a->oper_bw && uo == a->oper_offset;
}

static int union_ifbmp(struct dvobj_priv *d, u8 ifbmp, u8 *ch, u8 *bw, u8 *off)
{
	u8 ch_r, bw_r = 0, off_r = 0;
	int n = 0;

	if (ch)
		*ch = 0;
	if (bw)
		*bw = 0;
	if (off)
		*off = 0;
	for (int i = 0; i < d->iface_nums; i++) {
		struct _adapter *iface = d->padapters[i];
		struct mlme_ext_priv *mx;

		if (!iface || !(ifbmp & BIT(iface->iface_id)))
			continue;
		mx = &iface->mlmeextpriv;
		if (!linked(&iface->mlmepriv, ASOC | LINK) ||
		    linked(&iface->mlmepriv, OP_SW))
			continue;
		if (!n) {
			ch_r = mx->cur_channel;
			bw_r = mx->cur_bwmode;
			off_r = mx->cur_ch_offset;
			n = 1;
			continue;
		}
		if (ch_r != mx->cur_channel) {
			n = 0;
			break;
		}
		if (bw_r < mx->cur_bwmode) {
			bw_r = mx->cur_bwmode;
			off_r = mx->cur_ch_offset;
		} else if (bw_r == mx->cur_bwmode && off_r != mx->cur_ch_offset) {
			n = 0;
			break;
		}
		n++;
	}
	if (n) {
		if (ch)
			*ch = ch_r;
		if (bw)
			*bw = bw_r;
		if (off)
			*off = off_r;
	}
	return n;
}
#endif

static struct dvobj_priv g_dv;
static struct _adapter g_if[4];

struct vector {
	char name[32], op[16];
	int a, b, c, d, e, f, g, h, p, x, y, z, w;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {"a", "b", "c", "d", "e", "f", "g", "h", "p",
			      "x", "y", "z", "w", NULL};
	int *vals[] = {&v->a, &v->b, &v->c, &v->d, &v->e, &v->f, &v->g, &v->h,
		       &v->p, &v->x, &v->y, &v->z, &v->w};

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	for (int i = 0; keys[i]; i++)
		host_json_parse_int_in(obj, len, keys[i], vals[i]);
	return 0;
}

static int run_vector(struct vector *v)
{
	u8 ch, bw, off;
	int n;

	memset(&g_dv, 0, sizeof(g_dv));
	memset(g_if, 0, sizeof(g_if));
	g_dv.iface_nums = 4;
	g_dv.union_ch_bak = 9;
	for (int i = 0; i < 4; i++) {
		g_if[i].iface_id = (u8)i;
		g_if[i].dvobj = &g_dv;
		g_dv.padapters[i] = &g_if[i];
	}
	g_dv.union_ch = (u8)v->a;
	g_dv.union_bw = (u8)v->b;
	g_dv.union_offset = (u8)v->c;
	g_if[0].oper_ch = (u8)v->d;
	g_if[0].oper_bw = (u8)v->e;
	g_if[0].oper_offset = (u8)v->f;
	g_if[0].mlmepriv.fw_state = v->g;
	g_if[0].mlmeextpriv.cur_channel = (u8)v->a;
	g_if[0].mlmeextpriv.cur_bwmode = (u8)v->b;
	g_if[0].mlmeextpriv.cur_ch_offset = (u8)v->c;
	g_if[1].mlmepriv.fw_state = v->h;
	g_if[1].mlmeextpriv.cur_channel = (u8)(v->p ? v->p : v->a);
	g_if[1].mlmeextpriv.cur_bwmode = (u8)v->b;
	g_if[1].mlmeextpriv.cur_ch_offset = (u8)v->c;

	if (!strcmp(v->op, "update")) {
		if (v->d) {
			g_dv.union_ch = (u8)v->d;
			g_dv.union_bw = (u8)v->e;
			g_dv.union_offset = (u8)v->f;
		}
		update_union(&g_if[0], (u8)v->a, (u8)v->c, (u8)v->b);
		return (g_dv.union_ch == (u8)v->x && g_dv.union_bw == (u8)v->y &&
			g_dv.union_offset == (u8)v->z && g_dv.union_ch_bak == (u8)v->w) ?
			       0 :
			       -1;
	}
	if (!strcmp(v->op, "stay"))
		return stay_ch(&g_if[0]) == (u8)v->x ? 0 : -1;
	if (!strcmp(v->op, "union")) {
		n = union_ifbmp(&g_dv, 0xff, &ch, &bw, &off);
		return n == v->x && (!n || (ch == (u8)v->y && bw == (u8)v->z &&
					    off == (u8)v->w)) ?
			       0 :
			       -1;
	}
	return -1;
}

int main(int argc, char **argv)
{
	struct vector vecs[12];
	size_t n = 0;
	int fail = 0;
	const char *path = (argc > 1) ? argv[1] : "mi_ch_union_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 12, parse_vector_object, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		fail += run_vector(&vecs[i]) != 0;
	if (fail)
		fprintf(stderr, "FAIL %d/%zu (%s)\n", fail, n, path);
	else
		printf("PASS %zu vectors (%s)\n", n, path);
	return fail ? 1 : 0;
}
