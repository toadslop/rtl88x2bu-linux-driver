// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_odm_iface_types.h"
#include "host_vector_json.h"

struct host_hal_dm_odm_iface_trace host_hal_dm_odm_iface_trace;

void host_hal_dm_odm_iface_reset(void)
{
	memset(&host_hal_dm_odm_iface_trace, 0, sizeof(host_hal_dm_odm_iface_trace));
}

void odm_cmn_info_init(struct dm_struct *dm, u32 cmn_info, u64 value)
{
	(void)dm;
	switch (cmn_info) {
	case ODM_CMNINFO_PLATFORM:
		host_hal_dm_odm_iface_trace.platform = value;
		break;
	case ODM_CMNINFO_INTERFACE:
		host_hal_dm_odm_iface_trace.interface = value;
		break;
	case ODM_CMNINFO_MP_TEST_CHIP:
		host_hal_dm_odm_iface_trace.mp_test_chip = value;
		break;
	case ODM_CMNINFO_PATCH_ID:
		host_hal_dm_odm_iface_trace.patch_id = value;
		break;
	case ODM_CMNINFO_BWIFI_TEST:
		host_hal_dm_odm_iface_trace.wifi_test = value;
		break;
	case ODM_CMNINFO_ADVANCE_OTA:
		host_hal_dm_odm_iface_trace.advance_ota = value;
		break;
	case ODM_CMNINFO_RF_TYPE:
		host_hal_dm_odm_iface_trace.rf_type = value;
		break;
	default:
		break;
	}
}

struct vector {
	char name[48];
	int op;
	int intf_type;
	int version_id;
	int customer_id;
	int wifi_spec;
	int adv_ota;
	int rf_type;
	int expect_platform;
	int expect_interface;
	int expect_mp;
	int expect_patch;
	int expect_wifi;
	int expect_adv_ota;
	int expect_rf;
	int expect_op_mode;
	int expect_ic_init;
	int expect_regulation;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {
		"op", "intf_type", "version_id", "customer_id", "wifi_spec", "adv_ota",
		"rf_type", "expect_platform", "expect_interface", "expect_mp", "expect_patch",
		"expect_wifi", "expect_adv_ota", "expect_rf", "expect_op_mode", "expect_ic_init",
		"expect_regulation"};
	int *dst[] = {
		&v->op, &v->intf_type, &v->version_id, &v->customer_id, &v->wifi_spec,
		&v->adv_ota, &v->rf_type, &v->expect_platform, &v->expect_interface,
		&v->expect_mp, &v->expect_patch, &v->expect_wifi, &v->expect_adv_ota,
		&v->expect_rf, &v->expect_op_mode, &v->expect_ic_init, &v->expect_regulation};

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
		(void)host_json_parse_int_in(obj, len, keys[i], dst[i]);
	return 0;
}

static void fill_adapter(struct vector *v, _adapter *adapter)
{
	memset(adapter, 0, sizeof(*adapter));
	adapter->host_intf_type = (u8)v->intf_type;
	GET_HAL_DATA(adapter)->version_id = (u32)v->version_id;
	GET_HAL_DATA(adapter)->CustomerID = (u16)v->customer_id;
	adapter->registrypriv.wifi_spec = (u8)v->wifi_spec;
	adapter->registrypriv.adv_ota = (u8)v->adv_ota;
	GET_HAL_DATA(adapter)->rf_type = (u8)v->rf_type;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;

	host_hal_dm_odm_iface_reset();
	fill_adapter(v, &adapter);
	if (v->op == 1) {
		rtw_phydm_priv_init(&adapter);
		if (host_hal_dm_odm_iface_trace.platform != (u64)v->expect_platform)
			return -1;
		if (adapter_to_phydm(&adapter)->adapter != &adapter)
			return -1;
	} else if (v->op == 2) {
		rtw_rust_hal_dm_init_odm_cominfo_head(&adapter);
		if (GET_HAL_DATA(&adapter)->phydm_op_mode != (u8)v->expect_op_mode ||
		    host_hal_dm_odm_iface_trace.ic_type_init != (u8)v->expect_ic_init ||
		    host_hal_dm_odm_iface_trace.interface != (u64)v->expect_interface ||
		    host_hal_dm_odm_iface_trace.mp_test_chip != (u64)v->expect_mp ||
		    host_hal_dm_odm_iface_trace.patch_id != (u64)v->expect_patch ||
		    host_hal_dm_odm_iface_trace.wifi_test != (u64)v->expect_wifi ||
		    host_hal_dm_odm_iface_trace.advance_ota != (u64)v->expect_adv_ota ||
		    host_hal_dm_odm_iface_trace.rf_type != (u64)v->expect_rf)
			return -1;
	} else if (v->op == 3) {
		rtw_rust_hal_dm_init_odm_cominfo_regulation_dfs(&adapter);
		if (host_hal_dm_odm_iface_trace.regulation_set != (u8)v->expect_regulation)
			return -1;
	} else {
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[16];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_dm_odm_iface_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 16, parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	printf("OK %zu vectors\n", count);
	return 0;
}