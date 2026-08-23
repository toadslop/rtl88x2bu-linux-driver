/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _RTW_RF_OP_CLASS_DUMP_C_

#ifdef HOST_RF_OP_CLASS_DUMP_TEST
#include "host_rf_op_class_dump_types.h"
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_RF_OP_CLASS_DUMP_TEST) || !defined(CONFIG_RUST_RF_OP_CLASS_DUMP)

#ifdef CONFIG_RTW_DEBUG
bool dbg_global_op_class_validate(u8 gid)
{
	u8 i;
	u8 ch, bw, offset, cch;
	bool ret = 1;

	switch (global_op_class[gid].bw) {
	case OPC_BW20:
		bw = CHANNEL_WIDTH_20;
		offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
		break;
	case OPC_BW40PLUS:
		bw = CHANNEL_WIDTH_40;
		offset = HAL_PRIME_CHNL_OFFSET_LOWER;
		break;
	case OPC_BW40MINUS:
		bw = CHANNEL_WIDTH_40;
		offset = HAL_PRIME_CHNL_OFFSET_UPPER;
		break;
	case OPC_BW80:
		bw = CHANNEL_WIDTH_80;
		offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
		break;
	case OPC_BW160:
		bw = CHANNEL_WIDTH_160;
		offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
		break;
	case OPC_BW80P80: /* TODO */
	default:
		RTW_ERR("%s class:%u unsupported opc_bw:%u\n"
			, __func__, global_op_class[gid].class_id, global_op_class[gid].bw);
		ret = 0;
		goto exit;
	}

	for (i = 0; i < OPC_CH_LIST_LEN(global_op_class[gid]); i++) {
		u8 *op_chs;
		u8 op_ch_num;
		u8 k;

		ch = OPC_CH_LIST_CH(global_op_class[gid], i);
		cch = rtw_get_center_ch(ch ,bw, offset);
		if (!cch) {
			RTW_ERR("%s can't get cch from class:%u ch:%u\n"
				, __func__, global_op_class[gid].class_id, ch);
			ret = 0;
			continue;
		}

		if (!rtw_get_op_chs_by_cch_bw(cch, bw, &op_chs, &op_ch_num)) {
			RTW_ERR("%s can't get op chs from class:%u cch:%u\n"
				, __func__, global_op_class[gid].class_id, cch);
			ret = 0;
			continue;
		}

		for (k = 0; k < op_ch_num; k++) {
			if (*(op_chs + k) == ch)
				break;
		}
		if (k >= op_ch_num) {
			RTW_ERR("%s can't get ch:%u from op_chs class:%u cch:%u\n"
				, __func__, ch, global_op_class[i].class_id, cch);
			ret = 0;
		}
	}

exit:
	return ret;
}
#endif /* CONFIG_RTW_DEBUG */

static void dump_op_class_ch_title(void *sel)
{
	RTW_PRINT_SEL(sel, "%-5s %-4s %-7s ch_list\n"
		, "class", "band", "bw");
}

static void dump_global_op_class_ch_single(void *sel, u8 gid)
{
	u8 i;
	char buf[100];
	char *pos = buf;

	for (i = 0; i < OPC_CH_LIST_LEN(global_op_class[gid]); i++)
		pos += snprintf(pos, 100 - (pos - buf), " %u", OPC_CH_LIST_CH(global_op_class[gid], i));

	RTW_PRINT_SEL(sel, "%5u %4s %7s%s\n"
		, global_op_class[gid].class_id
		, band_str(global_op_class[gid].band)
		, opc_bw_str(global_op_class[gid].bw), buf);
}

void dump_global_op_class(void *sel)
{
	u8 i;

	dump_op_class_ch_title(sel);

	for (i = 0; i < global_op_class_num; i++)
		dump_global_op_class_ch_single(sel, i);
}

