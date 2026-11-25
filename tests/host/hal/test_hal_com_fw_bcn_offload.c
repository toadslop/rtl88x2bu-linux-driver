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
void rtw_hal_set_bcn_rsvdpage_loc_cmd(struct _adapter *adapter);
#else
#define rtw_hal_set_fw_ap_bcn_offload_cmd rtw_hal_set_fw_ap_bcn_offload_cmd_c_oracle
#define rtw_hal_set_bcn_rsvdpage_loc_cmd rtw_hal_set_bcn_rsvdpage_loc_cmd_c_oracle
#endif

void rtw_hal_set_fw_ap_bcn_offload_cmd_c_oracle(struct _adapter *, bool, u8);
void rtw_hal_set_bcn_rsvdpage_loc_cmd_c_oracle(struct _adapter *adapter);
void host_fw_bcn_offload_set_vap_map(u8 vap_map);
void host_fw_bcn_offload_set_page_size(u32 page_size);

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

static void case_rsvdpage_matches_oracle_page(u8 vap_map, u32 page_size)
{
	u8 dut_payload[H2C_BCN_RSVDPAGE_LEN];
	u8 orc_payload[H2C_BCN_RSVDPAGE_LEN];
	const u8 *buf;
	unsigned i;

	host_fw_bcn_offload_reset();
	host_fw_bcn_offload_set_page_size(page_size);
	host_fw_bcn_offload_set_vap_map(vap_map);
	rtw_hal_set_bcn_rsvdpage_loc_cmd(host_fw_bcn_offload_adapter());

	expect_u8("rsvd_h2c_count", host_fw_bcn_offload_h2c_count(), 1);
	expect_u8("rsvd_h2c_cmd", host_fw_bcn_offload_h2c_cmd(0), H2C_BCN_RSVDPAGE);
	expect_u8("rsvd_h2c_len", host_fw_bcn_offload_h2c_len(0), H2C_BCN_RSVDPAGE_LEN);
	buf = host_fw_bcn_offload_h2c_buf(0);
	if (!buf) {
		fprintf(stderr, "FAIL rsvdpage null dut h2c buffer\n");
		fail = 1;
		return;
	}
	memcpy(dut_payload, buf, H2C_BCN_RSVDPAGE_LEN);

	host_fw_bcn_offload_reset();
	host_fw_bcn_offload_set_page_size(page_size);
	host_fw_bcn_offload_set_vap_map(vap_map);
	rtw_hal_set_bcn_rsvdpage_loc_cmd_c_oracle(host_fw_bcn_offload_adapter());
	buf = host_fw_bcn_offload_h2c_buf(0);
	if (!buf) {
		fprintf(stderr, "FAIL rsvdpage null oracle h2c buffer\n");
		fail = 1;
		return;
	}
	memcpy(orc_payload, buf, H2C_BCN_RSVDPAGE_LEN);

	for (i = 0; i < H2C_BCN_RSVDPAGE_LEN; i++)
		expect_u8("rsvd_payload", dut_payload[i], orc_payload[i]);
}

static void case_rsvdpage_matches_oracle(u8 vap_map)
{
	case_rsvdpage_matches_oracle_page(vap_map, 128);
}

int main(void)
{
	fail = 0;

	/* Pin C bit layout (cmd 0xBA, len 1) — sparse map, disable path, VAP4 map bit. */
	case_absolute_layout(_TRUE, 0x01, 0x03, 1); /* ROOT only → H2C bit 1 */
	case_absolute_layout(_TRUE, 0x02, 0x05, 1); /* VAP1 only → H2C bit 2 */
	case_absolute_layout(_TRUE, 0x04, 0x09, 1);
	case_absolute_layout(_FALSE, 0x0f, 0x1e, 0);
	case_absolute_layout(_TRUE, 0x1f, 0x1f, 1);
	case_dut_matches_oracle(_TRUE, 0x0f);

	/* H2C 0x09 vs C oracle — sparse maps at default page_size 128 */
	case_rsvdpage_matches_oracle(0x00);
	case_rsvdpage_matches_oracle(0x01);
	case_rsvdpage_matches_oracle(0x02);
	case_rsvdpage_matches_oracle(0x04);
	case_rsvdpage_matches_oracle(0x05);
	case_rsvdpage_matches_oracle(0x08);
	case_rsvdpage_matches_oracle(0x0f);
	case_rsvdpage_matches_oracle(0x10);
	case_rsvdpage_matches_oracle(0x1f);
	case_rsvdpage_matches_oracle(0xaa); /* VAP4 bit set; byte 4 stays 0 */

	case_rsvdpage_matches_oracle_page(0x0f, 256);

	if (fail) {
		fprintf(stderr, "hal_com_fw_bcn_offload L2 failed\n");
		return 1;
	}
	puts("OK hal_com_fw_bcn_offload L2");
	return 0;
}