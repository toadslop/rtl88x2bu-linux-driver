// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-134 MSR network type get/set. */

#include <stdio.h>
#include "host_hal_com_msr_types.h"

#ifdef RUST_HAL_COM_MSR_ORACLE
void rtw_hal_get_msr(struct _adapter *adapter, u8 *net_type);
void rtw_hal_set_msr(struct _adapter *adapter, u8 net_type);
#else
#define rtw_hal_get_msr rtw_hal_get_msr_c_oracle
#define rtw_hal_set_msr rtw_hal_set_msr_c_oracle
#endif

static int failures;

static void eq_u8(const char *name, u8 a, u8 b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s got=0x%02x expect=0x%02x\n", name, a, b);
		failures++;
	}
}

static void run_get(u8 hw_port, u8 stored)
{
	u8 dut, oracle;
	u8 dut_port, oracle_port;

	host_msr_reset_trace();
	host_msr_set_hw_port(hw_port);
	rtw_hal_set_msr_c_oracle(host_msr_adapter(), stored);
	host_msr_clear_calls();
	rtw_hal_get_msr(host_msr_adapter(), &dut);
	dut_port = host_msr_last_get_port();
	host_msr_clear_calls();
	rtw_hal_get_msr_c_oracle(host_msr_adapter(), &oracle);
	oracle_port = host_msr_last_get_port();
	eq_u8("get_type", dut, stored);
	eq_u8("get_type_oracle", dut, oracle);
	eq_u8("get_hw_port", dut_port, hw_port);
	eq_u8("get_hw_port_oracle", oracle_port, hw_port);
}

static void run_get_after_promoted_set(u8 hw_port, u8 mbid, u8 in_type, u8 expect_type)
{
	u8 dut, oracle;
	u8 dut_port, oracle_port;

	host_msr_reset_trace();
	host_msr_set_hw_port(hw_port);
	host_msr_set_mbid_entry_num(mbid);
	host_msr_clear_calls();
	rtw_hal_set_msr_c_oracle(host_msr_adapter(), in_type);
	host_msr_clear_calls();
	rtw_hal_get_msr(host_msr_adapter(), &dut);
	dut_port = host_msr_last_get_port();
	host_msr_clear_calls();
	rtw_hal_get_msr_c_oracle(host_msr_adapter(), &oracle);
	oracle_port = host_msr_last_get_port();
	eq_u8("get_after_set_type", dut, expect_type);
	eq_u8("get_after_set_oracle", oracle, expect_type);
	eq_u8("get_after_set_hw_port", dut_port, hw_port);
	eq_u8("get_after_set_hw_port_oracle", oracle_port, hw_port);
}

static void run_set(u8 hw_port, u8 mbid, u8 in_type, u8 expect)
{
	host_msr_reset_trace();
	host_msr_set_hw_port(hw_port);
	host_msr_set_mbid_entry_num(mbid);
	host_msr_clear_calls();
	rtw_hal_set_msr(host_msr_adapter(), in_type);
	eq_u8("set_hw_port", host_msr_last_set_port(), hw_port);
	eq_u8("set_net_type", host_msr_last_set_type(), expect);
	host_msr_clear_calls();
	rtw_hal_set_msr_c_oracle(host_msr_adapter(), in_type);
	eq_u8("oracle_set_net_type", host_msr_last_set_type(), expect);
}

int main(void)
{
	u8 rd;

	run_get(HW_PORT0, _HW_STATE_STATION_);
	run_get(HW_PORT1, _HW_STATE_AP_);
	run_get_after_promoted_set(HW_PORT0, 2, _HW_STATE_STATION_, _HW_STATE_AP_);
	run_get_after_promoted_set(HW_PORT1, 2, _HW_STATE_STATION_, _HW_STATE_STATION_);
	run_set(HW_PORT0, 0, _HW_STATE_STATION_, _HW_STATE_STATION_);
	run_set(HW_PORT1, 0, _HW_STATE_AP_, _HW_STATE_AP_);
	run_set(HW_PORT1, 2, _HW_STATE_STATION_, _HW_STATE_STATION_);
	run_set(HW_PORT0, 2, _HW_STATE_STATION_, _HW_STATE_AP_);
	run_set(HW_PORT0, 2, _HW_STATE_NOLINK_, _HW_STATE_NOLINK_);
	host_msr_reset_trace();
	host_msr_set_hw_port(HW_PORT0);
	rtw_hal_set_msr(host_msr_adapter(), _HW_STATE_STATION_);
	rtw_hal_get_msr(host_msr_adapter(), &rd);
	eq_u8("roundtrip", rd, _HW_STATE_STATION_);
	return failures ? 1 : (puts("OK hal_com_msr L2"), 0);
}