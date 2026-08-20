// SPDX-License-Identifier: GPL-2.0
/* Host fixtures for W3-52 txpwr_lmt L2 oracles. */

#include <stdlib.h>
#include <string.h>

#include "host_rf_txpwr_lmt_types.h"

static struct hal_spec_t host_rf_hal_spec = {
	.txgi_max = 63,
};

struct hal_spec_t *host_rf_hal_spec_ptr(void)
{
	return &host_rf_hal_spec;
}

const char *const _regd_str[] = {
	"NONE", "FCC", "MKK", "ETSI", "IC", "KCC", "NCC",
	"ACMA", "CHILE", "UKRAINE", "MEXICO", "CN", "WW",
};

const char *const _txpwr_lmt_rs_str[] = {
	"CCK", "OFDM", "HT", "VHT", "N/A",
};

void *rtw_zvmalloc(u32 sz)
{
	return calloc(1, sz);
}

void rtw_vmfree(u8 *p, u32 sz)
{
	(void)sz;
	free(p);
}

u8 center_ch_5g_all[CENTER_CH_5G_ALL_NUM] = {
	36, 38, 40, 42, 44, 46, 48, 52, 54, 56, 58, 60, 62, 64,
	100, 102, 104, 106, 108, 110, 112, 116, 118, 120, 122,
	124, 126, 128, 132, 134, 136, 138, 140, 142, 144, 149,
	151, 153, 155, 157, 159, 161, 165, 167, 169, 171, 173,
	175, 177,
};

void host_rf_txpwr_lmt_reset(struct rf_ctl_t *rfctl)
{
	memset(rfctl, 0, sizeof(*rfctl));
	_rtw_init_listhead(&rfctl->reg_exc_list);
	_rtw_init_listhead(&rfctl->txpwr_lmt_list);
	rfctl->regd_name = regd_str(TXPWR_LMT_NONE);
}
