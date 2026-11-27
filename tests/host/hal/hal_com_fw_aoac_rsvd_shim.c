// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-145 WOW AOAC reserved-page H2C L2 harness. */

#include <string.h>
#include "host_hal_com_fw_aoac_rsvd_types.h"

static struct {
	u8 cmd;
	unsigned int len;
	u8 buf[8];
} g_h2c[4];
static int g_h2c_count;
static struct _adapter g_adapter;

void host_fw_aoac_rsvd_reset(void)
{
	memset(g_h2c, 0, sizeof(g_h2c));
	g_h2c_count = 0;
	memset(&g_adapter, 0, sizeof(g_adapter));
}

void host_fw_aoac_rsvd_set_asoc(int asoc)
{
	if (asoc)
		g_adapter.mlmepriv.fw_state |= WIFI_ASOC_STATE;
	else
		g_adapter.mlmepriv.fw_state &= ~WIFI_ASOC_STATE;
}

void host_fw_aoac_rsvd_set_wowlan_in_resume(u8 v)
{
	g_adapter.pwrctl.wowlan_in_resume = v;
}

u8 host_fw_aoac_rsvd_wowlan_aoac_rpt_loc(void)
{
	return g_adapter.pwrctl.wowlan_aoac_rpt_loc;
}

int host_fw_aoac_rsvd_h2c_count(void)
{
	return g_h2c_count;
}

u8 host_fw_aoac_rsvd_h2c_cmd(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return 0;
	return g_h2c[idx].cmd;
}

unsigned int host_fw_aoac_rsvd_h2c_len(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return 0;
	return g_h2c[idx].len;
}

const u8 *host_fw_aoac_rsvd_h2c_buf(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return NULL;
	return g_h2c[idx].buf;
}

struct _adapter *host_fw_aoac_rsvd_adapter(void)
{
	return &g_adapter;
}

struct pwrctrl_priv *adapter_to_pwrctl(struct _adapter *adapter)
{
	return &adapter->pwrctl;
}

s32 check_fwstate(struct mlme_priv *pmlmepriv, s32 state)
{
	return (pmlmepriv->fw_state & state) ? 1 : 0;
}

void _rtw_memset(void *p, int v, size_t n)
{
	memset(p, v, n);
}

s32 rtw_hal_fill_h2c_cmd(void *adapter, u8 id, u32 len, u8 *buf)
{
	(void)adapter;
	if (g_h2c_count >= (int)(sizeof(g_h2c) / sizeof(g_h2c[0])))
		return 1;
	g_h2c[g_h2c_count].cmd = id;
	g_h2c[g_h2c_count].len = len;
	if (len > sizeof(g_h2c[g_h2c_count].buf))
		len = sizeof(g_h2c[g_h2c_count].buf);
	memcpy(g_h2c[g_h2c_count].buf, buf, len);
	g_h2c_count++;
	return 1;
}