static void dump_opc_pref_single(void *sel, struct op_class_pref_t *opc_pref, bool show_snon_ocp, bool show_no_ir, bool detail)
{
	u8 i;
	char buf[256];
	char *pos = buf;

	if (!show_snon_ocp && !opc_pref->op_ch_num)
		return;
	if (!show_no_ir && !opc_pref->ir_ch_num)
		return;

	for (i = 0; opc_pref->chs[i].ch != 0; i++) {
		if ((show_snon_ocp || !opc_pref->chs[i].static_non_op)
			&& (show_no_ir || !opc_pref->chs[i].no_ir)
		) {
			if (detail)
				pos += snprintf(pos, 256 - (pos - buf), " %4u", opc_pref->chs[i].ch);
			else
				pos += snprintf(pos, 256 - (pos - buf), " %u", opc_pref->chs[i].ch);
		}
	}

	RTW_PRINT_SEL(sel, "%5u %4s %7s%s\n"
		, opc_pref->class_id
		, band_str(opc_pref->band)
		, opc_bw_str(opc_pref->bw), buf);

	if (!detail)
		return;

	pos = buf;
	for (i = 0; opc_pref->chs[i].ch != 0; i++) {
		if ((show_snon_ocp || !opc_pref->chs[i].static_non_op)
			&& (show_no_ir || !opc_pref->chs[i].no_ir)
		) {
			pos += snprintf(pos, 256 - (pos - buf), "   %c%c"
				, opc_pref->chs[i].no_ir ? ' ' : 'I'
				, opc_pref->chs[i].static_non_op ? ' ' : 'E'
			);
		}
	}
	RTW_PRINT_SEL(sel, "                  %s\n", buf);

	pos = buf;
	for (i = 0; opc_pref->chs[i].ch != 0; i++) {
		if ((show_snon_ocp || !opc_pref->chs[i].static_non_op)
			&& (show_no_ir || !opc_pref->chs[i].no_ir)
		) {
			if (opc_pref->chs[i].max_txpwr == UNSPECIFIED_MBM)
				pos += snprintf(pos, 256 - (pos - buf), "     ");
			else
				pos += snprintf(pos, 256 - (pos - buf), " %4d", opc_pref->chs[i].max_txpwr);
		}
	}
	RTW_PRINT_SEL(sel, "                  %s\n", buf);
}

void dump_cap_spt_op_class_ch(void *sel, struct rf_ctl_t *rfctl, bool detail)
{
	u8 i;

	dump_op_class_ch_title(sel);

	for (i = 0; i < global_op_class_num; i++) {
		if (!rfctl->spt_op_class_ch[i])
			continue;
		dump_opc_pref_single(sel, rfctl->spt_op_class_ch[i], 1, 1, detail);
	}

	RTW_PRINT_SEL(sel, "op_class number:%d\n", rfctl->cap_spt_op_class_num);
}

void dump_reg_spt_op_class_ch(void *sel, struct rf_ctl_t *rfctl, bool detail)
{
	u8 i;

	dump_op_class_ch_title(sel);

	for (i = 0; i < global_op_class_num; i++) {
		if (!rfctl->spt_op_class_ch[i])
			continue;
		dump_opc_pref_single(sel, rfctl->spt_op_class_ch[i], 0, 1, detail);
	}

	RTW_PRINT_SEL(sel, "op_class number:%d\n", rfctl->reg_spt_op_class_num);
}

void dump_cur_spt_op_class_ch(void *sel, struct rf_ctl_t *rfctl, bool detail)
{
	u8 i;

	dump_op_class_ch_title(sel);

	for (i = 0; i < global_op_class_num; i++) {
		if (!rfctl->spt_op_class_ch[i])
			continue;
		dump_opc_pref_single(sel, rfctl->spt_op_class_ch[i], 0, 0, detail);
	}

	RTW_PRINT_SEL(sel, "op_class number:%d\n", rfctl->cur_spt_op_class_num);
}

#endif /* !CONFIG_RUST || HOST_RF_OP_CLASS_DUMP_TEST || !CONFIG_RUST_RF_OP_CLASS_DUMP */
