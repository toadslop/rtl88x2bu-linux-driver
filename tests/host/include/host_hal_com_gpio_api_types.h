/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_GPIO_API_TYPES_H
#define HOST_HAL_COM_GPIO_API_TYPES_H

typedef unsigned char u8;
typedef unsigned int u32;

#define _SUCCESS 1
#define _FAIL 0

/* PS_DENY_IOCTL in include/rtw_pwrctrl.h (enum value 5). */
#define PS_DENY_IOCTL 5u

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

#define HOST_GPIO_API_PS_STEP_DENY 1u
#define HOST_GPIO_API_PS_STEP_RF_PWRSTATE_LOG 2u
#define HOST_GPIO_API_PS_STEP_LEAVE_PS 3u
#define HOST_GPIO_API_PS_STEP_DENY_CANCEL 4u
#define HOST_GPIO_API_PS_STEP_MULTI_FUNC_RESET 5u
#define HOST_GPIO_API_PS_MAX_STEPS 16u

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
	u32 ps_deny_reason;
	u32 ps_deny_cancel_reason;
	u8 rf_pwrstate_logged;
	u8 multi_func_reset_last_gpio;
	unsigned int multi_func_reset_mmio_writes_at_call;
	u8 ps_steps[HOST_GPIO_API_PS_MAX_STEPS];
	unsigned int ps_step_count;
	unsigned int mmio_before_leave_count;
	unsigned int deny_cancel_before_leave_count;
	unsigned int mmio_before_mfr_count;
	u8 first_write_mfr_snapshot;
};

extern struct host_gpio_api_trace host_gpio_api_trace;

struct _adapter *host_gpio_api_adapter(void);
void host_gpio_api_reset(void);
void host_gpio_api_set_func_check_fail(u8 gpio_num, int fail);
void host_gpio_api_record_ps_step(u8 step);
void host_gpio_api_note_rf_pwrstate(u8 rf_pwrstate);
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