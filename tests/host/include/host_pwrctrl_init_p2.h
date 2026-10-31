/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_INIT_P2_H
#define HOST_PWRCTRL_INIT_P2_H

#include "host_pwrctrl_init.h"

struct adapter_init_p2 {
	struct adapter_init core;
	u8 wow_power_mgnt;
	u8 wow_lps_level;
	u32 wakeup_event;
	u8 wowlan_power_mgmt;
	u8 wowlan_lps_level;
	u8 default_patterns_en;
	u8 wowlan_aoac_rpt_loc;
	signed char lps_ack_status;
};

typedef struct adapter_init_p2 adapter_p2;

struct host_pwrctrl_init_p2_trace {
	u32 wow_pattern_reset;
	u32 destroy_wq;
	u32 cancel_workitem;
	u32 free_pwrlock;
};

void host_pwrctrl_init_p2_reset_trace(void);
struct host_pwrctrl_init_p2_trace *host_pwrctrl_init_p2_get_trace(void);

void rtw_init_pwrctrl_priv_part2(adapter_p2 *padapter);
void rtw_free_pwrctrl_priv_p2(adapter_p2 *adapter);

#endif /* HOST_PWRCTRL_INIT_P2_H */
