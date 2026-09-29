// SPDX-License-Identifier: GPL-2.0
/* W3-127 L2 C oracle: btcoex handler + AMPDU/policy leaf (core/rtw_btcoex.c). */
#include <stdio.h>
#include <string.h>

#include "host_types.h"
#include "host_vector_json.h"

typedef int sint;
#define _TRUE 1
#define _FALSE 0

typedef struct {
	u8 eeprom_coexist;
} mock_hal;
typedef struct {
	mock_hal hal;
} mock_adpt;

struct host_btcoex_hp_tr {
	unsigned handler, reject, ctrl, get_size, manual, policy, disabled, coexist;
	s32 hal_reject, hal_ctrl;
	u32 hal_size;
	u8 last_manual, last_policy, last_coexist, last_disabled;
};

static struct host_btcoex_hp_tr g_tr;

static void tr_reset(void) { memset(&g_tr, 0, sizeof(g_tr)); }

void hal_btcoex_set_hal_reject(s32 v) { g_tr.hal_reject = v; }
void hal_btcoex_set_hal_ctrl(s32 v) { g_tr.hal_ctrl = v; }
void hal_btcoex_set_hal_size(u32 v) { g_tr.hal_size = v; }
void hal_btcoex_set_hal_disabled(u8 v) { g_tr.last_disabled = v; }

void hal_btcoex_Hanlder(mock_adpt *a) { (void)a; g_tr.handler++; }
s32 hal_btcoex_IsBTCoexRejectAMPDU(mock_adpt *a)
{
	(void)a;
	g_tr.reject++;
	return g_tr.hal_reject;
}
s32 hal_btcoex_IsBTCoexCtrlAMPDUSize(mock_adpt *a)
{
	(void)a;
	g_tr.ctrl++;
	return g_tr.hal_ctrl;
}
u32 hal_btcoex_GetAMPDUSize(mock_adpt *a)
{
	(void)a;
	g_tr.get_size++;
	return g_tr.hal_size;
}
void hal_btcoex_SetManualControl(mock_adpt *a, u8 manual)
{
	(void)a;
	g_tr.manual++;
	g_tr.last_manual = manual;
}
void hal_btcoex_set_policy_control(mock_adpt *a, u8 btc_policy)
{
	(void)a;
	g_tr.policy++;
	g_tr.last_policy = btc_policy;
}
u8 hal_btcoex_IsBtDisabled(mock_adpt *a)
{
	(void)a;
	g_tr.disabled++;
	return g_tr.last_disabled;
}
void hal_btcoex_SetBTCoexist(mock_adpt *a, u8 enable)
{
	(void)a;
	g_tr.coexist++;
	g_tr.last_coexist = enable;
}

#ifndef HOST_BTCOEX_HANDLER_POLICY_RUST
static void o_handler(mock_adpt *a)
{
	if (!a->hal.eeprom_coexist)
		return;
	hal_btcoex_Hanlder(a);
}
static sint o_reject_ampdu(mock_adpt *a) { return hal_btcoex_IsBTCoexRejectAMPDU(a); }
static sint o_ctrl_ampdu(mock_adpt *a) { return hal_btcoex_IsBTCoexCtrlAMPDUSize(a); }
static u32 o_get_ampdu(mock_adpt *a) { return hal_btcoex_GetAMPDUSize(a); }
static void o_set_manual(mock_adpt *a, u8 manual)
{
	if (manual == _TRUE)
		hal_btcoex_SetManualControl(a, _TRUE);
	else
		hal_btcoex_SetManualControl(a, _FALSE);
}
static void o_set_policy(mock_adpt *a, u8 pol) { hal_btcoex_set_policy_control(a, pol); }
static u8 o_is_disabled(mock_adpt *a) { return hal_btcoex_IsBtDisabled(a); }
static void o_switch(mock_adpt *a, u8 en) { hal_btcoex_SetBTCoexist(a, en); }
#else
extern void o_handler(mock_adpt *a);
extern sint o_reject_ampdu(mock_adpt *a);
extern sint o_ctrl_ampdu(mock_adpt *a);
extern u32 o_get_ampdu(mock_adpt *a);
extern void o_set_manual(mock_adpt *a, u8 manual);
extern void o_set_policy(mock_adpt *a, u8 pol);
extern u8 o_is_disabled(mock_adpt *a);
extern void o_switch(mock_adpt *a, u8 en);
#endif

