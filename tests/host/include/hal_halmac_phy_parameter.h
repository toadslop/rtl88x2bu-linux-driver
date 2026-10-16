/* SPDX-License-Identifier: GPL-2.0 */
/* Host layouts for rtw_phy_parameter / halmac_phy_parameter_info (hal_halmac). */

#ifndef HAL_HALMAC_PHY_PARAMETER_H
#define HAL_HALMAC_PHY_PARAMETER_H

#include <stdint.h>

enum halmac_parameter_cmd {
	HALMAC_PARAMETER_CMD_MAC_W8 = 0x4,
	HALMAC_PARAMETER_CMD_MAC_W16 = 0x5,
	HALMAC_PARAMETER_CMD_MAC_W32 = 0x6,
	HALMAC_PARAMETER_CMD_RF_W = 0x7,
	HALMAC_PARAMETER_CMD_BB_W8 = 0x8,
	HALMAC_PARAMETER_CMD_BB_W16 = 0x9,
	HALMAC_PARAMETER_CMD_BB_W32 = 0xA,
	HALMAC_PARAMETER_CMD_DELAY_US = 0x10,
	HALMAC_PARAMETER_CMD_DELAY_MS = 0x11,
	HALMAC_PARAMETER_CMD_END = 0xFF,
};

union rtw_phy_para_data {
	struct {
		uint32_t value;
		uint32_t msk;
		uint16_t offset;
		uint8_t msk_en;
		uint8_t size;
	} mac;
	struct {
		uint32_t value;
		uint32_t msk;
		uint16_t offset;
		uint8_t msk_en;
		uint8_t size;
	} bb;
	struct {
		uint32_t value;
		uint32_t msk;
		uint8_t offset;
		uint8_t msk_en;
		uint8_t path;
	} rf;
	struct {
		uint8_t unit;
		uint16_t value;
	} delay;
};

struct rtw_phy_parameter {
	uint8_t cmd;
	union rtw_phy_para_data data;
};

union halmac_parameter_content {
	struct {
		uint32_t value;
		uint32_t msk;
		uint16_t offset;
		uint8_t msk_en;
	} MAC_REG_W;
	struct {
		uint32_t value;
		uint32_t msk;
		uint16_t offset;
		uint8_t msk_en;
	} BB_REG_W;
	struct {
		uint32_t value;
		uint32_t msk;
		uint8_t offset;
		uint8_t msk_en;
		uint8_t rf_path;
	} RF_REG_W;
	struct {
		uint32_t rsvd1;
		uint32_t rsvd2;
		uint16_t delay_time;
		uint8_t rsvd3;
	} DELAY_TIME;
};

struct halmac_phy_parameter_info {
	enum halmac_parameter_cmd cmd_id;
	union halmac_parameter_content content;
};

int phy_parameter_drv2halmac(const struct rtw_phy_parameter *para,
			     struct halmac_phy_parameter_info *info);

#endif /* HAL_HALMAC_PHY_PARAMETER_H */
