// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_hal_dm_phydm_ops_types.h"

static int run_fixture(void)
{
	host_adapter adapter;
	struct dm_struct phydm;
	struct cmn_sta_info sta;
	u64 ra_mask = 0x00ff00ff000000aaULL;
	void (*saved_record)(void *, u8, struct cmn_sta_info *, u64);

	memset(&adapter, 0, sizeof(adapter));
	memset(&phydm, 0, sizeof(phydm));
	memset(&sta, 0, sizeof(sta));

	adapter.dvobj.macid_ctl.num = HOST_MACID_CTL_NUM;
	phydm.adapter = &adapter;
	sta.ra_info.ra_bw_mode = CHANNEL_WIDTH_80;
	sta.ra_info.is_vht_enable = 2;

	host_hal_dm_phydm_ops_reset();
	rtw_phydm_ops_func_init(&phydm);

	saved_record = phydm.dm_ra_table.record_ra_info;
	if (!saved_record)
		return 4;
#ifndef RUST_HAL_DM_PHYDM_OPS_ORACLE
	if (!host_hal_dm_phydm_ops_trace.init_called)
		return 1;
	if (!host_hal_dm_phydm_ops_trace.record_ptr_set)
		return 2;
#ifdef CONFIG_SUPPORT_DYNAMIC_TXPWR
	if (!host_hal_dm_phydm_ops_trace.fill_desc_ptr_set)
		return 3;
	if (phydm.fill_desc_dyntxpwr != rtw_phydm_fill_desc_dpt)
		return 3;
#endif
#else
#ifdef CONFIG_SUPPORT_DYNAMIC_TXPWR
	if (phydm.fill_desc_dyntxpwr == NULL)
		return 3;
	if (phydm.fill_desc_dyntxpwr != rtw_phydm_fill_desc_dpt)
		return 3;
#endif
#endif

	saved_record(&phydm, 2, &sta, ra_mask);

#ifndef RUST_HAL_DM_PHYDM_OPS_ORACLE
	if (host_hal_dm_phydm_ops_trace.record_calls != 1)
		return 10;
#endif
	if (host_hal_dm_phydm_ops_trace.last_macid != 2)
		return 11;
	if (host_hal_dm_phydm_ops_trace.last_bw != CHANNEL_WIDTH_80)
		return 12;
	if (host_hal_dm_phydm_ops_trace.last_vht_en != 2)
		return 13;
	if (host_hal_dm_phydm_ops_trace.last_bmp0 != 0x000000aaU)
		return 14;
	if (host_hal_dm_phydm_ops_trace.last_bmp1 != 0x00ff00ffU)
		return 15;
	if (host_hal_dm_phydm_ops_trace.update_tx_rate_bmp_calls != 1)
		return 16;
	if (adapter.dvobj.macid_ctl.bw[2] != CHANNEL_WIDTH_80)
		return 17;
	if (adapter.dvobj.macid_ctl.vht_en[2] != 2)
		return 18;
	if (adapter.dvobj.macid_ctl.rate_bmp0[2] != 0x000000aaU)
		return 19;
	if (adapter.dvobj.macid_ctl.rate_bmp1[2] != 0x00ff00ffU)
		return 20;

	host_hal_dm_phydm_ops_reset();
	saved_record(&phydm, 1, NULL, ra_mask);
	if (host_hal_dm_phydm_ops_trace.record_calls != 0)
		return 30;

	return 0;
}

int main(void)
{
	int rc = run_fixture();

	if (rc == 0) {
		printf("hal_dm_phydm_ops: OK\n");
		return 0;
	}
	printf("hal_dm_phydm_ops: FAIL (%d)\n", rc);
	return 1;
}
