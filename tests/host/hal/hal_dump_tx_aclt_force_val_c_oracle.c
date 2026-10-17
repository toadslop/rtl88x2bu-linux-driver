// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-39 dump_tx_aclt_force_val (from hal/hal_com_rest.c). */

#include <stdio.h>
#include <string.h>

#include "host_hal_com_tx_aclt_force_types.h"

#define TX_ACLT_FORCE_MSG_LEN 64

int dump_tx_aclt_force_val_format(const host_hal_com_tx_aclt_force_fixture *fx,
				  char *buf, size_t buflen)
{
	uint8_t unit_factor;
	uint32_t unit_us;
	uint32_t max_us;
	char msg[TX_ACLT_FORCE_MSG_LEN];
	int cnt = 0;
	int pos = 0;
	int n;

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

	n = snprintf(buf + pos, buflen - (size_t)pos, "%-5s %-12s %-12s\n",
		     "en", "vo_vi(us)", "be_bk(us)");
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos, " 0x%02x %12u %12u\n",
		     fx->en, fx->vo_vi * unit_factor * 32u,
		     fx->be_bk * unit_factor * 32u);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	cnt += snprintf(msg + cnt, TX_ACLT_FORCE_MSG_LEN - cnt - 1, "%5s",
			fx->en == 0xFF ? "AUTO" : "FORCE");
	if (cnt >= TX_ACLT_FORCE_MSG_LEN - 1)
		goto append_msg;

	if (fx->vo_vi)
		cnt += snprintf(msg + cnt, TX_ACLT_FORCE_MSG_LEN - cnt - 1,
				" FORCE:0x%04x", (unsigned)fx->vo_vi);
	else
		cnt += snprintf(msg + cnt, TX_ACLT_FORCE_MSG_LEN - cnt - 1,
				"         AUTO");
	if (cnt >= TX_ACLT_FORCE_MSG_LEN - 1)
		goto append_msg;

	if (fx->be_bk)
		cnt += snprintf(msg + cnt, TX_ACLT_FORCE_MSG_LEN - cnt - 1,
				" FORCE:0x%04x", (unsigned)fx->be_bk);
	else
		cnt += snprintf(msg + cnt, TX_ACLT_FORCE_MSG_LEN - cnt - 1,
				"         AUTO");
	if (cnt >= TX_ACLT_FORCE_MSG_LEN - 1)
		goto append_msg;

append_msg:
	n = snprintf(buf + pos, buflen - (size_t)pos, "%s\n", msg);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;
	return pos;
}
