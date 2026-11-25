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
static u32 g_page_size = 128;
static char g_adapter_blob[1];

void host_fw_bcn_offload_reset(void)
{
	memset(g_h2c, 0, sizeof(g_h2c));
	g_h2c_count = 0;
	g_fill_ret = 1;
	g_page_size = 128;
	memset(&g_dvobj, 0, sizeof(g_dvobj));
}

void host_fw_bcn_offload_set_vap_map(u8 vap_map)
{
	g_dvobj.vap_map = vap_map;
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

void *rtw_rust_hal_com_adapter_to_dvobj(void *adapter)
{
	return adapter_to_dvobj((struct _adapter *)adapter);
}

void rtw_rust_hal_com_dvobj_set_vap_tbtt_rpt_map(void *dvobj, u8 v)
{
	((struct dvobj_priv *)dvobj)->vap_tbtt_rpt_map = v;
}

void rtw_rust_hal_com_dvobj_set_fw_bcn_offload(void *dvobj, u8 v)
{
	((struct dvobj_priv *)dvobj)->fw_bcn_offload = v ? 1 : 0;
}

void rtw_rust_hal_com_log_fw_bcn_offload(u8 en, u8 map)
{
	(void)en;
	(void)map;
}

u8 rtw_rust_hal_com_dvobj_get_vap_map(void *dvobj)
{
	return ((struct dvobj_priv *)dvobj)->vap_map;
}

void rtw_rust_hal_com_log_bcn_rsvdpage_loc(u8 vap_map, u32 page_size, u8 b0, u8 b1,
					   u8 b2, u8 b3, u8 b4)
{
	(void)vap_map;
	(void)page_size;
	(void)b0;
	(void)b1;
	(void)b2;
	(void)b3;
	(void)b4;
}

void rtw_hal_get_def_var(struct _adapter *adapter, int def, u8 *val)
{
	(void)adapter;

	if (def == HAL_DEF_TX_PAGE_SIZE && val)
		memcpy(val, &g_page_size, sizeof(g_page_size));
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
