/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_STA_ALIVE_TYPES_H
#define HOST_AP_STA_ALIVE_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0

struct stainfo_stats {
	u64 rx_ctrl_pkts;
	u64 last_rx_ctrl_pkts;
	u64 rx_data_pkts;
	u64 last_rx_data_pkts;
};

struct sta_info {
	struct stainfo_stats sta_stats;
};

#define sta_update_last_rx_pkts(sta) do { \
	struct stainfo_stats *_s = &(sta)->sta_stats; \
	_s->last_rx_ctrl_pkts = _s->rx_ctrl_pkts; \
	_s->last_rx_data_pkts = _s->rx_data_pkts; \
} while (0)

u8 chk_sta_is_alive(struct sta_info *psta);

#endif /* HOST_AP_STA_ALIVE_TYPES_H */
