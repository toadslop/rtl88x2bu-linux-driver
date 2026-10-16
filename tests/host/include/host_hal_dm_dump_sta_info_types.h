/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_DM_DUMP_STA_INFO_TYPES_H
#define HOST_HAL_DM_DUMP_STA_INFO_TYPES_H

#include <stddef.h>
#include <stdint.h>

#define HOST_HAL_DM_DUMP_STA_MAC_LEN 6

struct host_hal_dm_dump_sta_fixture {
	uint8_t mac_addr[HOST_HAL_DM_DUMP_STA_MAC_LEN];
	uint8_t mac_id;
	uint8_t wireless_mode;
	uint8_t mimo_type;
	uint8_t sm_ps;
	uint8_t bw_mode;
	uint8_t ra_bw_mode;
	uint8_t rate_id;
	int8_t rssi;
	uint8_t rssi_level;
	uint8_t is_support_sgi;
	uint8_t is_vht_enable;
	uint8_t disable_ra;
	uint8_t disable_pt;
	uint8_t is_noisy;
	uint8_t txrx_state;
	uint8_t stub_tx_rate;
	uint8_t stub_tx_sgi;
	uint8_t curr_tx_bw;
	uint8_t curr_retry_ratio;
	uint64_t ramask;
};

int dump_sta_info_format(const struct host_hal_dm_dump_sta_fixture *fx, char *buf,
			 size_t buflen);

void dump_sta_info(void *sel, void *psta);

#endif /* HOST_HAL_DM_DUMP_STA_INFO_TYPES_H */
