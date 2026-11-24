// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-136 FwRsvdPage H2C L2 harness. */

#include <string.h>
#include "host_hal_com_fw_rsvdpage_h2c_types.h"

static struct h2c_record {
	u8 cmd;
	unsigned int len;
	u8 buf[H2C_RSVDPAGE_LOC_LEN];
} g_h2c[4];
static int g_h2c_count;
static s32 g_fill_ret = 1;

static char g_adapter_blob[1];

void host_fw_rsvdpage_h2c_reset(void)
{
	memset(g_h2c, 0, sizeof(g_h2c));
	g_h2c_count = 0;
	g_fill_ret = 1;
}

void host_fw_rsvdpage_h2c_set_fill_ret(s32 v)
{
	g_fill_ret = v;
}

int host_fw_rsvdpage_h2c_count(void)
{
	return g_h2c_count;
}

u8 host_fw_rsvdpage_h2c_cmd(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return 0;
	return g_h2c[idx].cmd;
}

unsigned int host_fw_rsvdpage_h2c_len(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return 0;
	return g_h2c[idx].len;
}

const u8 *host_fw_rsvdpage_h2c_buf(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return NULL;
	return g_h2c[idx].buf;
}

struct _adapter *host_fw_rsvdpage_h2c_adapter(void)
{
	return (struct _adapter *)g_adapter_blob;
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

		if (n > H2C_RSVDPAGE_LOC_LEN)
			n = H2C_RSVDPAGE_LOC_LEN;
		memcpy(g_h2c[g_h2c_count].buf, buf, n);
	}
	g_h2c_count++;
	return g_fill_ret;
}