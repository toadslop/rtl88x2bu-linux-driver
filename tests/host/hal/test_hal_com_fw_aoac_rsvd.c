// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-145 WOW AOAC reserved-page H2C. */

#include <stdio.h>
#include <string.h>
#include "host_hal_com_fw_aoac_rsvd_types.h"

void host_fw_aoac_rsvd_reset(void);
void host_fw_aoac_rsvd_set_asoc(int asoc);
void host_fw_aoac_rsvd_set_wowlan_in_resume(u8 v);
u8 host_fw_aoac_rsvd_wowlan_aoac_rpt_loc(void);
int host_fw_aoac_rsvd_h2c_count(void);
u8 host_fw_aoac_rsvd_h2c_cmd(int idx);
unsigned int host_fw_aoac_rsvd_h2c_len(int idx);
const u8 *host_fw_aoac_rsvd_h2c_buf(int idx);
struct _adapter *host_fw_aoac_rsvd_adapter(void);

#ifdef RUST_HAL_COM_FW_AOAC_RSVD_ORACLE
void rtw_hal_set_FwAoacRsvdPage_cmd(struct _adapter *a, PRSVDPAGE_LOC loc);
#else
#define rtw_hal_set_FwAoacRsvdPage_cmd rtw_hal_set_FwAoacRsvdPage_cmd_c_oracle
#endif

void rtw_hal_set_FwAoacRsvdPage_cmd_c_oracle(struct _adapter *a, PRSVDPAGE_LOC loc);

static int failures;

static void expect_u32(const char *tag, const char *name, unsigned got, unsigned expect)
{
	if (got != expect)
		fprintf(stderr, "FAIL %s %s got=%u expect=%u\n", tag, name, got, expect),
			failures++;
}

static void assert_assoc_h2c0(const char *tag, const RSVDPAGE_LOC *loc, u8 cmd,
			      const u8 *buf)
{
	unsigned i;
	u8 expect[7] = {0, 0, 0, 0, 0, 0, 0};

	expect_u32(tag, "h2c0_cmd", cmd, H2C_AOAC_RSVD_PAGE);
	expect[0] = loc->LocRemoteCtrlInfo;
	expect[1] = loc->LocArpRsp;
	expect[2] = loc->LocNbrAdv;
	expect[3] = loc->LocGTKRsp;
	expect[4] = loc->LocGTKInfo;
	expect[5] = loc->LocGTKEXTMEM;
	expect[6] = loc->LocNDPInfo;
	for (i = 0; i < H2C_AOAC_RSVDPAGE_LOC_LEN; i++)
		expect_u32(tag, "h2c0_pay", buf[i], expect[i]);
}

static void assert_assoc_h2c1(const char *tag, const RSVDPAGE_LOC *loc, u8 cmd,
			    const u8 *buf)
{
	unsigned i;

	expect_u32(tag, "h2c1_cmd", cmd, H2C_AOAC_RSVDPAGE3);
	for (i = 0; i < H2C_AOAC_RSVDPAGE_LOC_LEN; i++) {
		u8 expect = (i == 1) ? loc->LocAOACReport : 0;

		expect_u32(tag, "h2c1_pay", buf[i], expect);
	}
}

static void assert_pno_h2c0(const char *tag, const RSVDPAGE_LOC *loc, u8 cmd,
			    const u8 *buf)
{
	unsigned i;

	expect_u32(tag, "pno_cmd", cmd, H2C_AOAC_RSVDPAGE3);
	for (i = 0; i < H2C_AOAC_RSVDPAGE_LOC_LEN; i++) {
		u8 expect = (i == 0) ? loc->LocPNOInfo : 0;

		expect_u32(tag, "pno_pay", buf[i], expect);
	}
}

