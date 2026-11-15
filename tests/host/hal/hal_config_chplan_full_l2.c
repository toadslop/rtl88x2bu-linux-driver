// SPDX-License-Identifier: GPL-2.0
/* W4-91 full hal_com_config_channel_plan host L2 oracle + runner. */

#include "../include/host_chplan_types.h"
#include "host_vector_json.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define RTW_CHPLAN_UNSPECIFIED 0xffu
#define HAL_CFG_CHPLAN_DEF 0x7fu
#define EEPROM_CHANNEL_PLAN_BY_HW_MASK 0x80

struct country_chplan {
	char alpha2[2];
	uint8_t chplan;
	uint8_t en_11ac;
};

struct host_config_chplan_adapter {
	struct {
		uint8_t channel_plan;
		const struct country_chplan *country_ent;
	} rfctl;
	struct { uint8_t b_disable_sw_channel_plan; } hal_data;
};

extern const struct country_chplan *rtw_get_chplan_from_country(const char *cc);
extern uint8_t rtw_is_channel_plan_valid(uint8_t id);

static int is_alpha2_no_specified(const char *alpha2)
{
	uint16_t v;

	if (!alpha2)
		return 0;
	memcpy(&v, alpha2, sizeof(v));
	return v == 0xffff;
}

static void host_hal_com_config_channel_plan(
	struct host_config_chplan_adapter *padapter,
	const char *hw_alpha2, uint8_t hw_chplan,
	const char *sw_alpha2, uint8_t sw_chplan, uint8_t autoload_fail)
{
	uint8_t force_hw = 0;
	int chplan = -1;
	const struct country_chplan *country_ent = NULL, *ent;

	if (!padapter)
		return;
	memset(padapter, 0, sizeof(*padapter));
	if (hw_chplan != 0xff) {
		if (autoload_fail)
			goto chk_sw;
		if (hw_chplan & EEPROM_CHANNEL_PLAN_BY_HW_MASK)
			force_hw = 1;
		hw_chplan &= (uint8_t)~EEPROM_CHANNEL_PLAN_BY_HW_MASK;
	}
	if (hw_alpha2 && !is_alpha2_no_specified(hw_alpha2)) {
		ent = rtw_get_chplan_from_country(hw_alpha2);
		if (ent) {
			country_ent = ent;
			chplan = ent->chplan;
			goto chk_sw;
		}
	}
	if (rtw_is_channel_plan_valid(hw_chplan))
		chplan = hw_chplan;
	else if (force_hw)
		force_hw = 0;
chk_sw:
	if (force_hw)
		goto done;
	if (sw_alpha2 && !is_alpha2_no_specified(sw_alpha2)) {
		ent = rtw_get_chplan_from_country(sw_alpha2);
		if (ent) {
			country_ent = ent;
			chplan = ent->chplan;
			goto done;
		}
	}
	if (rtw_is_channel_plan_valid(sw_chplan)) {
		country_ent = NULL;
		chplan = sw_chplan;
	}
done:
	if (chplan < 0)
		chplan = HAL_CFG_CHPLAN_DEF;
	padapter->rfctl.country_ent = country_ent;
	padapter->rfctl.channel_plan = (uint8_t)chplan;
	padapter->hal_data.b_disable_sw_channel_plan = force_hw;
}

#ifdef RUST_HAL_COM_CONFIG_CHPLAN_FULL_ORACLE
extern void host_hal_com_config_channel_plan(
	struct host_config_chplan_adapter *, const char *, uint8_t,
	const char *, uint8_t, uint8_t);
#endif

struct vector {
	char name[48];
	char hw_alpha2[3], sw_alpha2[3];
	uint8_t hw_chplan, sw_chplan, autoload_fail;
	uint8_t exp_plan, exp_force, exp_has_ctry, exp_ctry_plan;
};

#ifndef RUST_HAL_COM_CONFIG_CHPLAN_FULL_ORACLE
static
#endif
int parse_vec(const char *obj, size_t len, void *v)
{
	struct vector *x = v;
	int t;
	char a[8];

	memset(x, 0, sizeof(*x));
	if (host_json_parse_string_in(obj, len, "name", x->name, sizeof(x->name)))
		return -1;
	if (!host_json_parse_string_in(obj, len, "hw_alpha2", a, sizeof(a)) && a[0]) {
		x->hw_alpha2[0] = a[0];
		x->hw_alpha2[1] = a[1];
	}
	if (!host_json_parse_string_in(obj, len, "sw_alpha2", a, sizeof(a)) && a[0]) {
		x->sw_alpha2[0] = a[0];
		x->sw_alpha2[1] = a[1];
	}
	if (host_json_parse_int_in(obj, len, "hw_chplan", &t))
		x->hw_chplan = 0xff;
	else
		x->hw_chplan = (uint8_t)t;
	if (host_json_parse_int_in(obj, len, "sw_chplan", &t))
		x->sw_chplan = 0xff;
	else
		x->sw_chplan = (uint8_t)t;
	host_json_parse_int_in(obj, len, "autoload_fail", &t);
	x->autoload_fail = (uint8_t)t;
	host_json_parse_int_in(obj, len, "expect_channel_plan", &t);
	x->exp_plan = (uint8_t)t;
	host_json_parse_int_in(obj, len, "expect_disable_sw", &t);
	x->exp_force = (uint8_t)t;
	host_json_parse_int_in(obj, len, "expect_has_country", &t);
	x->exp_has_ctry = (uint8_t)t;
	host_json_parse_int_in(obj, len, "expect_country_chplan", &t);
	x->exp_ctry_plan = (uint8_t)t;
	return 0;
}

#ifndef RUST_HAL_COM_CONFIG_CHPLAN_FULL_ORACLE
int main(int argc, char **argv)
{
	struct vector vecs[12];
	size_t n = 0;
	const char *path = argc > 1 ? argv[1] :
		"hal_config_chplan_full_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 12, parse_vec, &n))
		return 1;
	for (size_t i = 0; i < n; i++) {
		struct host_config_chplan_adapter a;

		host_hal_com_config_channel_plan(
			&a, vecs[i].hw_alpha2[0] ? vecs[i].hw_alpha2 : NULL,
			vecs[i].hw_chplan,
			vecs[i].sw_alpha2[0] ? vecs[i].sw_alpha2 : NULL,
			vecs[i].sw_chplan, vecs[i].autoload_fail);
		if (a.rfctl.channel_plan != vecs[i].exp_plan ||
		    a.hal_data.b_disable_sw_channel_plan != vecs[i].exp_force)
			return 1;
		if (vecs[i].exp_has_ctry) {
			if (!a.rfctl.country_ent ||
			    a.rfctl.country_ent->chplan != vecs[i].exp_ctry_plan)
				return 1;
		} else if (a.rfctl.country_ent)
			return 1;
	}
	printf("PASS: %zu vectors from %s\n", n, path);
	return 0;
}
#endif