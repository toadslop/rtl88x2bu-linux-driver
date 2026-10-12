// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-10/W4-11 HALMAC mappers (hal/hal_halmac.c). */

#include <stdint.h>
#include <string.h>

#include "hal_halmac_drv_bcn_ctrl.h"

#define HW_STATE_NOLINK 0x00
#define HW_STATE_ADHOC 0x01
#define HW_STATE_STATION 0x02
#define HW_STATE_AP 0x03

enum halmac_network_type_select {
	HALMAC_NETWORK_NO_LINK = 0,
	HALMAC_NETWORK_ADHOC = 1,
	HALMAC_NETWORK_INFRASTRUCTURE = 2,
	HALMAC_NETWORK_AP = 3,
	HALMAC_NETWORK_UNDEFINE = 0x7F,
};

struct halmac_bcn_ctrl {
	uint8_t dis_rx_bssid_fit;
	uint8_t en_txbcn_rpt;
	uint8_t dis_tsf_udt;
	uint8_t en_bcn;
	uint8_t en_rxbcn_rpt;
	uint8_t en_p2p_ctwin;
	uint8_t en_p2p_bcn_area;
};

uint8_t network_type_halmac2drv(uint32_t network)
{
	uint8_t type = HW_STATE_NOLINK;

	switch (network) {
	case HALMAC_NETWORK_NO_LINK:
	case HALMAC_NETWORK_UNDEFINE:
		type = HW_STATE_NOLINK;
		break;
	case HALMAC_NETWORK_ADHOC:
		type = HW_STATE_ADHOC;
		break;
	case HALMAC_NETWORK_INFRASTRUCTURE:
		type = HW_STATE_STATION;
		break;
	case HALMAC_NETWORK_AP:
		type = HW_STATE_AP;
		break;
	default:
		break;
	}

	return type;
}

void beacon_ctrl_halmac2drv(const struct halmac_bcn_ctrl *ctrl,
			    struct rtw_halmac_bcn_ctrl *drv_ctrl)
{
	memset(drv_ctrl, 0, sizeof(*drv_ctrl));
	drv_ctrl->rx_bssid_fit = ctrl->dis_rx_bssid_fit ? 0 : 1;
	drv_ctrl->txbcn_rpt = ctrl->en_txbcn_rpt ? 1 : 0;
	drv_ctrl->tsf_update = ctrl->dis_tsf_udt ? 0 : 1;
	drv_ctrl->enable_bcn = ctrl->en_bcn ? 1 : 0;
	drv_ctrl->rxbcn_rpt = ctrl->en_rxbcn_rpt ? 1 : 0;
	drv_ctrl->p2p_ctwin = ctrl->en_p2p_ctwin ? 1 : 0;
	drv_ctrl->p2p_bcn_area = ctrl->en_p2p_bcn_area ? 1 : 0;
}

void beacon_ctrl_drv2halmac(const struct rtw_halmac_bcn_ctrl *drv_ctrl,
			    struct halmac_bcn_ctrl *ctrl)
{
	memset(ctrl, 0, sizeof(*ctrl));
	ctrl->dis_rx_bssid_fit = drv_ctrl->rx_bssid_fit ? 0 : 1;
	ctrl->en_txbcn_rpt = drv_ctrl->txbcn_rpt ? 1 : 0;
	ctrl->dis_tsf_udt = drv_ctrl->tsf_update ? 0 : 1;
	ctrl->en_bcn = drv_ctrl->enable_bcn ? 1 : 0;
	ctrl->en_rxbcn_rpt = drv_ctrl->rxbcn_rpt ? 1 : 0;
	ctrl->en_p2p_ctwin = drv_ctrl->p2p_ctwin ? 1 : 0;
	ctrl->en_p2p_bcn_area = drv_ctrl->p2p_bcn_area ? 1 : 0;
}

enum halmac_drv_rsvd_pg_num {
	HALMAC_RSVD_PG_NUM8,
	HALMAC_RSVD_PG_NUM16,
	HALMAC_RSVD_PG_NUM24,
	HALMAC_RSVD_PG_NUM32,
	HALMAC_RSVD_PG_NUM64,
	HALMAC_RSVD_PG_NUM128,
	HALMAC_RSVD_PG_NUM256,
};

unsigned rsvd_page_num_drv2halmac(uint16_t num)
{
	if (num <= 8)
		return HALMAC_RSVD_PG_NUM8;
	if (num <= 16)
		return HALMAC_RSVD_PG_NUM16;
	if (num <= 24)
		return HALMAC_RSVD_PG_NUM24;
	if (num <= 32)
		return HALMAC_RSVD_PG_NUM32;
	if (num <= 64)
		return HALMAC_RSVD_PG_NUM64;
	if (num <= 128)
		return HALMAC_RSVD_PG_NUM128;

	return HALMAC_RSVD_PG_NUM256;
}

unsigned rsvd_page_num_halmac2drv(unsigned rsvd_page_number)
{
	switch (rsvd_page_number) {
	case HALMAC_RSVD_PG_NUM8:
		return 8;
	case HALMAC_RSVD_PG_NUM16:
		return 16;
	case HALMAC_RSVD_PG_NUM24:
		return 24;
	case HALMAC_RSVD_PG_NUM32:
		return 32;
	case HALMAC_RSVD_PG_NUM64:
		return 64;
	case HALMAC_RSVD_PG_NUM128:
		return 128;
	case HALMAC_RSVD_PG_NUM256:
		return 256;
	default:
		return 0;
	}
}