struct vector {
	char name[48], fn[24];
	int coex, arg, hal_reject, hal_ctrl, hal_size, hal_disabled;
	int exp_handler, exp_reject, exp_ctrl, exp_size, exp_manual, exp_manual_val;
	int exp_policy, exp_policy_val, exp_disabled, exp_coexist, exp_coexist_val;
};

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(o, l, "fn", v->fn, sizeof(v->fn)))
		return -1;
#define I(k, f) host_json_parse_int_in(o, l, k, &v->f)
	I("coex", coex);
	I("arg", arg);
	I("hal_reject", hal_reject);
	I("hal_ctrl", hal_ctrl);
	I("hal_size", hal_size);
	I("hal_disabled", hal_disabled);
	I("exp_handler", exp_handler);
	I("exp_reject", exp_reject);
	I("exp_ctrl", exp_ctrl);
	I("exp_size", exp_size);
	I("exp_manual", exp_manual);
	I("exp_manual_val", exp_manual_val);
	I("exp_policy", exp_policy);
	I("exp_policy_val", exp_policy_val);
	I("exp_disabled", exp_disabled);
	I("exp_coexist", exp_coexist);
	I("exp_coexist_val", exp_coexist_val);
#undef I
	return 0;
}

static int run_vec(struct vector *v)
{
	mock_adpt a = {0};
	sint ret_s = 0;
	u32 ret_u = 0;
	u8 ret_u8 = 0;

	tr_reset();
	hal_btcoex_set_hal_reject(v->hal_reject);
	hal_btcoex_set_hal_ctrl(v->hal_ctrl);
	hal_btcoex_set_hal_size((u32)v->hal_size);
	hal_btcoex_set_hal_disabled((u8)v->hal_disabled);
	a.hal.eeprom_coexist = (u8)v->coex;

	if (!strcmp(v->fn, "handler"))
		o_handler(&a);
	else if (!strcmp(v->fn, "reject_ampdu"))
		ret_s = o_reject_ampdu(&a);
	else if (!strcmp(v->fn, "ctrl_ampdu"))
		ret_s = o_ctrl_ampdu(&a);
	else if (!strcmp(v->fn, "get_ampdu"))
		ret_u = o_get_ampdu(&a);
	else if (!strcmp(v->fn, "set_manual"))
		o_set_manual(&a, (u8)v->arg);
	else if (!strcmp(v->fn, "set_policy"))
		o_set_policy(&a, (u8)v->arg);
	else if (!strcmp(v->fn, "is_disabled"))
		ret_u8 = o_is_disabled(&a);
	else if (!strcmp(v->fn, "switch"))
		o_switch(&a, (u8)v->arg);
	else
		return -1;

	if (g_tr.handler != (unsigned)v->exp_handler ||
	    g_tr.reject != (unsigned)(v->exp_reject ? 1 : 0) ||
	    (v->exp_reject && ret_s != v->exp_reject) ||
	    g_tr.ctrl != (unsigned)(v->exp_ctrl ? 1 : 0) ||
	    (v->exp_ctrl && ret_s != v->exp_ctrl) ||
	    g_tr.get_size != (unsigned)(v->exp_size ? 1 : 0) ||
	    (v->exp_size && ret_u != (u32)v->exp_size) ||
	    g_tr.manual != (unsigned)v->exp_manual ||
	    (v->exp_manual && g_tr.last_manual != (u8)v->exp_manual_val) ||
	    g_tr.policy != (unsigned)v->exp_policy ||
	    (v->exp_policy && g_tr.last_policy != (u8)v->exp_policy_val) ||
	    g_tr.disabled != (unsigned)(v->exp_disabled ? 1 : 0) ||
	    (v->exp_disabled && ret_u8 != (u8)v->exp_disabled) ||
	    g_tr.coexist != (unsigned)v->exp_coexist ||
	    (v->exp_coexist && g_tr.last_coexist != (u8)v->exp_coexist_val)) {
		fprintf(stderr, "FAIL %s\n", v->name);
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector v[16];
	size_t n = 0;
	int bad = 0;
	const char *p = argc > 1 ? argv[1] : "btcoex_handler_policy_vectors.json";

	if (host_load_vectors(p, v, sizeof(v[0]), 16, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&v[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, p);
	return bad ? 1 : 0;
}
