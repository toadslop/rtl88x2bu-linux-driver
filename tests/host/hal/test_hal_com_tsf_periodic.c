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

struct chk_pair_out {
	systime etime;
	u32 last_timer_ms;
	unsigned set_timer_count;
	u8 last_rcr_action;
	struct _adapter *last_rcr_adapter;
	u32 rcr_chk_bssid_count;
};

static void run_chk_pair(void (*under_test)(struct _adapter *),
			 void (*oracle)(struct _adapter *),
			 struct chk_pair_out *prod_out)
{
	struct dvobj_priv snap_dv;
	struct _adapter snap_a[2];
	struct _adapter *snap_p0, *snap_p1;
	struct host_hal_com_tsf_periodic_trace snap_t;
	systime etime_u, etime_o;
	u32 timer_u, timer_o, rcr_u, rcr_o;

	snap_dv = *host_hal_com_tsf_periodic_dvobj();
	snap_p0 = snap_dv.padapters[0];
	snap_p1 = snap_dv.padapters[1];
	if (snap_p0)
		snap_a[0] = *snap_p0;
	if (snap_p1)
		snap_a[1] = *snap_p1;
	snap_t = host_hal_com_tsf_periodic_trace;

	under_test(snap_dv.padapters[0]);
	etime_u = host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime;
	timer_u = host_hal_com_tsf_periodic_trace.last_timer_ms;
	rcr_u = host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count;
	if (prod_out) {
		prod_out->etime = etime_u;
		prod_out->last_timer_ms = timer_u;
		prod_out->set_timer_count = host_hal_com_tsf_periodic_trace.set_timer_count;
		prod_out->last_rcr_action = host_hal_com_tsf_periodic_trace.last_rcr_chk_action;
		prod_out->last_rcr_adapter = host_hal_com_tsf_periodic_trace.last_rcr_adapter;
		prod_out->rcr_chk_bssid_count = rcr_u;
	}

	*host_hal_com_tsf_periodic_dvobj() = snap_dv;
	host_hal_com_tsf_periodic_dvobj()->padapters[0] = snap_p0;
	host_hal_com_tsf_periodic_dvobj()->padapters[1] = snap_p1;
	if (snap_p0)
		*snap_p0 = snap_a[0];
	if (snap_p1)
		*snap_p1 = snap_a[1];
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
	host_hal_com_tsf_periodic_set_current_time(600);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_u32("offch_timer", host_hal_com_tsf_periodic_trace.set_timer_count, 0);
	eq_u32("offch_rcr", host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count, 0);
	eq_stime("offch_etime", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 0);
}

static void test_etime_future_returns(void)
{
	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 10, 1, 2, 100);
	host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime = 5000;
	host_hal_com_tsf_periodic_set_current_time(1000);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_u32("future_rcr", host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count, 0);
	eq_u32("future_timer", host_hal_com_tsf_periodic_trace.set_timer_count, 0);
	eq_stime("future_etime", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 5000);
}

static void test_etime_at_equality(void)
{
	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 10, 1, 2, 100);
	host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime = 1000;
	host_hal_com_tsf_periodic_set_current_time(1000);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_u32("eq_etime_rcr", host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count, 0);
	eq_u32("eq_etime_timer", host_hal_com_tsf_periodic_trace.set_timer_count, 0);
	eq_stime("eq_etime", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 1000);
}

static void test_pause_at_threshold_no_arm(void)
{
	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 10, 2, 3, 100);
	host_hal_com_tsf_periodic_set_current_time(210);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_u32("thresh_timer", host_hal_com_tsf_periodic_trace.set_timer_count, 0);
	eq_u32("thresh_rcr", host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count, 0);
	eq_stime("thresh_etime", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 0);
}

static void test_pause_past_threshold_arms(void)
{
	struct chk_pair_out prod;

	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 10, 2, 3, 100);
	host_hal_com_tsf_periodic_set_current_time(211);

	run_chk_pair(rtw_hal_periodic_tsf_update_chk, rtw_hal_periodic_tsf_update_chk_c_oracle,
		     &prod);
	eq_u32("past_ms", prod.last_timer_ms, 300);
}

