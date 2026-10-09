// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include "host_rtw_debug_test_hooks_types.h"

void host_debug_test_hooks_set_passing_ms(u32 ms);
void host_set_fwdl_test_case(u8 chksum, u8 wintint);
void host_set_del_rx_ampdu_test_case(u8 no_tx_fail);
void host_set_wait_hiq_empty_ms(u32 ms);
void host_set_sta_linking_test(u32 wait_ms, u8 force_fail);
void host_set_ap_linking_test(u16 auth_fail, u16 asoc_fail);

int rtw_fwdl_test_trigger_chksum_fail(void);
int rtw_fwdl_test_trigger_wintint_rdy_fail(void);
int rtw_del_rx_ampdu_test_trigger_no_tx_fail(void);
u32 rtw_get_wait_hiq_empty_ms(void);
void rtw_sta_linking_test_set_start(void);
int rtw_sta_linking_test_wait_done(void);
int rtw_sta_linking_test_force_fail(void);
u16 rtw_ap_linking_test_force_auth_fail(void);
u16 rtw_ap_linking_test_force_asoc_fail(void);

int main(void)
{
	int bad = 0;

	host_set_fwdl_test_case(2, 0);
	if (!rtw_fwdl_test_trigger_chksum_fail() || !rtw_fwdl_test_trigger_chksum_fail() ||
	    rtw_fwdl_test_trigger_chksum_fail())
		bad++;

	host_set_fwdl_test_case(0, 1);
	if (!rtw_fwdl_test_trigger_wintint_rdy_fail() || rtw_fwdl_test_trigger_wintint_rdy_fail())
		bad++;

	host_set_del_rx_ampdu_test_case(1);
	if (!rtw_del_rx_ampdu_test_trigger_no_tx_fail() ||
	    rtw_del_rx_ampdu_test_trigger_no_tx_fail())
		bad++;

	host_set_wait_hiq_empty_ms(1234);
	if (rtw_get_wait_hiq_empty_ms() != 1234)
		bad++;

	host_debug_test_hooks_set_passing_ms(0);
	host_set_sta_linking_test(100, 0);
	rtw_sta_linking_test_set_start();
	if (rtw_sta_linking_test_wait_done())
		bad++;
	host_debug_test_hooks_set_passing_ms(100);
	if (!rtw_sta_linking_test_wait_done())
		bad++;

	host_set_sta_linking_test(0, 1);
	if (!rtw_sta_linking_test_force_fail())
		bad++;

	host_set_ap_linking_test(0x12, 0x34);
	if (rtw_ap_linking_test_force_auth_fail() != 0x12 ||
	    rtw_ap_linking_test_force_asoc_fail() != 0x34)
		bad++;

	if (bad) {
		fprintf(stderr, "FAIL debug test hooks (%d)\n", bad);
		return 1;
	}
	printf("PASS debug test hooks (8 checks)\n");
	return 0;
}
