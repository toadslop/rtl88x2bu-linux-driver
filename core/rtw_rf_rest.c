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
#define _RTW_RF_REST_C_

#ifdef HOST_RF_TEST
#include "host_rf_types.h"
#else
#include <drv_types.h>
#include <rtw_rf.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_RF_TEST)
u8 center_ch_2g[CENTER_CH_2G_NUM] = {
/* G00 */1, 2,
/* G01 */3, 4, 5,
/* G02 */6, 7, 8,
/* G03 */9, 10, 11,
/* G04 */12, 13,
/* G05 */14
};

#define ch_to_cch_2g_idx(ch) ((ch) - 1)

u8 center_ch_2g_40m[CENTER_CH_2G_40M_NUM] = {
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
};

u8 op_chs_of_cch_2g_40m[CENTER_CH_2G_40M_NUM][2] = {
	{1, 5}, /* 3 */
	{2, 6}, /* 4 */
	{3, 7}, /* 5 */
	{4, 8}, /* 6 */
	{5, 9}, /* 7 */
	{6, 10}, /* 8 */
	{7, 11}, /* 9 */
	{8, 12}, /* 10 */
	{9, 13}, /* 11 */
};
u8 center_ch_5g_20m[CENTER_CH_5G_20M_NUM] = {
/* G00 */36, 40,
/* G01 */44, 48,
/* G02 */52, 56,
/* G03 */60, 64,
/* G04 */100, 104,
/* G05 */108, 112,
/* G06 */116, 120,
/* G07 */124, 128,
/* G08 */132, 136,
/* G09 */140, 144,
/* G10 */149, 153,
/* G11 */157, 161,
/* G12 */165, 169,
/* G13 */173, 177
};

#define ch_to_cch_5g_20m_idx(ch) \
	( \
		((ch) >= 36 && (ch) <= 64) ? (((ch) - 36) >> 2) : \
		((ch) >= 100 && (ch) <= 144) ? 8 + (((ch) - 100) >> 2) : \
		((ch) >= 149 && (ch) <= 177) ? 20 + (((ch) - 149) >> 2) : 255 \
	)

u8 center_ch_5g_40m[CENTER_CH_5G_40M_NUM] = {
/* G00 */38,
/* G01 */46,
/* G02 */54,
/* G03 */62,
/* G04 */102,
/* G05 */110,
/* G06 */118,
/* G07 */126,
/* G08 */134,
/* G09 */142,
/* G10 */151,
/* G11 */159,
/* G12 */167,
/* G13 */175
};

u8 op_chs_of_cch_5g_40m[CENTER_CH_5G_40M_NUM][2] = {
	{36, 40}, /* 38 */
	{44, 48}, /* 46 */
	{52, 56}, /* 54 */
	{60, 64}, /* 62 */
	{100, 104}, /* 102 */
	{108, 112}, /* 110 */
	{116, 120}, /* 118 */
	{124, 128}, /* 126 */
	{132, 136}, /* 134 */
	{140, 144}, /* 142 */
	{149, 153}, /* 151 */
	{157, 161}, /* 159 */
	{165, 169}, /* 167 */
	{173, 177}, /* 175 */
};

u8 center_ch_5g_80m[CENTER_CH_5G_80M_NUM] = {
/* G00 ~ G01*/42,
/* G02 ~ G03*/58,
/* G04 ~ G05*/106,
/* G06 ~ G07*/122,
/* G08 ~ G09*/138,
/* G10 ~ G11*/155,
/* G12 ~ G13*/171
};

u8 op_chs_of_cch_5g_80m[CENTER_CH_5G_80M_NUM][4] = {
	{36, 40, 44, 48}, /* 42 */
	{52, 56, 60, 64}, /* 58 */
	{100, 104, 108, 112}, /* 106 */
	{116, 120, 124, 128}, /* 122 */
	{132, 136, 140, 144}, /* 138 */
	{149, 153, 157, 161}, /* 155 */
	{165, 169, 173, 177}, /* 171 */
};

u8 center_ch_5g_160m[CENTER_CH_5G_160M_NUM] = {
/* G00 ~ G03*/50,
/* G04 ~ G07*/114,
/* G10 ~ G13*/163
};

u8 op_chs_of_cch_5g_160m[CENTER_CH_5G_160M_NUM][8] = {
	{36, 40, 44, 48, 52, 56, 60, 64}, /* 50 */
	{100, 104, 108, 112, 116, 120, 124, 128}, /* 114 */
	{149, 153, 157, 161, 165, 169, 173, 177}, /* 163 */
};

struct center_chs_ent_t {
	u8 ch_num;
	u8 *chs;
};

struct center_chs_ent_t center_chs_2g_by_bw[] = {
	{CENTER_CH_2G_NUM, center_ch_2g},
	{CENTER_CH_2G_40M_NUM, center_ch_2g_40m},
};

struct center_chs_ent_t center_chs_5g_by_bw[] = {
	{CENTER_CH_5G_20M_NUM, center_ch_5g_20m},
	{CENTER_CH_5G_40M_NUM, center_ch_5g_40m},
	{CENTER_CH_5G_80M_NUM, center_ch_5g_80m},
	{CENTER_CH_5G_160M_NUM, center_ch_5g_160m},
};

