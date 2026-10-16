// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-10/W4-11 HALMAC mappers (hal/hal_halmac.c). */

#include <stdint.h>
#include <string.h>

#include "hal_halmac_drv_bcn_ctrl.h"
#include "hal_halmac_phy_parameter.h"

#define HW_STATE_NOLINK 0x00
#define HW_STATE_ADHOC 0x01
#define HW_STATE_STATION 0x02
#define HW_STATE_AP 0x03
#define HW_STATE_MONITOR 0x04

enum _hw_port {
	HW_PORT0 = 0,
	HW_PORT1,
	HW_PORT2,
	HW_PORT3,
	HW_PORT4,
	MAX_HW_PORT,
};

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

struct halmac_bcn_ctrl {
	uint8_t dis_rx_bssid_fit;
	uint8_t en_txbcn_rpt;
	uint8_t dis_tsf_udt;
	uint8_t en_bcn;
	uint8_t en_rxbcn_rpt;
	uint8_t en_p2p_ctwin;
	uint8_t en_p2p_bcn_area;
};

unsigned hw_port_drv2halmac(unsigned hwport)
{
	enum halmac_portid port = HALMAC_PORTID_NUM;

	switch (hwport) {
	case HW_PORT0:
		port = HALMAC_PORTID0;
		break;
	case HW_PORT1:
		port = HALMAC_PORTID1;
		break;
	case HW_PORT2:
		port = HALMAC_PORTID2;
		break;
	case HW_PORT3:
		port = HALMAC_PORTID3;
		break;
	case HW_PORT4:
		port = HALMAC_PORTID4;
		break;
	default:
		break;
	}

	return (unsigned)port;
}

unsigned network_type_drv2halmac(uint8_t type)
{
	enum halmac_network_type_select network = HALMAC_NETWORK_UNDEFINE;

	switch (type) {
	case HW_STATE_NOLINK:
	case HW_STATE_MONITOR:
		network = HALMAC_NETWORK_NO_LINK;
		break;
	case HW_STATE_ADHOC:
		network = HALMAC_NETWORK_ADHOC;
		break;
	case HW_STATE_STATION:
		network = HALMAC_NETWORK_INFRASTRUCTURE;
		break;
	case HW_STATE_AP:
		network = HALMAC_NETWORK_AP;
		break;
	default:
		break;
	}

	return (unsigned)network;
}

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

/* W4-22: RF type mappers (hal/hal_halmac.c). */
#define RF_TYPE_MAX 16
#define HALMAC_RF_MAX_TYPE 0xF

static const uint8_t rf_drv_to_halmac[8] = { 4, 0, 2, 3, 1, 6, 7, 8 };

unsigned rf_type_drv2halmac(unsigned rf_drv)
{
	if (rf_drv < 8)
		return rf_drv_to_halmac[rf_drv];
	return HALMAC_RF_MAX_TYPE;
}

unsigned rf_type_halmac2drv(unsigned rf_mac)
{
	switch (rf_mac) {
	case 0:
		return 1;
	case 1:
		return 4;
	case 2:
	case 5:
		return 2;
	case 3:
		return 3;
	case 4:
		return 0;
	case 6:
		return 5;
	case 7:
		return 6;
	case 8:
		return 7;
	default:
		return RF_TYPE_MAX;
	}
}

/* W4-23: FIFO sel + USB mode mappers (hal/hal_halmac.c). */
enum hal_fifo_sel {
	HAL_FIFO_SEL_TX,
	HAL_FIFO_SEL_RX,
	HAL_FIFO_SEL_RSVD_PAGE,
	HAL_FIFO_SEL_REPORT,
	HAL_FIFO_SEL_LLT,
	HAL_FIFO_SEL_RXBUF_FW,
};

unsigned fifo_sel_drv2halmac(unsigned fifo_sel)
{
	switch (fifo_sel) {
	case 0:
		return HAL_FIFO_SEL_TX;
	case 1:
		return HAL_FIFO_SEL_RX;
	case 2:
		return HAL_FIFO_SEL_RSVD_PAGE;
	case 3:
		return HAL_FIFO_SEL_REPORT;
	case 4:
		return HAL_FIFO_SEL_LLT;
	case 5:
		return HAL_FIFO_SEL_RXBUF_FW;
	default:
		return HAL_FIFO_SEL_RSVD_PAGE;
	}
}

#define RTW_USB_SPEED_2 2
#define RTW_USB_SPEED_3 3
#define HALMAC_USB_MODE_U2 1
#define HALMAC_USB_MODE_U3 2

unsigned usb_mode_drv2halmac(unsigned usb_mode)
{
	unsigned halmac_usb_mode = HALMAC_USB_MODE_U2;

	switch (usb_mode) {
	case RTW_USB_SPEED_2:
		halmac_usb_mode = HALMAC_USB_MODE_U2;
		break;
	case RTW_USB_SPEED_3:
		halmac_usb_mode = HALMAC_USB_MODE_U3;
		break;
	default:
		halmac_usb_mode = HALMAC_USB_MODE_U2;
		break;
	}

	return halmac_usb_mode;
}

