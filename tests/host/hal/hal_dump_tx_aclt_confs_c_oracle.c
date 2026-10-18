// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-40 dump_tx_aclt_confs (from hal/hal_com_rest.c). */

#include <stdio.h>
#include <string.h>

#include "host_hal_com_tx_aclt_confs_types.h"

#define TX_ACLT_CONF_MSG_LEN 32
#define TX_ACLT_CONF_NUM HOST_TX_ACLT_CONF_NUM

static const char *const tx_aclt_conf_str_table[] = {
	"DEFAULT",
	"AP_M2U",
	"MESH",
	"INVALID",
};

static const char *tx_aclt_conf_str(int conf)
{
	if (conf >= TX_ACLT_CONF_NUM)
		return tx_aclt_conf_str_table[TX_ACLT_CONF_NUM];
	return tx_aclt_conf_str_table[conf];
}

int dump_tx_aclt_confs_format(const host_hal_com_tx_aclt_confs_fixture *fx,
			      char *buf, size_t buflen)
{
	uint8_t unit_factor;
	uint32_t unit_us;
	uint32_t max_us;
	int pos = 0;
	int n;
	int i;

	if (!fx || !buf || buflen == 0)
		return -1;

	unit_factor = fx->tx_aclt_unit_factor;
	unit_us = (uint32_t)unit_factor * 32u;
	max_us = 0xFFFFu * unit_factor * 32u;

	n = snprintf(buf + pos, buflen - (size_t)pos,
		     "unit:%uus, maximum:%uus\n", unit_us, max_us);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos,
		     "%-7s %-1s %-3s %-9s %-9s %-10s %-10s\n", "name", "#",
		     "en", "vo_vi(us)", "be_bk(us)", "vo_vi(reg)", "be_bk(reg)");
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	for (i = 0; i < TX_ACLT_CONF_NUM; i++) {
		const host_hal_com_tx_aclt_conf_row *conf = &fx->confs[i];
		char row_msg[TX_ACLT_CONF_MSG_LEN];
		int cnt = 0;

		if (conf->vo_vi)
			cnt += snprintf(row_msg + cnt, TX_ACLT_CONF_MSG_LEN - cnt - 1,
					"     0x%04x", (unsigned)conf->vo_vi);
		else
			cnt += snprintf(row_msg + cnt, TX_ACLT_CONF_MSG_LEN - cnt - 1,
					"        N/A");
		if (cnt >= TX_ACLT_CONF_MSG_LEN - 1)
			continue;

		if (conf->be_bk)
			cnt += snprintf(row_msg + cnt, TX_ACLT_CONF_MSG_LEN - cnt - 1,
					"     0x%04x", (unsigned)conf->be_bk);
		else
			cnt += snprintf(row_msg + cnt, TX_ACLT_CONF_MSG_LEN - cnt - 1,
					"        N/A");
		if (cnt >= TX_ACLT_CONF_MSG_LEN - 1)
			continue;

		n = snprintf(buf + pos, buflen - (size_t)pos,
			     "%7s %1u 0x%x %9u %9u%s\n", tx_aclt_conf_str(i), i,
			     conf->en, conf->vo_vi * unit_factor * 32u,
			     conf->be_bk * unit_factor * 32u, row_msg);
		if (n < 0 || (size_t)(pos + n) >= buflen)
			return -1;
		pos += n;
	}
	return pos;
}
