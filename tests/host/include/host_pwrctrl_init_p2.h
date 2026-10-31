/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_INIT_P2_H
#define HOST_PWRCTRL_INIT_P2_H

#include "host_pwrctrl_init.h"

#define HOST_HW_VAR_LPS_POFF_INIT 201
#define HOST_HW_VAR_LPS_POFF_DEINIT 202

struct host_pwrctrl_init_p2_trace {
	u32 wow_pattern_reset;
	u32 flush_wq;
	u32 destroy_wq;
	u32 lps_poff_deinit;
	u32 cancel_workitem;
	u32 rsvd_page_free;
	u32 free_pwrlock;
	u32 lps_ack_mutex_free;
	u32 lps_ack_mutex_init;
	u32 lps_poff_init;
};

void host_pwrctrl_init_p2_reset_trace(void);
struct host_pwrctrl_init_p2_trace *host_pwrctrl_init_p2_get_trace(void);

void host_pwrctrl_init_p2_run_init(PADAPTER padapter);
void host_pwrctrl_init_p2_run_free(PADAPTER padapter);

#endif /* HOST_PWRCTRL_INIT_P2_H */
