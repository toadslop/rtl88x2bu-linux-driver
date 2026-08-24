// SPDX-License-Identifier: GPL-2.0
/* Host fixtures for W3-57 op-class dump L2 oracles. */

#include <stdlib.h>
#include <string.h>

#include "host_rf_op_class_dump_types.h"

struct host_sel_capture host_sel_out;

void *rtw_zmalloc(u32 sz)
{
	return calloc(1, sz);
}

void rtw_mfree(void *p, u32 sz)
{
	(void)sz;
	free(p);
}

void host_rf_op_class_dump_reset(struct rf_ctl_t *rfctl)
{
	struct op_class_pref_t **spt = rfctl->spt_op_class_ch;
	int i;

	if (spt) {
		for (i = 0; i < global_op_class_num; i++) {
			if (spt[i])
				rtw_mfree(spt[i], sizeof(struct op_class_pref_t));
		}
		rtw_mfree(spt, sizeof(struct op_class_pref_t *) * global_op_class_num);
	}
	memset(rfctl, 0, sizeof(*rfctl));
}

struct op_class_pref_t *host_rf_op_class_dump_add_pref(struct rf_ctl_t *rfctl,
						       u8 class_id, u8 op_ch_num,
						       u8 ir_ch_num)
{
	struct op_class_pref_t *pref;
	int i, j, slot = -1;

	if (!rfctl->spt_op_class_ch) {
		rfctl->spt_op_class_ch = rtw_zmalloc(sizeof(struct op_class_pref_t *)
						    * global_op_class_num);
		if (!rfctl->spt_op_class_ch)
			return NULL;
	}

	for (i = 0; i < global_op_class_num; i++) {
		if (global_op_class[i].class_id == class_id) {
			slot = i;
			break;
		}
	}
	if (slot < 0)
		return NULL;

	pref = rtw_zmalloc(sizeof(*pref));
	if (!pref)
		return NULL;

	pref->class_id = global_op_class[slot].class_id;
	pref->band = global_op_class[slot].band;
	pref->bw = global_op_class[slot].bw;
	pref->op_ch_num = op_ch_num;
	pref->ir_ch_num = ir_ch_num;

	for (j = 0; j < OPC_CH_LIST_LEN(global_op_class[slot]); j++) {
		pref->chs[j].ch = OPC_CH_LIST_CH(global_op_class[slot], j);
		pref->chs[j].static_non_op = (j >= op_ch_num) ? 1 : 0;
		pref->chs[j].no_ir = (j >= ir_ch_num) ? 1 : 0;
		if (j < ir_ch_num)
			pref->chs[j].max_txpwr = 2000;
		else
			pref->chs[j].max_txpwr = UNSPECIFIED_MBM;
	}

	rfctl->spt_op_class_ch[slot] = pref;
	return pref;
}
