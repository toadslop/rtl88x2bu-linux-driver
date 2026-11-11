// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-205 driver AMPDU tuning procs (from core/rtw_debug_rest.c). */

#include <stdio.h>
#include <string.h>

#include "host_rtw_debug_proc_ampdu_types.h"

static int g_warn_on_count;

int host_rtw_debug_proc_warn_on_count(void)
{
	return g_warn_on_count;
}

void host_rtw_debug_proc_reset_warn_on_count(void)
{
	g_warn_on_count = 0;
}

static inline void rtw_warn_on(int cond)
{
	if (cond)
		g_warn_on_count++;
}

struct host_proc_seq {
	struct _adapter *adapter;
	FILE *out;
};

int proc_get_rx_ampdu_factor(struct seq_file *m, void *v)
{
	struct host_proc_seq *ctx = (struct host_proc_seq *)m;

	(void)v;
	if (ctx->adapter)
		fprintf(ctx->out, "rx ampdu factor = %x\n",
			ctx->adapter->driver_rx_ampdu_factor);
	return 0;
}

long proc_set_rx_ampdu_factor(void *file, const void *buffer, unsigned long count,
			      long *pos, void *data)
{
	struct _adapter *padapter = data;
	char tmp[32];
	int factor;

	(void)file;
	(void)pos;
	if (count < 1)
		return -14;
	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -14;
	}
	if (!buffer)
		return (long)count;
	memcpy(tmp, buffer, count);
	tmp[count] = '\0';
	if (sscanf(tmp, "%d ", &factor) == 1 && padapter) {
		if (factor > 0x03)
			padapter->driver_rx_ampdu_factor = 0xFF;
		else
			padapter->driver_rx_ampdu_factor = (u8)factor;
	}
	return (long)count;
}

int proc_get_rx_ampdu_density(struct seq_file *m, void *v)
{
	struct host_proc_seq *ctx = (struct host_proc_seq *)m;

	(void)v;
	if (ctx->adapter)
		fprintf(ctx->out, "rx ampdu densityg = %x\n",
			ctx->adapter->driver_rx_ampdu_spacing);
	return 0;
}

long proc_set_rx_ampdu_density(void *file, const void *buffer, unsigned long count,
			       long *pos, void *data)
{
	struct _adapter *padapter = data;
	char tmp[32];
	int density;

	(void)file;
	(void)pos;
	if (count < 1)
		return -14;
	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -14;
	}
	if (!buffer)
		return (long)count;
	memcpy(tmp, buffer, count);
	tmp[count] = '\0';
	if (sscanf(tmp, "%d ", &density) == 1 && padapter) {
		if (density > 0x07)
			padapter->driver_rx_ampdu_spacing = 0xFF;
		else
			padapter->driver_rx_ampdu_spacing = (u8)density;
	}
	return (long)count;
}

int proc_get_tx_ampdu_density(struct seq_file *m, void *v)
{
	struct host_proc_seq *ctx = (struct host_proc_seq *)m;

	(void)v;
	if (ctx->adapter)
		fprintf(ctx->out, "tx ampdu density = %x\n",
			ctx->adapter->driver_ampdu_spacing);
	return 0;
}

long proc_set_tx_ampdu_density(void *file, const void *buffer, unsigned long count,
			       long *pos, void *data)
{
	struct _adapter *padapter = data;
	char tmp[32];
	int density;

	(void)file;
	(void)pos;
	if (count < 1)
		return -14;
	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -14;
	}
	if (!buffer)
		return (long)count;
	memcpy(tmp, buffer, count);
	tmp[count] = '\0';
	if (sscanf(tmp, "%d ", &density) == 1 && padapter) {
		if (density > 0x07)
			padapter->driver_ampdu_spacing = 0xFF;
		else
			padapter->driver_ampdu_spacing = (u8)density;
	}
	return (long)count;
}

int proc_get_tx_max_agg_num(struct seq_file *m, void *v)
{
	struct host_proc_seq *ctx = (struct host_proc_seq *)m;

	(void)v;
	if (ctx->adapter)
		fprintf(ctx->out, "tx max AMPDU num = 0x%02x\n",
			ctx->adapter->driver_tx_max_agg_num);
	return 0;
}

long proc_set_tx_max_agg_num(void *file, const void *buffer, unsigned long count,
			     long *pos, void *data)
{
	struct _adapter *padapter = data;
	char tmp[32];
	unsigned int agg_num;

	(void)file;
	(void)pos;
	if (count < 1 || count > sizeof(tmp))
		return -14;
	if (!buffer)
		return (long)count;
	memcpy(tmp, buffer, count);
	tmp[count] = '\0';
	if (sscanf(tmp, "%x", &agg_num) == 1 && padapter)
		padapter->driver_tx_max_agg_num = (u8)agg_num;
	return (long)count;
}
