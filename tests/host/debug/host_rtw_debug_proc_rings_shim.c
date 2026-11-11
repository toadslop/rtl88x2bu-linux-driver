// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-233 TRX ring proc L2 Rust oracle. */

#include "host_rtw_debug_proc_rings_types.h"

void *rtw_rust_debug_proc_adapter_from_data(void *data)
{
	return rtw_netdev_priv((struct net_device *)data);
}

long rtw_rust_debug_proc_set_tx_ring_ext(void *adapter, const void *buffer,
				       unsigned long count)
{
	(void)adapter;
	(void)buffer;
	return (long)count;
}
