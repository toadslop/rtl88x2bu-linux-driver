/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal types for host L2 band-change IE update tests (W3-72).
 */
#ifndef HOST_MLME_EXT_BAND_IE_TYPES_H
#define HOST_MLME_EXT_BAND_IE_TYPES_H

#include "host_ieee80211_types.h"
#include "host_autoconf.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define _FIXED_IE_LENGTH_ 12
#define _ERPINFO_IE_ 42

#define EID_EXTCapability 127
#define EID_VHTCapability 191
#define EID_VHTOperation 192

#define REGSTY_IS_11AC_ENABLE(regsty) ((regsty)->vht_enable != 0)
#define REGSTY_IS_11AC_AUTO(regsty) ((regsty)->vht_enable == 2)
#define is_supported_vht(NetType) ((NetType) & (WIRELESS_11AC) ? _TRUE : _FALSE)

#define COUNTRY_CHPLAN_EN_11AC(_ent) ((_ent) ? (_ent)->en_11ac : 0)

typedef unsigned int uint;

typedef struct _country_ent {
	u8 en_11ac;
} country_ent_t;

struct rf_ctl_t {
	country_ent_t *country_ent;
};

struct ht_priv {
	u8 ht_option;
};

struct vht_priv {
	u8 vht_option;
};

struct registry_priv {
	u32 wireless_mode;
	u8 vht_enable;
};

struct mlme_priv {
	struct ht_priv htpriv;
	u8 ori_vht_en;
	struct vht_priv vhtpriv;
};

typedef struct _adapter {
	struct registry_priv registrypriv;
	struct mlme_priv mlmepriv;
	struct rf_ctl_t rfctl;
} _adapter;

static inline uint get_WLAN_BSSID_EX_sz(WLAN_BSSID_EX *bss)
{
	return (uint)(sizeof(WLAN_BSSID_EX) - HOST_IEEE80211_MAX_IE_SZ + bss->IELength);
}

#define adapter_to_rfctl(adapter) (&(adapter)->rfctl)

void change_band_update_ie(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 ch);

void rtw_add_bcn_ie(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 index,
		    u8 *data, u8 len);
void rtw_remove_bcn_ie(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 index);
void rtw_set_supported_rate(u8 *SupportedRates, unsigned int mode);
void UpdateBrateTbl(_adapter *adapter, u8 *mBratesOS);

void rtw_vht_ies_attach(_adapter *padapter, WLAN_BSSID_EX *pnetwork);
void rtw_vht_ies_detach(_adapter *padapter, WLAN_BSSID_EX *pnetwork);

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit);
void *rtw_malloc(size_t sz);
void rtw_mfree(void *p, size_t sz);

#endif /* HOST_MLME_EXT_BAND_IE_TYPES_H */