/*
 * Get center channel of smaller bandwidth by @param cch, @param bw, @param offset
 * @cch: the given center channel
 * @bw: the given bandwidth
 * @offset: the given primary SC offset of the given bandwidth
 *
 * return center channel of smaller bandiwdth if valid, or 0
 */
u8 rtw_get_scch_by_cch_offset(u8 cch, u8 bw, u8 offset)
{
	u8 t_cch = 0;

	if (bw == CHANNEL_WIDTH_20) {
		t_cch = cch;
		goto exit;
	}

	if (offset == HAL_PRIME_CHNL_OFFSET_DONT_CARE) {
		rtw_warn_on(1);
		goto exit;
	}

	/* 2.4G, 40MHz */
	if (cch >= 3 && cch <= 11 && bw == CHANNEL_WIDTH_40) {
		t_cch = (offset == HAL_PRIME_CHNL_OFFSET_UPPER) ? cch + 2 : cch - 2;
		goto exit;
	}

	/* 5G, 160MHz */
	if (cch >= 50 && cch <= 163 && bw == CHANNEL_WIDTH_160) {
		t_cch = (offset == HAL_PRIME_CHNL_OFFSET_UPPER) ? cch + 8 : cch - 8;
		goto exit;

	/* 5G, 80MHz */
	} else if (cch >= 42 && cch <= 171 && bw == CHANNEL_WIDTH_80) {
		t_cch = (offset == HAL_PRIME_CHNL_OFFSET_UPPER) ? cch + 4 : cch - 4;
		goto exit;

	/* 5G, 40MHz */
	} else if (cch >= 38 && cch <= 175 && bw == CHANNEL_WIDTH_40) {
		t_cch = (offset == HAL_PRIME_CHNL_OFFSET_UPPER) ? cch + 2 : cch - 2;
		goto exit;

	} else {
		rtw_warn_on(1);
		goto exit;
	}

exit:
	return t_cch;
}

/*
 * Get center channel of smaller bandwidth by @param cch, @param bw, @param opch
 * @cch: the given center channel
 * @bw: the given bandwidth
 * @opch: the given operating channel
 *
 * return center channel of smaller bandiwdth if valid, or 0
 */
u8 rtw_get_scch_by_cch_opch(u8 cch, u8 bw, u8 opch)
{
	u8 offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;

	if (opch > cch)
		offset = HAL_PRIME_CHNL_OFFSET_UPPER;
	else if (opch < cch)
		offset = HAL_PRIME_CHNL_OFFSET_LOWER;

	return rtw_get_scch_by_cch_offset(cch, bw, offset);
}

struct op_chs_ent_t {
	u8 ch_num;
	u8 *chs;
};

struct op_chs_ent_t op_chs_of_cch_2g_by_bw[] = {
	{1, center_ch_2g},
	{2, (u8 *)op_chs_of_cch_2g_40m},
};

struct op_chs_ent_t op_chs_of_cch_5g_by_bw[] = {
	{1, center_ch_5g_20m},
	{2, (u8 *)op_chs_of_cch_5g_40m},
	{4, (u8 *)op_chs_of_cch_5g_80m},
	{8, (u8 *)op_chs_of_cch_5g_160m},
};

u8 center_chs_2g_num(u8 bw)
{
	if (bw > CHANNEL_WIDTH_40)
		return 0;

	return center_chs_2g_by_bw[bw].ch_num;
}

u8 center_chs_2g(u8 bw, u8 id)
{
	if (bw > CHANNEL_WIDTH_40)
		return 0;

	if (id >= center_chs_2g_num(bw))
		return 0;

	return center_chs_2g_by_bw[bw].chs[id];
}

u8 center_chs_5g_num(u8 bw)
{
	if (bw > CHANNEL_WIDTH_160)
		return 0;

	return center_chs_5g_by_bw[bw].ch_num;
}

u8 center_chs_5g(u8 bw, u8 id)
{
	if (bw > CHANNEL_WIDTH_160)
		return 0;

	if (id >= center_chs_5g_num(bw))
		return 0;

	return center_chs_5g_by_bw[bw].chs[id];
}

/*
 * Get available op channels by @param cch, @param bw
 * @cch: the given center channel
 * @bw: the given bandwidth
 * @op_chs: the pointer to return pointer of op channel array
 * @op_ch_num: the pointer to return pointer of op channel number
 *
 * return valid (1) or not (0)
 */
u8 rtw_get_op_chs_by_cch_bw(u8 cch, u8 bw, u8 **op_chs, u8 *op_ch_num)
{
	int i;
	struct center_chs_ent_t *c_chs_ent = NULL;
	struct op_chs_ent_t *op_chs_ent = NULL;
	u8 valid = 1;

	if (cch <= 14
		&& bw <= CHANNEL_WIDTH_40
	) {
		c_chs_ent = &center_chs_2g_by_bw[bw];
		op_chs_ent = &op_chs_of_cch_2g_by_bw[bw];
	} else if (cch >= 36 && cch <= 177
		&& bw <= CHANNEL_WIDTH_160
	) {
		c_chs_ent = &center_chs_5g_by_bw[bw];
		op_chs_ent = &op_chs_of_cch_5g_by_bw[bw];
	} else {
		valid = 0;
		goto exit;
	}

	for (i = 0; i < c_chs_ent->ch_num; i++)
		if (cch == *(c_chs_ent->chs + i))
			break;

	if (i == c_chs_ent->ch_num) {
		valid = 0;
		goto exit;
	}

	*op_chs = op_chs_ent->chs + op_chs_ent->ch_num * i;
	*op_ch_num = op_chs_ent->ch_num;

exit:
	return valid;
}

