// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-139 FW media status H2C L2 harness. */

#include <string.h>
#include "host_hal_com_fw_media_status_types.h"

struct host_fw_media_status_trace host_fw_media_status_trace;

static struct {
	u8 cmd;
	unsigned int len;
	u8 buf[H2C_MEDIA_STATUS_RPT_LEN];
} g_h2c[4];
static int g_h2c_count;
static s32 g_fill_ret = _SUCCESS;

static struct _adapter g_adapter;

struct dvobj_priv *adapter_to_dvobj(struct _adapter *a)
{
	return a ? &a->dvobj : NULL;
}

struct macid_ctl_t *dvobj_to_macidctl(struct dvobj_priv *d)
{
	return d ? &d->macid_ctl : NULL;
}

struct _adapter *host_fw_media_status_adapter(void)
{
	if (!g_adapter.dvobj.macid_ctl.num)
		g_adapter.dvobj.macid_ctl.num = MACID_NUM_SW_LIMIT;
	return &g_adapter;
}

void host_fw_media_status_reset(void)
{
	memset(g_h2c, 0, sizeof(g_h2c));
	g_h2c_count = 0;
	g_fill_ret = _SUCCESS;
	memset(&host_fw_media_status_trace, 0,
	       sizeof(host_fw_media_status_trace));
	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.dvobj.macid_ctl.num = MACID_NUM_SW_LIMIT;
	g_adapter.hw_port = 2;
}

void host_fw_media_status_set_fill_ret(s32 v)
{
	g_fill_ret = v;
}

int host_fw_media_status_h2c_count(void)
{
	return g_h2c_count;
}

u8 host_fw_media_status_h2c_cmd(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return 0;
	return g_h2c[idx].cmd;
}

unsigned int host_fw_media_status_h2c_len(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return 0;
	return g_h2c[idx].len;
}

const u8 *host_fw_media_status_h2c_buf(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return NULL;
	return g_h2c[idx].buf;
}

s32 rtw_hal_fill_h2c_cmd(struct _adapter *adapter, u8 cmd, unsigned int len,
			 u8 *buf)
{
	(void)adapter;

	if (g_h2c_count >= (int)(sizeof(g_h2c) / sizeof(g_h2c[0])))
		return _FAIL;
	g_h2c[g_h2c_count].cmd = cmd;
	g_h2c[g_h2c_count].len = len;
	memset(g_h2c[g_h2c_count].buf, 0, sizeof(g_h2c[g_h2c_count].buf));
	if (buf && len > 0) {
		unsigned int n = len;

		if (n > H2C_MEDIA_STATUS_RPT_LEN)
			n = H2C_MEDIA_STATUS_RPT_LEN;
		memcpy(g_h2c[g_h2c_count].buf, buf, n);
	}
	g_h2c_count++;
	return g_fill_ret;
}

u8 rtw_hal_get_port(struct _adapter *adapter)
{
	return adapter ? adapter->hw_port : 0;
}

u8 rtw_macid_ctl_set_h2c_msr(struct macid_ctl_t *macid_ctl, u8 id, u8 h2c_msr)
{
	u8 op_num_change_bmp = 0;

	if (!macid_ctl || id >= macid_ctl->num)
		return 0;

	if (GET_H2CCMD_MSRRPT_PARM_OPMODE(&macid_ctl->h2c_msr[id]) &&
	    !GET_H2CCMD_MSRRPT_PARM_OPMODE(&h2c_msr)) {
		u8 role = GET_H2CCMD_MSRRPT_PARM_ROLE(&macid_ctl->h2c_msr[id]);

		if (role < H2C_MSR_ROLE_MAX) {
			macid_ctl->op_num[role]--;
			op_num_change_bmp |= (u8)(1u << role);
		}
	} else if (!GET_H2CCMD_MSRRPT_PARM_OPMODE(&macid_ctl->h2c_msr[id]) &&
		   GET_H2CCMD_MSRRPT_PARM_OPMODE(&h2c_msr)) {
		u8 role = GET_H2CCMD_MSRRPT_PARM_ROLE(&h2c_msr);

		if (role < H2C_MSR_ROLE_MAX) {
			macid_ctl->op_num[role]++;
			op_num_change_bmp |= (u8)(1u << role);
		}
	}

	macid_ctl->h2c_msr[id] = h2c_msr;
	host_fw_media_status_trace.last_h2c_msr = h2c_msr;
	return op_num_change_bmp;
}

void rtw_macid_ctl_set_bw(struct macid_ctl_t *c, u8 id, u8 bw)
{
	if (c && id < c->num)
		c->bw[id] = bw;
}

void rtw_macid_ctl_set_vht_en(struct macid_ctl_t *c, u8 id, u8 en)
{
	if (c && id < c->num)
		c->vht_en[id] = en;
}

void rtw_macid_ctl_set_rate_bmp0(struct macid_ctl_t *c, u8 id, u32 bmp)
{
	if (c && id < c->num)
		c->rate_bmp0[id] = bmp;
}

void rtw_macid_ctl_set_rate_bmp1(struct macid_ctl_t *c, u8 id, u32 bmp)
{
	if (c && id < c->num)
		c->rate_bmp1[id] = bmp;
}

void rtw_hal_update_tx_aclt(struct _adapter *adapter)
{
	(void)adapter;
	host_fw_media_status_trace.tx_aclt_calls++;
}

void rtw_update_tx_rate_bmp(struct dvobj_priv *dvobj)
{
	(void)dvobj;
	host_fw_media_status_trace.update_tx_rate_bmp_calls++;
}