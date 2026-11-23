// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-131 default port H2C. */

#include <stdio.h>
#include <string.h>
#include "host_hal_com_dft_port_types.h"

struct _adapter;

void host_dft_port_reset(void);
void host_dft_port_set_fill_ret(s32 v);
void host_dft_port_set_hal_port(u8 v);
void host_dft_port_set_cached(u8 port_id, u8 mac_id);
void host_dft_port_set_client_assoc(u8 v, u8 mac_id);
void host_dft_port_set_assoc_sta_found(u8 v);
void host_dft_port_set_ap_state(u8 v);
int host_dft_port_h2c_count(void);
u8 host_dft_port_h2c_cmd(int idx);
unsigned int host_dft_port_h2c_len(int idx);
const u8 *host_dft_port_h2c_buf(int idx);
u8 host_dft_port_dft_port(void);
u8 host_dft_port_dft_mac(void);
struct _adapter *host_dft_port_adapter(void);

#ifdef RUST_HAL_COM_DFT_PORT_ORACLE
s32 rtw_hal_set_default_port_id_cmd(struct _adapter *adapter, u8 mac_id);
s32 rtw_set_default_port_id(struct _adapter *adapter);
#else
#define rtw_hal_set_default_port_id_cmd rtw_hal_set_default_port_id_cmd_c_oracle
#define rtw_set_default_port_id rtw_set_default_port_id_c_oracle
#endif

s32 rtw_hal_set_default_port_id_cmd_c_oracle(struct _adapter *adapter, u8 mac_id);
s32 rtw_set_default_port_id_c_oracle(struct _adapter *adapter);

static int fail_line;

static void expect_u32(const char *name, unsigned int got, unsigned int expect)
{
	if (got != expect) {
		fprintf(stderr, "FAIL %s got=%u expect=%u\n", name, got, expect);
		fail_line = __LINE__;
	}
}

static void run_cmd_case(u8 hal_port, u8 cached_port, u8 cached_mac, u8 mac_id, s32 fill_ret,
			 const char *tag)
{
	s32 dut_rc, oracle_rc;
	int dut_cnt, oracle_cnt;
	u8 dut_buf[2], oracle_buf[2];
	u8 dut_h2c_cmd, dut_h2c_len;
	u8 oracle_h2c_cmd, oracle_h2c_len;
	u8 dut_dft_port, dut_dft_mac, oracle_dft_port, oracle_dft_mac;

	host_dft_port_reset();
	host_dft_port_set_hal_port(hal_port);
	host_dft_port_set_cached(cached_port, cached_mac);
	host_dft_port_set_fill_ret(fill_ret);

	dut_rc = rtw_hal_set_default_port_id_cmd(host_dft_port_adapter(), mac_id);
	dut_cnt = host_dft_port_h2c_count();
	if (dut_cnt > 0) {
		dut_h2c_cmd = host_dft_port_h2c_cmd(0);
		dut_h2c_len = (u8)host_dft_port_h2c_len(0);
		memcpy(dut_buf, host_dft_port_h2c_buf(0), 2);
	} else {
		dut_h2c_cmd = 0;
		dut_h2c_len = 0;
		memset(dut_buf, 0, sizeof(dut_buf));
	}
	dut_dft_port = host_dft_port_dft_port();
	dut_dft_mac = host_dft_port_dft_mac();

	host_dft_port_reset();
	host_dft_port_set_hal_port(hal_port);
	host_dft_port_set_cached(cached_port, cached_mac);
	host_dft_port_set_fill_ret(fill_ret);

	oracle_rc = rtw_hal_set_default_port_id_cmd_c_oracle(host_dft_port_adapter(), mac_id);
	oracle_cnt = host_dft_port_h2c_count();
	if (oracle_cnt > 0) {
		oracle_h2c_cmd = host_dft_port_h2c_cmd(0);
		oracle_h2c_len = (u8)host_dft_port_h2c_len(0);
		memcpy(oracle_buf, host_dft_port_h2c_buf(0), 2);
	} else {
		oracle_h2c_cmd = 0;
		oracle_h2c_len = 0;
		memset(oracle_buf, 0, sizeof(oracle_buf));
	}
	oracle_dft_port = host_dft_port_dft_port();
	oracle_dft_mac = host_dft_port_dft_mac();

	(void)tag;
	expect_u32("cmd_rc", (unsigned int)dut_rc, (unsigned int)oracle_rc);
	expect_u32("cmd_h2c_count", (unsigned int)dut_cnt, (unsigned int)oracle_cnt);
	if (dut_cnt > 0) {
		expect_u32("cmd_h2c_cmd", dut_h2c_cmd, oracle_h2c_cmd);
		expect_u32("cmd_h2c_len", dut_h2c_len, oracle_h2c_len);
		expect_u32("cmd_h2c_cmd_id", dut_h2c_cmd, H2C_DEFAULT_PORT_ID);
		expect_u32("cmd_h2c_len_val", dut_h2c_len, H2C_DEFAULT_PORT_ID_LEN);
		expect_u32("cmd_buf0", dut_buf[0], oracle_buf[0]);
		expect_u32("cmd_buf1", dut_buf[1], oracle_buf[1]);
	}
	expect_u32("cmd_dft_port", dut_dft_port, oracle_dft_port);
	expect_u32("cmd_dft_mac", dut_dft_mac, oracle_dft_mac);
}