u8 rtw_get_offset_by_chbw(u8 ch, u8 bw, u8 *r_offset)
{
	u8 valid = 1;
	u8 offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;

	if (bw == CHANNEL_WIDTH_20)
		goto exit;

	if (bw >= CHANNEL_WIDTH_80 && ch <= 14) {
		valid = 0;
		goto exit;
	}

	if (ch >= 1 && ch <= 4)
		offset = HAL_PRIME_CHNL_OFFSET_LOWER;
	else if (ch >= 5 && ch <= 9) {
		if (*r_offset == HAL_PRIME_CHNL_OFFSET_LOWER || *r_offset == HAL_PRIME_CHNL_OFFSET_UPPER)
			offset = *r_offset; /* both lower and upper is valid, obey input value */
		else
			offset = HAL_PRIME_CHNL_OFFSET_UPPER; /* default use upper */
	} else if (ch >= 10 && ch <= 13)
		offset = HAL_PRIME_CHNL_OFFSET_UPPER;
	else if (ch == 14) {
		valid = 0; /* ch14 doesn't support 40MHz bandwidth */
		goto exit;
	} else if (ch >= 36 && ch <= 177) {
		switch (ch) {
		case 36:
		case 44:
		case 52:
		case 60:
		case 100:
		case 108:
		case 116:
		case 124:
		case 132:
		case 140:
		case 149:
		case 157:
		case 165:
		case 173:
			offset = HAL_PRIME_CHNL_OFFSET_LOWER;
			break;
		case 40:
		case 48:
		case 56:
		case 64:
		case 104:
		case 112:
		case 120:
		case 128:
		case 136:
		case 144:
		case 153:
		case 161:
		case 169:
		case 177:
			offset = HAL_PRIME_CHNL_OFFSET_UPPER;
			break;
		default:
			valid = 0;
			break;
		}
	} else
		valid = 0;

exit:
	if (valid && r_offset)
		*r_offset = offset;
	return valid;
}

u8 rtw_get_center_ch(u8 ch, u8 bw, u8 offset)
{
	u8 cch = ch;

	if (bw == CHANNEL_WIDTH_160) {
		if (ch % 4 == 0) {
			if (ch >= 36 && ch <= 64)
				cch = 50;
			else if (ch >= 100 && ch <= 128)
				cch = 114;
		} else if (ch % 4 == 1) {
			if (ch >= 149 && ch <= 177)
				cch = 163;
		}

	} else if (bw == CHANNEL_WIDTH_80) {
		if (ch <= 14)
			cch = 7; /* special case for 2.4G */
		else if (ch % 4 == 0) {
			if (ch >= 36 && ch <= 48)
				cch = 42;
			else if (ch >= 52 && ch <= 64)
				cch = 58;
			else if (ch >= 100 && ch <= 112)
				cch = 106;
			else if (ch >= 116 && ch <= 128)
				cch = 122;
			else if (ch >= 132 && ch <= 144)
				cch = 138;
		} else if (ch % 4 == 1) {
			if (ch >= 149 && ch <= 161)
				cch = 155;
			else if (ch >= 165 && ch <= 177)
				cch = 171;
		}

	} else if (bw == CHANNEL_WIDTH_40) {
		if (offset == HAL_PRIME_CHNL_OFFSET_LOWER)
			cch = ch + 2;
		else if (offset == HAL_PRIME_CHNL_OFFSET_UPPER)
			cch = ch - 2;

	} else if (bw == CHANNEL_WIDTH_20
		|| bw == CHANNEL_WIDTH_10
		|| bw == CHANNEL_WIDTH_5
	)
		; /* same as ch */
	else
		rtw_warn_on(1);

	return cch;
}

u8 rtw_get_ch_group(u8 ch, u8 *group, u8 *cck_group)
{
	BAND_TYPE band = BAND_MAX;
	s8 gp = -1, cck_gp = -1;

	if (ch <= 14) {
		band = BAND_ON_2_4G;

		if (1 <= ch && ch <= 2)
			gp = 0;
		else if (3  <= ch && ch <= 5)
			gp = 1;
		else if (6  <= ch && ch <= 8)
			gp = 2;
		else if (9  <= ch && ch <= 11)
			gp = 3;
		else if (12 <= ch && ch <= 14)
			gp = 4;
		else
			band = BAND_MAX;

		if (ch == 14)
			cck_gp = 5;
		else
			cck_gp = gp;
	} else {
		band = BAND_ON_5G;

		if (36 <= ch && ch <= 42)
			gp = 0;
		else if (44   <= ch && ch <=  48)
			gp = 1;
		else if (50   <= ch && ch <=  58)
			gp = 2;
		else if (60   <= ch && ch <=  64)
			gp = 3;
		else if (100  <= ch && ch <= 106)
			gp = 4;
		else if (108  <= ch && ch <= 114)
			gp = 5;
		else if (116  <= ch && ch <= 122)
			gp = 6;
		else if (124  <= ch && ch <= 130)
			gp = 7;
		else if (132  <= ch && ch <= 138)
			gp = 8;
		else if (140  <= ch && ch <= 144)
			gp = 9;
		else if (149  <= ch && ch <= 155)
			gp = 10;
		else if (157  <= ch && ch <= 161)
			gp = 11;
		else if (165  <= ch && ch <= 171)
			gp = 12;
		else if (173  <= ch && ch <= 177)
			gp = 13;
		else
			band = BAND_MAX;
	}

	if (band == BAND_MAX
		|| (band == BAND_ON_2_4G && cck_gp == -1)
		|| gp == -1
	) {
		RTW_WARN("%s invalid channel:%u", __func__, ch);
		rtw_warn_on(1);
		goto exit;
	}

	if (group)
		*group = gp;
	if (cck_group && band == BAND_ON_2_4G)
		*cck_group = cck_gp;

exit:
	return band;
}
#endif /* !CONFIG_RUST || HOST_RF_TEST */

