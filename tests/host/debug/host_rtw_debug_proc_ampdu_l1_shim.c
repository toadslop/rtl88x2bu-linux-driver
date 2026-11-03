// SPDX-License-Identifier: GPL-2.0
/* Stub linker symbols for W3-204/W3-205 L1. */

#include <stddef.h>

typedef struct seq_file seq_file;

#define STUB_PROC_GET(name) \
	int name(seq_file *m, void *v) \
	{ \
		(void)m; \
		(void)v; \
		return 0; \
	}

#define STUB_PROC_SET(name) \
	long name(void *file, const void *buffer, unsigned long count, \
		  long *pos, void *data) \
	{ \
		(void)file; \
		(void)buffer; \
		(void)count; \
		(void)pos; \
		(void)data; \
		return 0; \
	}

STUB_PROC_GET(proc_get_rx_ampdu)
STUB_PROC_SET(proc_set_rx_ampdu)
STUB_PROC_GET(proc_get_rx_ampdu_factor)
STUB_PROC_SET(proc_set_rx_ampdu_factor)
STUB_PROC_GET(proc_get_rx_ampdu_density)
STUB_PROC_SET(proc_set_rx_ampdu_density)
STUB_PROC_GET(proc_get_tx_ampdu_density)
STUB_PROC_SET(proc_set_tx_ampdu_density)
STUB_PROC_GET(proc_get_tx_max_agg_num)
STUB_PROC_SET(proc_set_tx_max_agg_num)
