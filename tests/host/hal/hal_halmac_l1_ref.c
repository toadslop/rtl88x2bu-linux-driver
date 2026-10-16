// SPDX-License-Identifier: GPL-2.0
/* L1 reference object for W4-10 HALMAC mapper symbols. */

#include <stdint.h>

#include "hal_halmac_phy_parameter.h"

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
unsigned hw_port_drv2halmac(unsigned hwport);
unsigned network_type_drv2halmac(uint8_t type);

enum halmac_portid {
	HALMAC_PORTID0 = 0,
	HALMAC_PORTID1 = 1,
	HALMAC_PORTID2 = 2,
	HALMAC_PORTID3 = 3,
	HALMAC_PORTID4 = 4,
	HALMAC_PORTID_NUM = 5,
};

enum halmac_network_type_select {
	HALMAC_NETWORK_NO_LINK = 0,
	HALMAC_NETWORK_ADHOC = 1,
	HALMAC_NETWORK_INFRASTRUCTURE = 2,
	HALMAC_NETWORK_AP = 3,
	HALMAC_NETWORK_UNDEFINE = 0x7F,
};

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

enum halmac_portid _hw_port_drv2halmac(unsigned hwport)
{
	return (enum halmac_portid)hw_port_drv2halmac(hwport);
}

enum halmac_network_type_select _network_type_drv2halmac(uint8_t type)
{
	return (enum halmac_network_type_select)network_type_drv2halmac(type);
}

enum halmac_rf_type {
	HALMAC_RF_1T2R = 0,
	HALMAC_RF_2T4R = 1,
	HALMAC_RF_2T2R = 2,
	HALMAC_RF_2T3R = 3,
	HALMAC_RF_1T1R = 4,
	HALMAC_RF_2T2R_GREEN = 5,
	HALMAC_RF_3T3R = 6,
	HALMAC_RF_3T4R = 7,
	HALMAC_RF_4T4R = 8,
};

enum rf_type {
	RF_1T1R = 0,
	RF_1T2R = 1,
	RF_2T2R = 2,
	RF_2T3R = 3,
	RF_2T4R = 4,
	RF_3T3R = 5,
	RF_3T4R = 6,
	RF_4T4R = 7,
	RF_TYPE_MAX = 16,
};

unsigned rf_type_drv2halmac(unsigned rf_drv);
unsigned rf_type_halmac2drv(unsigned rf_mac);

enum halmac_rf_type _rf_type_drv2halmac(enum rf_type rf_drv)
{
	return (enum halmac_rf_type)rf_type_drv2halmac((unsigned)rf_drv);
}

enum rf_type _rf_type_halmac2drv(enum halmac_rf_type rf_mac)
{
	return (enum rf_type)rf_type_halmac2drv((unsigned)rf_mac);
}

enum hal_fifo_sel {
	HAL_FIFO_SEL_TX,
	HAL_FIFO_SEL_RX,
	HAL_FIFO_SEL_RSVD_PAGE,
	HAL_FIFO_SEL_REPORT,
	HAL_FIFO_SEL_LLT,
	HAL_FIFO_SEL_RXBUF_FW,
};

unsigned fifo_sel_drv2halmac(unsigned fifo_sel);

enum hal_fifo_sel _fifo_sel_drv2halmac(uint8_t fifo_sel)
{
	return (enum hal_fifo_sel)fifo_sel_drv2halmac((unsigned)fifo_sel);
}

#ifdef CONFIG_USB_HCI
enum RTW_USB_SPEED {
	RTW_USB_SPEED_UNKNOWN = 0,
	RTW_USB_SPEED_1_1 = 1,
	RTW_USB_SPEED_2 = 2,
	RTW_USB_SPEED_3 = 3,
};

enum halmac_usb_mode {
	HALMAC_USB_MODE_U2 = 1,
	HALMAC_USB_MODE_U3 = 2,
};

unsigned usb_mode_drv2halmac(unsigned usb_mode);

enum halmac_usb_mode _usb_mode_drv2halmac(enum RTW_USB_SPEED usb_mode)
{
	return (enum halmac_usb_mode)usb_mode_drv2halmac((unsigned)usb_mode);
}
#endif /* CONFIG_USB_HCI */

enum fw_mem {
	FW_EMEM,
	FW_IMEM,
	FW_DMEM,
};

enum halmac_dlfw_mem {
	HALMAC_DLFW_MEM_EMEM = 0x00,
	HALMAC_DLFW_MEM_EMEM_RSVD_PG = 0x01,
	HALMAC_DLFW_MEM_UNDEFINE = 0x7F,
};

unsigned fw_mem_drv2halmac(unsigned mem, unsigned tx_stop);

enum halmac_dlfw_mem _fw_mem_drv2halmac(enum fw_mem mem, uint8_t tx_stop)
{
	return (enum halmac_dlfw_mem)fw_mem_drv2halmac((unsigned)mem, tx_stop);
}

enum halmac_wlled_mode {
	HALMAC_WLLED_MODE_TRX = 0,
	HALMAC_WLLED_MODE_TX = 1,
	HALMAC_WLLED_MODE_RX = 2,
	HALMAC_WLLED_MODE_SW_CTRL = 3,
};

unsigned led_mode_drv2halmac(unsigned drv_mode);

enum halmac_wlled_mode _led_mode_drv2halmac(uint8_t drv_mode)
{
	return (enum halmac_wlled_mode)led_mode_drv2halmac(drv_mode);
}

unsigned phy_parameter_val_drv2halmac(unsigned val, unsigned msk_en, unsigned msk);

uint32_t _phy_parameter_val_drv2halmac(uint32_t val, uint8_t msk_en, uint32_t msk)
{
	return phy_parameter_val_drv2halmac(val, msk_en, msk);
}

int _phy_parameter_drv2halmac(struct rtw_phy_parameter *para,
			      struct halmac_phy_parameter_info *info)
{
	return phy_parameter_drv2halmac(para, info);
}

#ifdef CONFIG_SUPPORT_TRX_SHARED
enum halmac_rx_fifo_expanding_mode {
	HALMAC_RX_FIFO_EXPANDING_MODE_DISABLE = 0,
	HALMAC_RX_FIFO_EXPANDING_MODE_1_BLOCK = 1,
	HALMAC_RX_FIFO_EXPANDING_MODE_2_BLOCK = 2,
	HALMAC_RX_FIFO_EXPANDING_MODE_3_BLOCK = 3,
};

unsigned trx_share_mode_drv2halmac(uint8_t mode);

enum halmac_rx_fifo_expanding_mode _trx_share_mode_drv2halmac(uint8_t mode)
{
	return (enum halmac_rx_fifo_expanding_mode)trx_share_mode_drv2halmac(mode);
}

void dump_trx_share_mode(void *sel, void *adapter);
#endif /* CONFIG_SUPPORT_TRX_SHARED */
