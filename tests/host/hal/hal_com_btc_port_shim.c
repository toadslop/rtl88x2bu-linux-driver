// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-144 BT WLAN port H2C L2 harness. */

#include <string.h>
#include "host_hal_com_btc_port_types.h"

static struct {
	u8 cmd;
	unsigned int len;
	u8 buf[4];
} g_h2c[4];
static int g_h2c_count;
static s32 g_fill_ret = _SUCCESS;
static u8 g_hw_port;
static char g_adapter_blob[1];

void host_btc_port_reset(void)
{
	memset(g_h2c, 0, sizeof(g_h2c));
	g_h2c_count = 0;
	g_fill_ret = _SUCCESS;
	g_hw_port = 0;
}

void host_btc_port_set_hw_port(u8 port)
{
	g_hw_port = port;
}

void host_btc_port_set_fill_ret(s32 v)
{
	g_fill_ret = v;
}

int host_btc_port_h2c_count(void)
{
	return g_h2c_count;
}

u8 host_btc_port_h2c_cmd(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return 0;
	return g_h2c[idx].cmd;
}

unsigned int host_btc_port_h2c_len(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return 0;
	return g_h2c[idx].len;
}

const u8 *host_btc_port_h2c_buf(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return NULL;
	return g_h2c[idx].buf;
}

struct _adapter *host_btc_port_adapter(void)
{
	return (struct _adapter *)g_adapter_blob;
}

u8 rtw_hal_get_port(void *adapter)
{
	(void)adapter;
	return g_hw_port;
}

s32 rtw_hal_fill_h2c_cmd(void *adapter, u8 id, u32 len, u8 *buf)
{
	(void)adapter;
	if (g_h2c_count >= (int)(sizeof(g_h2c) / sizeof(g_h2c[0])))
		return g_fill_ret;
	g_h2c[g_h2c_count].cmd = id;
	g_h2c[g_h2c_count].len = len;
	if (len > sizeof(g_h2c[g_h2c_count].buf))
		len = sizeof(g_h2c[g_h2c_count].buf);
	memcpy(g_h2c[g_h2c_count].buf, buf, len);
	g_h2c_count++;
	return g_fill_ret;
}