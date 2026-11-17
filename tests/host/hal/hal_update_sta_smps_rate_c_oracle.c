// SPDX-License-Identifier: GPL-2.0
#include "host_hal_update_sta_smps_rate_types.h"

#define _TRUE 1

static const u8 dot11_rates[] = {2, 4, 11, 22, 12, 18, 24, 36, 48, 72, 96, 108, 0};

static u8 highest_rate_idx(u64 m)
{
	int i;
	for (i = 63; i >= 0; i--)
		if ((m >> i) & 1)
			return (u8)i;
	return 0;
}

static int ieee_to_bit(u8 val)
{
	unsigned bit = 1, i;
	for (i = 0; dot11_rates[i]; i++, bit <<= 1)
		if (dot11_rates[i] == val)
			return (int)bit;
	return 0;
}

static u32 ht_mcs_bitmap(u8 *m, u8 nss)
{
	u8 i;
	u32 b = 0;
	for (i = 0; i < nss; i++)
		b |= (u32)m[i] << (i * 8);
	return b;
}

static u64 vht_mcs_bitmap(u8 *map, u8 nss)
{
	u8 i, j, t;
	u64 b = 0;
	for (i = j = 0; i < nss * 2; i += 2, j += 10) {
		t = (map[i / 8] >> i) & 3;
		if (t == 2)
			b |= (u64)0x3ff << j;
		else if (t == 1)
			b |= (u64)0x1ff << j;
		else if (t == 0)
			b |= (u64)0xff << j;
	}
	return b;
}

#define is_supported_ht(m) (((m) & (WIRELESS_11_24N | WIRELESS_11_5N)) ? _TRUE : 0)

void rtw_hal_update_sta_smps_cap(struct host_adapter_smps_rate *adapter,
				 struct host_sta_smps_rate *psta)
{
	(void)adapter;
	psta->sm_ps = SM_PS_DISABLE;
}

void rtw_hal_update_sta_rate_mask(struct host_adapter_smps_rate *a,
				  struct host_sta_smps_rate *p)
{
	u8 i, tx;
	u64 mask = 0, ht;

	if (!p)
		return;
	for (i = 0; i < (int)BSS_RATES_LEN; i++)
		if (p->bssrateset[i])
			mask |= (u64)ieee_to_bit(p->bssrateset[i] & 0x7f);
	if (!a->ht_enable || !is_supported_ht(a->wireless_mode))
		goto out;
	tx = (u8)a->hal_tx_nss;
	if (p->vht_option) {
		mask |= vht_mcs_bitmap(p->vht_mcs_map, tx) << 12;
	} else if (p->ht_option) {
		if (a->ap_state && (p->smps_cap == 0 || p->smps_cap == 1) && tx > 1)
			tx = 1;
		ht = ht_mcs_bitmap(p->supp_mcs_set, tx);
		mask |= ht << 12;
	}
out:
	p->ramask = mask;
	p->init_rate = highest_rate_idx(mask) & 0x3f;
}