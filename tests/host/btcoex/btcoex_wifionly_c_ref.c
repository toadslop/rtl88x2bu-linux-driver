// SPDX-License-Identifier: GPL-2.0
/* Host L1 reference for W3-137 — mirrors core/rtw_btcoex_wifionly.c without kernel headers. */

typedef void *PADAPTER;

static void hal_btcoex_wifionly_switchband_notify(PADAPTER padapter)
{
	(void)padapter;
}

static void hal_btcoex_wifionly_scan_notify(PADAPTER padapter)
{
	(void)padapter;
}

static void hal_btcoex_wifionly_connect_notify(PADAPTER padapter)
{
	(void)padapter;
}

static void hal_btcoex_wifionly_hw_config(PADAPTER padapter)
{
	(void)padapter;
}

static void hal_btcoex_wifionly_initlizevariables(PADAPTER padapter)
{
	(void)padapter;
}

static void hal_btcoex_wifionly_AntInfoSetting(PADAPTER padapter)
{
	(void)padapter;
}

void rtw_btcoex_wifionly_switchband_notify(PADAPTER padapter)
{
	hal_btcoex_wifionly_switchband_notify(padapter);
}

void rtw_btcoex_wifionly_scan_notify(PADAPTER padapter)
{
	hal_btcoex_wifionly_scan_notify(padapter);
}

void rtw_btcoex_wifionly_connect_notify(PADAPTER padapter)
{
	hal_btcoex_wifionly_connect_notify(padapter);
}

void rtw_btcoex_wifionly_hw_config(PADAPTER padapter)
{
	hal_btcoex_wifionly_hw_config(padapter);
}

void rtw_btcoex_wifionly_initialize(PADAPTER padapter)
{
	hal_btcoex_wifionly_initlizevariables(padapter);
}

void rtw_btcoex_wifionly_AntInfoSetting(PADAPTER padapter)
{
	hal_btcoex_wifionly_AntInfoSetting(padapter);
}
