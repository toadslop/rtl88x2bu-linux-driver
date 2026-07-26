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

#define RTW_CHF_NO_IR (1 << 0)
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

#define BAND_CAP_2G BIT0
#define BAND_CAP_5G BIT1

#define WIRELESS_11B BIT0
#define WIRELESS_11G BIT1
#define WIRELESS_11A BIT2
#define WIRELESS_11_24N BIT3
#define WIRELESS_11_5N BIT4
#define WIRELESS_11AC BIT6
#define WIRELESS_MODE_24G (WIRELESS_11B | WIRELESS_11G | WIRELESS_11_24N)
#define WIRELESS_MODE_5G (WIRELESS_11A | WIRELESS_11_5N | WIRELESS_11AC)
#define SUPPORTED_24G_NETTYPE_MSK WIRELESS_MODE_24G
#define SUPPORTED_5G_NETTYPE_MSK WIRELESS_MODE_5G
#define IsSupported24G(NetType) ((NetType) & SUPPORTED_24G_NETTYPE_MSK ? _TRUE : _FALSE)
#define is_supported_5g(NetType) ((NetType) & SUPPORTED_5G_NETTYPE_MSK ? _TRUE : _FALSE)

#define rtw_is_5g_band1(ch) ((ch) >= 36 && (ch) <= 48)
#define rtw_is_5g_band2(ch) ((ch) >= 52 && (ch) <= 64)
#define rtw_is_5g_band3(ch) ((ch) >= 100 && (ch) <= 144)
#define rtw_is_5g_band4(ch) ((ch) >= 149 && (ch) <= 177)

#define FUNC_ADPT_FMT "%p"
#define FUNC_ADPT_ARG(adapter) (void *)(adapter)

#define RTW_PRINT(...) do { } while (0)
#define RTW_WARN(...) do { } while (0)
#define RTW_ERR(...) do { } while (0)
#define RTW_INFO(...) do { } while (0)
#define RTW_DBG(...) do { } while (0)
#define rtw_warn_on(cond) ((void)(cond))

struct registry_priv {
	u8 wireless_mode;
	u8 excl_chs[MAX_CHANNEL_NUM];
};

struct country_chplan;

typedef struct _RT_CHANNEL_INFO {
	u8 ChannelNum;
	u8 flags;
} RT_CHANNEL_INFO;

struct rf_ctl_t {
	u8 regd_src;
	const struct country_chplan *country_ent;
	u8 ChannelPlan;
	RT_CHANNEL_INFO channel_set[MAX_CHANNEL_NUM];
};

typedef struct {
	struct registry_priv registrypriv;
	struct rf_ctl_t rf_ctl;
} _adapter;

typedef void WLAN_BSSID_EX;

#define adapter_to_regsty(adapter) (&(adapter)->registrypriv)
#define adapter_to_rfctl(adapter) (&(adapter)->rf_ctl)

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

void rtw_chplan_warn_regd_mismatch(u8 id, u8 regd_2g, u8 regd_5g);

void host_chplan_set_band_cap(u8 cap);
bool hal_chk_band_cap(_adapter *adapter, u8 cap);
u8 rtw_os_init_channel_set(_adapter *padapter, RT_CHANNEL_INFO *channel_set);

#endif /* HOST_CHPLAN_TYPES_H */
