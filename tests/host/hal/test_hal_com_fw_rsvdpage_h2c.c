// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-136 rtw_hal_set_FwRsvdPage_cmd. */

#include <stdio.h>
#include <string.h>
#include "host_hal_com_fw_rsvdpage_h2c_types.h"

struct _adapter;

void host_fw_rsvdpage_h2c_reset(void);
int host_fw_rsvdpage_h2c_count(void);
u8 host_fw_rsvdpage_h2c_cmd(int idx);
unsigned int host_fw_rsvdpage_h2c_len(int idx);
const u8 *host_fw_rsvdpage_h2c_buf(int idx);
struct _adapter *host_fw_rsvdpage_h2c_adapter(void);

#ifdef RUST_HAL_COM_FW_RSVDPAGE_H2C_ORACLE
void rtw_hal_set_FwRsvdPage_cmd(struct _adapter *adapter, RSVDPAGE_LOC *loc);
#else
#define rtw_hal_set_FwRsvdPage_cmd rtw_hal_set_FwRsvdPage_cmd_c_oracle
#endif

void rtw_hal_set_FwRsvdPage_cmd_c_oracle(struct _adapter *a, RSVDPAGE_LOC *loc);

static int fail;

static void run_case(RSVDPAGE_LOC *loc)
{
	u8 dut[H2C_RSVDPAGE_LOC_LEN], oracle[H2C_RSVDPAGE_LOC_LEN];
	int dut_n, oracle_n;

	host_fw_rsvdpage_h2c_reset();
	rtw_hal_set_FwRsvdPage_cmd(host_fw_rsvdpage_h2c_adapter(), loc);
	dut_n = host_fw_rsvdpage_h2c_count();
	memcpy(dut, host_fw_rsvdpage_h2c_buf(0), H2C_RSVDPAGE_LOC_LEN);

	host_fw_rsvdpage_h2c_reset();
	rtw_hal_set_FwRsvdPage_cmd_c_oracle(host_fw_rsvdpage_h2c_adapter(), loc);
	oracle_n = host_fw_rsvdpage_h2c_count();
	memcpy(oracle, host_fw_rsvdpage_h2c_buf(0), H2C_RSVDPAGE_LOC_LEN);

	if (dut_n != oracle_n || dut_n != 1 ||
	    host_fw_rsvdpage_h2c_cmd(0) != H2C_RSVD_PAGE ||
	    host_fw_rsvdpage_h2c_len(0) != H2C_RSVDPAGE_LOC_LEN ||
	    memcmp(dut, oracle, H2C_RSVDPAGE_LOC_LEN))
		fail = 1;
}

int main(void)
{
	RSVDPAGE_LOC loc;

	fail = 0;
	memset(&loc, 0, sizeof(loc));
	run_case(&loc);
	loc = (RSVDPAGE_LOC){1, 2, 3, 4, 5};
	run_case(&loc);
	if (fail) {
		fprintf(stderr, "hal_com_fw_rsvdpage_h2c L2 failed\n");
		return 1;
	}
	printf("OK hal_com_fw_rsvdpage_h2c L2\n");
	return 0;
}