/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_FW_IPS_H
#define HOST_PWRCTRL_FW_IPS_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef u32 systime;

#define _TRUE 1
#define _FALSE 0
#define IPS_NONE 0

#define H2C_INACTIVE_PS_ 0x27
#define H2C_INACTIVE_PS_LEN 4
#define REG_HMETFR 0x01CC
#define REG_CR 0x0100

#define HOST_HW_VAR_RPWM_TOG 147
#define HOST_HW_VAR_SET_RPWM 109
#define HOST_HW_VAR_CPWM 110

struct pwrctrl_priv_fw_ips {
	u8 ips_mode_req;
	u8 tog;
};

struct adapter_fw_ips {
	u8 netif_up;
	struct pwrctrl_priv_fw_ips pwrctrlpriv;
};

typedef struct adapter_fw_ips _adapter_fw_ips;
typedef struct adapter_fw_ips *PADAPTER_FW_IPS;

#define adapter_to_pwrctl_fw_ips(a) (&(a)->pwrctrlpriv)

struct host_pwrctrl_fw_ips_h2c {
	u8 cmd;
	u8 len;
	u8 parm[H2C_INACTIVE_PS_LEN];
};

struct host_pwrctrl_fw_ips_hwreg {
	u8 var;
	u8 val;
};

struct host_pwrctrl_fw_ips_trace {
	int btcoex_ips_notify_calls;
	u8 last_btcoex_mode;
	int h2c_count;
	struct host_pwrctrl_fw_ips_h2c h2c[4];
	int read8_count;
	u8 read8_reg[32];
	u8 read8_val[32];
	int hwreg_count;
	struct host_pwrctrl_fw_ips_hwreg hwreg[16];
	u8 final_tog;
};

#ifndef FW_IPS_KERN_REF
void host_pwrctrl_fw_ips_reset_trace(void);
struct host_pwrctrl_fw_ips_trace *host_pwrctrl_fw_ips_get_trace(void);

void host_pwrctrl_fw_ips_set_hmetfr_sequence(const u8 *vals, int n);
void host_pwrctrl_fw_ips_set_reg_cr_sequence(const u8 *vals, int n);
void host_pwrctrl_fw_ips_set_cpwm_leave(u8 cpwm_orig, u8 cpwm_after_reads, u32 reads_before_toggle);
void host_pwrctrl_fw_ips_set_time_ms(u32 ms);
#endif

void rtw_set_fw_in_ips_mode(PADAPTER_FW_IPS padapter, u8 enable);

#endif /* HOST_PWRCTRL_FW_IPS_H */
