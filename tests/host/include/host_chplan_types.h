/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal types for host L2 chplan tests (W2-17+).
 */
#ifndef HOST_CHPLAN_TYPES_H
#define HOST_CHPLAN_TYPES_H

#include "host_types.h"
#include "host_autoconf.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef int sint;
typedef unsigned int uint;

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0

#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

#define MAX_CHANNEL_NUM 59

#define TXPWR_LMT_NONE 0
#define TXPWR_LMT_FCC 1
#define TXPWR_LMT_MKK 2
#define TXPWR_LMT_ETSI 3
#define TXPWR_LMT_IC 4
#define TXPWR_LMT_KCC 5
#define TXPWR_LMT_NCC 6
#define TXPWR_LMT_ACMA 7
#define TXPWR_LMT_CHILE 8
#define TXPWR_LMT_UKRAINE 9
#define TXPWR_LMT_MEXICO 10
#define TXPWR_LMT_CN 11
#define TXPWR_LMT_WW 12

#define RTW_CHF_DFS (1 << 1)

#define CHANNEL_WIDTH_5 0
#define CHANNEL_WIDTH_10 1
#define CHANNEL_WIDTH_20 2
#define CHANNEL_WIDTH_40 3
#define CHANNEL_WIDTH_80 4
#define CHANNEL_WIDTH_160 5
#define CHANNEL_WIDTH_80_80 6
#define CHANNEL_WIDTH_MAX 7

#define HAL_PRIME_CHNL_OFFSET_LOWER 1
#define HAL_PRIME_CHNL_OFFSET_UPPER 2

#define RTW_PRINT(...) do { } while (0)
#define RTW_WARN(...) do { } while (0)
#define RTW_ERR(...) do { } while (0)
#define RTW_INFO(...) do { } while (0)
#define RTW_DBG(...) do { } while (0)
#define rtw_warn_on(cond) ((void)(cond))

struct registry_priv {
	u8 excl_chs[MAX_CHANNEL_NUM];
};

typedef struct _RT_CHANNEL_INFO {
	u8 ChannelNum;
	u8 flags;
} RT_CHANNEL_INFO;

typedef struct {
	struct registry_priv registrypriv;
} _adapter;

typedef void WLAN_BSSID_EX;

static inline char alpha_to_upper(char c)
{
	if (c >= 'a' && c <= 'z')
		return c - 'a' + 'A';
	return c;
}

int rtw_freq2ch(int freq);
int rtw_ch2freq(int chan);
u8 rtw_get_center_ch(u8 ch, u8 bw, u8 offset);
bool rtw_chbw_to_freq_range(u8 ch, u8 bw, u8 offset, u32 *hi, u32 *lo);

#endif /* HOST_CHPLAN_TYPES_H */
