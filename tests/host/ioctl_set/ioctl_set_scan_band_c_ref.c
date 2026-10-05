// SPDX-License-Identifier: GPL-2.0
/* L1 reference: C leaf setters for W3-125 PR3 (scan_mode + setband). */
#include <stddef.h>

typedef int RT_SCAN_TYPE;
typedef unsigned char u8;

#define _SUCCESS 1
#define _FAIL 0
#define SCAN_PASSIVE 0
#define SCAN_ACTIVE 1
#define WIFI_FREQUENCY_BAND_2GHZ 2

struct mlme_priv {
	RT_SCAN_TYPE scan_mode;
};

struct adapter {
	struct mlme_priv mlmepriv;
	u8 setband;
};

#define rtw_band_valid(band) ((band) <= WIFI_FREQUENCY_BAND_2GHZ)

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
