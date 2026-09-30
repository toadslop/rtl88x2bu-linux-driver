/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_BEAMFORMING_BF_INIT_CMD_ORACLE_H
#define HOST_BEAMFORMING_BF_INIT_CMD_ORACLE_H

#include <string.h>

#include "host_beamforming_bf_types.h"

#define BF_HOST_BF_STATE_IDLE 0
#define BF_HOST_SOUND_NONE 0
#define BF_HOST_IDX_NONE 0xFF
#define BF_HOST_OFDM24 0

#define BF_HOST_CMD_ENTER 0
#define BF_HOST_CMD_LEAVE 1
#define BF_HOST_CMD_START_PERIOD 2
#define BF_HOST_CMD_END_PERIOD 3
#define BF_HOST_CMD_SET_GID 6
#define BF_HOST_CMD_SET_CSI 7

struct bf_host_cmd_hdl_tr {
	unsigned enter, leave, reset, start_period, end_period, set_gid, set_csi;
	u8 end_period_macid;
};

extern struct bf_host_cmd_hdl_tr bf_host_cmd_hdl_tr;

void bf_host_beamforming_enter(bf_host_padpt a, u8 *p);
void bf_host_beamforming_leave(bf_host_padpt a, u8 *p);
void bf_host_beamforming_reset(bf_host_padpt a);
void bf_host_sounding_handler(bf_host_padpt a);
void bf_host_beamforming_sounding_down(bf_host_padpt a, u8 macid);
void bf_host_hal_set_gid(bf_host_padpt a, u8 *p);
void bf_host_hal_set_csi(bf_host_padpt a, u8 *p);

static inline void bf_host_sounding_init(struct bf_host_sounding_info *s)
{
	memset(s->su_sounding_list, 0xFF, BF_HOST_SU_SOUND);
	memset(s->mu_sounding_list, 0xFF, BF_HOST_MU_SOUND);
	s->state = BF_HOST_SOUND_NONE;
	s->su_bfee_curidx = BF_HOST_IDX_NONE;
	s->candidate_mu_bfee_cnt = 0;
	s->min_sounding_period = 0;
	s->sound_remain_cnt_per_period = 0;
}

#ifndef HOST_BF_INIT_CMD_RUST
static inline void o_bf_init(bf_host_padpt adapter)
{
	struct bf_host_info *info = BF_GET_INFO(adapter);

	info->beamforming_cap = BF_HOST_CAP_NONE;
	info->beamforming_state = BF_HOST_BF_STATE_IDLE;
	info->sounding_sequence = 0;
	info->beamformee_su_cnt = 0;
	info->beamformer_su_cnt = 0;
	info->beamformee_su_reg_maping = 0;
	info->beamformer_su_reg_maping = 0;
	info->beamformee_mu_cnt = 0;
	info->beamformer_mu_cnt = 0;
	info->beamformee_mu_reg_maping = 0;
	info->first_mu_bfee_index = BF_HOST_IDX_NONE;
	info->mu_bfer_curidx = BF_HOST_IDX_NONE;
	info->cur_csi_rpt_rate = BF_HOST_OFDM24;
	bf_host_sounding_init(&info->sounding_info);
	info->timer_inits = 2;
	info->SetHalBFEnterOnDemandCnt = 0;
	info->SetHalBFLeaveOnDemandCnt = 0;
	info->SetHalSoundownOnDemandCnt = 0;
	info->bEnableSUTxBFWorkAround = 1;
	info->TargetSUBFee = NULL;
	info->sounding_running = 0;
}

static inline void o_bf_cmd_hdl(bf_host_padpt adapter, u8 type, u8 *pbuf)
{
	switch (type) {
	case BF_HOST_CMD_ENTER:
		bf_host_beamforming_enter(adapter, pbuf);
		break;
	case BF_HOST_CMD_LEAVE:
		if (pbuf == NULL)
			bf_host_beamforming_reset(adapter);
		else
			bf_host_beamforming_leave(adapter, pbuf);
		break;
	case BF_HOST_CMD_START_PERIOD:
		bf_host_sounding_handler(adapter);
		break;
	case BF_HOST_CMD_END_PERIOD:
		bf_host_beamforming_sounding_down(adapter, pbuf ? *pbuf : 0);
		break;
	case BF_HOST_CMD_SET_GID:
		bf_host_hal_set_gid(adapter, pbuf);
		break;
	case BF_HOST_CMD_SET_CSI:
		bf_host_hal_set_csi(adapter, pbuf);
		break;
	default:
		break;
	}
}
#else
void o_bf_init(bf_host_padpt adapter);
void o_bf_cmd_hdl(bf_host_padpt adapter, u8 type, u8 *pbuf);
#endif

#endif