/* W4-24: FW mem, LED mode, phy param val mappers (hal/hal_halmac.c). */
enum fw_mem {
	FW_EMEM,
	FW_IMEM,
	FW_DMEM,
};

#define HALMAC_DLFW_MEM_EMEM 0x00
#define HALMAC_DLFW_MEM_EMEM_RSVD_PG 0x01
#define HALMAC_DLFW_MEM_UNDEFINE 0x7f

#define _FALSE 0
#define _TRUE 1

static uint32_t bitshift(uint32_t bitmask)
{
	uint32_t i;

	for (i = 0; i <= 31; i++)
		if (((bitmask >> i) & 0x1) == 1)
			break;

	return i;
}

unsigned fw_mem_drv2halmac(unsigned mem, unsigned tx_stop)
{
	unsigned mem_halmac = HALMAC_DLFW_MEM_UNDEFINE;

	switch (mem) {
	case FW_EMEM:
		if (tx_stop == _FALSE)
			mem_halmac = HALMAC_DLFW_MEM_EMEM_RSVD_PG;
		else
			mem_halmac = HALMAC_DLFW_MEM_EMEM;
		break;
	case FW_IMEM:
	case FW_DMEM:
		mem_halmac = HALMAC_DLFW_MEM_UNDEFINE;
		break;
	}

	return mem_halmac;
}

enum halmac_wlled_mode {
	HALMAC_WLLED_MODE_TRX = 0,
	HALMAC_WLLED_MODE_TX = 1,
	HALMAC_WLLED_MODE_RX = 2,
	HALMAC_WLLED_MODE_SW_CTRL = 3,
};

unsigned led_mode_drv2halmac(unsigned drv_mode)
{
	unsigned halmac_mode;

	switch (drv_mode) {
	case 1:
		halmac_mode = HALMAC_WLLED_MODE_TX;
		break;
	case 2:
		halmac_mode = HALMAC_WLLED_MODE_RX;
		break;
	case 3:
		halmac_mode = HALMAC_WLLED_MODE_SW_CTRL;
		break;
	case 0:
	default:
		halmac_mode = HALMAC_WLLED_MODE_TRX;
		break;
	}

	return halmac_mode;
}

unsigned phy_parameter_val_drv2halmac(unsigned val, unsigned msk_en, unsigned msk)
{
	if (!msk_en)
		return val;

	return val << bitshift(msk);
}

int phy_parameter_drv2halmac(const struct rtw_phy_parameter *para,
			     struct halmac_phy_parameter_info *info)
{
	if (!para || !info)
		return -1;

	memset(info, 0, sizeof(*info));

	switch (para->cmd) {
	case 0:
		switch (para->data.mac.size) {
		case 1:
			info->cmd_id = HALMAC_PARAMETER_CMD_MAC_W8;
			break;
		case 2:
			info->cmd_id = HALMAC_PARAMETER_CMD_MAC_W16;
			break;
		default:
			info->cmd_id = HALMAC_PARAMETER_CMD_MAC_W32;
			break;
		}
		info->content.MAC_REG_W.value =
			phy_parameter_val_drv2halmac(para->data.mac.value,
						     para->data.mac.msk_en,
						     para->data.mac.msk);
		info->content.MAC_REG_W.msk = para->data.mac.msk;
		info->content.MAC_REG_W.offset = para->data.mac.offset;
		info->content.MAC_REG_W.msk_en = para->data.mac.msk_en;
		break;

	case 1:
		switch (para->data.bb.size) {
		case 1:
			info->cmd_id = HALMAC_PARAMETER_CMD_BB_W8;
			break;
		case 2:
			info->cmd_id = HALMAC_PARAMETER_CMD_BB_W16;
			break;
		default:
			info->cmd_id = HALMAC_PARAMETER_CMD_BB_W32;
			break;
		}
		info->content.BB_REG_W.value =
			phy_parameter_val_drv2halmac(para->data.bb.value,
						     para->data.bb.msk_en,
						     para->data.bb.msk);
		info->content.BB_REG_W.msk = para->data.bb.msk;
		info->content.BB_REG_W.offset = para->data.bb.offset;
		info->content.BB_REG_W.msk_en = para->data.bb.msk_en;
		break;

	case 2:
		info->cmd_id = HALMAC_PARAMETER_CMD_RF_W;
		info->content.RF_REG_W.value =
			phy_parameter_val_drv2halmac(para->data.rf.value,
						     para->data.rf.msk_en,
						     para->data.rf.msk);
		info->content.RF_REG_W.msk = para->data.rf.msk;
		info->content.RF_REG_W.offset = para->data.rf.offset;
		info->content.RF_REG_W.msk_en = para->data.rf.msk_en;
		info->content.RF_REG_W.rf_path = para->data.rf.path;
		break;

	case 3:
		if (para->data.delay.unit == 0)
			info->cmd_id = HALMAC_PARAMETER_CMD_DELAY_US;
		else
			info->cmd_id = HALMAC_PARAMETER_CMD_DELAY_MS;
		info->content.DELAY_TIME.delay_time = para->data.delay.value;
		break;

	case 0xFF:
		info->cmd_id = HALMAC_PARAMETER_CMD_END;
		break;

	default:
		return -1;
	}

	return 0;
}
