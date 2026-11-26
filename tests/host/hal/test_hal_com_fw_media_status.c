// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-139 FW media status H2C. */

#include <stdio.h>
#include <string.h>
#include "host_hal_com_fw_media_status_types.h"

#ifdef RUST_HAL_COM_FW_MEDIA_STATUS_ORACLE
#define DUT_CMD rtw_hal_set_FwMediaStatusRpt_single_cmd
#else
#define DUT_CMD(d, op, mc, sink, role, macid)                               \
	do {                                                                \
		rtw_hal_set_FwMediaStatusRpt_single_cmd(d, op, mc, sink, role, \
							macid);             \
	} while (0)
#endif

static int fail_line;

static void expect_u32(const char *name, unsigned int got, unsigned int exp)
{
	if (got != exp) {
		fprintf(stderr, "FAIL %s got=%u expect=%u\n", name, got, exp);
		fail_line = __LINE__;
	}
}

static void expect_u8(const char *name, unsigned int got, unsigned int exp)
{
	expect_u32(name, got, exp);
}

static void expect_msr_byte(const char *tag, u8 got, u8 expect_opmode, u8 expect_role)
{
	u8 exp = (u8)((expect_opmode & 1) | ((expect_role & 0xf) << 4));

	if (got != exp) {
		fprintf(stderr, "FAIL %s_msr got=0x%02x expect=0x%02x\n", tag, got,
			exp);
		fail_line = __LINE__;
	}
	if (GET_H2CCMD_MSRRPT_PARM_MACID_IND(&got)) {
		fprintf(stderr, "FAIL %s_msr_ind MACID_IND still set\n", tag);
		fail_line = __LINE__;
	}
}

static void run_connect_sta(u8 macid)
{
	struct _adapter *a = host_fw_media_status_adapter();
	struct macid_ctl_t *ctl = &a->dvobj.macid_ctl;

	host_fw_media_status_reset();
	a->hw_port = 2;
	ctl->h2c_msr[macid] = 0;
	ctl->bw[macid] = 3;
	ctl->vht_en[macid] = 1;
	ctl->rate_bmp0[macid] = 0xaa;
	ctl->rate_bmp1[macid] = 0xbb;

	DUT_CMD(a, 1, 0, 0, H2C_MSR_ROLE_STA, macid);

	expect_u8("h2c_count", host_fw_media_status_h2c_count(), 1);
	expect_u8("h2c_cmd", host_fw_media_status_h2c_cmd(0), H2C_MEDIA_STATUS_RPT);
	expect_u8("h2c_len", host_fw_media_status_h2c_len(0),
		  H2C_MEDIA_STATUS_RPT_LEN);
	{
		const u8 *buf = host_fw_media_status_h2c_buf(0);

		expect_u8("opmode", GET_H2CCMD_MSRRPT_PARM_OPMODE(buf), 1);
		expect_u8("macid_ind_h2c", GET_H2CCMD_MSRRPT_PARM_MACID_IND(buf), 0);
		expect_u8("role", GET_H2CCMD_MSRRPT_PARM_ROLE(buf),
			  H2C_MSR_ROLE_STA);
		expect_u8("h2c_macid", GET_H2CCMD_MSRRPT_PARM_MACID(buf), macid);
		expect_u8("h2c_macid_end", GET_H2CCMD_MSRRPT_PARM_MACID_END(buf),
			  0);
	}
	expect_msr_byte("ctl", ctl->h2c_msr[macid], 1, H2C_MSR_ROLE_STA);
	expect_u8("op_num_sta", ctl->op_num[H2C_MSR_ROLE_STA], 1);
	expect_u8("tx_aclt", host_fw_media_status_trace.tx_aclt_calls, 1);
	expect_u8("rate_bmp", host_fw_media_status_trace.update_tx_rate_bmp_calls,
		  0);
}

static void run_disconnect_sta(u8 macid)
{
	struct _adapter *a = host_fw_media_status_adapter();
	struct macid_ctl_t *ctl = &a->dvobj.macid_ctl;
	u8 prev_msr = 0x11; /* opmode=1, role=STA */

	host_fw_media_status_reset();
	a->hw_port = 1;
	ctl->h2c_msr[macid] = prev_msr;
	ctl->op_num[H2C_MSR_ROLE_STA] = 1;
	ctl->bw[macid] = 3;
	ctl->vht_en[macid] = 1;
	ctl->rate_bmp0[macid] = 0x11;
	ctl->rate_bmp1[macid] = 0x22;

	DUT_CMD(a, 0, 0, 0, H2C_MSR_ROLE_STA, macid);

	expect_u8("disconnect_bw", ctl->bw[macid], CHANNEL_WIDTH_20);
	expect_u8("disconnect_vht", ctl->vht_en[macid], 0);
	expect_u32("disconnect_bmp0", ctl->rate_bmp0[macid], 0);
	expect_u32("disconnect_bmp1", ctl->rate_bmp1[macid], 0);
	expect_msr_byte("disc_ctl", ctl->h2c_msr[macid], 0, H2C_MSR_ROLE_STA);
	expect_u8("op_num_clear", ctl->op_num[H2C_MSR_ROLE_STA], 0);
	expect_u8("tx_aclt_disc", host_fw_media_status_trace.tx_aclt_calls, 1);
	expect_u8("rate_bmp_disc",
		  host_fw_media_status_trace.update_tx_rate_bmp_calls, 1);
}

static void run_h2c_fail_skips_sidefx(void)
{
	struct _adapter *a = host_fw_media_status_adapter();
	s32 rc;

	host_fw_media_status_reset();
	host_fw_media_status_set_fill_ret(_FAIL);
	rc = rtw_hal_set_FwMediaStatusRpt_single_cmd(a, 1, 0, 0, H2C_MSR_ROLE_STA, 2);
	expect_u8("fail_rc", (unsigned int)rc, _FAIL);
	expect_u8("fail_h2c", host_fw_media_status_h2c_count(), 1);
	expect_u8("fail_no_msr", a->dvobj.macid_ctl.h2c_msr[2], 0);
	expect_u8("fail_no_aclt", host_fw_media_status_trace.tx_aclt_calls, 0);
}

static void run_high_macid_connect(void)
{
	struct _adapter *a = host_fw_media_status_adapter();
	struct macid_ctl_t *ctl = &a->dvobj.macid_ctl;
	const u8 macid = 16;

	host_fw_media_status_reset();
	DUT_CMD(a, 1, 0, 0, H2C_MSR_ROLE_STA, macid);
	expect_msr_byte("hi_macid", ctl->h2c_msr[macid], 1, H2C_MSR_ROLE_STA);
	expect_u8("hi_op_num", ctl->op_num[H2C_MSR_ROLE_STA], 1);
}

int main(void)
{
	fail_line = 0;
	run_connect_sta(1);
	run_disconnect_sta(1);
	run_h2c_fail_skips_sidefx();
	run_high_macid_connect();
	if (fail_line) {
		fprintf(stderr, "hal_com_fw_media_status L2 failed\n");
		return 1;
	}
	printf("OK hal_com_fw_media_status L2\n");
	return 0;
}