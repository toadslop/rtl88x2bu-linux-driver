// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-07 dump_hal_trx_mode / dump_hal_runtime_trx_mode formatters. */

#include <stdio.h>
#include <string.h>

#include "host_hal_intf_trx_types.h"

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

static int format_runtime_trx_mode(const host_hal_intf_trx_fixture *fx, char *buf,
				   size_t buflen, int *cnt)
{
	int i;

	*cnt += snprintf(buf + *cnt, buflen - *cnt, "txpath=0x%x, rxpath=0x%x\n",
			 fx->txpath, fx->rxpath);
	if ((size_t)*cnt >= buflen)
		return -1;
	for (i = 0; i < fx->tx_nss; i++) {
		*cnt += snprintf(buf + *cnt, buflen - *cnt,
				 "txpath_%uss:0x%x, num:%u\n", i + 1,
				 fx->txpath_nss[i], fx->txpath_num_nss[i]);
		if ((size_t)*cnt >= buflen)
			return -1;
	}
	return 0;
}

int dump_hal_trx_mode_format(const host_hal_intf_trx_fixture *fx, char *buf,
			     size_t buflen)
{
	int cnt = 0;
	int i;

	if (!fx || !buf || buflen == 0)
		return -1;

	cnt += snprintf(buf + cnt, buflen - cnt,
			"trx_path_bmp:0x%02x(%s), NumTotalRFPath:%u, max_tx_cnt:%u\n",
			fx->trx_path_bmp,
			rf_type_to_rfpath_str_local(fx->rf_type),
			fx->NumTotalRFPath, fx->max_tx_cnt);
	if ((size_t)cnt >= buflen)
		return -1;
	cnt += snprintf(buf + cnt, buflen - cnt, "tx_nss:%u, rx_nss:%u\n",
			fx->tx_nss, fx->rx_nss);
	if ((size_t)cnt >= buflen)
		return -1;
	for (i = 0; i < fx->tx_nss; i++) {
		cnt += snprintf(buf + cnt, buflen - cnt,
				"txpath_cap_num_%uss:%u\n", i + 1,
				fx->txpath_cap_num_nss[i]);
		if ((size_t)cnt >= buflen)
			return -1;
	}
	cnt += snprintf(buf + cnt, buflen - cnt, "\n");
	if ((size_t)cnt >= buflen)
		return -1;
	if (format_runtime_trx_mode(fx, buf, buflen, &cnt) < 0)
		return -1;
	return cnt;
}
