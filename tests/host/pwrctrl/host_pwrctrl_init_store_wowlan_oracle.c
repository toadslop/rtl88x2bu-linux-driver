/* SPDX-License-Identifier: GPL-2.0 */
/* Host C oracle for W3-200 — rtw_rust_init_store_u8 wowlan field cases. */

#include "host_pwrctrl_init.h"

#include <string.h>

static _adapter g_adapter;

PADAPTER init_store_wowlan_test_adapter(void)
{
	memset(&g_adapter, 0, sizeof(g_adapter));
	return &g_adapter;
}

struct pwrctrl_priv_init *init_store_wowlan_test_pwr(PADAPTER padapter)
{
	return adapter_to_pwrctl(padapter);
}

void init_store_wowlan_oracle_store_u8(PADAPTER padapter, u32 field, u8 val)
{
	struct pwrctrl_priv_init *p = adapter_to_pwrctl(padapter);

	switch (field) {
	case 18:
		p->wowlan_mode = val;
		break;
	case 19:
		p->wowlan_ap_mode = val;
		break;
	case 20:
		p->wowlan_p2p_mode = val;
		break;
	case 21:
		p->wowlan_in_resume = val;
		break;
	case 22:
		p->wowlan_last_wake_reason = val;
		break;
#ifdef CONFIG_WOWLAN
	case 23:
		p->wowlan_power_mgmt = val;
		break;
	case 24:
		p->wowlan_lps_level = val;
		break;
	case 25:
		p->default_patterns_en = val;
		break;
	case 26:
		p->wowlan_aoac_rpt_loc = val;
		break;
#endif
	default:
		break;
	}
}
