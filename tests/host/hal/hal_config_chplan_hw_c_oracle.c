// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-90 hal_com_config_channel_plan HW branch (hal/hal_com_rest.c). */

#include "../include/host_hal_config_chplan_hw_types.h"
#include "../include/host_chplan_types.h"

#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef int32_t sint;

#define EEPROM_CHANNEL_PLAN_BY_HW_MASK 0x80

struct country_chplan {
	char alpha2[2];
	u8 chplan;
	u8 en_11ac;
};

#define IS_ALPHA2_NO_SPECIFIED(_alpha2) \
	((*((uint16_t *)(_alpha2))) == 0xffff)

extern const struct country_chplan *rtw_get_chplan_from_country(const char *country_code);
extern u8 rtw_is_channel_plan_valid(u8 id);

void hal_com_config_chplan_hw_branch(
	const char *hw_alpha2,
	u8 hw_chplan,
	u8 force_hw_chplan_in,
	struct hal_config_chplan_hw_out *out)
{
	u8 force_hw_chplan = force_hw_chplan_in;
	sint chplan = HAL_CFG_CHPLAN_CHPLAN_UNSET;
	const struct country_chplan *country_ent = NULL;
	const struct country_chplan *ent;

	if (!out)
		return;

	out->chplan = HAL_CFG_CHPLAN_CHPLAN_UNSET;
	out->country_chplan = HAL_CFG_CHPLAN_NO_COUNTRY;
	out->force_hw_chplan = force_hw_chplan;
	out->goto_sw_config = 0;

	if (hw_alpha2 && !IS_ALPHA2_NO_SPECIFIED(hw_alpha2)) {
		ent = rtw_get_chplan_from_country(hw_alpha2);
		if (ent) {
			country_ent = ent;
			chplan = ent->chplan;
			out->chplan = chplan;
			out->country_chplan = ent->chplan;
			out->goto_sw_config = 1;
			return;
		}
	}

	if (rtw_is_channel_plan_valid(hw_chplan))
		chplan = hw_chplan;
	else if (force_hw_chplan)
		force_hw_chplan = 0;

	out->chplan = chplan;
	out->force_hw_chplan = force_hw_chplan;
	if (country_ent)
		out->country_chplan = country_ent->chplan;
}