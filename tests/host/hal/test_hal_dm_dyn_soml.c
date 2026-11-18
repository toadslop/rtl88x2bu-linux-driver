// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_hal_dm_dyn_soml_types.h"

struct case_vec {
	const char *name;
	const char *op;
	u8 dyn_en, train, intvl, period, delay, rate;
	u32 size;
	u8 x_bytes, x_para, x_hw, x_err;
	u8 x_rate;
	u32 x_len;
};

static const struct case_vec cases[] = {
	{"byte_update", "byte_update", 0, 0, 0, 0, 0, 12, 1500, 1, 0, 0, 0, 12, 1500},
	{"para_set", "para_set", 0, 4, 20, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
	{"config_en0", "config", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{"config_en1", "config", 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{"config_en2", "config", 2, 3, 10, 2, 5, 0, 0, 0, 1, 0, 0, 0, 0},
	{"config_bad", "config", 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
};

static int run_case(const struct case_vec *v)
{
	_adapter a;
	struct host_hal_dm_dyn_soml_trace *t = &host_hal_dm_dyn_soml_trace;

	host_hal_dm_dyn_soml_reset();
	memset(&a, 0, sizeof(a));
	if (!strcmp(v->op, "byte_update"))
		rtw_dyn_soml_byte_update(&a, v->rate, v->size);
	else if (!strcmp(v->op, "para_set"))
		rtw_dyn_soml_para_set(&a, v->train, v->intvl, v->period, v->delay);
	else {
		a.registrypriv.dyn_soml_en = v->dyn_en;
		a.registrypriv.dyn_soml_train_num = v->train;
		a.registrypriv.dyn_soml_interval = v->intvl;
		a.registrypriv.dyn_soml_period = v->period;
		a.registrypriv.dyn_soml_delay = v->delay;
		rtw_dyn_soml_config(&a);
	}
	if (t->bytes_acq_calls != v->x_bytes || t->para_set_calls != v->x_para ||
	    t->set_hwreg_calls != v->x_hw || !!t->err_logged != !!v->x_err)
		return -1;
	if (v->x_bytes && (t->bytes_acq_rate != v->x_rate || t->bytes_acq_length != v->x_len))
		return -1;
	if (v->x_para && (t->para_train != v->train || t->para_intvl != v->intvl ||
			  t->para_period != v->period || t->para_delay != v->delay))
		return -1;
	if (v->x_hw && t->set_hwreg_var != HOST_HW_VAR_SET_SOML_PARAM)
		return -1;
	printf("PASS %s\n", v->name);
	return 0;
}

int main(void)
{
	size_t i;

	for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
		if (run_case(&cases[i]))
			return fprintf(stderr, "FAIL %s\n", cases[i].name), 1;
	return 0;
}