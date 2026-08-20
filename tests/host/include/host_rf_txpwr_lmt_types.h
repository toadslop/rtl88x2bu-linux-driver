/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Host L2 types for txpwr_lmt list CRUD tests (W3-52).
 */
#ifndef HOST_RF_TXPWR_LMT_TYPES_H
#define HOST_RF_TXPWR_LMT_TYPES_H

#include "host_rf_regd_exc_types.h"
#include "host_rf_types.h"

#include <string.h>

#define MAX_2_4G_BANDWIDTH_NUM 2
#define MAX_5G_BANDWIDTH_NUM 4
#define MAX_TX_COUNT 4
#define TXPWR_LMT_RS_NUM 4
#define TXPWR_LMT_RS_NUM_2G 4
#define TXPWR_LMT_RS_NUM_5G 3
#define CENTER_CH_5G_ALL_NUM \
	(CENTER_CH_5G_20M_NUM + CENTER_CH_5G_40M_NUM + CENTER_CH_5G_80M_NUM)

#define RTW_PRINT(fmt, ...) ((void)0)

#define rtw_min(a, b) (((a) > (b)) ? (b) : (a))

typedef enum _REGULATION_TXPWR_LMT {
	TXPWR_LMT_NONE = 0,
	TXPWR_LMT_FCC = 1,
	TXPWR_LMT_MKK = 2,
	TXPWR_LMT_ETSI = 3,
	TXPWR_LMT_IC = 4,
	TXPWR_LMT_KCC = 5,
	TXPWR_LMT_NCC = 6,
	TXPWR_LMT_ACMA = 7,
	TXPWR_LMT_CHILE = 8,
	TXPWR_LMT_UKRAINE = 9,
	TXPWR_LMT_MEXICO = 10,
	TXPWR_LMT_CN = 11,
	TXPWR_LMT_WW,
} REGULATION_TXPWR_LMT;

struct hal_spec_t {
	u8 txgi_max;
};

struct txpwr_lmt_ent {
	_list list;
	s8 lmt_2g[MAX_2_4G_BANDWIDTH_NUM][TXPWR_LMT_RS_NUM_2G][CENTER_CH_2G_NUM]
		[MAX_TX_COUNT];
#if CONFIG_IEEE80211_BAND_5GHZ
	s8 lmt_5g[MAX_5G_BANDWIDTH_NUM][TXPWR_LMT_RS_NUM_5G][CENTER_CH_5G_ALL_NUM]
		[MAX_TX_COUNT];
#endif
	char regd_name[0];
};

extern const char *const _regd_str[];
extern const char *const _txpwr_lmt_rs_str[];
extern u8 center_ch_5g_all[CENTER_CH_5G_ALL_NUM];

#define regd_str(regd) \
	(((regd) > TXPWR_LMT_WW) ? _regd_str[TXPWR_LMT_WW] : _regd_str[(regd)])
#define txpwr_lmt_rs_str(rs) \
	(((rs) >= TXPWR_LMT_RS_NUM) ? _txpwr_lmt_rs_str[TXPWR_LMT_RS_NUM] \
				    : _txpwr_lmt_rs_str[(rs)])

struct hal_spec_t *host_rf_hal_spec_ptr(void);

struct dvobj_priv;
struct _adapter;

#define rfctl_to_dvobj(rfctl) ((void)(rfctl), (struct dvobj_priv *)1)
#define dvobj_get_primary_adapter(dvobj) ((void)(dvobj), (struct _adapter *)1)
#define GET_HAL_SPEC(adapter) host_rf_hal_spec_ptr()

void rtw_txpwr_lmt_add_with_nlen(struct rf_ctl_t *rfctl, const char *regd_name,
				 u32 nlen, u8 band, u8 bw, u8 tlrs, u8 ntx_idx,
				 u8 ch_idx, s8 lmt);
void rtw_txpwr_lmt_add(struct rf_ctl_t *rfctl, const char *regd_name, u8 band,
		       u8 bw, u8 tlrs, u8 ntx_idx, u8 ch_idx, s8 lmt);
struct txpwr_lmt_ent *_rtw_txpwr_lmt_get_by_name(struct rf_ctl_t *rfctl,
						const char *regd_name);
struct txpwr_lmt_ent *rtw_txpwr_lmt_get_by_name(struct rf_ctl_t *rfctl,
						const char *regd_name);
void rtw_txpwr_lmt_list_free(struct rf_ctl_t *rfctl);

void *rtw_zvmalloc(u32 sz);
void rtw_vmfree(u8 *p, u32 sz);

void host_rf_txpwr_lmt_reset(struct rf_ctl_t *rfctl);

#endif /* HOST_RF_TXPWR_LMT_TYPES_H */
