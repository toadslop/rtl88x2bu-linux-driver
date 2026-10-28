/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_LPS_EXEC_H
#define HOST_PWRCTRL_LPS_EXEC_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;

#define _TRUE 1
#define _FALSE 0

#define PS_MODE_ACTIVE 0
#define PS_MODE_MIN 1

#define rf_on 0
#define rf_off 2

#define WIFI_ASOC_STATE 0x00000001U

#define HOST_HW_VAR_H2C_FW_PWRMODE 0
#define HOST_HW_VAR_LPS_STATE_CHK 1
#define HOST_HW_VAR_H2C_FW_PWRMODE_RFON_CTRL 2
#define HOST_HW_VAR_LPS_RFON_CHK 3

struct mlme_priv {
	u32 fw_state;
};

struct pwrctrl_priv {
	u8 bFwCurrentInPSMode;
	u8 pwr_mode;
};

struct adapter {
	struct pwrctrl_priv pwrctrlpriv;
	struct mlme_priv mlmepriv;
};

typedef struct adapter _adapter;
typedef struct adapter *PADAPTER;

#define adapter_to_pwrctl(a) (&(a)->pwrctrlpriv)

static inline u8 check_fwstate(struct mlme_priv *m, u32 s)
{
	if (!s && !m->fw_state)
		return _TRUE;
	return (m->fw_state & s) ? _TRUE : _FALSE;
}

#define MLME_IS_ASOC(a) check_fwstate(&(a)->mlmepriv, WIFI_ASOC_STATE)

void rtw_exec_lps(PADAPTER padapter, u8 ps_mode);
void rtw_lps_rfon_ctrl(PADAPTER padapter, u8 rfon_ctrl);

struct host_pwrctrl_lps_exec_hwreg {
	u8 var;
	u8 val;
};

struct host_pwrctrl_lps_exec_trace {
	u32 hwreg_count;
	struct host_pwrctrl_lps_exec_hwreg hwreg[8];
};

void host_pwrctrl_lps_exec_reset_trace(void);
struct host_pwrctrl_lps_exec_trace *host_pwrctrl_lps_exec_get_trace(void);

#endif /* HOST_PWRCTRL_LPS_EXEC_H */
