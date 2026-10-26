/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-170 regsty RX AMPDU limit dump. */
#ifndef HOST_RTW_DEBUG_REGSTY_TYPES_H
#define HOST_RTW_DEBUG_REGSTY_TYPES_H

#include <stdio.h>
#include <stdint.h>

typedef uint8_t u8;

struct registry_priv {
	u8 rx_ampdu_sz_limit_by_nss_bw[4][4];
};

struct _adapter {
	struct registry_priv registrypriv;
};

#define adapter_to_regsty(adapter) (&(adapter)->registrypriv)

#define RTW_PRINT_SEL(sel, fmt, ...) fprintf((FILE *)(sel), fmt, ##__VA_ARGS__)

void dump_regsty_rx_ampdu_size_limit(void *sel, struct _adapter *adapter);

#endif /* HOST_RTW_DEBUG_REGSTY_TYPES_H */
