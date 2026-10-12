// SPDX-License-Identifier: GPL-2.0
/* L1 reference object for W4-10 HALMAC mapper symbols. */

#include <stdint.h>

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

uint8_t network_type_halmac2drv(uint32_t network);
void beacon_ctrl_halmac2drv(const struct halmac_bcn_ctrl *ctrl,
			    struct rtw_halmac_bcn_ctrl *drv_ctrl);
void beacon_ctrl_drv2halmac(const struct rtw_halmac_bcn_ctrl *drv_ctrl,
			    struct halmac_bcn_ctrl *ctrl);

enum halmac_drv_rsvd_pg_num {
	HALMAC_RSVD_PG_NUM8,
	HALMAC_RSVD_PG_NUM16,
	HALMAC_RSVD_PG_NUM24,
	HALMAC_RSVD_PG_NUM32,
	HALMAC_RSVD_PG_NUM64,
	HALMAC_RSVD_PG_NUM128,
	HALMAC_RSVD_PG_NUM256,
};

unsigned rsvd_page_num_drv2halmac(uint16_t num);
unsigned rsvd_page_num_halmac2drv(unsigned pg);

uint8_t _network_type_halmac2drv(uint32_t network)
{
	return network_type_halmac2drv(network);
}

void _beacon_ctrl_halmac2drv(struct halmac_bcn_ctrl *ctrl,
			     struct rtw_halmac_bcn_ctrl *drv_ctrl)
{
	beacon_ctrl_halmac2drv(ctrl, drv_ctrl);
}

void _beacon_ctrl_drv2halmac(struct rtw_halmac_bcn_ctrl *drv_ctrl,
			     struct halmac_bcn_ctrl *ctrl)
{
	beacon_ctrl_drv2halmac(drv_ctrl, ctrl);
}

enum halmac_drv_rsvd_pg_num _rsvd_page_num_drv2halmac(uint16_t num)
{
	return (enum halmac_drv_rsvd_pg_num)rsvd_page_num_drv2halmac(num);
}

uint16_t _rsvd_page_num_halmac2drv(enum halmac_drv_rsvd_pg_num pg)
{
	return (uint16_t)rsvd_page_num_halmac2drv((unsigned)pg);
}
