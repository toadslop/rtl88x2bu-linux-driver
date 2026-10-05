// SPDX-License-Identifier: GPL-2.0
/* L1 reference: C leaf setters for W3-125 PR3/PR6 (scan_mode, setband, regd). */
#include <stddef.h>

typedef int RT_SCAN_TYPE;
typedef unsigned char u8;

#define _SUCCESS 1
#define _FAIL 0
#define SCAN_PASSIVE 0
#define SCAN_ACTIVE 1
#define WIFI_FREQUENCY_BAND_2GHZ 2
#define RTW_CMDF_WAIT_ACK 2
#define REGD_SRC_OS 1

#ifndef CONFIG_REGD_SRC_FROM_OS
#define REGSTY_REGD_SRC_FROM_OS(regsty) 0
#else
#define REGSTY_REGD_SRC_FROM_OS(regsty) ((regsty)->regd_src == REGD_SRC_OS)
#endif

struct mlme_priv {
	RT_SCAN_TYPE scan_mode;
};

struct registry_priv {
	u8 regd_src;
};

struct adapter {
	struct mlme_priv mlmepriv;
	u8 setband;
	struct registry_priv regsty;
};

#define adapter_to_regsty(a) (&(a)->regsty)

#define rtw_band_valid(band) ((band) <= WIFI_FREQUENCY_BAND_2GHZ)

static u8 rtw_set_chplan_cmd(struct adapter *adapter, int flags, u8 chplan, u8 swconfig)
{
	(void)adapter;
	(void)flags;
	(void)chplan;
	(void)swconfig;
	return _SUCCESS;
}

static u8 rtw_set_country_cmd(struct adapter *adapter, int flags, const char *country_code,
			      u8 swconfig)
{
	(void)adapter;
	(void)flags;
	(void)country_code;
	(void)swconfig;
	return _SUCCESS;
}

int rtw_set_scan_mode(struct adapter *adapter, RT_SCAN_TYPE scan_mode)
{
	if (scan_mode != SCAN_ACTIVE && scan_mode != SCAN_PASSIVE)
		return _FAIL;

	adapter->mlmepriv.scan_mode = scan_mode;

	return _SUCCESS;
}

int rtw_set_band(struct adapter *adapter, u8 band)
{
	if (rtw_band_valid(band)) {
		adapter->setband = band;
		return _SUCCESS;
	}

	return _FAIL;
}

int rtw_set_channel_plan(struct adapter *adapter, u8 channel_plan)
{
	struct registry_priv *regsty = adapter_to_regsty(adapter);

	(void)regsty;
	if (!REGSTY_REGD_SRC_FROM_OS(regsty))
		return rtw_set_chplan_cmd(adapter, RTW_CMDF_WAIT_ACK, channel_plan, 1);
	return _SUCCESS;
}

int rtw_set_country(struct adapter *adapter, const char *country_code)
{
#ifdef CONFIG_RTW_IOCTL_SET_COUNTRY
	struct registry_priv *regsty = adapter_to_regsty(adapter);

	(void)regsty;
	if (!REGSTY_REGD_SRC_FROM_OS(regsty))
		return rtw_set_country_cmd(adapter, RTW_CMDF_WAIT_ACK, country_code, 1);
#endif
	return _SUCCESS;
}
