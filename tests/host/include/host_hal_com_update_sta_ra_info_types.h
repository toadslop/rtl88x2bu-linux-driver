// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_COM_UPDATE_STA_RA_INFO_TYPES_H
#define HOST_HAL_COM_UPDATE_STA_RA_INFO_TYPES_H

#include <stdint.h>

typedef uint8_t u8;

struct host_adapter_ra {
	u8 _pad;
};

struct host_sta_ra {
	u8 _pad;
};

#define HOST_RA_STEP_MIMO 1u
#define HOST_RA_STEP_SMPS 2u
#define HOST_RA_STEP_RATE_MASK 3u

#define HOST_RA_MAX_STEPS 8u

struct host_hal_com_update_sta_ra_info_trace {
	u8 steps[HOST_RA_MAX_STEPS];
	u8 step_count;
	u8 mimo_calls;
	u8 smps_calls;
	u8 rate_mask_calls;
};

extern struct host_hal_com_update_sta_ra_info_trace host_hal_com_update_sta_ra_info_trace;

void host_hal_com_update_sta_ra_info_reset(void);
void host_hal_com_update_sta_ra_info_record_step(u8 step);

void rtw_hal_update_sta_mimo_type(struct host_adapter_ra *adapter, struct host_sta_ra *psta);
void rtw_hal_update_sta_smps_cap(struct host_adapter_ra *adapter, struct host_sta_ra *psta);
void rtw_hal_update_sta_rate_mask(struct host_adapter_ra *adapter, struct host_sta_ra *psta);

void rtw_hal_update_sta_ra_info(struct host_adapter_ra *adapter, struct host_sta_ra *psta);

#endif