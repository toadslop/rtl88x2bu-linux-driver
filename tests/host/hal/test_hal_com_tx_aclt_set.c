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

int main(void)
{
	struct dvobj_priv ds, do_;
	struct _adapter s, o;
	struct tx_aclt_conf_t in;

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

	in.vo_vi = 128;
	in.be_bk = 256;
	rtw_hal_set_tx_aclt_force_val(&s, &in, 3);
	rtw_hal_set_tx_aclt_force_val_c_oracle(&o, &in, 3);
	eq8("scale", s.dvobj.tx_aclt_force_val.vo_vi, o.dvobj.tx_aclt_force_val.vo_vi);

	if (failures)
		return 1;
	puts("OK hal_com_tx_aclt_set L2");
	return 0;
}