static void test_not_required_skips(void)
{
	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 0, 10, 1, 2, 100);
	host_hal_com_tsf_periodic_set_current_time(5000);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_u32("noreq_timer", host_hal_com_tsf_periodic_trace.set_timer_count, 0);
}

static void test_zero_pause_skips(void)
{
	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 0, 1, 2, 100);
	host_hal_com_tsf_periodic_set_current_time(5000);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_u32("nopause_timer", host_hal_com_tsf_periodic_trace.set_timer_count, 0);
}

static void test_etime_expired_next_tick(void)
{
	host_hal_com_tsf_periodic_reset();
	host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime = 1000;
	host_hal_com_tsf_periodic_set_current_time(1001);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_stime("next_etime", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 0);
	eq_u32("next_rcr", host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count, 1);
}

static void test_etime_wrap_bump(void)
{
	struct chk_pair_out prod;

	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 10, 1, 5, 100);
	host_hal_com_tsf_periodic_set_current_time((systime)-500);

	run_chk_pair(rtw_hal_periodic_tsf_update_chk, rtw_hal_periodic_tsf_update_chk_c_oracle,
		     &prod);
	eq_stime("wrap_etime", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 1);
}

static void test_etime_wrap_prod_bump(void)
{
	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 10, 1, 5, 100);
	host_hal_com_tsf_periodic_set_current_time((systime)-500);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_stime("wrap_prod_etime", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 1);
}

static void test_etime_one_blocks_scan(void)
{
	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 0, 0, 5, 100);
	host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime = 1;
	host_hal_com_tsf_periodic_set_current_time(1);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_stime("etime_one", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 1);
	eq_u32("etime_one_timer", host_hal_com_tsf_periodic_trace.set_timer_count, 0);
}

static void test_etime_wrap_idle_second_chk(void)
{
	struct chk_pair_out prod;

	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 10, 1, 5, 100);
	host_hal_com_tsf_periodic_set_current_time((systime)-500);

	run_chk_pair(rtw_hal_periodic_tsf_update_chk, rtw_hal_periodic_tsf_update_chk_c_oracle,
		     &prod);
	eq_stime("wrap_idle_setup", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 1);

	host_hal_com_tsf_periodic_set_current_time(1);
	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	run_chk_pair(rtw_hal_periodic_tsf_update_chk, rtw_hal_periodic_tsf_update_chk_c_oracle,
		     NULL);
	eq_stime("wrap_idle_etime", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 1);
	eq_u32("wrap_idle_rcr", host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count, 0);
	eq_u32("wrap_idle_timer", host_hal_com_tsf_periodic_trace.set_timer_count, 0);
}

static void test_iface_nums_respected(void)
{
	struct dvobj_priv *dv = host_hal_com_tsf_periodic_dvobj();
	struct chk_pair_out prod;

	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 10, 2, 3, 100);
	setup_iface(dv->padapters[1], 1, 10, 2, 10, 100);
	setup_iface(dv->padapters[2], 1, 10, 2, 10, 100);
	dv->iface_nums = 1;
	host_hal_com_tsf_periodic_set_current_time(211);

	run_chk_pair(rtw_hal_periodic_tsf_update_chk, rtw_hal_periodic_tsf_update_chk_c_oracle,
		     &prod);
	eq_u32("ifaces_ms", prod.last_timer_ms, 300);
}

static void test_null_iface_skipped(void)
{
	struct dvobj_priv *dv = host_hal_com_tsf_periodic_dvobj();
	struct chk_pair_out prod;
	struct _adapter *a2;

	host_hal_com_tsf_periodic_reset();
	a2 = dv->padapters[1];
	dv->padapters[0] = NULL;
	setup_iface(a2, 1, 10, 2, 3, 100);
	dv->iface_nums = 2;
	host_hal_com_tsf_periodic_set_current_time(211);

	run_chk_pair(rtw_hal_periodic_tsf_update_chk, rtw_hal_periodic_tsf_update_chk_c_oracle,
		     &prod);
	eq_u32("null_ms", prod.last_timer_ms, 300);
}

