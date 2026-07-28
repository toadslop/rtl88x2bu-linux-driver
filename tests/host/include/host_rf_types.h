/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal types for host L2 rtw_rf_rest tests (W3-19).
 * Channel-width enums match include/cmn_info/rtw_sta_info.h (kernel build).
 */
#ifndef HOST_RF_TYPES_H
#define HOST_RF_TYPES_H

#include <stdbool.h>

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0

#define CHANNEL_WIDTH_20 0
#define CHANNEL_WIDTH_40 1
#define CHANNEL_WIDTH_80 2
#define CHANNEL_WIDTH_160 3
#define CHANNEL_WIDTH_80_80 4
#define CHANNEL_WIDTH_5 5
#define CHANNEL_WIDTH_10 6
#define CHANNEL_WIDTH_MAX 7

#define HAL_PRIME_CHNL_OFFSET_DONT_CARE 0
#define HAL_PRIME_CHNL_OFFSET_LOWER 1
#define HAL_PRIME_CHNL_OFFSET_UPPER 2

#define CENTER_CH_2G_40M_NUM 9
#define CENTER_CH_2G_NUM 14
#define CENTER_CH_5G_20M_NUM 28
#define CENTER_CH_5G_40M_NUM 14
#define CENTER_CH_5G_80M_NUM 7
#define CENTER_CH_5G_160M_NUM 3

#define RTW_PRINT(...) do { } while (0)
#define RTW_WARN(...) do { } while (0)
#define rtw_warn_on(cond) ((void)(cond))

typedef enum _BAND_TYPE {
	BAND_ON_2_4G = 0,
	BAND_ON_5G = 1,
	BAND_MAX,
} BAND_TYPE;

#endif /* HOST_RF_TYPES_H */
