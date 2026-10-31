// SPDX-License-Identifier: GPL-2.0
/* Host L2 smoke for W3-190 init part 2 + free (vectors in init_p2_vectors.json). */

#include <stdio.h>
#include <string.h>

#include "host_pwrctrl_init.h"

static _adapter g_adapter;

static int check_wowlan_defaults(void)
{
	struct pwrctrl_priv_init *p;
	struct host_pwrctrl_init_trace *tr;

	host_pwrctrl_init_reset_trace();
	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.registrypriv.wow_power_mgnt = 2;
	g_adapter.registrypriv.wow_lps_level = 1;
	g_adapter.registrypriv.wakeup_event = 2U;
	rtw_init_pwrctrl_priv_part1(&g_adapter);
	rtw_init_pwrctrl_priv_part2(&g_adapter);
	p = adapter_to_pwrctl(&g_adapter);
	tr = host_pwrctrl_init_get_trace();
	if (p->wowlan_power_mgmt != 2 || p->wowlan_lps_level != 1 ||
	    p->default_patterns_en != _TRUE || tr->wow_pattern_reset != 1 ||
	    p->lps_ack_status != -1) {
		fputs("FAIL wowlan_defaults\n", stderr);
		return 1;
	}
	puts("PASS wowlan_defaults");
	return 0;
}

static int check_init_free(void)
{
	struct host_pwrctrl_init_trace *tr;

	host_pwrctrl_init_reset_trace();
	memset(&g_adapter, 0, sizeof(g_adapter));
	rtw_init_pwrctrl_priv_part1(&g_adapter);
	rtw_init_pwrctrl_priv_part2(&g_adapter);
	rtw_free_pwrctrl_priv(&g_adapter);
	tr = host_pwrctrl_init_get_trace();
	if (tr->destroy_wq != 1 || tr->cancel_workitem != 2 || tr->free_pwrlock != 2) {
		fputs("FAIL init_free_roundtrip\n", stderr);
		return 1;
	}
	puts("PASS init_free_roundtrip");
	return 0;
}

int main(void)
{
	int bad = check_wowlan_defaults() + check_init_free();

	if (!bad)
		puts("ALL PASS (2 vectors)");
	return bad ? 1 : 0;
}
