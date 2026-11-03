/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-212/W3-213 proc TDLS debug display. */
#ifndef HOST_RTW_DEBUG_PROC_TDLS_TYPES_H
#define HOST_RTW_DEBUG_PROC_TDLS_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef unsigned long _irqL;
typedef int _lock;
typedef long ssize_t;

#define _TRUE 1
#define _FALSE 0
#define BIT(x) (1U << (x))
#define ETH_ALEN 6

#define MAX_ALLOWED_TDLS_STA_NUM 4
#define TDLS_DBG_INFO_SPACE_BTWN_ITEM_AND_VALUE 41
#define WIFI_ASOC_STATE 0x00000001
#define WIFI_STATION_STATE 0x00000008

#define WIRELESS_11B BIT(0)
#define WIRELESS_11G BIT(1)
#define WIRELESS_11A BIT(2)
#define WIRELESS_11_24N BIT(3)
#define WIRELESS_11_5N BIT(4)
#define WIRELESS_AUTO BIT(5)
#define WIRELESS_11AC BIT(6)

#define _NO_PRIVACY_ 0x00
#define _WEP40_ 0x01
#define _TKIP_ 0x02
#define _TKIP_WTMIC_ 0x03
#define _AES_ 0x04
#define _WEP104_ 0x05
#define _SMS4_ 0x06

#define HAL_PRIME_CHNL_OFFSET_DONT_CARE 0
#define HAL_PRIME_CHNL_OFFSET_LOWER 1
#define HAL_PRIME_CHNL_OFFSET_UPPER 2

#define CHANNEL_WIDTH_20 0
#define CHANNEL_WIDTH_40 1
#define CHANNEL_WIDTH_80 2
#define CHANNEL_WIDTH_160 3
#define CHANNEL_WIDTH_80_80 4
#define CHANNEL_WIDTH_5 5
#define CHANNEL_WIDTH_10 6

#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(x) (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5]

struct registry_priv {
	u8 en_tdls;
};

struct tdls_info {
	u8 ap_prohibited;
	u8 ch_switch_prohibited;
	u8 link_established;
	u8 sta_cnt;
	u8 sta_maximum;
	u8 dev_discovered;
	u8 driver_setup;
};

struct WLAN_SSID {
	u8 Ssid[33];
	u8 SsidLength;
};

struct WLAN_BSSID_EX {
	struct WLAN_SSID Ssid;
	u8 MacAddress[ETH_ALEN];
};

struct wlan_network {
	struct WLAN_BSSID_EX network;
};

struct mlme_priv {
	u32 fw_state;
	struct wlan_network cur_network;
};

struct mlme_ext_priv {
	u8 cur_wireless_mode;
	u8 cur_channel;
	u8 cur_ch_offset;
	u8 cur_bwmode;
};

struct security_priv {
	u32 dot11PrivacyAlgrthm;
};

struct _adapter {
	struct registry_priv registrypriv;
	struct tdls_info tdlsinfo;
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
	struct security_priv securitypriv;
};

struct net_device {
	void *priv;
};

struct seq_file {
	void *private;
	FILE *out;
};

#define RTW_PRINT_SEL(sel, fmt, ...)                                           \
	fprintf(((struct seq_file *)(sel))->out ? ((struct seq_file *)(sel))->out \
					       : stdout,                           \
		fmt, ##__VA_ARGS__)

#define rtw_warn_on(c) ((void)(c))

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

u8 rtw_is_tdls_enabled(struct _adapter *padapter);
void rtw_enable_tdls_func(struct _adapter *padapter);
void rtw_disable_tdls_func(struct _adapter *padapter, u8 enqueue_cmd);

int proc_get_tdls_enable(struct seq_file *m, void *v);
ssize_t proc_set_tdls_enable(void *file, const void *buffer, size_t count,
			     long *pos, void *data);
int proc_tdls_display_tdls_function_info(struct seq_file *m);
int proc_tdls_display_network_info(struct seq_file *m);

#endif /* HOST_RTW_DEBUG_PROC_TDLS_TYPES_H */
