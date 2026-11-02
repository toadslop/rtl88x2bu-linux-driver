/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_RTW_DEBUG_PROC_AMPDU_TYPES_H
#define HOST_RTW_DEBUG_PROC_AMPDU_TYPES_H

#include "host_rtw_debug_regsty_types.h"

#define RX_AMPDU_ACCEPT_INVALID 0xFF
#define RX_AMPDU_SIZE_INVALID 0xFF
#define RX_AMPDU_DRV_FIXED 1

struct seq_file {
	void *private;
};

int proc_get_rx_ampdu(struct seq_file *m, void *v);
long proc_set_rx_ampdu(void *file, const void *buffer, unsigned long count,
		       long *pos, void *data);

#endif