static void test_restore_max_not_last(void)
{
	struct chk_pair_out prod;
	struct _adapter *a1;
	struct _adapter *a2;

	host_hal_com_tsf_periodic_reset();
	a1 = host_hal_com_tsf_periodic_adapter();
	a2 = host_hal_com_tsf_periodic_dvobj()->padapters[1];
	setup_iface(a1, 1, 10, 1, 5, 100);
	setup_iface(a2, 1, 10, 1, 2, 100);
	host_hal_com_tsf_periodic_set_current_time(500);

	run_chk_pair(rtw_hal_periodic_tsf_update_chk, rtw_hal_periodic_tsf_update_chk_c_oracle,
		     &prod);
	eq_u32("max_ms", prod.last_timer_ms, 500);
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
	if (host_hal_com_tsf_periodic_trace.last_rcr_adapter !=
	    host_hal_com_tsf_periodic_adapter())
		fail("expired_rcr_primary");
}

static void test_restore_arm_timer(void)
{
	struct _adapter *a1;
	struct _adapter *a2;
	struct chk_pair_out prod;

	host_hal_com_tsf_periodic_reset();
	a1 = host_hal_com_tsf_periodic_adapter();
	a2 = host_hal_com_tsf_periodic_dvobj()->padapters[1];
	setup_iface(a1, 1, 10, 2, 3, 100);
	setup_iface(a2, 1, 10, 2, 5, 100);
	host_hal_com_tsf_periodic_set_current_time(260);

	run_chk_pair(rtw_hal_periodic_tsf_update_chk, rtw_hal_periodic_tsf_update_chk_c_oracle,
		     &prod);
	eq_u32("restore_ms", prod.last_timer_ms, 500);
	eq_u32("restore_timer_once", prod.set_timer_count, 1);
	eq_u8("restore_rcr", prod.last_rcr_action, MLME_ACTION_NONE);
}

static void test_timer_hdl(void)
{
	host_hal_com_tsf_periodic_reset();
	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_end_timer_hdl(host_hal_com_tsf_periodic_dvobj());
	eq_u32("hdl_cmd", host_hal_com_tsf_periodic_trace.periodic_end_cmd_count, 1);
	if (host_hal_com_tsf_periodic_trace.last_end_cmd_adapter !=
	    host_hal_com_tsf_periodic_adapter())
		fail("hdl_primary");
}

static void test_timer_hdl_stopped(void)
{
	host_hal_com_tsf_periodic_reset();
	host_hal_com_tsf_periodic_dvobj()->drv_stopped = 1;
	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_end_timer_hdl(host_hal_com_tsf_periodic_dvobj());
	eq_u32("hdl_stopped", host_hal_com_tsf_periodic_trace.periodic_end_cmd_count, 0);
}

static void test_timer_hdl_surprise(void)
{
	host_hal_com_tsf_periodic_reset();
	host_hal_com_tsf_periodic_dvobj()->surprise_removed = 1;
	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_end_timer_hdl(host_hal_com_tsf_periodic_dvobj());
	eq_u32("hdl_surprise", host_hal_com_tsf_periodic_trace.periodic_end_cmd_count, 0);
}

static void test_etime_expired_no_iface_scan(void)
{
	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 10, 1, 2, 100);
	host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime = 500;
	host_hal_com_tsf_periodic_set_current_time(1000);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_stime("exp_noscan_etime", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 0);
	eq_u32("exp_noscan_rcr", host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count, 1);
	eq_u32("exp_noscan_timer", host_hal_com_tsf_periodic_trace.set_timer_count, 0);
}

static void test_offch_after_etime_block(void)
{
	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 10, 1, 2, 100);
	host_hal_com_tsf_periodic_dvobj()->rf_ctl.offch_state = OFFCHS_LEAVE_OP;
	host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime = 500;
	host_hal_com_tsf_periodic_set_current_time(1000);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_stime("offch_etime_first", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 0);
	eq_u32("offch_etime_rcr", host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count, 1);
}