#if !defined(CONFIG_RUST) || defined(HOST_RF_TEST)
/*
 * W3-20: frequency helpers extracted from core/rtw_rf.c.
 * C oracle for host L2; kernel builds use rust/rtw_rf_rest.rs when CONFIG_RUST.
 */
int rtw_ch2freq(int chan)
{
	/* see 802.11 17.3.8.3.2 and Annex J
	* there are overlapping channel numbers in 5GHz and 2GHz bands */

	/*
	* RTK: don't consider the overlapping channel numbers: 5G channel <= 14,
	* because we don't support it. simply judge from channel number
	*/

	if (chan >= 1 && chan <= 14) {
		if (chan == 14)
			return 2484;
		else if (chan < 14)
			return 2407 + chan * 5;
	} else if (chan >= 36 && chan <= 177)
		return 5000 + chan * 5;

	return 0; /* not supported */
}

int rtw_freq2ch(int freq)
{
	/* see 802.11 17.3.8.3.2 and Annex J */
	if (freq == 2484)
		return 14;
	else if (freq < 2484)
		return (freq - 2407) / 5;
	else if (freq >= 4910 && freq <= 4980)
		return (freq - 4000) / 5;
	else if (freq <= 45000) /* DMG band lower limit */
		return (freq - 5000) / 5;
	else if (freq >= 58320 && freq <= 64800)
		return (freq - 56160) / 2160;
	else
		return 0;
}

bool rtw_chbw_to_freq_range(u8 ch, u8 bw, u8 offset, u32 *hi, u32 *lo)
{
	u8 c_ch;
	u32 freq;
	u32 hi_ret = 0, lo_ret = 0;
	bool valid = _FALSE;

	if (hi)
		*hi = 0;
	if (lo)
		*lo = 0;

	c_ch = rtw_get_center_ch(ch, bw, offset);
	freq = rtw_ch2freq(c_ch);

	if (!freq) {
		rtw_warn_on(1);
		goto exit;
	}

	if (bw == CHANNEL_WIDTH_160) {
		hi_ret = freq + 80;
		lo_ret = freq - 80;
	} else if (bw == CHANNEL_WIDTH_80) {
		hi_ret = freq + 40;
		lo_ret = freq - 40;
	} else if (bw == CHANNEL_WIDTH_40) {
		hi_ret = freq + 20;
		lo_ret = freq - 20;
	} else if (bw == CHANNEL_WIDTH_20) {
		hi_ret = freq + 10;
		lo_ret = freq - 10;
	} else
		rtw_warn_on(1);

	if (hi)
		*hi = hi_ret;
	if (lo)
		*lo = lo_ret;

	valid = _TRUE;

exit:
	return valid;
}
#endif /* !CONFIG_RUST || HOST_RF_TEST */

#if !defined(CONFIG_RUST) || defined(HOST_RF_TEST)
/*
 * W3-21: lookup/format tables extracted from core/rtw_rf.c.
 * C oracle for host L2; kernel builds use rust/rtw_rf_rest.rs when CONFIG_RUST.
 */
const char *const _ch_width_str[CHANNEL_WIDTH_MAX] = {
	[CHANNEL_WIDTH_20]		= "20MHz",
	[CHANNEL_WIDTH_40]		= "40MHz",
	[CHANNEL_WIDTH_80]		= "80MHz",
	[CHANNEL_WIDTH_160]		= "160MHz",
	[CHANNEL_WIDTH_80_80]	= "80_80MHz",
	[CHANNEL_WIDTH_5]		= "5MHz",
	[CHANNEL_WIDTH_10]		= "10MHz",
};

const u8 _ch_width_to_bw_cap[CHANNEL_WIDTH_MAX] = {
	[CHANNEL_WIDTH_20]		= BW_CAP_20M,
	[CHANNEL_WIDTH_40]		= BW_CAP_40M,
	[CHANNEL_WIDTH_80]		= BW_CAP_80M,
	[CHANNEL_WIDTH_160]		= BW_CAP_160M,
	[CHANNEL_WIDTH_80_80]	= BW_CAP_80_80M,
	[CHANNEL_WIDTH_5]		= BW_CAP_5M,
	[CHANNEL_WIDTH_10]		= BW_CAP_10M,
};

const char *const _band_str[] = {
	"2.4G",
	"5G",
	"BAND_MAX",
};

const u8 _band_to_band_cap[] = {
	BAND_CAP_2G,
	BAND_CAP_5G,
	0,
};

