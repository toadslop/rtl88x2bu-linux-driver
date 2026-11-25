// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_hal_com_fw_bcn_offload_types.h"

void host_fw_bcn_offload_reset(void);
int host_fw_bcn_offload_h2c_count(void);
u8 host_fw_bcn_offload_h2c_cmd(int idx);
const u8 *host_fw_bcn_offload_h2c_buf(int idx);
u8 host_fw_bcn_offload_get_fw_bcn(void);
u8 host_fw_bcn_offload_get_tbtt_map(void);
struct _adapter *host_fw_bcn_offload_adapter(void);
void rtw_hal_set_fw_ap_bcn_offload_cmd_c_oracle(struct _adapter *, bool, u8);

static int fail;
static void chk_u8(unsigned got, unsigned exp, const char *msg)
{
	if (got != exp) {
		fprintf(stderr, "FAIL %s got=%u exp=%u\n", msg, got, exp);
		fail = 1;
	}
}

static void run1(bool prod, bool en, u8 map, u8 *payload, u8 *tbtt)
{
	host_fw_bcn_offload_reset();
	if (prod)
		rtw_hal_set_fw_ap_bcn_offload_cmd(host_fw_bcn_offload_adapter(), en, map);
	else
		rtw_hal_set_fw_ap_bcn_offload_cmd_c_oracle(host_fw_bcn_offload_adapter(), en,
						       map);
	*payload = host_fw_bcn_offload_h2c_buf(0) ? host_fw_bcn_offload_h2c_buf(0)[0] : 0;
	*tbtt = host_fw_bcn_offload_get_tbtt_map();
	chk_u8(host_fw_bcn_offload_h2c_count(), 1, "h2c_count");
	chk_u8(host_fw_bcn_offload_h2c_cmd(0), H2C_FW_BCN_OFFLOAD, "h2c_cmd");
}

int main(void)
{
	u8 p_dut, p_orc, t_dut, t_orc;
	fail = 0;
	run1(1, _TRUE, 0x0f, &p_dut, &t_dut);
	run1(0, _TRUE, 0x0f, &p_orc, &t_orc);
	chk_u8(p_dut, p_orc, "payload");
	chk_u8(t_dut, t_orc, "tbtt");
	run1(1, _TRUE, 0x1f, &p_dut, &t_dut);
	chk_u8(t_dut, 0x1f, "map_store");

	if (fail) {
		fprintf(stderr, "hal_com_fw_bcn_offload L2 failed\n");
		return 1;
	}
	puts("OK hal_com_fw_bcn_offload L2");
	return 0;
}