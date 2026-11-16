/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_UPDATE_STA_TYPES_H
#define HOST_HAL_UPDATE_STA_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef int8_t s8;

struct _adapter { u8 _unused; };

#define WIRELESS_11B (1U << 0)
#define WIRELESS_11G (1U << 1)
#define WIRELESS_11A (1U << 2)
#define WIRELESS_11_24N (1U << 3)
#define WIRELESS_11_5N (1U << 4)
#define WIRELESS_11AC (1U << 6)
#define WIRELESS_CCK 0x01U
#define WIRELESS_OFDM 0x02U
#define WIRELESS_HT 0x04U
#define WIRELESS_VHT 0x08U

enum host_rf_type {
	RF_1T1R, RF_1T2R, RF_2T2R, RF_2T3R, RF_2T4R, RF_3T3R, RF_3T4R, RF_4T4R,
};

struct host_cmn_sta_info {
	u8 mac_id;
	enum host_rf_type mimo_type;
	u8 support_wireless_set;
};

struct host_sta_info {
	u8 wireless_mode;
	struct host_cmn_sta_info cmn;
};

void host_hal_update_sta_reset_nss(void);
void host_hal_update_sta_set_nss(s8 tx_nss, s8 rx_nss);
void rtw_hal_update_sta_wset(struct _adapter *adapter, struct host_sta_info *psta);
void rtw_hal_update_sta_mimo_type(struct _adapter *adapter, struct host_sta_info *psta);

#endif