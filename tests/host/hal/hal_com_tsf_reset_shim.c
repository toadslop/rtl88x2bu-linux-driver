// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-143 TSF reset H2C offload L2 harness. */

#include <string.h>
#include "host_hal_com_tsf_reset_types.h"

struct host_tsf_reset_state {
	u8 cnt0;
	u8 cnt1;
	unsigned read0_idx;
	unsigned read1_idx;
	u8 read0_seq[32];
	u8 read1_seq[32];
	unsigned read0_seq_len;
	unsigned read1_seq_len;
};

static struct host_tsf_reset_state g_state;
static struct h2c_record {
	u8 cmd;
	unsigned int len;
	u8 buf[8];
} g_h2c[4];
static int g_h2c_count;
static s32 g_fill_ret = _SUCCESS;
static unsigned g_msleep_count;
static unsigned g_buddy_abort_count;
static unsigned g_reg0_read_count;
static unsigned g_reg1_read_count;
static unsigned g_msleep_arg_sum;
static u8 g_last_bwait;
static char g_adapter_blob[1];

void host_tsf_reset_reset(void)
{
	memset(&g_state, 0, sizeof(g_state));
	memset(g_h2c, 0, sizeof(g_h2c));
	g_h2c_count = 0;
	g_fill_ret = _SUCCESS;
	g_msleep_count = 0;
	g_buddy_abort_count = 0;
	g_reg0_read_count = 0;
	g_reg1_read_count = 0;
	g_msleep_arg_sum = 0;
	g_last_bwait = 0;
}

void host_tsf_reset_set_fill_ret(s32 v)
{
	g_fill_ret = v;
}

void host_tsf_reset_set_cnt_defaults(u8 cnt0, u8 cnt1)
{
	g_state.cnt0 = cnt0;
	g_state.cnt1 = cnt1;
}

void host_tsf_reset_set_cnt0_reads(const u8 *vals, unsigned len)
{
	unsigned i;

	if (len > sizeof(g_state.read0_seq))
		len = sizeof(g_state.read0_seq);
	g_state.read0_seq_len = len;
	for (i = 0; i < len; i++)
		g_state.read0_seq[i] = vals[i];
	g_state.read0_idx = 0;
}

void host_tsf_reset_set_cnt1_reads(const u8 *vals, unsigned len)
{
	unsigned i;

	if (len > sizeof(g_state.read1_seq))
		len = sizeof(g_state.read1_seq);
	g_state.read1_seq_len = len;
	for (i = 0; i < len; i++)
		g_state.read1_seq[i] = vals[i];
	g_state.read1_idx = 0;
}

unsigned host_tsf_reset_msleep_count(void)
{
	return g_msleep_count;
}

unsigned host_tsf_reset_buddy_abort_count(void)
{
	return g_buddy_abort_count;
}

unsigned host_tsf_reset_reg0_read_count(void)
{
	return g_reg0_read_count;
}

unsigned host_tsf_reset_reg1_read_count(void)
{
	return g_reg1_read_count;
}

unsigned host_tsf_reset_msleep_arg_sum(void)
{
	return g_msleep_arg_sum;
}

u8 host_tsf_reset_last_bwait(void)
{
	return g_last_bwait;
}

int host_tsf_reset_h2c_count(void)
{
	return g_h2c_count;
}

u8 host_tsf_reset_h2c_cmd(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return 0;
	return g_h2c[idx].cmd;
}

unsigned int host_tsf_reset_h2c_len(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return 0;
	return g_h2c[idx].len;
}

const u8 *host_tsf_reset_h2c_buf(int idx)
{
	if (idx < 0 || idx >= g_h2c_count)
		return NULL;
	return g_h2c[idx].buf;
}

struct _adapter *host_tsf_reset_adapter(void)
{
	return (struct _adapter *)g_adapter_blob;
}

static u8 next_cnt(u32 reg, u8 fallback)
{
	if (reg == REG_FW_RESET_TSF_CNT_0) {
		if (g_state.read0_idx < g_state.read0_seq_len)
			return g_state.read0_seq[g_state.read0_idx++];
		return fallback;
	}
	if (reg == REG_FW_RESET_TSF_CNT_1) {
		if (g_state.read1_idx < g_state.read1_seq_len)
			return g_state.read1_seq[g_state.read1_idx++];
		return fallback;
	}
	return 0;
}

u8 rtw_read8(void *adapter, u32 addr)
{
	(void)adapter;
	if (addr == REG_FW_RESET_TSF_CNT_0) {
		g_reg0_read_count++;
		return next_cnt(REG_FW_RESET_TSF_CNT_0, g_state.cnt0);
	}
	if (addr == REG_FW_RESET_TSF_CNT_1) {
		g_reg1_read_count++;
		return next_cnt(REG_FW_RESET_TSF_CNT_1, g_state.cnt1);
	}
	return 0;
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

void rtw_msleep_os(unsigned int ms)
{
	g_msleep_arg_sum += ms;
	g_msleep_count++;
}

void rtw_mi_buddy_scan_abort(void *adapter, u8 bwait)
{
	(void)adapter;
	g_last_bwait = bwait;
	g_buddy_abort_count++;
}