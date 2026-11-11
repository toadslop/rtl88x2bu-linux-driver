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

int proc_get_rx_ampdu_factor(struct seq_file *m, void *v);
long proc_set_rx_ampdu_factor(void *file, const void *buffer, unsigned long count,
			      long *pos, void *data);
int proc_get_rx_ampdu_density(struct seq_file *m, void *v);
long proc_set_rx_ampdu_density(void *file, const void *buffer, unsigned long count,
			       long *pos, void *data);
int proc_get_tx_ampdu_density(struct seq_file *m, void *v);
long proc_set_tx_ampdu_density(void *file, const void *buffer, unsigned long count,
			       long *pos, void *data);
int proc_get_tx_max_agg_num(struct seq_file *m, void *v);
long proc_set_tx_max_agg_num(void *file, const void *buffer, unsigned long count,
			     long *pos, void *data);

int proc_get_tx_quick_addba_req(struct seq_file *m, void *v);
long proc_set_tx_quick_addba_req(void *file, const void *buffer, unsigned long count,
				 long *pos, void *data);
int proc_get_tx_aval_th(struct seq_file *m, void *v);
long proc_set_tx_aval_th(void *file, const void *buffer, unsigned long count,
			 long *pos, void *data);

int host_rtw_debug_proc_warn_on_count(void);
void host_rtw_debug_proc_reset_warn_on_count(void);

#endif
