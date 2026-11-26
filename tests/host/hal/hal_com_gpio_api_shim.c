// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-142 CONFIG_GPIO_API L2 harness. */

#include <string.h>
#include "host_hal_com_gpio_api_types.h"

struct host_gpio_api_trace host_gpio_api_trace;

static struct _adapter g_adapter;
static u8 g_bank0[4];
static u8 g_bank1[4];
static u8 g_func_check_fail_gpio = 0xff;
static int g_func_check_fail_enable;

static u8 *bank_bytes(u32 addr, unsigned int *off_out)
{
	if (addr >= REG_GPIO_PIN_CTRL && addr < REG_GPIO_PIN_CTRL + 4) {
		*off_out = addr - REG_GPIO_PIN_CTRL;
		return g_bank0;
	}
	if (addr >= REG_GPIO_PIN_CTRL_2 && addr < REG_GPIO_PIN_CTRL_2 + 4) {
		*off_out = addr - REG_GPIO_PIN_CTRL_2;
		return g_bank1;
	}
	return NULL;
}

static void seed_gpio_banks(void)
{
	unsigned int i;

	for (i = 0; i < 4; i++) {
		g_bank0[i] = (u8)(0xc0 + i);
		g_bank1[i] = (u8)(0xd0 + i);
	}
}

struct _adapter *host_gpio_api_adapter(void)
{
	return &g_adapter;
}

void host_gpio_api_reset(void)
{
	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.pwrctrlpriv.rf_pwrstate = 0x2a;
	seed_gpio_banks();
	memset(&host_gpio_api_trace, 0, sizeof(host_gpio_api_trace));
	g_func_check_fail_enable = 0;
	g_func_check_fail_gpio = 0xff;
}

void host_gpio_api_set_func_check_fail(u8 gpio_num, int fail)
{
	g_func_check_fail_gpio = gpio_num;
	g_func_check_fail_enable = fail;
}

u8 host_gpio_api_reg_get(u32 base, unsigned int off)
{
	if (base == REG_GPIO_PIN_CTRL && off < 4)
		return g_bank0[off];
	if (base == REG_GPIO_PIN_CTRL_2 && off < 4)
		return g_bank1[off];
	return 0;
}

void host_gpio_api_reg_set(u32 base, unsigned int off, u8 val)
{
	if (base == REG_GPIO_PIN_CTRL && off < 4)
		g_bank0[off] = val;
	else if (base == REG_GPIO_PIN_CTRL_2 && off < 4)
		g_bank1[off] = val;
}

static unsigned int count_mmio_writes(void)
{
	unsigned int i, n = 0;

	for (i = 0; i < host_gpio_api_trace.op_count; i++) {
		if (host_gpio_api_trace.ops[i].is_write)
			n++;
	}
	return n;
}

static void trace_mmio(u32 addr, u8 val, u8 is_write)
{
	if (host_gpio_api_trace.op_count >= HOST_GPIO_API_MMIO_MAX) {
		host_gpio_api_trace.mmio_dropped++;
		return;
	}
	host_gpio_api_trace.ops[host_gpio_api_trace.op_count].addr = addr;
	host_gpio_api_trace.ops[host_gpio_api_trace.op_count].val = val;
	host_gpio_api_trace.ops[host_gpio_api_trace.op_count].is_write = is_write;
	host_gpio_api_trace.op_count++;
}

u8 rtw_read8(struct _adapter *padapter, u32 addr)
{
	unsigned int off;
	u8 *bank = bank_bytes(addr, &off);

	(void)padapter;
	if (!bank || off >= 4)
		return 0;
	trace_mmio(addr, bank[off], 0);
	return bank[off];
}

void rtw_write8(struct _adapter *padapter, u32 addr, u8 val)
{
	unsigned int off;
	u8 *bank = bank_bytes(addr, &off);

	(void)padapter;
	if (!bank || off >= 4)
		return;
	bank[off] = val;
	trace_mmio(addr, val, 1);
}

