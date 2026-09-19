// SPDX-License-Identifier: GPL-2.0
#include "host_ap_sta_info_apmode_types.h"

static u8 g_vcs_calls;
static u8 g_delba_calls;
static u8 g_odm_calls;
static u8 g_ra_sgi;
static u8 g_tx_bw;

void host_apmode_reset(void)
{
	g_vcs_calls = 0;
	g_delba_calls = 0;
	g_odm_calls = 0;
	g_ra_sgi = 0;
	g_tx_bw = CHANNEL_WIDTH_20;
}

u8 host_apmode_vcs_calls(void)
{
	return g_vcs_calls;
}

u8 host_apmode_delba_calls(void)
{
	return g_delba_calls;
}

u8 host_apmode_odm_calls(void)
{
	return g_odm_calls;
}

void host_apmode_set_ra_sgi(u8 sgi)
{
	g_ra_sgi = sgi;
}

void host_apmode_set_tx_bw(u8 bw)
{
	g_tx_bw = bw;
}

void VCS_update(_adapter *padapter, struct sta_info *psta)
{
	(void)padapter;
	(void)psta;
	g_vcs_calls++;
}

void send_delba(_adapter *padapter, int initiator, u8 *addr)
{
	(void)padapter;
	(void)initiator;
	(void)addr;
	g_delba_calls++;
}

u8 query_ra_short_GI(struct sta_info *psta, u8 bw)
{
	(void)psta;
	(void)bw;
	return g_ra_sgi;
}

u8 rtw_get_tx_bw_mode(_adapter *padapter, struct sta_info *psta)
{
	(void)padapter;
	(void)psta;
	return g_tx_bw;
}

void update_ldpc_stbc_cap(struct sta_info *psta)
{
	(void)psta;
}

void update_sta_vht_info_apmode(_adapter *padapter, void *psta)
{
	(void)padapter;
	(void)psta;
}

void rtw_hal_set_odm_var(_adapter *padapter, enum hal_odm_var variable, struct sta_info *psta, u8 val)
{
	(void)padapter;
	(void)variable;
	(void)psta;
	(void)val;
	g_odm_calls++;
}
