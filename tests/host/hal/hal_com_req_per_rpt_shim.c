// SPDX-License-Identifier: GPL-2.0
/* Host shim recording rtw_hal_fill_h2c_cmd for W4-118 L2. */

#include <string.h>
#include "host_hal_com_req_per_rpt_types.h"

#define MAX_H2C_RECORDS 8

struct h2c_record {
	unsigned char cmd;
	unsigned int len;
	unsigned char buf[8];
};

static struct h2c_record g_h2c[MAX_H2C_RECORDS];
static int g_h2c_count;
static unsigned char g_fill_ret = 1;

static char g_adapter_storage[1];

void host_req_per_rpt_reset(void)
{
	memset(g_h2c, 0, sizeof(g_h2c));
	g_h2c_count = 0;
	g_fill_ret = 1;
}

void host_req_per_rpt_set_fill_ret(unsigned char v)
{
	g_fill_ret = v;
}

int host_req_per_rpt_h2c_count(void)
{
	return g_h2c_count;
}

unsigned char host_req_per_rpt_last_cmd(void)
{
	return g_h2c_count ? g_h2c[g_h2c_count - 1].cmd : 0;
}

unsigned int host_req_per_rpt_last_len(void)
{
	return g_h2c_count ? g_h2c[g_h2c_count - 1].len : 0;
}

const unsigned char *host_req_per_rpt_last_buf(void)
{
	return g_h2c_count ? g_h2c[g_h2c_count - 1].buf : NULL;
}

struct _adapter *host_req_per_rpt_adapter(void)
{
	return (struct _adapter *)g_adapter_storage;
}

long rtw_hal_fill_h2c_cmd(struct _adapter *adapter, unsigned char cmd, unsigned int len,
			  unsigned char *buf)
{
	(void)adapter;
	if (g_h2c_count >= MAX_H2C_RECORDS)
		return 0;
	g_h2c[g_h2c_count].cmd = cmd;
	g_h2c[g_h2c_count].len = len;
	if (len > sizeof(g_h2c[0].buf))
		len = sizeof(g_h2c[0].buf);
	memcpy(g_h2c[g_h2c_count].buf, buf, len);
	g_h2c_count++;
	return g_fill_ret;
}