// SPDX-License-Identifier: GPL-2.0
/* Host/kernel FFI for rust/rtw_debug_proc_ampdu.rs (W3-204). */

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "host_rtw_debug_proc_ampdu_types.h"

struct host_proc_seq {
	struct _adapter *adapter;
	FILE *out;
};

void rtw_rust_debug_print_sel(void *sel, const char *line)
{
	struct host_proc_seq *ctx = sel;

	if (!sel || !line || !ctx->out)
		return;
	fputs(line, ctx->out);
}

void *rtw_rust_debug_proc_adapter(void *m)
{
	struct host_proc_seq *ctx = m;

	if (!m)
		return NULL;
	return ctx->adapter;
}

void *rtw_rust_debug_proc_adapter_from_data(void *data)
{
	return data;
}

unsigned char rtw_rust_debug_fix_rx_ampdu_accept(void *adapter)
{
	return ((_adapter *)adapter)->fix_rx_ampdu_accept;
}

unsigned char rtw_rust_debug_fix_rx_ampdu_size(void *adapter)
{
	return ((_adapter *)adapter)->fix_rx_ampdu_size;
}

long rtw_rust_debug_proc_read_user_line(const void *buffer, unsigned long count,
					char *tmp, unsigned long tmp_size)
{
	if (count < 1 || count > tmp_size || !buffer)
		return -1;
	memcpy(tmp, buffer, count);
	return (long)count;
}

u8 rtw_rx_ampdu_is_accept(_adapter *adapter)
{
	if (adapter->fix_rx_ampdu_accept != RX_AMPDU_ACCEPT_INVALID)
		return adapter->fix_rx_ampdu_accept;
	return adapter->host_auto_accept;
}

u8 rtw_rx_ampdu_size(_adapter *adapter)
{
	if (adapter->fix_rx_ampdu_size != RX_AMPDU_SIZE_INVALID)
		return adapter->fix_rx_ampdu_size;
	return adapter->host_auto_size;
}

u8 rtw_rx_ampdu_set_accept(_adapter *adapter, u8 accept, u8 reason)
{
	(void)reason;
	adapter->fix_rx_ampdu_accept = accept;
	return accept;
}

u8 rtw_rx_ampdu_set_size(_adapter *adapter, u8 size, u8 reason)
{
	(void)reason;
	adapter->fix_rx_ampdu_size = size;
	return size;
}

void rtw_rx_ampdu_apply(_adapter *adapter)
{
	(void)adapter;
}
