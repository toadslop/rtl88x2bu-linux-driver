// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-90 hal_com_config_channel_plan HW branch (hal/hal_com.c). */

#include "../include/host_hal_config_chplan_hw_types.h"
#include "../include/host_chplan_types.h"

#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t uint;
typedef int32_t sint;

#define _TRUE 1
#define _FALSE 0

#define EEPROM_CHANNEL_PLAN_BY_HW_MASK 0x80

struct country_chplan {
	char alpha2[2];
	u8 chplan;
	u8 en_11ac;
};

static int is_alpha2_no_specified(const char *alpha2)
{
	uint16_t v;

	if (!alpha2)
		return 0;
	memcpy(&v, alpha2, sizeof(v));
	return v == 0xffff;
}

extern const struct country_chplan *rtw_get_chplan_from_country(const char *country_code);
extern u8 rtw_is_channel_plan_valid(u8 id);

/*
 * Mirrors hal_com_config_channel_plan from chk_hw_country_code through the HW
 * path immediately before chk_sw_config SW handling (W4-91).
 */
void hal_com_config_chplan_hw_branch(
	const char *hw_alpha2,
	u8 hw_chplan_raw,
	u8 autoload_fail,
	struct hal_config_chplan_hw_out *out)
{
	u8 force_hw_chplan = _FALSE;
	u8 hw_chplan = hw_chplan_raw;
	sint chplan = HAL_CFG_CHPLAN_CHPLAN_UNSET;
	const struct country_chplan *country_ent = NULL;
	const struct country_chplan *ent;

	if (!out)
		return;

	out->chplan = HAL_CFG_CHPLAN_CHPLAN_UNSET;
	out->country_chplan = HAL_CFG_CHPLAN_NO_COUNTRY;
	out->force_hw_chplan = _FALSE;
	out->early_goto_sw_config = 0;

	if (hw_chplan == 0xFF)
		goto chk_hw_country_code;

	if (autoload_fail) {
		out->early_goto_sw_config = 1;
		goto chk_sw_config;
	}

	if (hw_chplan & EEPROM_CHANNEL_PLAN_BY_HW_MASK)
		force_hw_chplan = _TRUE;

	hw_chplan &= (~EEPROM_CHANNEL_PLAN_BY_HW_MASK);

chk_hw_country_code:
	if (hw_alpha2 && !is_alpha2_no_specified(hw_alpha2)) {
		ent = rtw_get_chplan_from_country(hw_alpha2);
		if (ent) {
			country_ent = ent;
			chplan = ent->chplan;
			out->early_goto_sw_config = 1;
			goto chk_sw_config;
		}
	}

	if (rtw_is_channel_plan_valid(hw_chplan))
		chplan = hw_chplan;
	else if (force_hw_chplan == _TRUE)
		force_hw_chplan = _FALSE;

chk_sw_config:
	out->chplan = chplan;
	out->force_hw_chplan = force_hw_chplan;
	if (country_ent)
		out->country_chplan = country_ent->chplan;
}