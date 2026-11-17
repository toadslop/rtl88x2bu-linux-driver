// SPDX-License-Identifier: GPL-2.0
/* L1 ref: rtw_hal_update_sta_smps_cap / rtw_hal_update_sta_rate_mask (W4-99). */

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;
struct _adapter;
struct sta_info;

void rtw_rust_hal_com_sta_set_sm_ps(struct sta_info *p, u8 v);
u8 rtw_rust_hal_com_sta_mac_id(struct sta_info *p);
void rtw_rust_hal_com_log_sta_sm_ps(u8 mac, u8 sm);
u8 rtw_rust_hal_com_adapter_ht_enabled(struct _adapter *a);
u8 rtw_rust_hal_com_adapter_wireless_mode(struct _adapter *a);
u8 rtw_rust_hal_com_get_hal_tx_nss(struct _adapter *a);
u8 rtw_rust_hal_com_mlmepriv_is_ap(struct _adapter *a);
u8 rtw_rust_hal_com_sta_bssrate(struct sta_info *p, unsigned int i);
u8 rtw_rust_hal_com_sta_ht_option(struct sta_info *p);
u8 rtw_rust_hal_com_sta_ht_smps_cap(struct sta_info *p);
u8 rtw_rust_hal_com_sta_ht_supp_mcs(struct sta_info *p, unsigned int i);
u8 rtw_rust_hal_com_sta_vht_option(struct sta_info *p);
u8 rtw_rust_hal_com_sta_vht_mcs_map(struct sta_info *p, unsigned int i);
void rtw_rust_hal_com_sta_set_ramask(struct sta_info *p, u64 m);
void rtw_rust_hal_com_sta_set_init_rate(struct sta_info *p, u8 r);
int rtw_get_bit_value_from_ieee_value(u8 val);
u32 rtw_ht_mcs_set_to_bitmap(u8 *m, u8 nss);
u64 rtw_vht_mcs_map_to_bitmap(u8 *m, u8 nss);
u8 get_highest_rate_idx(u64 mask);

void rtw_hal_update_sta_smps_cap(struct _adapter *adapter, struct sta_info *psta)
{
	u8 mac;

	(void)adapter;
	if (!psta)
		return;
	rtw_rust_hal_com_sta_set_sm_ps(psta, 3);
	mac = rtw_rust_hal_com_sta_mac_id(psta);
	rtw_rust_hal_com_log_sta_sm_ps(mac, 3);
}

void rtw_hal_update_sta_rate_mask(struct _adapter *padapter, struct sta_info *psta)
{
	u8 i, tx;
	u64 mask = 0, ht;
	u8 mcs[16], vmap[2];

	if (!psta)
		return;
	for (i = 0; i < 16; i++) {
		u8 r = rtw_rust_hal_com_sta_bssrate(psta, i);
		if (r)
			mask |= (u64)rtw_get_bit_value_from_ieee_value(r & 0x7f);
	}
	if (rtw_rust_hal_com_adapter_ht_enabled(padapter) &&
	    (rtw_rust_hal_com_adapter_wireless_mode(padapter) & 0x18)) {
		tx = rtw_rust_hal_com_get_hal_tx_nss(padapter);
		if (rtw_rust_hal_com_sta_vht_option(psta)) {
			vmap[0] = rtw_rust_hal_com_sta_vht_mcs_map(psta, 0);
			vmap[1] = rtw_rust_hal_com_sta_vht_mcs_map(psta, 1);
			mask |= rtw_vht_mcs_map_to_bitmap(vmap, tx) << 12;
		} else if (rtw_rust_hal_com_sta_ht_option(psta)) {
			for (i = 0; i < 16; i++)
				mcs[i] = rtw_rust_hal_com_sta_ht_supp_mcs(psta, i);
			if (rtw_rust_hal_com_mlmepriv_is_ap(padapter) &&
			    (rtw_rust_hal_com_sta_ht_smps_cap(psta) == 0 ||
			     rtw_rust_hal_com_sta_ht_smps_cap(psta) == 1) &&
			    tx > 1)
				tx = 1;
			ht = rtw_ht_mcs_set_to_bitmap(mcs, tx);
			mask |= ht << 12;
		}
	}
	rtw_rust_hal_com_sta_set_ramask(psta, mask);
	rtw_rust_hal_com_sta_set_init_rate(psta, get_highest_rate_idx(mask) & 0x3f);
}