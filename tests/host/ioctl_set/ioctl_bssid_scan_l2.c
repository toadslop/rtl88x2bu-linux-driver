// SPDX-License-Identifier: GPL-2.0
/* W3-125 follow-up (#982) PR7: L2 oracle for bssid_list_scan leaf wrapper. */
#include <stdio.h>
#include <string.h>

#include "host_vector_json.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1

typedef unsigned char u8;
typedef unsigned long _irqL;

struct sitesurvey_parm {
	int marker;
};

struct mlme_priv {
	int lock_depth;
};

struct adapter {
	struct mlme_priv mlmepriv;
	u8 ss_cmd_ret;
	int ss_cmd_calls;
	struct sitesurvey_parm *last_parm;
};

#ifdef HOST_IOCTL_BSSID_SCAN_RUST
extern u8 rtw_set_802_11_bssid_list_scan_rust(struct adapter *a, struct sitesurvey_parm *p);
#define SCAN_FN rtw_set_802_11_bssid_list_scan_rust
#else
static void host_enter_critical(int *depth, _irqL *irq)
{
	(*depth)++;
	(void)irq;
}

static void host_exit_critical(int *depth, _irqL *irq)
{
	(*depth)--;
	(void)irq;
}

static u8 host_sitesurvey_cmd(struct adapter *a, struct sitesurvey_parm *pparm)
{
	a->ss_cmd_calls++;
	a->last_parm = pparm;
	return a->ss_cmd_ret;
}

static u8 SCAN_FN(struct adapter *padapter, struct sitesurvey_parm *pparm)
{
	_irqL irqL;
	u8 res = _TRUE;

	host_enter_critical(&padapter->mlmepriv.lock_depth, &irqL);
	res = host_sitesurvey_cmd(padapter, pparm);
	host_exit_critical(&padapter->mlmepriv.lock_depth, &irqL);

	return res;
}
#endif

struct vector {
	char name[64];
	int cmd_ret;
	int null_parm;
	int expect_ret;
	int expect_cmd_calls;
	int expect_lock_depth;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "cmd_ret", &v->cmd_ret);
	host_json_parse_int_in(obj, len, "null_parm", &v->null_parm);
	host_json_parse_int_in(obj, len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, len, "expect_cmd_calls", &v->expect_cmd_calls);
	host_json_parse_int_in(obj, len, "expect_lock_depth", &v->expect_lock_depth);
	return 0;
}

static int run_vector(struct vector *v)
{
	struct adapter a;
	struct sitesurvey_parm parm;
	u8 ret;

	memset(&a, 0, sizeof(a));
	a.ss_cmd_ret = (u8)v->cmd_ret;
	parm.marker = 42;

	ret = SCAN_FN(&a, v->null_parm ? NULL : &parm);
	if (ret != (u8)v->expect_ret) {
		fprintf(stderr, "FAIL %s: ret got %u expect %d\n", v->name, ret, v->expect_ret);
		return 1;
	}
	if (a.ss_cmd_calls != v->expect_cmd_calls) {
		fprintf(stderr, "FAIL %s: cmd_calls got %d expect %d\n", v->name, a.ss_cmd_calls,
			v->expect_cmd_calls);
		return 1;
	}
	if (a.mlmepriv.lock_depth != v->expect_lock_depth) {
		fprintf(stderr, "FAIL %s: lock_depth got %d expect %d\n", v->name,
			a.mlmepriv.lock_depth, v->expect_lock_depth);
		return 1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[8];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "ioctl_bssid_scan_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 8, parse_vector_object, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vector(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
