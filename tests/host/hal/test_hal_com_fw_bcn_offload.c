// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_hal_com_fw_bcn_offload_types.h"

void host_fw_bcn_offload_reset(void);
int host_fw_bcn_offload_h2c_count(void);
u8 host_fw_bcn_offload_h2c_cmd(int idx);
unsigned int host_fw_bcn_offload_h2c_len(int idx);
const u8 *host_fw_bcn_offload_h2c_buf(int idx);
u8 host_fw_bcn_offload_get_fw_bcn(void);
u8 host_fw_bcn_offload_get_tbtt_map(void);
struct _adapter *host_fw_bcn_offload_adapter(void);

#ifdef RUST_HAL_COM_FW_BCN_OFFLOAD_ORACLE
void rtw_hal_set_fw_ap_bcn_offload_cmd(struct _adapter *, bool, u8);
#else
#define rtw_hal_set_fw_ap_bcn_offload_cmd rtw_hal_set_fw_ap_bcn_offload_cmd_c_oracle
#endif

void rtw_hal_set_fw_ap_bcn_offload_cmd_c_oracle(struct _adapter *, bool, u8);

static int fail;

static void expect_u8(const char *name, unsigned got, unsigned exp)
{
	if (got != exp) {
		fprintf(stderr, "FAIL %s got=%u exp=%u\n", name, got, exp);
		fail = 1;
	}
}

static void call_dut(bool en, u8 map)
{
	host_fw_bcn_offload_reset();
	rtw_hal_set_fw_ap_bcn_offload_cmd(host_fw_bcn_offload_adapter(), en, map);
}

static void assert_layout(bool en, u8 map, u8 payload, u8 fw_bcn, u8 tbtt_map)
{
	const u8 *buf;

	expect_u8("h2c_count", host_fw_bcn_offload_h2c_count(), 1);
	expect_u8("h2c_cmd", host_fw_bcn_offload_h2c_cmd(0), H2C_FW_BCN_OFFLOAD);
	expect_u8("h2c_len", host_fw_bcn_offload_h2c_len(0), H2C_BCN_OFFLOAD_LEN);
	buf = host_fw_bcn_offload_h2c_buf(0);
	expect_u8("payload", buf ? buf[0] : 0, payload);
	expect_u8("fw_bcn_offload", host_fw_bcn_offload_get_fw_bcn(), fw_bcn);
	expect_u8("vap_tbtt_rpt_map", host_fw_bcn_offload_get_tbtt_map(), tbtt_map);
}

static void case_absolute_layout(bool en, u8 map, u8 payload, u8 fw_bcn)
{
	call_dut(en, map);
	assert_layout(en, map, payload, fw_bcn, map);
}

static void case_dut_matches_oracle(bool en, u8 map)
{
	u8 p_dut, p_orc;

	host_fw_bcn_offload_reset();
	rtw_hal_set_fw_ap_bcn_offload_cmd(host_fw_bcn_offload_adapter(), en, map);
	p_dut = host_fw_bcn_offload_h2c_buf(0) ? host_fw_bcn_offload_h2c_buf(0)[0] : 0;

	host_fw_bcn_offload_reset();
	rtw_hal_set_fw_ap_bcn_offload_cmd_c_oracle(host_fw_bcn_offload_adapter(), en,
						   map);
	p_orc = host_fw_bcn_offload_h2c_buf(0) ? host_fw_bcn_offload_h2c_buf(0)[0] : 0;
	expect_u8("dut_vs_oracle_payload", p_dut, p_orc);
}

int main(void)
{
	fail = 0;

	/* Pin C bit layout (cmd 0xBA, len 1) — sparse map, disable path, VAP4 map bit. */
	case_absolute_layout(_TRUE, 0x04, 0x09, 1);
	case_absolute_layout(_FALSE, 0x0f, 0x1e, 0);
	case_absolute_layout(_TRUE, 0x1f, 0x1f, 1);
	case_dut_matches_oracle(_TRUE, 0x0f);

	if (fail) {
		fprintf(stderr, "hal_com_fw_bcn_offload L2 failed\n");
		return 1;
	}
	puts("OK hal_com_fw_bcn_offload L2");
	return 0;
}