static void run_set_case(u8 client_assoc, u8 assoc_sta_found, u8 ap_state, u8 assoc_mac,
			 u8 hal_port, const char *tag)
{
	s32 dut_rc, oracle_rc;
	int dut_cnt, oracle_cnt;
	u8 dut_buf[2], oracle_buf[2];
	u8 dut_h2c_cmd, dut_h2c_len;
	u8 oracle_h2c_cmd, oracle_h2c_len;
	u8 dut_dft_port, dut_dft_mac, oracle_dft_port, oracle_dft_mac;

	host_dft_port_reset();
	host_dft_port_set_hal_port(hal_port);
	host_dft_port_set_client_assoc(client_assoc, assoc_mac);
	host_dft_port_set_assoc_sta_found(assoc_sta_found);
	host_dft_port_set_ap_state(ap_state);

	dut_rc = rtw_set_default_port_id(host_dft_port_adapter());
	dut_cnt = host_dft_port_h2c_count();
	if (dut_cnt > 0) {
		dut_h2c_cmd = host_dft_port_h2c_cmd(0);
		dut_h2c_len = (u8)host_dft_port_h2c_len(0);
		memcpy(dut_buf, host_dft_port_h2c_buf(0), 2);
	} else {
		dut_h2c_cmd = 0;
		dut_h2c_len = 0;
		memset(dut_buf, 0, sizeof(dut_buf));
	}
	dut_dft_port = host_dft_port_dft_port();
	dut_dft_mac = host_dft_port_dft_mac();

	host_dft_port_reset();
	host_dft_port_set_hal_port(hal_port);
	host_dft_port_set_client_assoc(client_assoc, assoc_mac);
	host_dft_port_set_assoc_sta_found(assoc_sta_found);
	host_dft_port_set_ap_state(ap_state);

	oracle_rc = rtw_set_default_port_id_c_oracle(host_dft_port_adapter());
	oracle_cnt = host_dft_port_h2c_count();
	if (oracle_cnt > 0) {
		oracle_h2c_cmd = host_dft_port_h2c_cmd(0);
		oracle_h2c_len = (u8)host_dft_port_h2c_len(0);
		memcpy(oracle_buf, host_dft_port_h2c_buf(0), 2);
	} else {
		oracle_h2c_cmd = 0;
		oracle_h2c_len = 0;
		memset(oracle_buf, 0, sizeof(oracle_buf));
	}
	oracle_dft_port = host_dft_port_dft_port();
	oracle_dft_mac = host_dft_port_dft_mac();

	(void)tag;
	expect_u32("set_rc", (unsigned int)dut_rc, (unsigned int)oracle_rc);
	expect_u32("set_h2c_count", (unsigned int)dut_cnt, (unsigned int)oracle_cnt);
	if (dut_cnt > 0) {
		expect_u32("set_h2c_cmd", dut_h2c_cmd, oracle_h2c_cmd);
		expect_u32("set_h2c_len", dut_h2c_len, oracle_h2c_len);
		expect_u32("set_h2c_cmd_id", dut_h2c_cmd, H2C_DEFAULT_PORT_ID);
		expect_u32("set_h2c_len_val", dut_h2c_len, H2C_DEFAULT_PORT_ID_LEN);
		expect_u32("set_buf0", dut_buf[0], oracle_buf[0]);
		expect_u32("set_buf1", dut_buf[1], oracle_buf[1]);
	}
	expect_u32("set_dft_port", dut_dft_port, oracle_dft_port);
	expect_u32("set_dft_mac", dut_dft_mac, oracle_dft_mac);
}

int main(void)
{
	fail_line = 0;
	host_dft_port_reset();

	run_cmd_case(2, 2, 5, 5, _SUCCESS, "early_out");
	run_cmd_case(2, 2, 5, 7, _SUCCESS, "mac_only_mismatch");
	run_cmd_case(3, 1, 4, 7, _SUCCESS, "pack_ok");
	run_cmd_case(1, 0, 0, 9, 0, "h2c_fail_still_cache");

	run_set_case(1, 1, 0, 11, 2, "sta_assoc");
	run_set_case(1, 0, 0, 11, 2, "assoc_no_sta");
	run_set_case(0, 0, 0, 0, 0, "idle");
	run_set_case(0, 0, 1, 0, 0, "ap_empty");

	if (fail_line)
		return fprintf(stderr, "tests failed at line %d\n", fail_line), 1;
	puts("PASS");
	return 0;
}