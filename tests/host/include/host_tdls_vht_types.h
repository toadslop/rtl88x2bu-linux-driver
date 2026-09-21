/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_TDLS_VHT_TYPES_H
#define HOST_TDLS_VHT_TYPES_H

#include "host_types.h"
#include <string.h>

#define _TRUE 1
#define _FALSE 0
#define BIT(x) (1U << (x))
#define TEST_FLAG(f, t) (((f) & (t)) != 0)
#define SET_FLAG(f, s) ((f) |= (s))
#define WLAN_STA_VHT (1U << 14)
#define WIRELESS_11AC (1U << 5)
#define CHANNEL_WIDTH_80 2
#define LDPC_VHT_ENABLE_TX BIT(0)
#define LDPC_VHT_CAP_TX BIT(1)
#define STBC_VHT_ENABLE_TX BIT(0)
#define STBC_VHT_CAP_TX BIT(1)
#define BIT_LEN_MASK_32(n) ((u32)(0xFFFFFFFFu >> (32 - (n))))

static inline u32 le_bits(const u8 *p, u32 off, u32 len)
{
	u32 v = 0, i;

	for (i = 0; i < len; i++) {
		u32 b = (off + i) / 8, bit = (off + i) % 8;

		if (p[b] & (1U << bit))
			v |= (1U << i);
	}
	return v & BIT_LEN_MASK_32(len);
}

#define LE1(p, o, l) ((u8)le_bits((p), (o), (l)))
#define LE2(p, o, l) ((u16)le_bits((p), (o), (l)))
#define GET_VHT_CAPABILITY_ELE_RX_LDPC(p) LE1((p), 4, 1)
#define GET_VHT_CAPABILITY_ELE_SHORT_GI80M(p) LE1((p), 5, 1)
#define GET_VHT_CAPABILITY_ELE_RX_STBC(p) LE1((p) + 1, 0, 3)
#define GET_VHT_CAPABILITY_ELE_MAX_RXAMPDU_FACTOR(p) LE2((p) + 2, 7, 3)
#define GET_VHT_OPERATION_ELE_CHL_WIDTH(p) LE1((p), 0, 8)
#define GET_VHT_OPERATING_MODE_FIELD_CHNL_WIDTH(p) LE1((p), 0, 2)
#define GET_VHT_OPERATING_MODE_FIELD_RX_NSS(p) LE1((p), 4, 3)
#define _rtw_memset(p, c, n) memset((p), (c), (n))
#define _rtw_memcpy(d, s, n) memcpy((d), (s), (n))

struct vht_priv {
	u8 vht_cap[12];
	u8 vht_mcs_map[2];
	u8 vht_op_mode_notify;
	u8 vht_option;
	u8 ldpc_cap;
	u8 stbc_cap;
	u8 sgi_80m;
	u8 ampdu_len;
	u8 vht_highest_rate;
};

struct registry_priv {
	u8 vht_enable;
	u8 wireless_mode;
};

struct mlme_ext_priv {
	u8 cur_bwmode;
};

struct mlme_priv {
	struct vht_priv vhtpriv;
};

#define GET_VHT_CAPABILITY_ELE_RX_MCS(p) ((p) + 4)

struct sta_info {
	int flags;
	struct vht_priv vhtpriv;
	u8 bw_mode;
	u8 ra_is_vht;
};

struct rf_ctl_t {
	void *country_ent;
};

struct _adapter {
	struct registry_priv registrypriv;
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
	struct rf_ctl_t rfctl;
};

typedef struct _adapter _adapter;

#define REGSTY_BW_5G(r) 2
#define REGSTY_IS_BW_5G_SUPPORT(r, bw) (REGSTY_BW_5G(r) >= (bw))
#define REGSTY_IS_11AC_ENABLE(r) ((r)->vht_enable != 0)
#define adapter_to_regsty(a) (&(a)->registrypriv)
#define adapter_to_rfctl(a) (&(a)->rfctl)
#define COUNTRY_CHPLAN_EN_11AC(e) 1

static inline u8 is_supported_vht(u8 mode)
{
	return (mode & WIRELESS_11AC) ? _TRUE : _FALSE;
}

void host_tdls_set_hal_tx_nss(u8 nss);
u8 host_tdls_hal_tx_nss(_adapter *a);
u8 rtw_get_vht_highest_rate(u8 *map);
void rtw_vht_nss_to_mcsmap(u8 nss, u8 *target, u8 *cur);
void rtw_tdls_process_vht_cap(_adapter *a, struct sta_info *s, u8 *d, u8 len);
u8 host_tdls_hal_bw_support(_adapter *a, u8 bw);
u8 rtw_vht_mcsmap_to_nss(u8 *map);
void rtw_tdls_process_vht_operation(_adapter *a, struct sta_info *s, u8 *d, u8 len);
void rtw_tdls_process_vht_op_mode_notify(_adapter *a, struct sta_info *s, u8 *d, u8 len);

#endif