struct pwrctrl_priv *adapter_to_pwrctl(struct _adapter *adapter)
{
	return &adapter->pwrctrlpriv;
}

int rtw_hal_gpio_func_check(struct _adapter *padapter, u8 gpio_num)
{
	(void)padapter;
	host_gpio_api_trace.func_check_calls++;
	if (g_func_check_fail_enable && gpio_num == g_func_check_fail_gpio)
		return _FAIL;
	return _SUCCESS;
}

void host_gpio_api_record_ps_step(u8 step)
{
	if (host_gpio_api_trace.ps_step_count >= HOST_GPIO_API_PS_MAX_STEPS)
		return;
	host_gpio_api_trace.ps_steps[host_gpio_api_trace.ps_step_count++] = step;
}

void host_gpio_api_note_rf_pwrstate(u8 rf_pwrstate)
{
	host_gpio_api_trace.rf_pwrstate_log_count++;
	host_gpio_api_trace.rf_pwrstate_logged = rf_pwrstate;
	host_gpio_api_record_ps_step(HOST_GPIO_API_PS_STEP_RF_PWRSTATE_LOG);
}

void rtw_hal_gpio_multi_func_reset(struct _adapter *padapter, u8 gpio_num)
{
	(void)padapter;
	host_gpio_api_trace.multi_func_reset_mmio_writes_at_call = count_mmio_writes();
	host_gpio_api_trace.multi_func_reset_last_gpio = gpio_num;
	host_gpio_api_trace.multi_func_reset_count++;
}

void rtw_ps_deny(struct _adapter *adapter, u32 reason)
{
	if (!adapter)
		return;
	host_gpio_api_trace.ps_deny_count++;
	host_gpio_api_trace.ps_deny_reason = reason;
	host_gpio_api_record_ps_step(HOST_GPIO_API_PS_STEP_DENY);
}

void rtw_ps_deny_cancel(struct _adapter *adapter, u32 reason)
{
	if (!adapter)
		return;
	host_gpio_api_trace.ps_deny_cancel_count++;
	host_gpio_api_trace.ps_deny_cancel_reason = reason;
	host_gpio_api_record_ps_step(HOST_GPIO_API_PS_STEP_DENY_CANCEL);
}

void LeaveAllPowerSaveModeDirect(struct _adapter *adapter)
{
	(void)adapter;
	host_gpio_api_trace.leave_ps_count++;
	host_gpio_api_record_ps_step(HOST_GPIO_API_PS_STEP_LEAVE_PS);
}

unsigned int host_gpio_api_mmio_read_count(void)
{
	unsigned int i, n = 0;

	for (i = 0; i < host_gpio_api_trace.op_count; i++) {
		if (!host_gpio_api_trace.ops[i].is_write)
			n++;
	}
	return n;
}

unsigned int host_gpio_api_mmio_write_count(void)
{
	unsigned int i, n = 0;

	for (i = 0; i < host_gpio_api_trace.op_count; i++) {
		if (host_gpio_api_trace.ops[i].is_write)
			n++;
	}
	return n;
}

u32 host_gpio_api_mmio_addr(unsigned int write_idx)
{
	unsigned int i, w = 0;

	for (i = 0; i < host_gpio_api_trace.op_count; i++) {
		if (!host_gpio_api_trace.ops[i].is_write)
			continue;
		if (w == write_idx)
			return host_gpio_api_trace.ops[i].addr;
		w++;
	}
	return 0;
}

u8 host_gpio_api_mmio_val(unsigned int write_idx)
{
	unsigned int i, w = 0;

	for (i = 0; i < host_gpio_api_trace.op_count; i++) {
		if (!host_gpio_api_trace.ops[i].is_write)
			continue;
		if (w == write_idx)
			return host_gpio_api_trace.ops[i].val;
		w++;
	}
	return 0;
}