const char *const _opc_bw_str[OPC_BW_NUM] = {
	"20M ",		/* OPC_BW20 */
	"40M+",		/* OPC_BW40PLUS */
	"40M-",		/* OPC_BW40MINUS */
	"80M ",		/* OPC_BW80 */
	"160M ",	/* OPC_BW160 */
	"80+80M ",	/* OPC_BW80P80 */
};

const u8 _opc_bw_to_ch_width[OPC_BW_NUM] = {
	CHANNEL_WIDTH_20,		/* OPC_BW20 */
	CHANNEL_WIDTH_40,		/* OPC_BW40PLUS */
	CHANNEL_WIDTH_40,		/* OPC_BW40MINUS */
	CHANNEL_WIDTH_80,		/* OPC_BW80 */
	CHANNEL_WIDTH_160,		/* OPC_BW160 */
	CHANNEL_WIDTH_80_80,	/* OPC_BW80P80 */
};
#endif /* !CONFIG_RUST || HOST_RF_TEST */

#if !defined(CONFIG_RUST) || defined(HOST_RF_TEST)
/*
 * W3-22: global operating-class lookup extracted from core/rtw_rf.c.
 * C oracle for host L2; kernel builds use rust/rtw_rf_rest.rs when CONFIG_RUST.
 */
