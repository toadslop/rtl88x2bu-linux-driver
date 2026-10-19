// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-41 dump_hal_spec (from hal/hal_com_rest.c). */

#include <stdio.h>
#include <string.h>

#include "host_hal_com_hal_spec_types.h"

#define BAND_CAP_BIT_NUM 2
#define BW_CAP_BIT_NUM 7
#define PROTO_CAP_BIT_NUM 4
#define WL_FUNC_BIT_NUM 4
#define BIT0 1

static const char *const band_cap_str[] = {
	"2G",
	"5G",
};

static const char *const bw_cap_str[] = {
	"5M",
	"10M",
	"20M",
	"40M",
	"80M",
	"160M",
	"80_80M",
};

static const char *const proto_cap_str[] = {
	"b",
	"g",
	"n",
	"ac",
};

static const char *const wl_func_str[] = {
	"P2P",
	"MIRACAST",
	"TDLS",
	"FTM",
};

static int append_cap_line(char *buf, size_t buflen, int pos, const char *label,
			   uint8_t cap, const char *const *table, int table_len)
{
	int n;
	int i;

	n = snprintf(buf + pos, buflen - (size_t)pos, "%s", label);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	for (i = 0; i < table_len; i++) {
		if (((cap >> i) & BIT0) && table[i]) {
			n = snprintf(buf + pos, buflen - (size_t)pos, "%s ", table[i]);
			if (n < 0 || (size_t)(pos + n) >= buflen)
				return -1;
			pos += n;
		}
	}

	n = snprintf(buf + pos, buflen - (size_t)pos, "\n");
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;
	return pos;
}

int dump_hal_spec_format(const host_hal_com_hal_spec_fixture *fx, char *buf,
			 size_t buflen)
{
	int pos = 0;
	int n;

	if (!fx || !buf || buflen == 0)
		return -1;

	n = snprintf(buf + pos, buflen - (size_t)pos, "macid_num:%u\n",
		     fx->macid_num);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos, "macid_cap:%u\n",
		     fx->macid_cap);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos, "sec_cap:0x%02x\n",
		     fx->sec_cap);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos, "sec_cam_ent_num:%u\n",
		     fx->sec_cam_ent_num);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos, "rfpath_num_2g:%u\n",
		     fx->rfpath_num_2g);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos, "rfpath_num_5g:%u\n",
		     fx->rfpath_num_5g);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos, "rf_reg_path_num:%u\n",
		     fx->rf_reg_path_num);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos,
		     "rf_reg_path_avail_num:%u\n", fx->rf_reg_path_avail_num);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos,
		     "rf_reg_trx_path_bmp:0x%02x\n", fx->rf_reg_trx_path_bmp);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos, "max_tx_cnt:%u\n",
		     fx->max_tx_cnt);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos, "tx_nss_num:%u\n",
		     fx->tx_nss_num);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos, "rx_nss_num:%u\n",
		     fx->rx_nss_num);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	pos = append_cap_line(buf, buflen, pos, "band_cap:", fx->band_cap,
			      band_cap_str, BAND_CAP_BIT_NUM);
	if (pos < 0)
		return -1;

	pos = append_cap_line(buf, buflen, pos, "bw_cap:", fx->bw_cap,
			      bw_cap_str, BW_CAP_BIT_NUM);
	if (pos < 0)
		return -1;

	pos = append_cap_line(buf, buflen, pos, "proto_cap:", fx->proto_cap,
			      proto_cap_str, PROTO_CAP_BIT_NUM);
	if (pos < 0)
		return -1;

	n = snprintf(buf + pos, buflen - (size_t)pos, "txgi_max:%u\n",
		     fx->txgi_max);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos, "txgi_pdbm:%u\n",
		     fx->txgi_pdbm);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	pos = append_cap_line(buf, buflen, pos, "wl_func:", fx->wl_func,
			      wl_func_str, WL_FUNC_BIT_NUM);
	if (pos < 0)
		return -1;

	n = snprintf(buf + pos, buflen - (size_t)pos,
		     "tx_aclt_unit_factor:%u (unit:%uus)\n",
		     fx->tx_aclt_unit_factor,
		     fx->tx_aclt_unit_factor * 32);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos, "rx_tsf_filter:%u\n",
		     fx->rx_tsf_filter);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos, "pg_txpwr_saddr:0x%X\n",
		     fx->pg_txpwr_saddr);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	n = snprintf(buf + pos, buflen - (size_t)pos, "pg_txgi_diff_factor:%u\n",
		     fx->pg_txgi_diff_factor);
	if (n < 0 || (size_t)(pos + n) >= buflen)
		return -1;
	pos += n;

	return pos;
}
