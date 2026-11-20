// SPDX-License-Identifier: GPL-2.0
/* L2 tests for W4-124 TX AC lifetime setters (force/conf; update in PR2). */

#include "host_hal_com_tx_aclt_set_types.h"
#include <stdio.h>
#include <string.h>

void rtw_hal_set_tx_aclt_force_val_c_oracle(struct _adapter *, struct tx_aclt_conf_t *, u8);
void rtw_hal_set_tx_aclt_conf_c_oracle(struct _adapter *, u8, struct tx_aclt_conf_t *, u8);
void rtw_hal_set_tx_aclt_force_val(struct _adapter *, struct tx_aclt_conf_t *, u8);
void rtw_hal_set_tx_aclt_conf(struct _adapter *, u8, struct tx_aclt_conf_t *, u8);

static int failures;

static struct _adapter mkadap(struct dvobj_priv *dv)
{
	struct _adapter a = { 0 };

	a.dvobj = *dv;
	a.hal.hal_spec.tx_aclt_unit_factor = 2;
	return a;
}

static void eq8(const char *t, u8 a, u8 b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s a=0x%02x b=0x%02x\n", t, a, b);
		failures++;
	}
}

static void eq32(const char *t, u32 a, u32 b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s a=%u b=%u\n", t, a, b);
		failures++;
	}
}

