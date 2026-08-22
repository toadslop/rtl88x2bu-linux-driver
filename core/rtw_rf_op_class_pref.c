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
#define _RTW_RF_OP_CLASS_PREF_C_

#ifdef HOST_RF_OP_CLASS_PREF_TEST
#include "host_rf_op_class_pref_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

static struct op_class_pref_t *opc_pref_alloc(u8 class_id)
{
	int i, j;
	struct op_class_pref_t *opc_pref = NULL;

	for (i = 0; i < global_op_class_num; i++)
		if (global_op_class[i].class_id == class_id)
			break;

	if (i >= global_op_class_num)
		goto exit;

	opc_pref = rtw_zmalloc(sizeof(*opc_pref));
	if (!opc_pref)
		goto exit;

	opc_pref->class_id = global_op_class[i].class_id;
	opc_pref->band = global_op_class[i].band;
	opc_pref->bw = global_op_class[i].bw;

	for (j = 0; j < OPC_CH_LIST_LEN(global_op_class[i]); j++) {
		opc_pref->chs[j].ch = OPC_CH_LIST_CH(global_op_class[i], j);
		opc_pref->chs[j].static_non_op = 1;
		opc_pref->chs[j].no_ir = 1;
		opc_pref->chs[j].max_txpwr = UNSPECIFIED_MBM;
	}
	opc_pref->ch_num = OPC_CH_LIST_LEN(global_op_class[i]);

exit:
	return opc_pref;
}

static void opc_pref_free(struct op_class_pref_t *opc_pref)
{
	rtw_mfree(opc_pref, sizeof(*opc_pref));
}

#ifdef CONFIG_RTW_DEBUG
static bool dbg_global_op_class_validate(u8 gid)
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

int op_class_pref_init(_adapter *adapter)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	struct registry_priv *regsty = adapter_to_regsty(adapter);
	u8 bw;
	struct op_class_pref_t *opc_pref;
	int i;
	u8 op_class_num = 0;
	u8 band_bmp = 0;
	u8 bw_bmp[BAND_MAX] = {0};
	int ret = _FAIL;

	rfctl->spt_op_class_ch = rtw_zmalloc(sizeof(struct op_class_pref_t *) * global_op_class_num);
	if (!rfctl->spt_op_class_ch) {
		RTW_ERR("%s alloc rfctl->spt_op_class_ch fail\n", __func__);
		goto exit;
	}

	if (IsSupported24G(regsty->wireless_mode) && hal_chk_band_cap(adapter, BAND_CAP_2G))
		band_bmp |= BAND_CAP_2G;
	if (is_supported_5g(regsty->wireless_mode) && hal_chk_band_cap(adapter, BAND_CAP_5G))
		band_bmp |= BAND_CAP_5G;

	bw_bmp[BAND_ON_2_4G] = (ch_width_to_bw_cap(REGSTY_BW_2G(regsty) + 1) - 1) & (GET_HAL_SPEC(adapter)->bw_cap);
	bw_bmp[BAND_ON_5G] = (ch_width_to_bw_cap(REGSTY_BW_5G(regsty) + 1) - 1) & (GET_HAL_SPEC(adapter)->bw_cap);
	if (!REGSTY_IS_11AC_ENABLE(regsty)
		|| !is_supported_vht(regsty->wireless_mode)
	)
		bw_bmp[BAND_ON_5G] &= ~(BW_CAP_80M | BW_CAP_160M);

	if (0) {
		RTW_INFO("REGSTY_BW_2G(regsty):%u\n", REGSTY_BW_2G(regsty));
		RTW_INFO("REGSTY_BW_5G(regsty):%u\n", REGSTY_BW_5G(regsty));
		RTW_INFO("GET_HAL_SPEC(adapter)->bw_cap:0x%x\n", GET_HAL_SPEC(adapter)->bw_cap);
		RTW_INFO("band_bmp:0x%x\n", band_bmp);
		RTW_INFO("bw_bmp[2G]:0x%x\n", bw_bmp[BAND_ON_2_4G]);
		RTW_INFO("bw_bmp[5G]:0x%x\n", bw_bmp[BAND_ON_5G]);
	}

	for (i = 0; i < global_op_class_num; i++) {
		#ifdef CONFIG_RTW_DEBUG
		rtw_warn_on(!dbg_global_op_class_validate(i));
		#endif

		if (!(band_bmp & band_to_band_cap(global_op_class[i].band)))
			continue;

		bw = opc_bw_to_ch_width(global_op_class[i].bw);
		if (bw == CHANNEL_WIDTH_MAX
			|| bw == CHANNEL_WIDTH_80_80 /* TODO */
		)
			continue;

		if (!(bw_bmp[global_op_class[i].band] & ch_width_to_bw_cap(bw)))
			continue;

		opc_pref = opc_pref_alloc(global_op_class[i].class_id);
		if (!opc_pref) {
			RTW_ERR("%s opc_pref_alloc(%u) fail\n", __func__, global_op_class[i].class_id);
			goto exit;
		}

		if (opc_pref->ch_num) {
			rfctl->spt_op_class_ch[i] = opc_pref;
			op_class_num++;
		} else
			opc_pref_free(opc_pref);
	}

	rfctl->cap_spt_op_class_num = op_class_num;
	ret = _SUCCESS;