#define OP_CLASS_ENT(_class, _band, _bw, _len, arg...) \
	{.class_id = _class, .band = _band, .bw = _bw, .len_ch_attr = (uint8_t[_len + 1]) {_len, ##arg},}

/* 802.11-2016 Table E-4, partial */
const struct op_class_t global_op_class[] = {
	/* 2G ch1~13, 20M */
	OP_CLASS_ENT(81,	BAND_ON_2_4G,	OPC_BW20,		13,	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13),
	/* 2G ch14, 20M */
	OP_CLASS_ENT(82,	BAND_ON_2_4G,	OPC_BW20,		1,	14),
	/* 2G, 40M */
	OP_CLASS_ENT(83,	BAND_ON_2_4G, 	OPC_BW40PLUS,	9,	1, 2, 3, 4, 5, 6, 7, 8, 9),
	OP_CLASS_ENT(84,	BAND_ON_2_4G,	OPC_BW40MINUS,	9,	5, 6, 7, 8, 9, 10, 11, 12, 13),
	/* 5G band 1, 20M & 40M */
	OP_CLASS_ENT(115,	BAND_ON_5G,		OPC_BW20,		4,	36, 40, 44, 48),
	OP_CLASS_ENT(116,	BAND_ON_5G,		OPC_BW40PLUS,	2,	36, 44),
	OP_CLASS_ENT(117,	BAND_ON_5G,		OPC_BW40MINUS,	2,	40, 48),
	/* 5G band 2, 20M & 40M */
	OP_CLASS_ENT(118,	BAND_ON_5G,		OPC_BW20,		4,	52, 56, 60, 64),
	OP_CLASS_ENT(119,	BAND_ON_5G,		OPC_BW40PLUS,	2,	52, 60),
	OP_CLASS_ENT(120,	BAND_ON_5G,		OPC_BW40MINUS,	2,	56, 64),
	/* 5G band 3, 20M & 40M */
	OP_CLASS_ENT(121,	BAND_ON_5G,		OPC_BW20,		12,	100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144),
	OP_CLASS_ENT(122,	BAND_ON_5G,		OPC_BW40PLUS,	6,	100, 108, 116, 124, 132, 140),
	OP_CLASS_ENT(123,	BAND_ON_5G,		OPC_BW40MINUS,	6,	104, 112, 120, 128, 136, 144),
	/* 5G band 4, 20M & 40M */
	OP_CLASS_ENT(124,	BAND_ON_5G,		OPC_BW20,		4,	149, 153, 157, 161),
	OP_CLASS_ENT(125,	BAND_ON_5G,		OPC_BW20,		6,	149, 153, 157, 161, 165, 169),
	OP_CLASS_ENT(126,	BAND_ON_5G,		OPC_BW40PLUS,	2,	149, 157),
	OP_CLASS_ENT(127,	BAND_ON_5G,		OPC_BW40MINUS,	2,	153, 161),
	/* 5G, 80M & 160M */
	OP_CLASS_ENT(128,	BAND_ON_5G,		OPC_BW80,		24,	36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 149, 153, 157, 161),
	OP_CLASS_ENT(129,	BAND_ON_5G,		OPC_BW160,		16,	36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128),
	#if 0 /* TODO */
	/* 5G, 80+80M */
	{130,	BAND_ON_5G,		OPC_BW80P80,	0x0FFFFFF},
	#endif
};

const int global_op_class_num = sizeof(global_op_class) / sizeof(struct op_class_t);

static const struct op_class_t *get_global_op_class_by_id(u8 gid)
{
	int i;

	for (i = 0; i < global_op_class_num; i++)
		if (global_op_class[i].class_id == gid)
			break;

	return i < global_op_class_num ? &global_op_class[i] : NULL;
}

bool is_valid_global_op_class_id(u8 gid)
{
	return get_global_op_class_by_id(gid) ? 1 : 0;
}

static bool is_valid_global_op_class_ch(const struct op_class_t *opc, u8 ch)
{
	int array_idx;
	int i;

	if (opc < global_op_class
		|| (((u8 *)opc) - ((u8 *)global_op_class)) % sizeof(struct op_class_t)
	) {
		RTW_ERR("Invalid opc pointer:%p (global_op_class:%p, sizeof(struct op_class_t):%zu, %zu)\n"
			, opc, global_op_class, sizeof(struct op_class_t), (((u8 *)opc) - ((u8 *)global_op_class)) % sizeof(struct op_class_t));
		return 0;
	}

	array_idx = (((u8 *)opc) - ((u8 *)global_op_class)) / sizeof(struct op_class_t);

	for (i = 0; i < OPC_CH_LIST_LEN(global_op_class[array_idx]); i++)
		if (OPC_CH_LIST_CH(global_op_class[array_idx], i) == ch)
			break;

	return i < OPC_CH_LIST_LEN(global_op_class[array_idx]);
}

static enum opc_bw get_global_opc_bw_by_id(u8 gid)
{
	int i;

	for (i = 0; i < global_op_class_num; i++)
		if (global_op_class[i].class_id == gid)
			break;

	return i < global_op_class_num ? global_op_class[i].bw : OPC_BW_NUM;
}

/* -2: logic error, -1: error, 0: is already BW20 */
s16 get_sub_op_class(u8 gid, u8 ch)
{
	const struct op_class_t *opc = get_global_op_class_by_id(gid);
	int i;
	u8 bw;

	if (!opc)
		return -1;

	if (!is_valid_global_op_class_ch(opc, ch))
		return -1;

	if (opc->bw == OPC_BW20)
		return 0;

	bw = opc_bw_to_ch_width(opc->bw);

	for (i = 0; i < global_op_class_num; i++) {
		if (bw != opc_bw_to_ch_width(global_op_class[i].bw) + 1)
			continue;
		if (is_valid_global_op_class_ch(&global_op_class[i], ch))
			break;
	}

	return i < global_op_class_num ? global_op_class[i].class_id : -2;
}

u8 rtw_get_op_class_by_chbw(u8 ch, u8 bw, u8 offset)
{
	BAND_TYPE band = BAND_MAX;
	int i;
	u8 gid = 0; /* invalid */

	if (rtw_is_2g_ch(ch))
		band = BAND_ON_2_4G;
	else if (rtw_is_5g_ch(ch))
		band = BAND_ON_5G;
	else
		goto exit;

	switch (bw) {
	case CHANNEL_WIDTH_20:
	case CHANNEL_WIDTH_40:
	case CHANNEL_WIDTH_80:
	case CHANNEL_WIDTH_160:
	#if 0 /* TODO */
	case CHANNEL_WIDTH_80_80:
	#endif
		break;
	default:
		goto exit;
	}

	for (i = 0; i < global_op_class_num; i++) {
		if (band != global_op_class[i].band)
			continue;

		if (opc_bw_to_ch_width(global_op_class[i].bw) != bw)
			continue;

		if ((global_op_class[i].bw == OPC_BW40PLUS
				&& offset != HAL_PRIME_CHNL_OFFSET_LOWER)
			|| (global_op_class[i].bw == OPC_BW40MINUS
				&& offset != HAL_PRIME_CHNL_OFFSET_UPPER)
		)
			continue;

		if (is_valid_global_op_class_ch(&global_op_class[i], ch))
			goto get;
	}

get:
	if (i < global_op_class_num) {
		#if 0 /* TODO */
		if (bw == CHANNEL_WIDTH_80_80) {
			/* search another ch */
			if (!is_valid_global_op_class_ch(&global_op_class[i], ch2))
				goto exit;
		}
		#endif

		gid = global_op_class[i].class_id;
	}

exit:
	return gid;
}

u8 rtw_get_bw_offset_by_op_class_ch(u8 gid, u8 ch, u8 *bw, u8 *offset)
{
	enum opc_bw opc_bw;
	u8 valid = 0;

	opc_bw = get_global_opc_bw_by_id(gid);
	if (opc_bw == OPC_BW_NUM)
		goto exit;

	*bw = opc_bw_to_ch_width(opc_bw);

	if (opc_bw == OPC_BW40PLUS)
		*offset = HAL_PRIME_CHNL_OFFSET_LOWER;
	else if (opc_bw == OPC_BW40MINUS)
		*offset = HAL_PRIME_CHNL_OFFSET_UPPER;

	if (rtw_get_offset_by_chbw(ch, *bw, offset))
		valid = 1;

exit:
	return valid;
}

#endif /* !CONFIG_RUST || HOST_RF_TEST */

#if !defined(CONFIG_RUST) || defined(HOST_RF_TEST)
/*
 * W3-23: RF type / trx-path helpers extracted from core/rtw_rf.c.
 * C oracle for host L2; kernel builds use rust/rtw_rf_rest.rs when CONFIG_RUST.
 */
#ifdef HOST_RF_TEST
const u8 _rf_type_to_rf_tx_cnt[RF_TYPE_MAX] = {
	[RF_1T1R] = 1,
	[RF_1T2R] = 1,
	[RF_1T3R] = 1,
	[RF_1T4R] = 1,
	[RF_2T1R] = 2,
	[RF_2T2R] = 2,
	[RF_2T3R] = 2,
	[RF_2T4R] = 2,
	[RF_3T1R] = 3,
	[RF_3T2R] = 3,
	[RF_3T3R] = 3,
	[RF_3T4R] = 3,
	[RF_4T1R] = 4,
	[RF_4T2R] = 4,
	[RF_4T3R] = 4,
	[RF_4T4R] = 4,
};

const u8 _rf_type_to_rf_rx_cnt[RF_TYPE_MAX] = {
	[RF_1T1R] = 1,
	[RF_1T2R] = 2,
	[RF_1T3R] = 3,
	[RF_1T4R] = 4,
	[RF_2T1R] = 1,
	[RF_2T2R] = 2,
	[RF_2T3R] = 3,
	[RF_2T4R] = 4,
	[RF_3T1R] = 1,
	[RF_3T2R] = 2,
	[RF_3T3R] = 3,
	[RF_3T4R] = 4,
	[RF_4T1R] = 1,
	[RF_4T2R] = 2,
	[RF_4T3R] = 3,
	[RF_4T4R] = 4,
};
#endif /* HOST_RF_TEST */

void rf_type_to_default_trx_bmp(enum rf_type rf, enum bb_path *tx, enum bb_path *rx)
{
	u8 tx_num = rf_type_to_rf_tx_cnt(rf);
	u8 rx_num = rf_type_to_rf_rx_cnt(rf);
	int i;

	*tx = *rx = 0;

	for (i = 0; i < tx_num; i++)
		*tx |= BIT(i);
	for (i = 0; i < rx_num; i++)
		*rx |= BIT(i);
}

static const u8 _trx_num_to_rf_type[RF_PATH_MAX][RF_PATH_MAX] = {
	{RF_1T1R,	RF_1T2R,	RF_1T3R,	RF_1T4R},
	{RF_2T1R,	RF_2T2R,	RF_2T3R,	RF_2T4R},
	{RF_3T1R,	RF_3T2R,	RF_3T3R,	RF_3T4R},
	{RF_4T1R,	RF_4T2R,	RF_4T3R,	RF_4T4R},
};

enum rf_type trx_num_to_rf_type(u8 tx_num, u8 rx_num)
{
	if (tx_num > 0 && tx_num <= RF_PATH_MAX && rx_num > 0 && rx_num <= RF_PATH_MAX)
		return _trx_num_to_rf_type[tx_num - 1][rx_num - 1];
	return RF_TYPE_MAX;
}

enum rf_type trx_bmp_to_rf_type(u8 tx_bmp, u8 rx_bmp)
{
	u8 tx_num = 0;
	u8 rx_num = 0;
	int i;

	for (i = 0; i < RF_PATH_MAX; i++) {
		if (tx_bmp >> i & BIT0)
			tx_num++;
		if (rx_bmp >> i & BIT0)
			rx_num++;
	}

	return trx_num_to_rf_type(tx_num, rx_num);
}

bool rf_type_is_a_in_b(enum rf_type a, enum rf_type b)
{
	return rf_type_to_rf_tx_cnt(a) <= rf_type_to_rf_tx_cnt(b)
		&& rf_type_to_rf_rx_cnt(a) <= rf_type_to_rf_rx_cnt(b);
}

static void rtw_path_bmp_limit_from_higher(u8 *bmp, u8 *bmp_bit_cnt, u8 bit_cnt_lmt)
{
	int i;

	for (i = RF_PATH_MAX - 1; *bmp_bit_cnt > bit_cnt_lmt && i >= 0; i--) {
		if (*bmp & BIT(i)) {
			*bmp &= ~BIT(i);
			(*bmp_bit_cnt)--;
		}
	}
}

u8 rtw_restrict_trx_path_bmp_by_trx_num_lmt(u8 trx_path_bmp, u8 tx_num_lmt, u8 rx_num_lmt, u8 *tx_num, u8 *rx_num)
{
	u8 bmp_tx = (trx_path_bmp & 0xF0) >> 4;
	u8 bmp_rx = trx_path_bmp & 0x0F;
	u8 bmp_tx_num = 0, bmp_rx_num = 0;
	enum rf_type ret_type = RF_TYPE_MAX;
	int i, j;

	for (i = 0; i < RF_PATH_MAX; i++) {
		if (bmp_tx & BIT(i))
			bmp_tx_num++;
		if (bmp_rx & BIT(i))
			bmp_rx_num++;
	}

	if (tx_num_lmt)
		rtw_path_bmp_limit_from_higher(&bmp_tx, &bmp_tx_num, tx_num_lmt);
	if (rx_num_lmt)
		rtw_path_bmp_limit_from_higher(&bmp_rx, &bmp_rx_num, rx_num_lmt);

	for (j = bmp_rx_num; j > 0; j--) {
		for (i = bmp_tx_num; i > 0; i--) {
			ret_type = trx_num_to_rf_type(i, j);
			if (RF_TYPE_VALID(ret_type)) {
				rtw_path_bmp_limit_from_higher(&bmp_tx, &bmp_tx_num, i);
				rtw_path_bmp_limit_from_higher(&bmp_rx, &bmp_rx_num, j);
				if (tx_num)
					*tx_num = bmp_tx_num;
				if (rx_num)
					*rx_num = bmp_rx_num;
				goto exit;
			}
		}
	}

exit:
	return RF_TYPE_VALID(ret_type) ? ((bmp_tx << 4) | bmp_rx) : 0x00;
}

u8 rtw_restrict_trx_path_bmp_by_rftype(u8 trx_path_bmp, enum rf_type type, u8 *tx_num, u8 *rx_num)
{
	return rtw_restrict_trx_path_bmp_by_trx_num_lmt(trx_path_bmp
		, rf_type_to_rf_tx_cnt(type), rf_type_to_rf_rx_cnt(type), tx_num, rx_num);
}
#endif /* !CONFIG_RUST || HOST_RF_TEST */

#if !defined(CONFIG_RUST) || defined(HOST_RF_TEST)
/*
 * W3-24: txpwr formatting and DFS CAC helpers extracted from core/rtw_rf.c.
 * C oracle for host L2; kernel builds use rust/rtw_rf_rest.rs when CONFIG_RUST.
 */
#ifdef HOST_RF_TEST
#include <stdio.h>
#endif

void txpwr_idx_get_dbm_str(s8 idx, u8 txgi_max, u8 txgi_pdbm, SIZE_T cwidth, char dbm_str[], u8 dbm_str_len)
{
	char fmt[16];

	if (idx == txgi_max) {
		snprintf(fmt, 16, "%%%zus", cwidth >= 6 ? cwidth + 1 : 6);
		snprintf(dbm_str, dbm_str_len, fmt, "NA");
	} else if (idx > -txgi_pdbm && idx < 0) { /* -0.xx */
		snprintf(fmt, 16, "%%%zus-0.%%02d", cwidth >= 6 ? cwidth - 4 : 1);
		snprintf(dbm_str, dbm_str_len, fmt, "", (rtw_abs(idx) % txgi_pdbm) * 100 / txgi_pdbm);
	} else if (idx % txgi_pdbm) { /* d.xx */
		snprintf(fmt, 16, "%%%zud.%%02d", cwidth >= 6 ? cwidth - 2 : 3);
		snprintf(dbm_str, dbm_str_len, fmt, idx / txgi_pdbm, (rtw_abs(idx) % txgi_pdbm) * 100 / txgi_pdbm);
	} else { /* d */
		snprintf(fmt, 16, "%%%zud", cwidth >= 6 ? cwidth + 1 : 6);
		snprintf(dbm_str, dbm_str_len, fmt, idx / txgi_pdbm);
	}
}

void txpwr_mbm_get_dbm_str(s16 mbm, SIZE_T cwidth, char dbm_str[], u8 dbm_str_len)
{
	char fmt[16];

	if (mbm == UNSPECIFIED_MBM) {
		snprintf(fmt, 16, "%%%zus", cwidth >= 6 ? cwidth + 1 : 6);
		snprintf(dbm_str, dbm_str_len, fmt, "NA");
	} else if (mbm > -MBM_PDBM && mbm < 0) { /* -0.xx */
		snprintf(fmt, 16, "%%%zus-0.%%02d", cwidth >= 6 ? cwidth - 4 : 1);
		snprintf(dbm_str, dbm_str_len, fmt, "", (rtw_abs(mbm) % MBM_PDBM) * 100 / MBM_PDBM);
	} else if (mbm % MBM_PDBM) { /* d.xx */
		snprintf(fmt, 16, "%%%zud.%%02d", cwidth >= 6 ? cwidth - 2 : 3);
		snprintf(dbm_str, dbm_str_len, fmt, mbm / MBM_PDBM, (rtw_abs(mbm) % MBM_PDBM) * 100 / MBM_PDBM);
	} else { /* d */
		snprintf(fmt, 16, "%%%zud", cwidth >= 6 ? cwidth + 1 : 6);
		snprintf(dbm_str, dbm_str_len, fmt, mbm / MBM_PDBM);
	}
}

static const s16 _mb_of_ntx[] = {
	0,		/* 1TX */
	301,	/* 2TX */
	477,	/* 3TX */
	602,	/* 4TX */
	699,	/* 5TX */
	778,	/* 6TX */
	845,	/* 7TX */
	903,	/* 8TX */
};

s16 mb_of_ntx(u8 ntx)
{
	if (ntx == 0 || ntx > 8) {
		RTW_ERR("ntx=%u, out of range\n", ntx);
		rtw_warn_on(1);
	}

	return _mb_of_ntx[ntx - 1];
}

bool rtw_is_long_cac_range(u32 hi, u32 lo, u8 dfs_region)
{
	return (dfs_region == RTW_DFS_REGD_ETSI && rtw_is_range_overlap(hi, lo, 5650, 5600)) ? _TRUE : _FALSE;
}

bool rtw_is_long_cac_ch(u8 ch, u8 bw, u8 offset, u8 dfs_region)
{
	u32 hi, lo;

	if (rtw_chbw_to_freq_range(ch, bw, offset, &hi, &lo) == _FALSE)
		return _FALSE;

	return rtw_is_long_cac_range(hi, lo, dfs_region) ? _TRUE : _FALSE;
}
#endif /* !CONFIG_RUST || HOST_RF_TEST */

#if defined(CONFIG_RUST) && !defined(HOST_RF_TEST)
void rtw_rust_rf_warn_on(int condition)
{
	rtw_warn_on(condition);
}

void rtw_rust_rf_warn_invalid_ch(const char *func, u8 ch)
{
	RTW_WARN("%s invalid channel:%u", func, ch);
}
#endif /* CONFIG_RUST && !HOST_RF_TEST */
