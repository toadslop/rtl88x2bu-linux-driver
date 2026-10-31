/* SPDX-License-Identifier: GPL-2.0 */
/* Host C oracle for W3-190 — init part 2 + rtw_free_pwrctrl_priv. */

#include "host_pwrctrl_init_p2.h"

#include <string.h>

static struct host_pwrctrl_init_p2_trace g_p2_trace;

void host_pwrctrl_init_p2_reset_trace(void)
{
	memset(&g_p2_trace, 0, sizeof(g_p2_trace));
}

struct host_pwrctrl_init_p2_trace *host_pwrctrl_init_p2_get_trace(void)
{
	return &g_p2_trace;
}

#ifdef CONFIG_WOWLAN
static void rtw_wow_pattern_sw_reset(PADAPTER padapter)
{
	(void)padapter;
	g_p2_trace.wow_pattern_reset++;
}
#endif

void rtw_init_pwrctrl_priv_part2(adapter_p2 *padapter)
{
#ifdef CONFIG_WOWLAN
	padapter->wowlan_power_mgmt = padapter->wow_power_mgnt;
	padapter->wowlan_lps_level = padapter->wow_lps_level;
	padapter->default_patterns_en =
		(padapter->wakeup_event & (1U << 1)) ? _TRUE : _FALSE;
	rtw_wow_pattern_sw_reset(&padapter->core);
	padapter->wowlan_aoac_rpt_loc = 0;
#endif

#ifdef CONFIG_LPS_ACK
	padapter->lps_ack_status = -1;
#endif
}

void rtw_free_pwrctrl_priv_p2(adapter_p2 *adapter)
{
	struct pwrctrl_priv_init *pwr = adapter_to_pwrctl(&adapter->core);

#if defined(CONFIG_CONCURRENT_MODE)
	if (!is_primary_adapter(&adapter->core))
		return;
#endif

#ifdef CONFIG_RESUME_IN_WORKQUEUE
	if (pwr->rtw_workqueue) {
		g_p2_trace.destroy_wq++;
		pwr->rtw_workqueue = NULL;
	}
#endif

#ifdef CONFIG_LPS_LCLK
	g_p2_trace.cancel_workitem += 2;
#endif

	g_p2_trace.free_pwrlock += 2;
}
