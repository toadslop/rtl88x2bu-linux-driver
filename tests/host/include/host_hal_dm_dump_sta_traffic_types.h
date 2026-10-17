/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_DM_DUMP_STA_TRAFFIC_TYPES_H
#define HOST_HAL_DM_DUMP_STA_TRAFFIC_TYPES_H

#include <stddef.h>
#include <stdint.h>

#define HOST_HAL_DM_DUMP_STA_TRAFFIC_MAC_LEN 6

struct host_hal_dm_dump_sta_traffic_fixture {
	uint8_t mac_addr[HOST_HAL_DM_DUMP_STA_TRAFFIC_MAC_LEN];
	uint8_t mac_id;
	uint8_t associated_to_ap;
	uint8_t bcn_cnt;
	uint8_t bcn_dtim;
	uint8_t stub_tx_rate;
	uint8_t stub_tx_sgi;
	uint8_t curr_rx_rate;
	uint8_t curr_rx_rate_bmc;
	int8_t rssi;
	uint32_t tx_tp_kbits;
	uint32_t rx_tp_kbits;
	uint32_t smooth_tx_tp_kbits;
	uint32_t smooth_rx_tp_kbits;
};

int dump_sta_traffic_format(const struct host_hal_dm_dump_sta_traffic_fixture *fx,
			    char *buf, size_t buflen);

void dump_sta_traffic(void *sel, void *adapter, void *psta);

#endif /* HOST_HAL_DM_DUMP_STA_TRAFFIC_TYPES_H */