exit:
	return ret;
}

void op_class_pref_deinit(_adapter *adapter)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	int i;

	if (!rfctl->spt_op_class_ch)
		return;

	for (i = 0; i < global_op_class_num; i++) {
		if (rfctl->spt_op_class_ch[i]) {
			opc_pref_free(rfctl->spt_op_class_ch[i]);
			rfctl->spt_op_class_ch[i] = NULL;
		}
	}

	rtw_mfree(rfctl->spt_op_class_ch, sizeof(struct op_class_pref_t *) * global_op_class_num);
	rfctl->spt_op_class_ch = NULL;
}

void op_class_pref_apply_regulatory(_adapter *adapter, u8 reason)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	RT_CHANNEL_INFO *chset = rfctl->channel_set;
	u8 ch, bw, offset, cch;
	struct op_class_pref_t *opc_pref;
	int i, j;
	u8 reg_op_class_num = 0;
	u8 op_class_num = 0;

	for (i = 0; i < global_op_class_num; i++) {
		if (!rfctl->spt_op_class_ch[i])
			continue;
		opc_pref = rfctl->spt_op_class_ch[i];

		/* reset all channel */
		for (j = 0; opc_pref->chs[j].ch != 0; j++) {
			if (reason >= REG_CHANGE)
				opc_pref->chs[j].static_non_op = 1;
			if (reason != REG_TXPWR_CHANGE)
				opc_pref->chs[j].no_ir = 1;
			if (reason >= REG_TXPWR_CHANGE)
				opc_pref->chs[j].max_txpwr = UNSPECIFIED_MBM;
		}
		if (reason >= REG_CHANGE)
			opc_pref->op_ch_num = 0;
		if (reason != REG_TXPWR_CHANGE)
			opc_pref->ir_ch_num = 0;

		switch (opc_pref->bw) {
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
			continue;
		}

		if (rfctl->country_ent && !COUNTRY_CHPLAN_EN_11AC(rfctl->country_ent)
			&& (bw == CHANNEL_WIDTH_80 || bw == CHANNEL_WIDTH_160))
			continue;

		for (j = 0; opc_pref->chs[j].ch != 0; j++) {
			u8 *op_chs;
			u8 op_ch_num;
			u8 k, l;
			int chset_idx;

			ch = opc_pref->chs[j].ch;

			if (reason >= REG_TXPWR_CHANGE)
				opc_pref->chs[j].max_txpwr = rtw_rfctl_get_reg_max_txpwr_mbm(rfctl, ch, bw, offset, 1);

			if (reason == REG_TXPWR_CHANGE)
				continue;

			cch = rtw_get_center_ch(ch ,bw, offset);
			if (!cch)
				continue;

			if (!rtw_get_op_chs_by_cch_bw(cch, bw, &op_chs, &op_ch_num))
				continue;

			for (k = 0, l = 0; k < op_ch_num; k++) {
				chset_idx = rtw_chset_search_ch(chset, *(op_chs + k));
				if (chset_idx == -1)
					break;
				if (bw >= CHANNEL_WIDTH_40) {
					if ((chset[chset_idx].flags & RTW_CHF_NO_HT40U) && k % 2 == 0)
						break;
					if ((chset[chset_idx].flags & RTW_CHF_NO_HT40L) && k % 2 == 1)
						break;
				}
				if (bw >= CHANNEL_WIDTH_80 && (chset[chset_idx].flags & RTW_CHF_NO_80MHZ))
					break;
				if (bw >= CHANNEL_WIDTH_160 && (chset[chset_idx].flags & RTW_CHF_NO_160MHZ))
					break;
				if ((chset[chset_idx].flags & RTW_CHF_DFS) && rtw_rfctl_dfs_domain_unknown(rfctl))
					continue;
				if (chset[chset_idx].flags & RTW_CHF_NO_IR)
					continue;
				l++;
			}
			if (k < op_ch_num)
				continue;

			if (reason >= REG_CHANGE) {
				opc_pref->chs[j].static_non_op = 0;
				opc_pref->op_ch_num++;
			}

			if (l >= op_ch_num) {
				opc_pref->chs[j].no_ir = 0;
				opc_pref->ir_ch_num++;
			}
		}

		if (opc_pref->op_ch_num)
			reg_op_class_num++;
		if (opc_pref->ir_ch_num)
			op_class_num++;
	}

	rfctl->reg_spt_op_class_num = reg_op_class_num;
	rfctl->cur_spt_op_class_num = op_class_num;
}
