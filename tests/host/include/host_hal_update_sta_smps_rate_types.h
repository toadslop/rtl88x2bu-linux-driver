/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_UPDATE_STA_SMPS_RATE_TYPES_H
#define HOST_HAL_UPDATE_STA_SMPS_RATE_TYPES_H
#include <stdint.h>
typedef uint8_t u8;
typedef int8_t s8;
typedef uint32_t u32;
typedef uint64_t u64;
#define BSS_RATES_LEN 12
#define HT_MCS_SET_LEN 16
#define VHT_MCS_MAP_LEN 2
#define WIRELESS_11_24N (1U << 3)
#define WIRELESS_11_5N (1U << 4)
#define SM_PS_DISABLE 3U
#define KIND_SMPS 0
#define KIND_RATE_MASK 1
struct host_sta_smps_rate {
	u8 mac_id, sm_ps, ht_option, smps_cap, vht_option, init_rate;
	u64 ramask;
	u8 bssrateset[BSS_RATES_LEN], supp_mcs_set[HT_MCS_SET_LEN], vht_mcs_map[VHT_MCS_MAP_LEN];
};
struct host_adapter_smps_rate { u8 ht_enable, wireless_mode, ap_state; s8 hal_tx_nss; };
void rtw_hal_update_sta_smps_cap(struct host_adapter_smps_rate *, struct host_sta_smps_rate *);
void rtw_hal_update_sta_rate_mask(struct host_adapter_smps_rate *, struct host_sta_smps_rate *);
#endif