/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Types for host L2 chplan_rest beacon-hint tests (W3-17).
 */
#ifndef HOST_CHPLAN_REST_TYPES_H
#define HOST_CHPLAN_REST_TYPES_H

#include "host_chplan_types.h"

struct host_country_chplan {
	u8 alpha2[2];
	u8 chplan;
	u8 en_11ac;
};

struct host_ndis_configuration {
	u32 length;
	u32 beacon_period;
	u32 atim_window;
	u32 ds_config;
	u32 fh_config[5];
};

typedef struct {
	u32 length;
	u8 mac_address[6];
	u8 reserved[2];
	u8 ssid[36];
	u8 mesh_id[36];
	u32 privacy;
	s32 rssi;
	struct host_ndis_configuration configuration;
} host_wlan_bssid_ex;

struct host_rf_ctl {
	u8 regd_src;
	const struct host_country_chplan *country_ent;
	u8 channel_plan;
	RT_CHANNEL_INFO channel_set[MAX_CHANNEL_NUM];
};

typedef struct {
	struct registry_priv registrypriv;
	struct host_rf_ctl rf_ctl;
} host_chplan_adapter;

#define HOST_IS_ALPHA2_WORLDWIDE(_alpha2) \
	((_alpha2)[0] == '0' && (_alpha2)[1] == '0')

u8 host_rest_process_beacon_hint(host_chplan_adapter *adapter,
				 host_wlan_bssid_ex *bss);

#endif /* HOST_CHPLAN_REST_TYPES_H */
