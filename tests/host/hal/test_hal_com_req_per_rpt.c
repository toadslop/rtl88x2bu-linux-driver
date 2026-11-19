// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-118 rtw_hal_set_req_per_rpt_cmd. */

#include <stdio.h>
#include <string.h>
#include "host_hal_com_req_per_rpt_types.h"

struct _adapter;

void host_req_per_rpt_reset(void);
void host_req_per_rpt_set_fill_ret(unsigned char v);
int host_req_per_rpt_h2c_count(void);
unsigned char host_req_per_rpt_last_cmd(void);
unsigned int host_req_per_rpt_last_len(void);
const unsigned char *host_req_per_rpt_last_buf(void);
struct _adapter *host_req_per_rpt_adapter(void);

#ifdef RUST_HAL_COM_REQ_PER_RPT_ORACLE
unsigned char rtw_hal_set_req_per_rpt_cmd(struct _adapter *adapter, unsigned char group_macid,
					  unsigned char rpt_type, unsigned int macid_bitmap);
#else
#define rtw_hal_set_req_per_rpt_cmd rtw_hal_set_req_per_rpt_cmd_c_oracle
#endif

unsigned char rtw_hal_set_req_per_rpt_cmd_c_oracle(struct _adapter *adapter, unsigned char group_macid,
						   unsigned char rpt_type, unsigned int macid_bitmap);

static int fail_line;

static void expect_u8(const char *name, unsigned int got, unsigned int expect)
{
	if (got != expect) {
		fprintf(stderr, "FAIL %s got=%u expect=%u\n", name, got, expect);
		fail_line = __LINE__;
	}
}

struct req_per_rpt_h2c_snap {
	unsigned char rc;
	int count;
	unsigned char cmd;
	unsigned int len;
	unsigned char buf[8];
};

static void snap_dut(unsigned char group_macid, unsigned char rpt_type, unsigned int bmp,
		     unsigned char fill_ret, struct req_per_rpt_h2c_snap *s)
{
	const unsigned char *buf;

	host_req_per_rpt_reset();
	if (fill_ret != 1)
		host_req_per_rpt_set_fill_ret(fill_ret);
	s->rc = rtw_hal_set_req_per_rpt_cmd(host_req_per_rpt_adapter(), group_macid, rpt_type,
					    bmp);
	s->count = host_req_per_rpt_h2c_count();
	s->cmd = host_req_per_rpt_last_cmd();
	s->len = host_req_per_rpt_last_len();
	buf = host_req_per_rpt_last_buf();
	memset(s->buf, 0, sizeof(s->buf));
	if (buf)
		memcpy(s->buf, buf, 5);
}

static void snap_oracle(unsigned char group_macid, unsigned char rpt_type, unsigned int bmp,
			unsigned char fill_ret, struct req_per_rpt_h2c_snap *s)
{
	const unsigned char *buf;

	host_req_per_rpt_reset();
	if (fill_ret != 1)
		host_req_per_rpt_set_fill_ret(fill_ret);
	s->rc = rtw_hal_set_req_per_rpt_cmd_c_oracle(host_req_per_rpt_adapter(), group_macid,
						     rpt_type, bmp);
	s->count = host_req_per_rpt_h2c_count();
	s->cmd = host_req_per_rpt_last_cmd();
	s->len = host_req_per_rpt_last_len();
	buf = host_req_per_rpt_last_buf();
	memset(s->buf, 0, sizeof(s->buf));
	if (buf)
		memcpy(s->buf, buf, 5);
}

static void run_case(unsigned char group_macid, unsigned char rpt_type, unsigned int bmp,
		     const char *tag)
{
	struct req_per_rpt_h2c_snap dut, oracle;
	char label[64];

	snap_dut(group_macid, rpt_type, bmp, 1, &dut);
	snap_oracle(group_macid, rpt_type, bmp, 1, &oracle);

	snprintf(label, sizeof(label), "%s_rc", tag);
	expect_u8(label, dut.rc, oracle.rc);
	snprintf(label, sizeof(label), "%s_h2c_count", tag);
	expect_u8(label, (unsigned int)dut.count, (unsigned int)oracle.count);
	snprintf(label, sizeof(label), "%s_cmd", tag);
	expect_u8(label, dut.cmd, oracle.cmd);
	snprintf(label, sizeof(label), "%s_len", tag);
	expect_u8(label, dut.len, oracle.len);
	if (memcmp(dut.buf, oracle.buf, 5) != 0) {
		fprintf(stderr, "FAIL %s cmd_buf mismatch\n", tag);
		fail_line = __LINE__;
	}
}

int main(void)
{
	fail_line = 0;
	host_req_per_rpt_reset();

	run_case(0, 0, 0u, "zero");
	run_case(0x0f, 0x0f, 0xffffffffu, "max_nibbles");
	run_case(5, 2, 0x00ff00a5u, "mixed");

	{
		struct req_per_rpt_h2c_snap dut, oracle;

		snap_dut(1, 1, 2u, 0, &dut);
		snap_oracle(1, 1, 2u, 0, &oracle);
		expect_u8("fail_ret", dut.rc, oracle.rc);
		expect_u8("fail_h2c_count", (unsigned int)dut.count,
			  (unsigned int)oracle.count);
	}

	if (fail_line)
		return fprintf(stderr, "tests failed at line %d\n", fail_line), 1;
	puts("PASS");
	return 0;
}