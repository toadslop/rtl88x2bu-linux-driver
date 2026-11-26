// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-140 GPIO pin MMIO L2 harness. */

#include <string.h>
#include "host_hal_com_gpio_pin_types.h"

struct host_gpio_pin_trace host_gpio_pin_trace;

static struct _adapter g_adapter;
static u8 g_bank0[4];
static u8 g_bank1[4];

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

struct _adapter *host_gpio_pin_adapter(void)
{
	return &g_adapter;
}

void host_gpio_pin_reset(void)
{
	memset(&g_adapter, 0, sizeof(g_adapter));
	memset(g_bank0, 0xff, sizeof(g_bank0));
	memset(g_bank1, 0xff, sizeof(g_bank1));
	memset(&host_gpio_pin_trace, 0, sizeof(host_gpio_pin_trace));
}

u8 host_gpio_pin_reg_get(u32 base, unsigned int off)
{
	if (base == REG_GPIO_PIN_CTRL && off < 4)
		return g_bank0[off];
	if (base == REG_GPIO_PIN_CTRL_2 && off < 4)
		return g_bank1[off];
	return 0;
}

void host_gpio_pin_reg_set(u32 base, unsigned int off, u8 val)
{
	if (base == REG_GPIO_PIN_CTRL && off < 4)
		g_bank0[off] = val;
	else if (base == REG_GPIO_PIN_CTRL_2 && off < 4)
		g_bank1[off] = val;
}

unsigned int host_gpio_pin_mmio_count(void)
{
	return host_gpio_pin_trace.op_count;
}

u32 host_gpio_pin_mmio_addr(unsigned int idx)
{
	if (idx >= host_gpio_pin_trace.op_count)
		return 0;
	return host_gpio_pin_trace.ops[idx].addr;
}

u8 host_gpio_pin_mmio_val(unsigned int idx)
{
	if (idx >= host_gpio_pin_trace.op_count)
		return 0;
	return host_gpio_pin_trace.ops[idx].val;
}

static void trace_mmio(u32 addr, u8 val)
{
	if (host_gpio_pin_trace.op_count >= HOST_GPIO_PIN_MMIO_MAX)
		return;
	host_gpio_pin_trace.ops[host_gpio_pin_trace.op_count].addr = addr;
	host_gpio_pin_trace.ops[host_gpio_pin_trace.op_count].val = val;
	host_gpio_pin_trace.op_count++;
}

u8 rtw_read8(struct _adapter *padapter, u32 addr)
{
	unsigned int off;
	u8 *bank = bank_bytes(addr, &off);

	(void)padapter;
	if (!bank || off >= 4)
		return 0;
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
	trace_mmio(addr, val);
}

void rtw_rust_hal_com_gpio_pin_invalid_output(struct _adapter *padapter, u8 index,
					      u8 outputval)
{
	(void)padapter;
	(void)index;
	(void)outputval;
	host_gpio_pin_trace.invalid_output_logs++;
}

void rtw_rust_hal_com_gpio_pin_invalid_input(struct _adapter *padapter, u8 index)
{
	(void)padapter;
	(void)index;
	host_gpio_pin_trace.invalid_input_logs++;
}