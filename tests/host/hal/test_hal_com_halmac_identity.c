// SPDX-License-Identifier: GPL-2.0
/* L2 tests for W4-135 rtw_hal_set_bssid HALMAC shim (PR1). */

#include <stdio.h>
#include <string.h>
#include "host_hal_com_halmac_identity_types.h"

#ifdef RUST_HAL_COM_HALMAC_IDENTITY_ORACLE
void rtw_hal_set_bssid(struct _adapter *adapter, u8 *val);
#else
#define rtw_hal_set_bssid rtw_hal_set_bssid_c_oracle
#endif

static int failures;

static void eq_u8(const char *name, u8 a, u8 b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s got=0x%02x expect=0x%02x\n", name, a, b);
		failures++;
	}
}

static void run_set_bssid(u8 hw, u8 clt, u8 expect_port)
{
	u8 bssid[ETH_ALEN] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

	host_halmac_identity_reset();
	host_halmac_identity_set_hw_port(hw);
	host_halmac_identity_set_clt_port(clt);
	host_halmac_identity_clear_trace();
	rtw_hal_set_bssid(host_halmac_identity_adapter(), bssid);
	eq_u8("set_bssid_port", host_halmac_identity_trace.set_bssid_port, expect_port);
	eq_u8("set_bssid_called", host_halmac_identity_trace.set_bssid_calls ? 1 : 0, 1);
	eq_u8("set_bssid_addr_null", host_halmac_identity_trace.set_bssid_addr_null, 0);
	if (memcmp(host_halmac_identity_trace.set_bssid_addr, bssid, ETH_ALEN) != 0)
		fprintf(stderr, "FAIL set_bssid_addr mismatch\n"), failures++;
}

int main(void)
{
	run_set_bssid(HW_PORT0, 0, HW_PORT0);
	run_set_bssid(HW_PORT1, 0, HW_PORT1);
	run_set_bssid(HW_PORT0, HW_PORT1, HW_PORT1);
	return failures ? 1 : (puts("OK hal_com_halmac_identity L2"), 0);
}