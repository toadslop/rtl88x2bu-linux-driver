/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_GPIO_PIN_TYPES_H
#define HOST_HAL_COM_GPIO_PIN_TYPES_H

typedef unsigned char u8;
typedef unsigned int u32;

#define REG_GPIO_PIN_CTRL 0x0044
#define REG_GPIO_PIN_CTRL_2 0x0060

#define BIT(x) (1U << (x))

struct _adapter {
	int _unused;
};

/* Full 0–15 sweep: up to 80 writes plus RMW reads (see W4-140 L2). */
#define HOST_GPIO_PIN_MMIO_MAX 128

struct host_gpio_pin_mmio_op {
	u32 addr;
	u8 val;
};

struct host_gpio_pin_trace {
	struct host_gpio_pin_mmio_op ops[HOST_GPIO_PIN_MMIO_MAX];
	unsigned int op_count;
	unsigned int mmio_dropped;
	unsigned int invalid_output_logs;
	unsigned int invalid_input_logs;
	unsigned int hwreg_output_count;
	unsigned int hwreg_input_count;
	u8 last_hwreg_output;
	u8 last_hwreg_input;
};

extern struct host_gpio_pin_trace host_gpio_pin_trace;

struct _adapter *host_gpio_pin_adapter(void);
void host_gpio_pin_reset(void);
unsigned int host_gpio_pin_mmio_count(void);
unsigned int host_gpio_pin_mmio_dropped(void);
u32 host_gpio_pin_mmio_addr(unsigned int idx);
u8 host_gpio_pin_mmio_val(unsigned int idx);

u8 rtw_rust_hal_com_read8(struct _adapter *adapter, u32 addr);
void rtw_rust_hal_com_write8(struct _adapter *adapter, u32 addr, u8 val);

u8 host_gpio_pin_reg_get(u32 base, unsigned int off);
void host_gpio_pin_reg_set(u32 base, unsigned int off, u8 val);

u8 rtw_read8(struct _adapter *padapter, u32 addr);
void rtw_write8(struct _adapter *padapter, u32 addr, u8 val);

void rtw_hal_set_output_gpio(struct _adapter *padapter, u8 index, u8 outputval);
void rtw_hal_set_input_gpio(struct _adapter *padapter, u8 index);

void rtw_rust_hal_com_gpio_pin_invalid_output(struct _adapter *padapter, u8 index,
					      u8 outputval);
void rtw_rust_hal_com_gpio_pin_invalid_input(struct _adapter *padapter, u8 index);

#endif