// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-10 HALMAC network/beacon ctrl mappers (hal/hal_halmac.c). */

#include <stdint.h>
#include <string.h>

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

struct rtw_halmac_bcn_ctrl {
	uint8_t rx_bssid_fit;
	uint8_t txbcn_rpt;
	uint8_t tsf_update;
	uint8_t enable_bcn;
	uint8_t rxbcn_rpt;
	uint8_t p2p_ctwin;
	uint8_t p2p_bcn_area;
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
