// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-08 _dump_rf_path / _dump_trx_nss formatters (hal_intf_rest.c). */

#include <stdio.h>
#include <string.h>

#include "host_hal_intf_rf_nss_types.h"

#define RF_TYPE_MAX 16

static const char *rf_type_to_rfpath_str_local(uint8_t rf_type)
{
	static const char *const tbl[RF_TYPE_MAX] = {
		"RF_1T1R", "RF_1T2R", "RF_2T2R", "RF_2T3R", "RF_2T4R", "RF_3T3R",
		"RF_3T4R", "RF_4T4R", "RF_4T3R", "RF_4T2R", "RF_4T1R", "RF_3T2R",
		"RF_3T1R", "RF_2T1R", "RF_1T4R", "RF_1T3R",
	};

	if (rf_type >= RF_TYPE_MAX)
		return "UNKNOWN";
	return tbl[rf_type];
}

int dump_rf_path_format(const host_hal_intf_rf_path_fixture *fx, char *buf,
			size_t buflen)
{
	int n;

	if (!fx || !buf || buflen == 0)
		return -1;

	n = snprintf(buf, buflen,
		     "[RF_PATH] ver_id.RF_TYPE:%s\n"
		     "[RF_PATH] HALSPEC's rf_reg_trx_path_bmp:0x%02x, rf_reg_path_avail_num:%u, max_tx_cnt:%u\n"
		     "[RF_PATH] PG's trx_path_bmp:0x%02x, max_tx_cnt:%u\n"
		     "[RF_PATH] Registry's trx_path_bmp:0x%02x, tx_path_lmt:%u, rx_path_lmt:%u\n"
		     "[RF_PATH] HALDATA's trx_path_bmp:0x%02x, max_tx_cnt:%u\n"
		     "[RF_PATH] HALDATA's rf_type:%s, NumTotalRFPath:%d\n",
		     rf_type_to_rfpath_str_local(fx->ver_id_rf_type),
		     fx->rf_reg_trx_path_bmp, fx->rf_reg_path_avail_num,
		     fx->hal_spec_max_tx_cnt, fx->eeprom_trx_path_bmp,
		     fx->eeprom_max_tx_cnt, fx->reg_trx_path_bmp,
		     fx->tx_path_lmt, fx->rx_path_lmt, fx->hal_trx_path_bmp,
		     fx->hal_max_tx_cnt,
		     rf_type_to_rfpath_str_local(fx->hal_rf_type),
		     fx->NumTotalRFPath);
	if (n < 0 || (size_t)n >= buflen)
		return -1;
	return n;
}

int dump_trx_nss_format(const host_hal_intf_trx_nss_fixture *fx, char *buf,
			size_t buflen)
{
	int n;

	if (!fx || !buf || buflen == 0)
		return -1;

	n = snprintf(buf, buflen,
		     "[TRX_Nss] HALSPEC - tx_nss:%d, rx_nss:%d\n"
		     "[TRX_Nss] Registry - tx_nss:%d, rx_nss:%d\n"
		     "[TRX_Nss] HALDATA - tx_nss:%d, rx_nss:%d\n",
		     fx->hal_spec_tx_nss, fx->hal_spec_rx_nss, fx->reg_tx_nss,
		     fx->reg_rx_nss, fx->hal_tx_nss, fx->hal_rx_nss);
	if (n < 0 || (size_t)n >= buflen)
		return -1;
	return n;
}
