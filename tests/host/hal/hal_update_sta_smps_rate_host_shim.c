// SPDX-License-Identifier: GPL-2.0
/* Host shims: exercise kernel Rust (FFI path) for W4-99 STA SMPS / rate mask L2. */

#include "host_hal_update_sta_smps_rate_types.h"

#define BIT(n) (1U << (n))

static const u8 dot11_rates[] = {2, 4, 11, 22, 12, 18, 24, 36, 48, 72, 96, 108, 0};

u8 get_highest_rate_idx(u64 mask)
{
	int i;

	for (i = 63; i >= 0; i--)
		if ((mask >> i) & 1)
			return (u8)i;
	return 0;
}

int rtw_get_bit_value_from_ieee_value(u8 val)
{
	unsigned bit = 1;
	unsigned i;

	for (i = 0; dot11_rates[i]; i++, bit <<= 1)
		if (dot11_rates[i] == val)
			return (int)bit;
	return 0;
}

u32 rtw_ht_mcs_set_to_bitmap(u8 *mcs_set, u8 nss)
{
	u8 i;
	u32 bitmap = 0;

	for (i = 0; i < nss; i++)
		bitmap |= (u32)mcs_set[i] << (i * 8);
	return bitmap;
}

u64 rtw_vht_mcs_map_to_bitmap(u8 *mcs_map, u8 nss)
{
	u8 i, j, tmp;
	u64 bitmap = 0;
	u8 bits = nss * 2;

	for (i = j = 0; i < bits; i += 2, j += 10) {
		tmp = (mcs_map[i / 8] >> i) & 3;
		if (tmp == 2)
			bitmap |= (u64)0x3ff << j;
		else if (tmp == 1)
			bitmap |= (u64)0x1ff << j;
		else if (tmp == 0)
			bitmap |= (u64)0xff << j;
	}
	return bitmap;
}

void rtw_rust_hal_com_sta_set_sm_ps(void *psta, u8 sm)
{
	struct host_sta_smps_rate *p = psta;

	if (p)
		p->sm_ps = sm;
}

u8 rtw_rust_hal_com_sta_mac_id(void *psta)
{
	struct host_sta_smps_rate *p = psta;

	return p ? p->mac_id : 0;
}

void rtw_rust_hal_com_log_sta_sm_ps(u8 mac, u8 sm)
{
	(void)mac;
	(void)sm;
}

u8 rtw_rust_hal_com_adapter_ht_enabled(void *adapter)
{
	struct host_adapter_smps_rate *a = adapter;

	return a ? a->ht_enable : 0;
}

u8 rtw_rust_hal_com_adapter_wireless_mode(void *adapter)
{
	struct host_adapter_smps_rate *a = adapter;

	return a ? a->wireless_mode : 0;
}

u8 rtw_rust_hal_com_get_hal_tx_nss(void *adapter)
{
	struct host_adapter_smps_rate *a = adapter;

	if (!a || a->hal_tx_nss < 0)
		return 1;
	return (u8)a->hal_tx_nss;
}

u8 rtw_rust_hal_com_mlmepriv_is_ap(void *adapter)
{
	struct host_adapter_smps_rate *a = adapter;

	return a ? a->ap_state : 0;
}

u8 rtw_rust_hal_com_sta_bssrate(void *psta, unsigned int i)
{
	struct host_sta_smps_rate *p = psta;

	if (!p || i >= BSS_RATES_LEN)
		return 0;
	return p->bssrateset[i];
}

u8 rtw_rust_hal_com_sta_ht_option(void *psta)
{
	struct host_sta_smps_rate *p = psta;

	return p ? p->ht_option : 0;
}

u8 rtw_rust_hal_com_sta_ht_smps_cap(void *psta)
{
	struct host_sta_smps_rate *p = psta;

	return p ? p->smps_cap : 0;
}

u8 rtw_rust_hal_com_sta_ht_supp_mcs(void *psta, unsigned int i)
{
	struct host_sta_smps_rate *p = psta;

	if (!p || i >= HT_MCS_SET_LEN)
		return 0;
	return p->supp_mcs_set[i];
}

u8 rtw_rust_hal_com_sta_vht_option(void *psta)
{
	struct host_sta_smps_rate *p = psta;

	return p ? p->vht_option : 0;
}

u8 rtw_rust_hal_com_sta_vht_mcs_map(void *psta, unsigned int i)
{
	struct host_sta_smps_rate *p = psta;

	if (!p || i >= VHT_MCS_MAP_LEN)
		return 0;
	return p->vht_mcs_map[i];
}

void rtw_rust_hal_com_sta_set_ramask(void *psta, u64 mask)
{
	struct host_sta_smps_rate *p = psta;

	if (p)
		p->ramask = mask;
}

void rtw_rust_hal_com_sta_set_init_rate(void *psta, u8 rate)
{
	struct host_sta_smps_rate *p = psta;

	if (p)
		p->init_rate = rate;
}