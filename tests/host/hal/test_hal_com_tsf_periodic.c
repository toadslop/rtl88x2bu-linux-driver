// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-130 periodic TSF restore check. */

#include "host_hal_com_tsf_periodic_types.h"
#include <stdio.h>
#include <string.h>

struct dvobj_priv *adapter_to_dvobj(struct _adapter *adapter);
struct _adapter *dvobj_get_primary_adapter(struct dvobj_priv *dvobj);
systime rtw_get_current_time(void);
systime rtw_ms_to_systime(int ms);
int rtw_time_after(systime a, systime b);
u32 rtw_get_passing_time_ms(systime start);
void _set_timer(struct _timer *t, u32 ms);
void rtw_hal_rcr_set_chk_bssid(struct _adapter *adapter, u8 self_action);
u8 rtw_periodic_tsf_update_end_cmd(struct _adapter *adapter);
int dev_is_surprise_removed(struct dvobj_priv *dvobj);
int dev_is_drv_stopped(struct dvobj_priv *dvobj);

void rtw_hal_periodic_tsf_update_chk_c_oracle(struct _adapter *adapter);
void rtw_hal_periodic_tsf_update_end_timer_hdl_c_oracle(void *ctx);

static int failures;

static void fail(const char *tag)
{
	fprintf(stderr, "FAIL %s\n", tag);
	failures++;
}

static void eq_u32(const char *tag, u32 a, u32 b)
{
	if (a != b)
		fail(tag);
}

static void eq_u8(const char *tag, u8 a, u8 b)
{
	if (a != b)
		fail(tag);
}

static void eq_stime(const char *tag, systime a, systime b)
{
	if (a != b)
		fail(tag);
}

static void setup_iface(struct _adapter *a, u8 required, systime pause_stime,
			u8 pause_factor, u8 restore_factor, u16 bcn_interval)
{
	a->mlmeextpriv.tsf_update_required = required;
	a->mlmeextpriv.tsf_update_pause_stime = pause_stime;
	a->mlmeextpriv.tsf_update_pause_factor = pause_factor;
	a->mlmeextpriv.tsf_update_restore_factor = restore_factor;
	a->mlmeextpriv.mlmext_info.bcn_interval = bcn_interval;
}

static void run_chk_pair(void (*under_test)(struct _adapter *),
			 void (*oracle)(struct _adapter *))
{
	struct dvobj_priv snap_dv;
	struct _adapter snap_a[2];
	struct host_hal_com_tsf_periodic_trace snap_t;
	systime etime_u, etime_o;
	u32 timer_u, timer_o, rcr_u, rcr_o;

	snap_dv = *host_hal_com_tsf_periodic_dvobj();
	snap_a[0] = *snap_dv.padapters[0];
	snap_a[1] = *snap_dv.padapters[1];
	snap_t = host_hal_com_tsf_periodic_trace;

	under_test(snap_dv.padapters[0]);
	etime_u = host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime;
	timer_u = host_hal_com_tsf_periodic_trace.last_timer_ms;
	rcr_u = host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count;

	*host_hal_com_tsf_periodic_dvobj() = snap_dv;
	*snap_dv.padapters[0] = snap_a[0];
	*snap_dv.padapters[1] = snap_a[1];
	host_hal_com_tsf_periodic_trace = snap_t;

	oracle(snap_dv.padapters[0]);
	etime_o = host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime;
	timer_o = host_hal_com_tsf_periodic_trace.last_timer_ms;
	rcr_o = host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count;

	eq_stime("chk_etime", etime_u, etime_o);
	eq_u32("chk_timer", timer_u, timer_o);
	eq_u32("chk_rcr", rcr_u, rcr_o);
}

static void test_offchannel_skip(void)
{
	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 1, 1, 2, 100);
	host_hal_com_tsf_periodic_dvobj()->rf_ctl.offch_state = 1;
	host_hal_com_tsf_periodic_set_passing_ms(500);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_u32("offch_timer", host_hal_com_tsf_periodic_trace.set_timer_count, 0);
	eq_stime("offch_etime", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 0);
}

static void test_etime_future_returns(void)
{
	host_hal_com_tsf_periodic_reset();
	host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime = 5000;
	host_hal_com_tsf_periodic_set_current_time(1000);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_u32("future_rcr", host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count, 0);
	eq_stime("future_etime", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 5000);
}

static void test_etime_expired_clears(void)
{
	host_hal_com_tsf_periodic_reset();
	host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime = 500;
	host_hal_com_tsf_periodic_set_current_time(1000);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_stime("expired_etime", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 0);
	eq_u32("expired_rcr", host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count, 1);
	eq_u8("expired_action", host_hal_com_tsf_periodic_trace.last_rcr_chk_action, MLME_ACTION_NONE);
}

static void test_restore_arm_timer(void)
{
	struct _adapter *a1 = host_hal_com_tsf_periodic_adapter();
	struct _adapter *a2 = host_hal_com_tsf_periodic_dvobj()->padapters[1];

	host_hal_com_tsf_periodic_reset();
	setup_iface(a1, 1, 10, 2, 3, 100);
	setup_iface(a2, 1, 10, 2, 5, 100);
	host_hal_com_tsf_periodic_set_passing_ms(250);

	run_chk_pair(rtw_hal_periodic_tsf_update_chk, rtw_hal_periodic_tsf_update_chk_c_oracle);
	eq_u32("restore_ms", host_hal_com_tsf_periodic_trace.last_timer_ms, 500);
	eq_u32("restore_timer_once", host_hal_com_tsf_periodic_trace.set_timer_count, 1);
	eq_u8("restore_rcr", host_hal_com_tsf_periodic_trace.last_rcr_chk_action, MLME_ACTION_NONE);
}

static void test_timer_hdl(void)
{
	host_hal_com_tsf_periodic_reset();
	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_end_timer_hdl(host_hal_com_tsf_periodic_dvobj());
	eq_u32("hdl_cmd", host_hal_com_tsf_periodic_trace.periodic_end_cmd_count, 1);
}

static void test_timer_hdl_stopped(void)
{
	host_hal_com_tsf_periodic_reset();
	host_hal_com_tsf_periodic_dvobj()->drv_stopped = 1;
	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_end_timer_hdl(host_hal_com_tsf_periodic_dvobj());
	eq_u32("hdl_stopped", host_hal_com_tsf_periodic_trace.periodic_end_cmd_count, 0);
}

int main(void)
{
#ifndef CONFIG_MI_WITH_MBSSID_CAM
	test_offchannel_skip();
	test_etime_future_returns();
	test_etime_expired_clears();
	test_restore_arm_timer();
#endif
	test_timer_hdl();
	test_timer_hdl_stopped();

	if (failures) {
		fprintf(stderr, "%d failures\n", failures);
		return 1;
	}
	puts("OK");
	return 0;
}