// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-204 proc rx_ampdu (from core/rtw_debug_rest.c). */

#include <stdio.h>
#include <string.h>

#include "host_rtw_debug_proc_ampdu_types.h"

struct host_proc_seq {
	struct _adapter *adapter;
	FILE *out;
};

static u8 oracle_rx_ampdu_is_accept(struct _adapter *adapter)
{
	if (adapter->fix_rx_ampdu_accept != RX_AMPDU_ACCEPT_INVALID)
		return adapter->fix_rx_ampdu_accept;
	return adapter->host_auto_accept;
}

static u8 oracle_rx_ampdu_size(struct _adapter *adapter)
{
	if (adapter->fix_rx_ampdu_size != RX_AMPDU_SIZE_INVALID)
		return adapter->fix_rx_ampdu_size;
	return adapter->host_auto_size;
}

static void oracle_rx_ampdu_set_accept(struct _adapter *adapter, u8 accept)
{
	adapter->fix_rx_ampdu_accept = accept;
}

static void oracle_rx_ampdu_set_size(struct _adapter *adapter, u8 size)
{
	adapter->fix_rx_ampdu_size = size;
}

int proc_get_rx_ampdu(struct seq_file *m, void *v)
{
	struct host_proc_seq *ctx = (struct host_proc_seq *)m;
	struct _adapter *padapter = ctx->adapter;
	FILE *out = ctx->out;

	(void)v;

	fprintf(out, "accept: ");
	if (padapter->fix_rx_ampdu_accept == RX_AMPDU_ACCEPT_INVALID)
		fprintf(out, "%u(auto)\n", oracle_rx_ampdu_is_accept(padapter));
	else
		fprintf(out, "%u(fixed)\n", padapter->fix_rx_ampdu_accept);

	fprintf(out, "size: ");
	if (padapter->fix_rx_ampdu_size == RX_AMPDU_SIZE_INVALID) {
		fprintf(out, "%u(auto) with conditional limit:\n",
			oracle_rx_ampdu_size(padapter));
		dump_regsty_rx_ampdu_size_limit(out, padapter);
	} else {
		fprintf(out, "%u(fixed)\n", padapter->fix_rx_ampdu_size);
	}
	fprintf(out, "\n");

	fprintf(out, "%19s %17s\n", "fix_rx_ampdu_accept", "fix_rx_ampdu_size");
	fprintf(out, "%-19d %-17u\n", padapter->fix_rx_ampdu_accept,
		padapter->fix_rx_ampdu_size);

	return 0;
}

long proc_set_rx_ampdu(void *file, const void *buffer, unsigned long count,
		     long *pos, void *data)
{
	struct _adapter *padapter = data;
	char tmp[32];
	u8 accept;
	u8 size;
	int num;

	(void)file;
	(void)pos;

	if (count < 1 || count > sizeof(tmp) || !buffer)
		return -1;

	memcpy(tmp, buffer, count);
	tmp[count] = '\0';

	num = sscanf(tmp, "%hhu %hhu", &accept, &size);
	if (num >= 1)
		oracle_rx_ampdu_set_accept(padapter, accept);
	if (num >= 2)
		oracle_rx_ampdu_set_size(padapter, size);

	return (long)count;
}
