// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_ODM_ADAPTIVITY_TYPES_H
#define HOST_ODM_ADAPTIVITY_TYPES_H

#include "host_types.h"

#include <stdio.h>

typedef int bool;
#define _TRUE 1
#define _FALSE 0

#define ADAPTIVITY_VERSION "9.7.07"

#define HDATA_RATE(rate) ((rate) == 12 ? "MCS0" : "UNKNOWN")

struct registry_priv {
	u8 adaptivity_en;
	u8 adaptivity_mode;
};

struct dm_struct {
	s8 th_l2h_ini;
	s8 th_edcca_hl_diff;
	u8 rx_rate;
	u8 rssi_a;
	u8 rssi_b;
};

typedef struct {
	struct dm_struct odmpriv;
} HAL_DATA_TYPE;

typedef struct {
	struct registry_priv registrypriv;
	HAL_DATA_TYPE HalData;
} _adapter;

struct host_sel_capture {
	char buf[4096];
	size_t len;
};

extern struct host_sel_capture host_sel_out;

static inline void host_sel_reset(void)
{
	host_sel_out.len = 0;
	host_sel_out.buf[0] = '\0';
}

#define RTW_PRINT_SEL(sel, fmt, ...) \
	do { \
		size_t _rem = sizeof(host_sel_out.buf) - host_sel_out.len; \
		int _n = snprintf(host_sel_out.buf + host_sel_out.len, _rem, \
				  fmt, ##__VA_ARGS__); \
		if (_n > 0) { \
			if ((size_t)_n >= _rem) \
				host_sel_out.len = sizeof(host_sel_out.buf) - 1; \
			else \
				host_sel_out.len += (size_t)_n; \
			host_sel_out.buf[host_sel_out.len] = '\0'; \
		} \
	} while (0)

#define _RTW_PRINT_SEL(sel, fmt, ...) RTW_PRINT_SEL(sel, fmt, ##__VA_ARGS__)

#define GET_HAL_DATA(a) (&((a)->HalData))
#define adapter_to_phydm(a) (&GET_HAL_DATA(a)->odmpriv)

void host_odm_adaptivity_reset(_adapter *adapter);
void rtw_odm_adaptivity_ver_msg(void *sel, _adapter *adapter);
void rtw_odm_adaptivity_en_msg(void *sel, _adapter *adapter);
void rtw_odm_adaptivity_mode_msg(void *sel, _adapter *adapter);
void rtw_odm_adaptivity_config_msg(void *sel, _adapter *adapter);
bool rtw_odm_adaptivity_needed(_adapter *adapter);
void rtw_odm_adaptivity_parm_msg(void *sel, _adapter *adapter);
void rtw_odm_adaptivity_parm_set(_adapter *adapter, s8 th_l2h_ini, s8 th_edcca_hl_diff);
void rtw_odm_get_perpkt_rssi(void *sel, _adapter *adapter);

#endif /* HOST_ODM_ADAPTIVITY_TYPES_H */
