// SPDX-License-Identifier: GPL-2.0
#include "host_mlme_ext_join_cmd_types.h"
#include <string.h>

unsigned char WMM_OUI[4] = { 0x00, 0x50, 0xf2, 0x02 };

struct host_join_cmd_trace host_join_cmd_trace;
int host_chk_start_clnt_join_result = _SUCCESS;
static u8 host_chk_ch = 6, host_chk_bw, host_chk_offset;

void host_join_cmd_reset_trace(void)
{
	memset(&host_join_cmd_trace, 0, sizeof(host_join_cmd_trace));
	host_chk_start_clnt_join_result = _SUCCESS;
	host_chk_ch = 6;
	host_chk_bw = 0;
	host_chk_offset = 0;
}

void issue_deauth_ex(_adapter *a, u8 *addr, u16 reason, u8 try_cnt, int try_ms)
{
	(void)a;
	(void)addr;
	(void)reason;
	(void)try_cnt;
	(void)try_ms;
	host_join_cmd_trace.deauth_called = 1;
}

void flush_all_cam_entry(_adapter *a)
{
	(void)a;
	host_join_cmd_trace.flush_cam = 1;
}

void _cancel_timer_ex(struct _timer *t)
{
	if (t)
		t->cancelled = 1;
	host_join_cmd_trace.cancel_link_timer = 1;
}

void Set_MSR(_adapter *a, u8 type)
{
	(void)a;
	(void)type;
}

void rtw_hal_set_hwreg(_adapter *a, int id, u8 *val)
{
	(void)a;
	switch (id) {
	case HW_VAR_MLME_DISCONNECT:
		host_join_cmd_trace.mlme_disconnect = 1;
		break;
	case HW_VAR_BSSID:
		host_join_cmd_trace.bssid_set = 1;
		break;
	case HW_VAR_MLME_JOIN:
		if (val)
			host_join_cmd_trace.join_type = *val;
		break;
	case HW_VAR_DO_IQK:
		if (val && *val)
			host_join_cmd_trace.do_iqk_on = 1;
		else
			host_join_cmd_trace.do_iqk_off = 1;
		break;
	default:
		break;
	}
}

void rtw_hal_rcr_set_chk_bssid(_adapter *a, int mode)
{
	(void)a;
	(void)mode;
}

void rtw_joinbss_reset(_adapter *a)
{
	(void)a;
	host_join_cmd_trace.joinbss_reset = 1;
}

u32 report_join_res(_adapter *a, int aid_res, u16 status)
{
	(void)a;
	(void)aid_res;
	host_join_cmd_trace.report_join_res_called = 1;
	host_join_cmd_trace.report_join_res_status = status;
	return 0;
}

u16 get_beacon_interval(WLAN_BSSID_EX *bss)
{
	u16 val;

	if (!bss || bss->IELength < 4)
		return 100;
	memcpy(&val, bss->IEs + 8, 2);
	return val;
}

int WMM_param_handler(_adapter *a, PNDIS_802_11_VARIABLE_IEs pIE)
{
	(void)pIE;
	a->mlmeextpriv.mlmext_info.WMM_enable = 1;
	host_join_cmd_trace.wmm_handler = 1;
	return _TRUE;
}

void rtw_adjust_chbw(_adapter *a, u8 ch, u8 *bw, u8 *offset)
{
	(void)a;
	(void)ch;
	(void)bw;
	(void)offset;
}

int rtw_chk_start_clnt_join(_adapter *a, u8 *ch, u8 *bw, u8 *offset)
{
	(void)a;
	if (host_chk_start_clnt_join_result == _FAIL)
		return _FAIL;
	if (ch)
		*ch = host_chk_ch;
	if (bw)
		*bw = host_chk_bw;
	if (offset)
		*offset = host_chk_offset;
	return _SUCCESS;
}

void rtw_btcoex_connect_notify(_adapter *a, u8 join_type)
{
	(void)a;
	(void)join_type;
}

void set_channel_bwmode(_adapter *a, u8 ch, u8 offset, u8 bw)
{
	(void)a;
	host_join_cmd_trace.set_channel = 1;
	host_join_cmd_trace.last_ch = ch;
	host_join_cmd_trace.last_offset = offset;
	host_join_cmd_trace.last_bw = bw;
}

void start_clnt_join(_adapter *a)
{
	(void)a;
	host_join_cmd_trace.start_clnt_join = 1;
}

void rtw_bss_get_chbw(WLAN_BSSID_EX *bss, u8 *ch, u8 *bw, u8 *offset, u8 ht,
		      u8 vht)
{
	(void)ht;
	(void)vht;
	if (ch)
		*ch = (u8)bss->Configuration.DSConfig;
	if (bw)
		*bw = 0;
	if (offset)
		*offset = 0;
}
