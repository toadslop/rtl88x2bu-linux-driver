// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-144 BT WLAN port H2C. */

#include <stdio.h>
#include <string.h>
#include "host_hal_com_btc_port_types.h"

struct _adapter;

void host_btc_port_reset(void);
void host_btc_port_set_hw_port(u8 port);
void host_btc_port_set_fill_ret(s32 v);
int host_btc_port_h2c_count(void);
u8 host_btc_port_h2c_cmd(int idx);
unsigned int host_btc_port_h2c_len(int idx);
const u8 *host_btc_port_h2c_buf(int idx);
struct _adapter *host_btc_port_adapter(void);

#ifdef RUST_HAL_COM_BTC_PORT_ORACLE
s32 rtw_hal_set_wifi_btc_port_id_cmd(struct _adapter *adapter);
#else
#define rtw_hal_set_wifi_btc_port_id_cmd rtw_hal_set_wifi_btc_port_id_cmd_c_oracle
#endif

s32 rtw_hal_set_wifi_btc_port_id_cmd_c_oracle(struct _adapter *adapter);

static int failures;

static void expect_u32(const char *tag, const char *name, unsigned got, unsigned expect)
{
	if (got != expect)
		fprintf(stderr, "FAIL %s %s got=%u expect=%u\n", tag, name, got, expect),
			failures++;
}

static void run_case(u8 hw_port, s32 fill_ret, const char *tag)
{
	s32 dut_rc, oracle_rc;
	int dut_cnt, oracle_cnt;
	u8 dut_cmd, oracle_cmd;
	unsigned dut_len, oracle_len;
	const u8 *dut_buf, *oracle_buf;

	host_btc_port_reset();
	host_btc_port_set_hw_port(hw_port);
	host_btc_port_set_fill_ret(fill_ret);
	dut_rc = rtw_hal_set_wifi_btc_port_id_cmd(host_btc_port_adapter());
	dut_cnt = host_btc_port_h2c_count();
	if (dut_cnt > 0) {
		dut_cmd = host_btc_port_h2c_cmd(0);
		dut_len = host_btc_port_h2c_len(0);
		dut_buf = host_btc_port_h2c_buf(0);
	} else {
		dut_cmd = 0;
		dut_len = 0;
		dut_buf = NULL;
	}

	host_btc_port_reset();
	host_btc_port_set_hw_port(hw_port);
	host_btc_port_set_fill_ret(fill_ret);
	oracle_rc = rtw_hal_set_wifi_btc_port_id_cmd_c_oracle(host_btc_port_adapter());
	oracle_cnt = host_btc_port_h2c_count();
	if (oracle_cnt > 0) {
		oracle_cmd = host_btc_port_h2c_cmd(0);
		oracle_len = host_btc_port_h2c_len(0);
		oracle_buf = host_btc_port_h2c_buf(0);
	} else {
		oracle_cmd = 0;
		oracle_len = 0;
		oracle_buf = NULL;
	}

	expect_u32(tag, "rc", (unsigned)dut_rc, (unsigned)oracle_rc);
	expect_u32(tag, "h2c_count", (unsigned)dut_cnt, (unsigned)oracle_cnt);
	if (dut_cnt > 0) {
		expect_u32(tag, "h2c_cmd", dut_cmd, oracle_cmd);
		expect_u32(tag, "h2c_cmd", dut_cmd, H2C_BTC_WL_PORT_ID);
		expect_u32(tag, "h2c_len", dut_len, oracle_len);
		expect_u32(tag, "h2c_len", dut_len, H2C_BTC_WL_PORT_ID_LEN);
		if (dut_buf && oracle_buf) {
			expect_u32(tag, "payload_low4", dut_buf[0], hw_port & 0x0f);
			expect_u32(tag, "payload_match", dut_buf[0], oracle_buf[0]);
		}
	}
}

int main(void)
{
	run_case(0, _SUCCESS, "port0");
	run_case(3, _SUCCESS, "port3");
	run_case(4, _SUCCESS, "port4");
	run_case(10, _SUCCESS, "port10");
	run_case(0x1a, _SUCCESS, "port_hi_nibble");
	run_case(7, -1, "fill_fail");

	if (failures) {
		fprintf(stderr, "hal_com_btc_port L2 failed (%d)\n", failures);
		return 1;
	}
	puts("OK hal_com_btc_port L2");
	return 0;
}