static void test_offch_leave_op_skips_restore(void)
{
	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 10, 1, 2, 100);
	host_hal_com_tsf_periodic_dvobj()->rf_ctl.offch_state = OFFCHS_LEAVE_OP;
	host_hal_com_tsf_periodic_set_current_time(5000);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_u32("leave_timer", host_hal_com_tsf_periodic_trace.set_timer_count, 0);
	eq_u32("leave_rcr", host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count, 0);
	eq_stime("leave_etime", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 0);
}

static void test_offch_backing_op_skips_restore(void)
{
	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 10, 1, 2, 100);
	host_hal_com_tsf_periodic_dvobj()->rf_ctl.offch_state = OFFCHS_BACKING_OP;
	host_hal_com_tsf_periodic_set_current_time(5000);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	eq_u32("backing_timer", host_hal_com_tsf_periodic_trace.set_timer_count, 0);
	eq_u32("backing_rcr", host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count, 0);
	eq_stime("backing_etime", host_hal_com_tsf_periodic_dvobj()->periodic_tsf_update_etime, 0);
}

static void test_restore_max_only_past_pause(void)
{
	struct chk_pair_out prod;
	struct _adapter *a1;
	struct _adapter *a2;

	host_hal_com_tsf_periodic_reset();
	a1 = host_hal_com_tsf_periodic_adapter();
	a2 = host_hal_com_tsf_periodic_dvobj()->padapters[1];
	setup_iface(a1, 1, 10, 2, 3, 100);
	setup_iface(a2, 1, 1000, 1, 10, 100);
	host_hal_com_tsf_periodic_set_current_time(211);

	run_chk_pair(rtw_hal_periodic_tsf_update_chk, rtw_hal_periodic_tsf_update_chk_c_oracle,
		     &prod);
	eq_u32("past_only_ms", prod.last_timer_ms, 300);
}

static void test_restore_rcr_primary(void)
{
	struct chk_pair_out prod;

	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 10, 2, 3, 100);
	host_hal_com_tsf_periodic_set_current_time(211);

	run_chk_pair(rtw_hal_periodic_tsf_update_chk, rtw_hal_periodic_tsf_update_chk_c_oracle,
		     &prod);
	if (prod.last_rcr_adapter != host_hal_com_tsf_periodic_adapter())
		fail("restore_rcr_primary");
}

static void test_restore_end_timer_field(void)
{
	struct dvobj_priv *dv = host_hal_com_tsf_periodic_dvobj();

	host_hal_com_tsf_periodic_reset();
	setup_iface(host_hal_com_tsf_periodic_adapter(), 1, 10, 2, 3, 100);
	host_hal_com_tsf_periodic_set_current_time(211);

	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	rtw_hal_periodic_tsf_update_chk(host_hal_com_tsf_periodic_adapter());
	if (host_hal_com_tsf_periodic_trace.last_timer_target !=
	    &dv->periodic_tsf_update_end_timer)
		fail("restore_end_timer");
}

int main(void)
{
#ifndef CONFIG_MI_WITH_MBSSID_CAM
	test_offchannel_skip();
	test_etime_future_returns();
	test_etime_at_equality();
	test_pause_at_threshold_no_arm();
	test_pause_past_threshold_arms();
	test_not_required_skips();
	test_zero_pause_skips();
	test_etime_expired_next_tick();
	test_etime_wrap_bump();
	test_etime_wrap_prod_bump();
	test_etime_one_blocks_scan();
	test_etime_wrap_idle_second_chk();
	test_etime_expired_clears();
	test_iface_nums_respected();
	test_null_iface_skipped();
	test_restore_max_not_last();
	test_restore_arm_timer();
	test_etime_expired_no_iface_scan();
	test_offch_after_etime_block();
	test_offch_leave_op_skips_restore();
	test_offch_backing_op_skips_restore();
	test_restore_max_only_past_pause();
	test_restore_rcr_primary();
	test_restore_end_timer_field();
#endif
	test_timer_hdl();
	test_timer_hdl_stopped();
	test_timer_hdl_surprise();

	if (failures) {
		fprintf(stderr, "%d failures\n", failures);
		return 1;
	}
	puts("OK");
	return 0;
}