static void compare_run(const char *tag, int asoc, u8 in_resume, const RSVDPAGE_LOC *loc,
			unsigned expect_h2c, u8 expect_rpt)
{
	unsigned i, len;
	u8 dut_cmd[2], dut_buf[2][8];
	unsigned dut_cnt;

	host_fw_aoac_rsvd_reset();
	host_fw_aoac_rsvd_set_asoc(asoc);
	host_fw_aoac_rsvd_set_wowlan_in_resume(in_resume);
	rtw_hal_set_FwAoacRsvdPage_cmd(host_fw_aoac_rsvd_adapter(), (PRSVDPAGE_LOC)loc);
	dut_cnt = (unsigned)host_fw_aoac_rsvd_h2c_count();
	for (i = 0; i < dut_cnt; i++) {
		dut_cmd[i] = host_fw_aoac_rsvd_h2c_cmd((int)i);
		len = host_fw_aoac_rsvd_h2c_len((int)i);
		if (host_fw_aoac_rsvd_h2c_buf((int)i) && len <= sizeof(dut_buf[i]))
			memcpy(dut_buf[i], host_fw_aoac_rsvd_h2c_buf((int)i), len);
	}

	if (asoc) {
		expect_u32(tag, "h2c_count", dut_cnt, 2u);
		if (dut_cnt >= 1)
			assert_assoc_h2c0(tag, loc, dut_cmd[0], dut_buf[0]);
		if (dut_cnt >= 2)
			assert_assoc_h2c1(tag, loc, dut_cmd[1], dut_buf[1]);
	} else if (!in_resume) {
		expect_u32(tag, "h2c_count", dut_cnt, 1u);
		if (dut_cnt >= 1)
			assert_pno_h2c0(tag, loc, dut_cmd[0], dut_buf[0]);
	} else {
		expect_u32(tag, "h2c_count", dut_cnt, 0u);
	}

	host_fw_aoac_rsvd_reset();
	host_fw_aoac_rsvd_set_asoc(asoc);
	host_fw_aoac_rsvd_set_wowlan_in_resume(in_resume);
	rtw_hal_set_FwAoacRsvdPage_cmd_c_oracle(host_fw_aoac_rsvd_adapter(), (PRSVDPAGE_LOC)loc);

	expect_u32(tag, "h2c_count", dut_cnt, expect_h2c);
	expect_u32(tag, "h2c_count", dut_cnt,
		   (unsigned)host_fw_aoac_rsvd_h2c_count());
	for (i = 0; i < dut_cnt; i++) {
		unsigned j;
		const u8 *ob = host_fw_aoac_rsvd_h2c_buf((int)i);

		expect_u32(tag, "h2c_cmd", dut_cmd[i], host_fw_aoac_rsvd_h2c_cmd((int)i));
		for (j = 0; j < H2C_AOAC_RSVDPAGE_LOC_LEN && ob; j++)
			expect_u32(tag, "pay", dut_buf[i][j], ob[j]);
	}
	if (expect_rpt)
		expect_u32(tag, "aoac_rpt", host_fw_aoac_rsvd_wowlan_aoac_rpt_loc(), expect_rpt);
}

int main(void)
{
	RSVDPAGE_LOC loc;

	memset(&loc, 0, sizeof(loc));
	loc.LocRemoteCtrlInfo = 1;
	loc.LocArpRsp = 2;
	loc.LocNbrAdv = 3;
	loc.LocNDPInfo = 9;
	loc.LocGTKRsp = 4;
	loc.LocGTKInfo = 5;
	loc.LocGTKEXTMEM = 6;
	loc.LocAOACReport = 7;
	compare_run("assoc", 1, 0, &loc, 2, loc.LocAOACReport);

	memset(&loc, 0, sizeof(loc));
	loc.LocPNOInfo = 0x55;
	compare_run("pno", 0, 0, &loc, 1, 0);

	if (failures) {
		fprintf(stderr, "hal_com_fw_aoac_rsvd L2 failed (%d)\n", failures);
		return 1;
	}
	puts("OK hal_com_fw_aoac_rsvd L2");
	return 0;
}