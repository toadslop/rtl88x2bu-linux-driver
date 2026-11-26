/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_GPIO_API_TYPES_H
#define HOST_HAL_COM_GPIO_API_TYPES_H

typedef unsigned char u8;
typedef unsigned int u32;

#define _SUCCESS 1
#define _FAIL 0

#define PS_DENY_IOCTL 0

#define REG_GPIO_PIN_CTRL 0x0044
#define REG_GPIO_PIN_CTRL_2 0x0060

#define BIT(x) (1U << (x))

struct pwrctrl_priv {
	u8 rf_pwrstate;
	u32 ps_deny;
};

struct _adapter {
	struct pwrctrl_priv pwrctrlpriv;
};

#define HOST_GPIO_API_MMIO_MAX 64

struct host_gpio_api_mmio_op {
	u32 addr;
	u8 val;
	u8 is_write;
};

struct host_gpio_api_trace {
	struct host_gpio_api_mmio_op ops[HOST_GPIO_API_MMIO_MAX];
	unsigned int op_count;
	unsigned int mmio_dropped;
	unsigned int ps_deny_count;
	unsigned int ps_deny_cancel_count;
	unsigned int leave_ps_count;
	unsigned int func_check_calls;
	unsigned int multi_func_reset_count;
	unsigned int rf_pwrstate_log_count;
};

extern struct host_gpio_api_trace host_gpio_api_trace;

struct _adapter *host_gpio_api_adapter(void);
void host_gpio_api_reset(void);
void host_gpio_api_set_func_check_fail(u8 gpio_num, int fail);
u8 host_gpio_api_reg_get(u32 base, unsigned int off);
void host_gpio_api_reg_set(u32 base, unsigned int off, u8 val);

unsigned int host_gpio_api_mmio_read_count(void);
unsigned int host_gpio_api_mmio_write_count(void);
u32 host_gpio_api_mmio_addr(unsigned int write_idx);
u8 host_gpio_api_mmio_val(unsigned int write_idx);

u8 rtw_read8(struct _adapter *padapter, u32 addr);
void rtw_write8(struct _adapter *padapter, u32 addr, u8 val);

struct pwrctrl_priv *adapter_to_pwrctl(struct _adapter *adapter);
int rtw_hal_gpio_func_check(struct _adapter *padapter, u8 gpio_num);
void rtw_hal_gpio_multi_func_reset(struct _adapter *padapter, u8 gpio_num);
void rtw_ps_deny(struct _adapter *adapter, u32 reason);
void rtw_ps_deny_cancel(struct _adapter *adapter, u32 reason);
void LeaveAllPowerSaveModeDirect(struct _adapter *adapter);

u8 rtw_hal_get_gpio(struct _adapter *adapter, u8 gpio_num);
int rtw_hal_set_gpio_output_value(struct _adapter *adapter, u8 gpio_num, u8 isHigh);
int rtw_hal_config_gpio(struct _adapter *adapter, u8 gpio_num, u8 isOutput);

#endif