/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_BMC_RATE_TYPES_H
#define HOST_AP_BMC_RATE_TYPES_H

typedef unsigned char u8;

#define BAND_ON_5G 1

typedef struct {
	u8 current_band_type;
} HAL_DATA_TYPE, *PHAL_DATA_TYPE;

struct _adapter {
	HAL_DATA_TYPE hal_data;
};

#define GET_HAL_DATA(a) (&((a)->hal_data))

u8 rtw_ap_find_bmc_rate(struct _adapter *adapter, u8 tx_rate);

#endif /* HOST_AP_BMC_RATE_TYPES_H */
