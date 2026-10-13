// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-12 TRX share mode mapper + dump (hal/hal_halmac.c). */

#include <stdio.h>
#include <stdint.h>

enum halmac_rx_fifo_expanding_mode {
	HALMAC_RX_FIFO_EXPANDING_MODE_DISABLE = 0,
	HALMAC_RX_FIFO_EXPANDING_MODE_1_BLOCK = 1,
	HALMAC_RX_FIFO_EXPANDING_MODE_2_BLOCK = 2,
	HALMAC_RX_FIFO_EXPANDING_MODE_3_BLOCK = 3,
};

static enum halmac_rx_fifo_expanding_mode
trx_share_mode_drv2halmac_impl(uint8_t trx_share_mode)
{
	if (trx_share_mode == 0)
		return HALMAC_RX_FIFO_EXPANDING_MODE_DISABLE;
	if (trx_share_mode == 1)
		return HALMAC_RX_FIFO_EXPANDING_MODE_1_BLOCK;
	if (trx_share_mode == 2)
		return HALMAC_RX_FIFO_EXPANDING_MODE_2_BLOCK;
	if (trx_share_mode == 3)
		return HALMAC_RX_FIFO_EXPANDING_MODE_3_BLOCK;

	return HALMAC_RX_FIFO_EXPANDING_MODE_DISABLE;
}

unsigned trx_share_mode_drv2halmac(uint8_t trx_share_mode)
{
	return (unsigned)trx_share_mode_drv2halmac_impl(trx_share_mode);
}

int dump_trx_share_mode_format(uint8_t trx_share_mode, char *buf, size_t buflen)
{
	enum halmac_rx_fifo_expanding_mode mode;
	const char *label;

	if (!buf || buflen == 0)
		return -1;

	mode = trx_share_mode_drv2halmac_impl(trx_share_mode);
	if (mode == HALMAC_RX_FIFO_EXPANDING_MODE_1_BLOCK)
		label = "RX_FIFO_EXPANDING_MODE_1";
	else if (mode == HALMAC_RX_FIFO_EXPANDING_MODE_2_BLOCK)
		label = "RX_FIFO_EXPANDING_MODE_2";
	else if (mode == HALMAC_RX_FIFO_EXPANDING_MODE_3_BLOCK)
		label = "RX_FIFO_EXPANDING_MODE_3";
	else
		label = "DISABLE";

	return snprintf(buf, buflen, "TRx share mode : %s\n", label);
}
