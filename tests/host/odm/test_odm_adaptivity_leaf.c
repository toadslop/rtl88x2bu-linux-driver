// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_odm_adaptivity_types.h"

static int check_needed(void)
{
	_adapter adapter;

	host_odm_adaptivity_reset(&adapter);
	adapter.registrypriv.adaptivity_en = 0;
	if (rtw_odm_adaptivity_needed(&adapter))
		return -1;
	adapter.registrypriv.adaptivity_en = 1;
	if (!rtw_odm_adaptivity_needed(&adapter))
		return -1;
	printf("PASS needed\n");
	return 0;
}

static int check_en_msg(void)
{
	_adapter adapter;

	host_odm_adaptivity_reset(&adapter);
	adapter.registrypriv.adaptivity_en = 1;
	rtw_odm_adaptivity_en_msg((void *)1, &adapter);
	if (strcmp(host_sel_out.buf, "RTW_ADAPTIVITY_EN_ENABLE\n") != 0)
		return -1;
	printf("PASS en_msg\n");
	return 0;
}

static int check_perpkt_rssi(void)
{
	_adapter adapter;
	struct dm_struct *odm;

	host_odm_adaptivity_reset(&adapter);
	odm = adapter_to_phydm(&adapter);
	odm->rx_rate = 12;
	odm->rssi_a = 80;
	odm->rssi_b = 75;
	rtw_odm_get_perpkt_rssi((void *)1, &adapter);
	if (strcmp(host_sel_out.buf,
		   "rx_rate = MCS0, rssi_a = 80(%), rssi_b = 75(%)\n") != 0)
		return -1;
	printf("PASS perpkt_rssi\n");
	return 0;
}

int main(void)
{
	int failed = 0;

	failed += check_needed() != 0;
	failed += check_en_msg() != 0;
	failed += check_perpkt_rssi() != 0;
	if (!failed)
		printf("PASS odm adaptivity leaf smoke\n");
	return failed ? 1 : 0;
}
