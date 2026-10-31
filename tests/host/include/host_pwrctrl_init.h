/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_INIT_H
#define HOST_PWRCTRL_INIT_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef u32 systime;

#define _TRUE 1
#define _FALSE 0
#define PS_MODE_ACTIVE 0
#define PS_STATE_S4 ((1U << 3) | (1U << 2))
#define rf_on 0
#define RTW_PWR_STATE_CHK_INTERVAL 2000
#define HOST_HW_VAR_SET_RPWM 200

typedef int _pwrlock;

struct registry_priv_init {
	u8 ips_mode;
	u8 lps_level;
	u8 mp_mode;
	u8 power_mgnt;
	u8 smart_ps;
};

struct pwrctrl_priv_init {
	_pwrlock lock;
	_pwrlock check_32k_lock;
	u8 rf_pwrstate;
	u32 ips_enter_cnts;
	u32 ips_leave_cnts;
	u32 lps_enter_cnts;
	u32 lps_leave_cnts;
	u32 bips_processing;
	u8 ips_mode;
	u8 ips_mode_req;
	systime ips_deny_time;
	u8 lps_level;
	u32 pwr_state_check_interval;
	u32 pwr_state_check_cnts;
	u8 bInSuspend;
	u8 bkeepfwalive;
	u8 LpsIdleCount;
	u8 power_mgnt;
	u8 bLeisurePs;
	u8 bFwCurrentInPSMode;
	systime lps_deny_time;
	u8 rpwm;
	u8 cpwm;
	u8 pwr_mode;
	u8 smart_ps;
	u8 bcn_ant_mode;
	u8 dtim;
	u8 tog;
	u8 rpwm_retry;
	u8 wowlan_mode;
	u8 wowlan_ap_mode;
	u8 wowlan_p2p_mode;
	u8 wowlan_in_resume;
	u8 wowlan_last_wake_reason;
	void *cpwm_event;
	void *dma_event;
	void *resume_work;
	void *pwr_state_check_timer;
#ifdef CONFIG_LPS_PG
	struct {
		const char *name;
	} lpspg_info;
#endif
#ifdef CONFIG_LPS_ACK
	int lps_ack_mutex;
#endif
	void *rtw_workqueue;
};

struct adapter_init {
	u8 adapter_type;
	struct registry_priv_init registrypriv;
	struct pwrctrl_priv_init pwrctrlpriv;
};

typedef struct adapter_init _adapter;
typedef struct adapter_init *PADAPTER;

#define adapter_to_pwrctl(a) (&(a)->pwrctrlpriv)
#define is_primary_adapter(a) ((a)->adapter_type == 0)

struct host_pwrctrl_init_trace {
	u32 set_rpwm_hwreg_calls;
	u32 init_workitem_calls;
	u32 init_timer_calls;
	u32 create_wq_calls;
	u8 skipped_non_primary;
};

void host_pwrctrl_init_reset_trace(void);
struct host_pwrctrl_init_trace *host_pwrctrl_init_get_trace(void);
void host_pwrctrl_init_set_current_time(systime t);

void rtw_init_pwrctrl_priv_part1(PADAPTER padapter);

#endif /* HOST_PWRCTRL_INIT_H */
