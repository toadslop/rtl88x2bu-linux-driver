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

static int fail_line;

static void expect_u8(const char *name, unsigned int got, unsigned int expect)
{
	if (got != expect) {
		fprintf(stderr, "FAIL %s got=%u expect=%u\n", name, got, expect);
		fail_line = __LINE__;
	}
}

struct fw_rsvdpage_h2c_snap {
	int count;
	u8 cmd;
	unsigned int len;
	u8 buf[H2C_RSVDPAGE_LOC_LEN];
};

static void snap_dut(RSVDPAGE_LOC *loc, struct fw_rsvdpage_h2c_snap *s)
{
	const u8 *buf;

	host_fw_rsvdpage_h2c_reset();
	rtw_hal_set_FwRsvdPage_cmd(host_fw_rsvdpage_h2c_adapter(), loc);
	s->count = host_fw_rsvdpage_h2c_count();
	s->cmd = host_fw_rsvdpage_h2c_cmd(0);
	s->len = host_fw_rsvdpage_h2c_len(0);
	buf = host_fw_rsvdpage_h2c_buf(0);
	memset(s->buf, 0, sizeof(s->buf));
	if (buf)
		memcpy(s->buf, buf, H2C_RSVDPAGE_LOC_LEN);
}

static void snap_oracle(RSVDPAGE_LOC *loc, struct fw_rsvdpage_h2c_snap *s)
{
	const u8 *buf;

	host_fw_rsvdpage_h2c_reset();
	rtw_hal_set_FwRsvdPage_cmd_c_oracle(host_fw_rsvdpage_h2c_adapter(), loc);
	s->count = host_fw_rsvdpage_h2c_count();
	s->cmd = host_fw_rsvdpage_h2c_cmd(0);
	s->len = host_fw_rsvdpage_h2c_len(0);
	buf = host_fw_rsvdpage_h2c_buf(0);
	memset(s->buf, 0, sizeof(s->buf));
	if (buf)
		memcpy(s->buf, buf, H2C_RSVDPAGE_LOC_LEN);
}

static void assert_loc_in_payload(const struct fw_rsvdpage_h2c_snap *s,
				  const RSVDPAGE_LOC *loc, const char *tag)
{
	char label[80];

	snprintf(label, sizeof(label), "%s_probe", tag);
	expect_u8(label, s->buf[0], loc->LocProbeRsp);
	snprintf(label, sizeof(label), "%s_pspoll", tag);
	expect_u8(label, s->buf[1], loc->LocPsPoll);
	snprintf(label, sizeof(label), "%s_null", tag);
	expect_u8(label, s->buf[2], loc->LocNullData);
	snprintf(label, sizeof(label), "%s_qosnull", tag);
	expect_u8(label, s->buf[3], loc->LocQosNull);
	snprintf(label, sizeof(label), "%s_btnull", tag);
	expect_u8(label, s->buf[4], loc->LocBTQosNull);
}

static void run_case(RSVDPAGE_LOC *loc, const char *tag)
{
	struct fw_rsvdpage_h2c_snap dut, oracle;
	char label[64];

	snap_dut(loc, &dut);

	snprintf(label, sizeof(label), "%s_h2c_count", tag);
	expect_u8(label, (unsigned int)dut.count, 1u);
	snprintf(label, sizeof(label), "%s_cmd", tag);
	expect_u8(label, dut.cmd, H2C_RSVD_PAGE);
	snprintf(label, sizeof(label), "%s_len", tag);
	expect_u8(label, dut.len, H2C_RSVDPAGE_LOC_LEN);
	assert_loc_in_payload(&dut, loc, tag);

	snap_oracle(loc, &oracle);

	snprintf(label, sizeof(label), "%s_oracle_count", tag);
	expect_u8(label, (unsigned int)dut.count, (unsigned int)oracle.count);
	snprintf(label, sizeof(label), "%s_oracle_cmd", tag);
	expect_u8(label, dut.cmd, oracle.cmd);
	snprintf(label, sizeof(label), "%s_oracle_len", tag);
	expect_u8(label, dut.len, oracle.len);
	if (memcmp(dut.buf, oracle.buf, H2C_RSVDPAGE_LOC_LEN) != 0) {
		fprintf(stderr, "FAIL %s payload mismatch vs oracle\n", tag);
		fail_line = __LINE__;
	}
}

int main(void)
{
	RSVDPAGE_LOC loc;

	fail_line = 0;
	memset(&loc, 0, sizeof(loc));
	run_case(&loc, "zero");
	loc = (RSVDPAGE_LOC){1, 2, 3, 4, 5};
	run_case(&loc, "seeded");
	if (fail_line) {
		fprintf(stderr, "hal_com_fw_rsvdpage_h2c L2 failed\n");
		return 1;
	}
	printf("OK hal_com_fw_rsvdpage_h2c L2\n");
	return 0;
}