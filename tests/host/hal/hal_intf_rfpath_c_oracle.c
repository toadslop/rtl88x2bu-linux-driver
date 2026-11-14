// SPDX-License-Identifier: GPL-2.0
/* Host L2 C oracle for rtw_hal_rfpath_init common path (W4-65). */

#include <stddef.h>

#include "host_hal_intf_rfpath_types.h"
#include "host_rf_types.h"

#define RTW_FAIL 0
#define RTW_SUCCESS 1

#define RFPATH_FAIL_NONE 0
#define RFPATH_FAIL_REG_INTERSECT 1
#define RFPATH_FAIL_EEPROM_INTERSECT 2
#define RFPATH_FAIL_RESTRICT 3

static u8 rtw_min_u8(u8 a, u8 b)
{
	return a > b ? b : a;
}

static u8 rtw_max_u8(u8 a, u8 b)
{
	return a > b ? a : b;
}

u8 host_hal_rfpath_init_common(const struct host_hal_rfpath_init_in *in,
			       struct host_hal_rfpath_init_out *out)
{
	u8 trx_path_bmp;
	u8 tx_path_num = 0;
	u8 rx_path_num = 0;

	if (!in || !out)
		return RTW_FAIL;

	out->fail_reason = RFPATH_FAIL_NONE;
	trx_path_bmp = in->rf_reg_trx_path_bmp;

	if (in->reg_trx_path_bmp != 0) {
		trx_path_bmp &= in->reg_trx_path_bmp;
		if (!trx_path_bmp) {
			out->fail_reason = RFPATH_FAIL_REG_INTERSECT;
			out->status = RTW_FAIL;
			return RTW_FAIL;
		}
	} else if (in->eeprom_trx_path_bmp != 0) {
		trx_path_bmp &= in->eeprom_trx_path_bmp;
		if (!trx_path_bmp) {
			out->fail_reason = RFPATH_FAIL_EEPROM_INTERSECT;
			out->status = RTW_FAIL;
			return RTW_FAIL;
		}
	}

	trx_path_bmp = rtw_restrict_trx_path_bmp_by_trx_num_lmt(
		trx_path_bmp, in->tx_path_lmt, in->rx_path_lmt, &tx_path_num,
		&rx_path_num);
	if (!trx_path_bmp) {
		out->fail_reason = RFPATH_FAIL_RESTRICT;
		out->status = RTW_FAIL;
		return RTW_FAIL;
	}

	out->hal_trx_path_bmp = trx_path_bmp;
	out->hal_rf_type = (u8)trx_bmp_to_rf_type((trx_path_bmp & 0xF0) >> 4,
						  trx_path_bmp & 0x0F);
	out->NumTotalRFPath = rtw_max_u8(tx_path_num, rx_path_num);
	out->hal_max_tx_cnt = in->hal_spec_max_tx_cnt;
	out->hal_max_tx_cnt = rtw_min_u8(out->hal_max_tx_cnt, tx_path_num);
	if (in->eeprom_max_tx_cnt)
		out->hal_max_tx_cnt = rtw_min_u8(out->hal_max_tx_cnt,
						 in->eeprom_max_tx_cnt);
	out->status = RTW_SUCCESS;
	return RTW_SUCCESS;
}
