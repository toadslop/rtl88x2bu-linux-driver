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

static void run_case(unsigned char group_macid, unsigned char rpt_type, unsigned int bmp,
		       const char *tag)
{
	unsigned char rc, rc_o;
	const unsigned char *buf, *buf_o;
	char label[64];

	host_req_per_rpt_reset();
	snprintf(label, sizeof(label), "%s_rc_c", tag);
	rc = rtw_hal_set_req_per_rpt_cmd(host_req_per_rpt_adapter(), group_macid, rpt_type, bmp);
	buf = host_req_per_rpt_last_buf();

	host_req_per_rpt_reset();
	snprintf(label, sizeof(label), "%s_rc_o", tag);
	rc_o = rtw_hal_set_req_per_rpt_cmd_c_oracle(host_req_per_rpt_adapter(), group_macid,
						    rpt_type, bmp);
	buf_o = host_req_per_rpt_last_buf();

	expect_u8(label, rc, rc_o);
	if (host_req_per_rpt_h2c_count() != 1) {
		fprintf(stderr, "FAIL %s h2c_count\n", tag);
		fail_line = __LINE__;
	}
	expect_u8(tag, host_req_per_rpt_last_cmd(), 0x4e);
	expect_u8(tag, host_req_per_rpt_last_len(), 5);
	if (!buf || !buf_o || memcmp(buf, buf_o, 5) != 0) {
		fprintf(stderr, "FAIL %s cmd_buf mismatch\n", tag);
		fail_line = __LINE__;
	}
	(void)buf;
	(void)buf_o;
}

int main(void)
{
	fail_line = 0;
	host_req_per_rpt_reset();

	run_case(0, 0, 0u, "zero");
	run_case(0x0f, 0x0f, 0xffffffffu, "max_nibbles");
	run_case(5, 2, 0x00ff00a5u, "mixed");

	host_req_per_rpt_reset();
	host_req_per_rpt_set_fill_ret(0);
	expect_u8("fail_ret",
		  rtw_hal_set_req_per_rpt_cmd(host_req_per_rpt_adapter(), 1, 1, 2u),
		  rtw_hal_set_req_per_rpt_cmd_c_oracle(host_req_per_rpt_adapter(), 1, 1, 2u));

	if (fail_line)
		return fprintf(stderr, "tests failed at line %d\n", fail_line), 1;
	puts("PASS");
	return 0;
}