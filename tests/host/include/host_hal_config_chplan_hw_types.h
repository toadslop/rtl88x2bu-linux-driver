/* SPDX-License-Identifier: GPL-2.0 */
/* Host types for hal_com_config_channel_plan HW branch (W4-90). */
#ifndef HOST_HAL_CONFIG_CHPLAN_HW_TYPES_H
#define HOST_HAL_CONFIG_CHPLAN_HW_TYPES_H

#include <stdint.h>

#define HAL_CFG_CHPLAN_CHPLAN_UNSET (-1)
#define HAL_CFG_CHPLAN_NO_COUNTRY 0xffu

struct hal_config_chplan_hw_out {
	int chplan;
	uint8_t country_chplan;
	uint8_t force_hw_chplan;
	uint8_t goto_sw_config;
};

struct country_chplan;

void hal_com_config_chplan_hw_branch(
	const char *hw_alpha2,
	uint8_t hw_chplan,
	uint8_t force_hw_chplan_in,
	struct hal_config_chplan_hw_out *out);

#endif /* HOST_HAL_CONFIG_CHPLAN_HW_TYPES_H */