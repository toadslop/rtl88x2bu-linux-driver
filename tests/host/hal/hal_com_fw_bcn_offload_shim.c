// SPDX-License-Identifier: GPL-2.0
#include <string.h>
#include "host_hal_com_fw_bcn_offload_types.h"

struct h2c_record {
	u8 cmd;
	unsigned int len;
	u8 buf[8];
};

static struct h2c_record g_h2c[8];
static int g_h2c_count;
static s32 g_fill_ret = 1;
static struct dvobj_priv g_dvobj;
static char g_adapter_blob[1];

void host_fw_bcn_offload_reset(void)
{
	memset(g_h2c, 0, sizeof(g_h2c));
	g_h2c_count = 0;
	g_fill_ret = 1;
	memset(&g_dvobj, 0, sizeof(g_dvobj));
}

int host_fw_bcn_offload_h2c_count(void)
{
	return g_h2c_count;
}

u8 host_fw_bcn_offload_h2c_cmd(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return 0;
	return g_h2c[idx].cmd;
}

unsigned int host_fw_bcn_offload_h2c_len(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return 0;
	return g_h2c[idx].len;
}

const u8 *host_fw_bcn_offload_h2c_buf(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return NULL;
	return g_h2c[idx].buf;
}

u8 host_fw_bcn_offload_get_fw_bcn(void)
{
	return g_dvobj.fw_bcn_offload;
}

u8 host_fw_bcn_offload_get_tbtt_map(void)
{
	return g_dvobj.vap_tbtt_rpt_map;
}

struct _adapter *host_fw_bcn_offload_adapter(void)
{
	return (struct _adapter *)g_adapter_blob;
}

struct dvobj_priv *adapter_to_dvobj(struct _adapter *adapter)
{
	(void)adapter;
	return &g_dvobj;
}

s32 rtw_hal_fill_h2c_cmd(struct _adapter *adapter, u8 cmd, unsigned int len, u8 *buf)
{
	(void)adapter;

	if (g_h2c_count >= (int)(sizeof(g_h2c) / sizeof(g_h2c[0])))
		return 0;
	g_h2c[g_h2c_count].cmd = cmd;
	g_h2c[g_h2c_count].len = len;
	memset(g_h2c[g_h2c_count].buf, 0, sizeof(g_h2c[g_h2c_count].buf));
	if (buf && len > 0) {
		unsigned int n = len;

		if (n > sizeof(g_h2c[g_h2c_count].buf))
			n = sizeof(g_h2c[g_h2c_count].buf);
		memcpy(g_h2c[g_h2c_count].buf, buf, n);
	}
	g_h2c_count++;
	return g_fill_ret;
}