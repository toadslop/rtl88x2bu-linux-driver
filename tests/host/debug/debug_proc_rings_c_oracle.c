// SPDX-License-Identifier: GPL-2.0
/* C oracle proc exports for W3-233 TRX ring proc getters (host L2). */

#include "host_rtw_debug_proc_rings_types.h"

int proc_get_rx_ring(struct seq_file *m, void *v)
{
	struct _adapter *padapter = rtw_rust_debug_proc_adapter(m);

	(void)v;
	if (padapter)
		rtw_rust_debug_proc_emit_rx_ring(m, padapter);
	return 0;
}

int proc_get_tx_ring(struct seq_file *m, void *v)
{
	struct _adapter *padapter = rtw_rust_debug_proc_adapter(m);

	(void)v;
	if (padapter)
		rtw_rust_debug_proc_emit_tx_ring(m, padapter);
	return 0;
}

int proc_get_tx_ring_ext(struct seq_file *m, void *v)
{
	struct _adapter *padapter = rtw_rust_debug_proc_adapter(m);

	(void)v;
	if (padapter)
		rtw_rust_debug_proc_emit_tx_ring_ext(m, padapter);
	return 0;
}
