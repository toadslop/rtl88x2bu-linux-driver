// SPDX-License-Identifier: GPL-2.0
/* W3-129 L2 C oracle: rtw_bf_init + rtw_bf_cmd_hdl leaf. */
#include <stdio.h>
#include <string.h>

#include "host_beamforming_bf_init_cmd_oracle.h"

struct bf_host_cmd_hdl_tr bf_host_cmd_hdl_tr;

void bf_host_cmd(bf_host_padpt a, int type, u8 *p, int sz, u8 enq)
{
	(void)a;
	(void)type;
	(void)p;
	(void)sz;
	(void)enq;
}

void bf_host_beamforming_enter(bf_host_padpt a, u8 *p)
{
	(void)p;
	(void)a;
	bf_host_cmd_hdl_tr.enter++;
}

void bf_host_beamforming_leave(bf_host_padpt a, u8 *p)
{
	(void)p;
	(void)a;
	bf_host_cmd_hdl_tr.leave++;
}

void bf_host_beamforming_reset(bf_host_padpt a)
{
	(void)a;
	bf_host_cmd_hdl_tr.reset++;
}

void bf_host_sounding_handler(bf_host_padpt a)
{
	(void)a;
	bf_host_cmd_hdl_tr.start_period++;
}

void bf_host_beamforming_sounding_down(bf_host_padpt a, u8 macid)
{
	(void)a;
	bf_host_cmd_hdl_tr.end_period++;
	bf_host_cmd_hdl_tr.end_period_macid = macid;
}

void bf_host_hal_set_gid(bf_host_padpt a, u8 *p)
{
	(void)p;
	(void)a;
	bf_host_cmd_hdl_tr.set_gid++;
}

void bf_host_hal_set_csi(bf_host_padpt a, u8 *p)
{
	(void)p;
	(void)a;
	bf_host_cmd_hdl_tr.set_csi++;
}

int main(void)
{
	struct bf_host_adpt a;
	u8 ra[6], macid = 3;
	int bad = 0;

	memset(&a, 0, sizeof(a));
	o_bf_init(&a);
	if (BF_GET_INFO(&a)->beamforming_state != BF_HOST_BF_STATE_IDLE ||
	    BF_GET_INFO(&a)->first_mu_bfee_index != BF_HOST_IDX_NONE ||
	    BF_GET_INFO(&a)->timer_inits != 2 ||
	    BF_GET_INFO(&a)->sounding_info.su_sounding_list[0] != BF_HOST_IDX_NONE) {
		fprintf(stderr, "FAIL init_defaults\n");
		bad++;
	} else {
		printf("PASS init_defaults\n");
	}

	memset(&bf_host_cmd_hdl_tr, 0, sizeof(bf_host_cmd_hdl_tr));
	o_bf_cmd_hdl(&a, BF_HOST_CMD_ENTER, ra);
	o_bf_cmd_hdl(&a, BF_HOST_CMD_LEAVE, ra);
	o_bf_cmd_hdl(&a, BF_HOST_CMD_START_PERIOD, NULL);
	o_bf_cmd_hdl(&a, BF_HOST_CMD_END_PERIOD, &macid);
	o_bf_cmd_hdl(&a, BF_HOST_CMD_SET_GID, ra);
	o_bf_cmd_hdl(&a, BF_HOST_CMD_SET_CSI, ra);
	o_bf_cmd_hdl(&a, BF_HOST_CMD_LEAVE, NULL);
	if (bf_host_cmd_hdl_tr.enter != 1 || bf_host_cmd_hdl_tr.reset != 1 ||
	    bf_host_cmd_hdl_tr.end_period_macid != macid) {
		fprintf(stderr, "FAIL cmd_hdl_dispatch\n");
		bad++;
	} else {
		printf("PASS cmd_hdl_dispatch\n");
	}

	if (!bad)
		printf("PASS 2 vectors (builtin)\n");
	return bad ? 1 : 0;
}
