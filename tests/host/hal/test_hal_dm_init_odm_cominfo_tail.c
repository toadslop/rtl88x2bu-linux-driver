// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_hal_dm_init_odm_cominfo_tail_types.h"

struct host_hal_dm_init_odm_tail_trace host_hal_dm_init_odm_tail_trace;

void host_hal_dm_init_odm_tail_reset(void)
{
	memset(&host_hal_dm_init_odm_tail_trace, 0, sizeof(host_hal_dm_init_odm_tail_trace));
}

static void tail_record(u8 kind, u32 id, void *ptr)
{
	struct host_hal_dm_init_odm_tail_trace *t = &host_hal_dm_init_odm_tail_trace;

	if (t->count >= HOST_TAIL_MAX_EVENTS)
		return;
	t->events[t->count].kind = kind;
	t->events[t->count].id = id;
	t->events[t->count].ptr = ptr;
	t->count++;
}

void odm_cmn_info_hook(struct dm_struct *dm, u32 cmn_info, void *ptr)
{
	(void)dm;
	tail_record(HOST_TAIL_EVT_ODM_HOOK, cmn_info, ptr);
}

void halrf_cmn_info_hook(struct dm_struct *dm, u8 id, void *ptr)
{
	(void)dm;
	tail_record(HOST_TAIL_EVT_HALRF_HOOK, id, ptr);
}

void phydm_cmn_sta_info_hook(struct dm_struct *dm, u8 macid, void *sta)
{
	(void)dm;
	tail_record(HOST_TAIL_EVT_STA_HOOK, macid, sta);
}

void rtw_phydm_ops_func_init(struct dm_struct *dm)
{
	(void)dm;
	tail_record(HOST_TAIL_EVT_OPS_INIT, 0, NULL);
}

void phydm_dm_early_init(struct dm_struct *dm)
{
	(void)dm;
	tail_record(HOST_TAIL_EVT_EARLY_INIT, 0, NULL);
}

int host_hal_dm_init_odm_tail_check_expected(struct _adapter *adapter)
{
	static const u32 expect_ids[] = {58, 64, 65, 33, 34, 70, 31, 73};
	struct host_hal_dm_init_odm_tail_trace *t = &host_hal_dm_init_odm_tail_trace;
	struct hal_data *hal = &adapter->hal;
	struct dvobj_priv *dvobj = &adapter->dvobj;
	struct pwrctrl_priv *pwr = adapter_to_pwrctl(adapter);
	void *const expect_ptrs[] = {
		&hal->current_channel,
		&hal->b_scan_in_process,
		&pwr->bpower_saving,
		&dvobj->traffic_stat.cur_tx_tp,
		&dvobj->traffic_stat.cur_rx_tp,
		&hal->antenna_test,
		&dvobj->usb_speed,
		&hal->multi_ch_switch_mode,
	};
	size_t i;
	const size_t n_odm = sizeof(expect_ids) / sizeof(expect_ids[0]);
	const size_t sta_start = n_odm + 1; /* +1 halrf hook */

	if (!adapter || !pwr)
		return 1;

	if ((size_t)t->count != sta_start + HOST_HAL_ODM_ASSOCIATE_ENTRY_NUM + 2)
		return 1;

	for (i = 0; i < n_odm; i++) {
		const struct host_tail_event *got = &t->events[i];

		if (got->kind != HOST_TAIL_EVT_ODM_HOOK ||
		    got->id != expect_ids[i] || got->ptr != expect_ptrs[i])
			return 1;
	}

	{
		const struct host_tail_event *got = &t->events[n_odm];

		if (got->kind != HOST_TAIL_EVT_HALRF_HOOK || got->id != 4 ||
		    got->ptr != &adapter->registrypriv.halrf_ability)
			return 1;
	}

	for (i = 0; i < HOST_HAL_ODM_ASSOCIATE_ENTRY_NUM; i++) {
		const struct host_tail_event *got = &t->events[sta_start + i];

		if (got->kind != HOST_TAIL_EVT_STA_HOOK ||
		    got->id != (u32)i || got->ptr != NULL)
			return 1;
	}

	i = sta_start + HOST_HAL_ODM_ASSOCIATE_ENTRY_NUM;
	if (t->events[i].kind != HOST_TAIL_EVT_OPS_INIT ||
	    t->events[i + 1].kind != HOST_TAIL_EVT_EARLY_INIT)
		return 1;

	return 0;
}

struct fixture {
	struct _adapter adapter;
};

static int run_case(void)
{
	struct fixture fx;

	memset(&fx, 0, sizeof(fx));
	fx.adapter.hal.current_channel = 36;
	fx.adapter.hal.b_scan_in_process = 1;
	fx.adapter.hal.antenna_test = 2;
	fx.adapter.hal.multi_ch_switch_mode = 3;
	fx.adapter.dvobj.usb_speed = 4;
	fx.adapter.dvobj.traffic_stat.cur_tx_tp = 100;
	fx.adapter.dvobj.traffic_stat.cur_rx_tp = 200;
	fx.adapter.registrypriv.halrf_ability = 0x5a5a5a5a;
	fx.adapter.pwrctl.bpower_saving = 1;

	host_hal_dm_init_odm_tail_reset();
	rtw_rust_hal_dm_init_odm_cominfo_tail(&fx.adapter);

	if (host_hal_dm_init_odm_tail_check_expected(&fx.adapter))
		return 1;
	return 0;
}

int main(void)
{
	if (run_case())
		return fprintf(stderr, "FAIL hal_dm_init_odm_cominfo_tail L2\n"), 1;
	printf("OK hal_dm_init_odm_cominfo_tail L2\n");
	return 0;
}