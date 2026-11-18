// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_cfg_phy_para_types.h"
#include "host_vector_json.h"

struct vector {
	char name[48];
	int config_type;
	unsigned long offset;
	unsigned long data;
	unsigned long mask;
	int rf_path;
	unsigned long delay_time;
	int halmac_stub_return;
	int expect_status;
	int expect_calls;
	int expect_cmd;
	int expect_mac_size;
	int expect_bb_size;
	int expect_rf_path;
	int expect_delay_unit;
	int expect_delay_value;
	int expect_msk_en;
	unsigned long expect_offset;
	unsigned long expect_data;
	unsigned long expect_mask;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	unsigned long ul;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "config_type", &ul))
		return -1;
	v->config_type = (int)ul;
	if (host_json_parse_ulong_in(obj, len, "offset", &v->offset))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "data", &v->data))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "mask", &v->mask))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "rf_path", &ul))
		return -1;
	v->rf_path = (int)ul;
	if (host_json_parse_ulong_in(obj, len, "delay_time", &v->delay_time))
		return -1;
	if (host_json_parse_int_in(obj, len, "halmac_stub_return", &v->halmac_stub_return))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_status", &v->expect_status))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_calls", &v->expect_calls))
		return -1;
	host_json_parse_int_in(obj, len, "expect_cmd", &v->expect_cmd);
	host_json_parse_int_in(obj, len, "expect_mac_size", &v->expect_mac_size);
	host_json_parse_int_in(obj, len, "expect_bb_size", &v->expect_bb_size);
	host_json_parse_int_in(obj, len, "expect_rf_path", &v->expect_rf_path);
	host_json_parse_int_in(obj, len, "expect_delay_unit", &v->expect_delay_unit);
	host_json_parse_int_in(obj, len, "expect_delay_value", &v->expect_delay_value);
	host_json_parse_int_in(obj, len, "expect_msk_en", &v->expect_msk_en);
	v->expect_offset = v->offset;
	v->expect_data = v->data;
	v->expect_mask = v->mask;
	return 0;
}

static int verify_mac(const struct rtw_phy_parameter *p, struct vector *v)
{
	if (p->cmd != (uint8_t)v->expect_cmd)
		return -1;
	if (p->data.mac.offset != (uint16_t)v->expect_offset)
		return -1;
	if (p->data.mac.value != (uint32_t)v->expect_data)
		return -1;
	if (p->data.mac.msk != (uint32_t)v->expect_mask)
		return -1;
	if (p->data.mac.msk_en != (uint8_t)v->expect_msk_en)
		return -1;
	if (v->expect_mac_size && p->data.mac.size != (uint8_t)v->expect_mac_size)
		return -1;
	return 0;
}

static int verify_bb(const struct rtw_phy_parameter *p, struct vector *v)
{
	if (p->cmd != (uint8_t)v->expect_cmd)
		return -1;
	if (p->data.bb.offset != (uint16_t)v->expect_offset)
		return -1;
	if (p->data.bb.value != (uint32_t)v->expect_data)
		return -1;
	if (p->data.bb.msk != (uint32_t)v->expect_mask)
		return -1;
	if (p->data.bb.msk_en != (uint8_t)v->expect_msk_en)
		return -1;
	if (v->expect_bb_size && p->data.bb.size != (uint8_t)v->expect_bb_size)
		return -1;
	return 0;
}

static int verify_rf(const struct rtw_phy_parameter *p, struct vector *v)
{
	if (p->cmd != (uint8_t)v->expect_cmd)
		return -1;
	if (p->data.rf.offset != (uint8_t)v->expect_offset)
		return -1;
	if (p->data.rf.value != (uint32_t)v->expect_data)
		return -1;
	if (p->data.rf.path != (uint8_t)v->expect_rf_path)
		return -1;
	return 0;
}

static int verify_delay(const struct rtw_phy_parameter *p, struct vector *v)
{
	if (p->cmd != (uint8_t)v->expect_cmd)
		return -1;
	if (p->data.delay.unit != (uint8_t)v->expect_delay_unit)
		return -1;
	if (p->data.delay.value != (uint16_t)v->expect_delay_value)
		return -1;
	return 0;
}

static int verify_para(const struct rtw_phy_parameter *p, struct vector *v)
{
	if (v->expect_cmd == 0)
		return verify_mac(p, v);
	if (v->expect_cmd == 1)
		return verify_bb(p, v);
	if (v->expect_cmd == 2)
		return verify_rf(p, v);
	if (v->expect_cmd == 3)
		return verify_delay(p, v);
	if (v->expect_cmd == 255 && p->cmd == 0xFF)
		return 0;
	return -1;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	struct dm_struct phydm;
	struct host_hal_dm_cfg_phy_para_trace *t = &host_hal_dm_cfg_phy_para_trace;
	enum hal_status ret;

	host_hal_dm_cfg_phy_para_reset();
	memset(&adapter, 0, sizeof(adapter));
	memset(&phydm, 0, sizeof(phydm));
	phydm.adapter = &adapter;
	t->stub_return = (int8_t)v->halmac_stub_return;

	ret = rtw_phydm_cfg_phy_para(&phydm, (enum phydm_halmac_param)v->config_type,
				     (u32)v->offset, (u32)v->data, (u32)v->mask,
				     (enum rf_path)v->rf_path, (u32)v->delay_time);
	if ((int)ret != v->expect_status)
		return -1;
	if (t->cfg_calls != (uint8_t)v->expect_calls)
		return -1;
	if (v->expect_calls && verify_para(&t->last_para, v))
		return -1;
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[12];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_dm_cfg_phy_para_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 12,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}