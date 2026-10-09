// SPDX-License-Identifier: GPL-2.0
/* Host C reference for W3-132 test hooks (L1 + L2 oracle). */
#include "host_rtw_debug_test_hooks_types.h"

static u8 fwdl_test_chksum_fail;
static u8 fwdl_test_wintint_rdy_fail;
static u8 del_rx_ampdu_test_no_tx_fail;
static u32 g_wait_hiq_empty_ms;
static systime sta_linking_test_start_time;
static u32 sta_linking_test_wait_ms;
static u8 sta_linking_test_force_fail;
static u16 ap_linking_test_force_auth_fail;
static u16 ap_linking_test_force_asoc_fail;

static u32 host_passing_time_ms;

void host_debug_test_hooks_set_passing_ms(u32 ms)
{
	host_passing_time_ms = ms;
}

static systime host_get_current_time(void)
{
	return 0;
}

static u32 host_get_passing_time_ms(systime start)
{
	(void)start;
	return host_passing_time_ms;
}

void host_set_fwdl_test_case(u8 chksum, u8 wintint)
{
	fwdl_test_chksum_fail = chksum;
	fwdl_test_wintint_rdy_fail = wintint;
}

void host_set_del_rx_ampdu_test_case(u8 no_tx_fail)
{
	del_rx_ampdu_test_no_tx_fail = no_tx_fail;
}

void host_set_wait_hiq_empty_ms(u32 ms)
{
	g_wait_hiq_empty_ms = ms;
}

void host_set_sta_linking_test(u32 wait_ms, u8 force_fail)
{
	sta_linking_test_wait_ms = wait_ms;
	sta_linking_test_force_fail = force_fail;
}

void host_set_ap_linking_test(u16 auth_fail, u16 asoc_fail)
{
	ap_linking_test_force_auth_fail = auth_fail;
	ap_linking_test_force_asoc_fail = asoc_fail;
}

int rtw_fwdl_test_trigger_chksum_fail(void)
{
	if (fwdl_test_chksum_fail) {
		fwdl_test_chksum_fail--;
		return _TRUE;
	}
	return _FALSE;
}

int rtw_fwdl_test_trigger_wintint_rdy_fail(void)
{
	if (fwdl_test_wintint_rdy_fail) {
		fwdl_test_wintint_rdy_fail--;
		return _TRUE;
	}
	return _FALSE;
}

int rtw_del_rx_ampdu_test_trigger_no_tx_fail(void)
{
	if (del_rx_ampdu_test_no_tx_fail) {
		del_rx_ampdu_test_no_tx_fail--;
		return _TRUE;
	}
	return _FALSE;
}

u32 rtw_get_wait_hiq_empty_ms(void)
{
	return g_wait_hiq_empty_ms;
}

void rtw_sta_linking_test_set_start(void)
{
	sta_linking_test_start_time = host_get_current_time();
}

int rtw_sta_linking_test_wait_done(void)
{
	return host_get_passing_time_ms(sta_linking_test_start_time) >=
	       sta_linking_test_wait_ms;
}

int rtw_sta_linking_test_force_fail(void)
{
	return sta_linking_test_force_fail ? _TRUE : _FALSE;
}

u16 rtw_ap_linking_test_force_auth_fail(void)
{
	return ap_linking_test_force_auth_fail;
}

u16 rtw_ap_linking_test_force_asoc_fail(void)
{
	return ap_linking_test_force_asoc_fail;
}