int main(void)
{
	struct dvobj_priv ds, do_;
	struct _adapter s, o;
	struct tx_aclt_conf_t in;
	int i;

	memset(&ds, 0, sizeof(ds));
	memset(&do_, 0, sizeof(do_));
	host_tx_aclt_reset();
	s = mkadap(&ds);
	o = mkadap(&do_);

	memset(&in, 0, sizeof(in));
	in.en = 0xFF;
	rtw_hal_set_tx_aclt_force_val(&s, &in, 1);
	rtw_hal_set_tx_aclt_force_val_c_oracle(&o, &in, 1);
	eq8("force_ff", s.dvobj.tx_aclt_force_val.en, 0xFF);

	in.en = 0x1A;
	rtw_hal_set_tx_aclt_force_val(&s, &in, 1);
	rtw_hal_set_tx_aclt_force_val_c_oracle(&o, &in, 1);
	eq8("force_mask", s.dvobj.tx_aclt_force_val.en, 0x0A);

	ds.tx_aclt_confs[0].en = 0x5;
	do_.tx_aclt_confs[0].en = 0x5;
	s.dvobj = ds;
	o.dvobj = do_;
	in.en = 0xFF;
	rtw_hal_set_tx_aclt_conf(&s, 0, &in, 1);
	rtw_hal_set_tx_aclt_conf_c_oracle(&o, 0, &in, 1);
	eq8("conf_ff", s.dvobj.tx_aclt_confs[0].en, 0x5);

	in.en = 0x17;
	rtw_hal_set_tx_aclt_conf(&s, 0, &in, 1);
	rtw_hal_set_tx_aclt_conf_c_oracle(&o, 0, &in, 1);
	eq8("conf_mask", s.dvobj.tx_aclt_confs[0].en, o.dvobj.tx_aclt_confs[0].en);
	eq8("conf_mask_exp", s.dvobj.tx_aclt_confs[0].en, 0x07);

	in.vo_vi = 128;
	in.be_bk = 256;
	rtw_hal_set_tx_aclt_force_val(&s, &in, 3);
	rtw_hal_set_tx_aclt_force_val_c_oracle(&o, &in, 3);
	eq32("scale_vo_vi", s.dvobj.tx_aclt_force_val.vo_vi,
	     o.dvobj.tx_aclt_force_val.vo_vi);
	eq32("scale_be_bk", s.dvobj.tx_aclt_force_val.be_bk,
	     o.dvobj.tx_aclt_force_val.be_bk);
	eq32("scale_vo_vi_exp", s.dvobj.tx_aclt_force_val.vo_vi, 2u);
	eq32("scale_be_bk_exp", s.dvobj.tx_aclt_force_val.be_bk, 4u);

	memset(&ds, 0, sizeof(ds));
	memset(&do_, 0, sizeof(do_));
	s = mkadap(&ds);
	o = mkadap(&do_);
	ds.tx_aclt_force_val.be_bk = 42;
	s.dvobj = ds;
	o.dvobj = do_;
	memset(&in, 0, sizeof(in));
	in.vo_vi = 64;
	rtw_hal_set_tx_aclt_force_val(&s, &in, 2);
	rtw_hal_set_tx_aclt_force_val_c_oracle(&o, &in, 2);
	eq32("arg2_vo_vi", s.dvobj.tx_aclt_force_val.vo_vi,
	     o.dvobj.tx_aclt_force_val.vo_vi);
	eq32("arg2_be_bk_unchanged", s.dvobj.tx_aclt_force_val.be_bk, 42u);

	memset(&in, 0, sizeof(in));
	in.vo_vi = 0x01000000u;
	rtw_hal_set_tx_aclt_force_val(&s, &in, 2);
	rtw_hal_set_tx_aclt_force_val_c_oracle(&o, &in, 2);
	eq32("clamp_vo_vi", s.dvobj.tx_aclt_force_val.vo_vi, 0xFFFFu);

	for (i = 0; i < TX_ACLT_CONF_NUM; i++) {
		ds.tx_aclt_confs[i].en = (u8)(0x11 + i);
		ds.tx_aclt_confs[i].vo_vi = 100u + i;
		ds.tx_aclt_confs[i].be_bk = 200u + i;
		do_.tx_aclt_confs[i] = ds.tx_aclt_confs[i];
	}
	ds.oob_canary = 0xA5;
	do_.oob_canary = 0xA5;
	s.dvobj = ds;
	o.dvobj = do_;
	memset(&in, 0, sizeof(in));
	in.en = 0x0F;
	in.vo_vi = 0x01000000u;
	rtw_hal_set_tx_aclt_conf(&s, TX_ACLT_CONF_NUM, &in, 3);
	rtw_hal_set_tx_aclt_conf_c_oracle(&o, TX_ACLT_CONF_NUM, &in, 3);
	eq8("conf_oob_canary", s.dvobj.oob_canary, 0xA5);
	eq8("conf_oob_canary_o", o.dvobj.oob_canary, 0xA5);
	for (i = 0; i < TX_ACLT_CONF_NUM; i++) {
		char tag[48];

		snprintf(tag, sizeof(tag), "conf_oob_%d", i);
		eq8(tag, s.dvobj.tx_aclt_confs[i].en, ds.tx_aclt_confs[i].en);
		snprintf(tag, sizeof(tag), "conf_oob_vo_%d", i);
		eq32(tag, s.dvobj.tx_aclt_confs[i].vo_vi, ds.tx_aclt_confs[i].vo_vi);
		snprintf(tag, sizeof(tag), "conf_oob_be_%d", i);
		eq32(tag, s.dvobj.tx_aclt_confs[i].be_bk, ds.tx_aclt_confs[i].be_bk);
	}

	memset(&ds, 0, sizeof(ds));
	memset(&do_, 0, sizeof(do_));
	s = mkadap(&ds);
	o = mkadap(&do_);
	ds.tx_aclt_confs[0].be_bk = 99;
	s.dvobj = ds;
	o.dvobj = do_;
	memset(&in, 0, sizeof(in));
	in.vo_vi = 128;
	rtw_hal_set_tx_aclt_conf(&s, 0, &in, 2);
	rtw_hal_set_tx_aclt_conf_c_oracle(&o, 0, &in, 2);
	eq32("conf_arg2_vo", s.dvobj.tx_aclt_confs[0].vo_vi, 2u);
	eq32("conf_arg2_be", s.dvobj.tx_aclt_confs[0].be_bk, 99u);

	memset(&in, 0, sizeof(in));
	in.en = 0x0A;
	in.be_bk = 0x01000000u;
	rtw_hal_set_tx_aclt_conf(&s, 0, &in, 3);
	rtw_hal_set_tx_aclt_conf_c_oracle(&o, 0, &in, 3);
	eq32("conf_clamp_be", s.dvobj.tx_aclt_confs[0].be_bk, 0xFFFFu);

	if (failures)
		return 1;
	puts("OK hal_com_tx_aclt_set L2");
	return 0;
}