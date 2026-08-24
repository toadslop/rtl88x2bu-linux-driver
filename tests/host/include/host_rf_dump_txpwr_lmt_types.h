/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Host L2 types for dump_txpwr_lmt formatter tests (W3-58).
 */
#ifndef HOST_RF_DUMP_TXPWR_LMT_TYPES_H
#define HOST_RF_DUMP_TXPWR_LMT_TYPES_H

#include "host_rf_types.h"

#include <stdio.h>
#include <string.h>

#define _TRUE 1
#define _FALSE 0

typedef unsigned long _irqL;

struct _mutex {
	int dummy;
};

typedef struct _mutex _mutex;

struct _list {
	struct _list *next;
	struct _list *prev;
};

typedef struct _list _list;

struct regd_exc_ent {
	_list list;
	char country[2];
	u8 domain;
	char regd_name[0];
};

#define MAX_2_4G_BANDWIDTH_NUM 2
#define MAX_5G_BANDWIDTH_NUM 4
#define MAX_TX_COUNT 4
#define TXPWR_LMT_RS_NUM 4
#define TXPWR_LMT_RS_NUM_2G 4
#define TXPWR_LMT_RS_NUM_5G 3
#define CENTER_CH_5G_ALL_NUM \
	(CENTER_CH_5G_20M_NUM + CENTER_CH_5G_40M_NUM + CENTER_CH_5G_80M_NUM)

#define TXPWR_LMT_RS_CCK 0
#define TXPWR_LMT_RS_OFDM 1
#define TXPWR_LMT_RS_HT 2
#define TXPWR_LMT_RS_VHT 3

#define TXPWR_LMT_HAS_CCK_1T	BIT0
#define TXPWR_LMT_HAS_CCK_2T	BIT1
#define TXPWR_LMT_HAS_CCK_3T	BIT2
#define TXPWR_LMT_HAS_CCK_4T	BIT3
#define TXPWR_LMT_HAS_OFDM_1T	BIT4
#define TXPWR_LMT_HAS_OFDM_2T	BIT5
#define TXPWR_LMT_HAS_OFDM_3T	BIT6
#define TXPWR_LMT_HAS_OFDM_4T	BIT7

#define TXPWR_LMT_REF_VHT_FROM_HT	BIT0
#define TXPWR_LMT_REF_HT_FROM_VHT	BIT1

#define RF_1TX 0
#define RF_PATH_A 0

#define CCK 0
#define OFDM 1
#define HT_1SS 2
#define VHT_1SS 6

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

struct rf_ctl_t {
	_mutex txpwr_lmt_mutex;
	_list reg_exc_list;
	u8 regd_exc_num;
	_list txpwr_lmt_list;
	u8 txpwr_regd_num;
	const char *regd_name;
	u8 txpwr_lmt_2g_cck_ofdm_state;
#if CONFIG_IEEE80211_BAND_5GHZ
	u8 txpwr_lmt_5g_cck_ofdm_state;
	u8 txpwr_lmt_5g_20_40_ref;
#endif
};

struct hal_spec_t {
	u8 txgi_max;
	u8 txgi_pdbm;
	u8 rfpath_num_2g;
	u8 rfpath_num_5g;
};

#define MAX_5G_BANDWIDTH_NUM 4

struct hal_data_type {
	u8 max_tx_cnt;
};

typedef struct hal_data_type HAL_DATA_TYPE;

struct _adapter {
	struct rf_ctl_t rf_ctl;
	struct hal_data_type hal_data;
	u8 band_cap;
	u8 jaguar;
};

typedef struct _adapter _adapter;

extern const char *const _regd_str[];
extern const char *const _txpwr_lmt_rs_str[];
extern u8 center_ch_5g_all[CENTER_CH_5G_ALL_NUM];

#define regd_str(regd) \
	(((regd) > TXPWR_LMT_WW) ? _regd_str[TXPWR_LMT_WW] : _regd_str[(regd)])
#define txpwr_lmt_rs_str(rs) \
	(((rs) >= TXPWR_LMT_RS_NUM) ? _txpwr_lmt_rs_str[TXPWR_LMT_RS_NUM] \
				    : _txpwr_lmt_rs_str[(rs)])
#define rf_path_char(path) (((path) >= RF_PATH_MAX) ? 'X' : 'A' + (path))

struct hal_spec_t *host_rf_hal_spec_ptr(void);
#define GET_HAL_SPEC(adapter) host_rf_hal_spec_ptr()

struct host_sel_capture {
	char buf[16384];
	size_t len;
};

extern struct host_sel_capture host_sel_out;

static inline void host_sel_reset(void)
{
	host_sel_out.len = 0;
	host_sel_out.buf[0] = '\0';
}

#undef RTW_PRINT_SEL
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

#undef _RTW_PRINT_SEL
#define _RTW_PRINT_SEL(sel, fmt, ...) RTW_PRINT_SEL(sel, fmt, ##__VA_ARGS__)

#define adapter_to_rfctl(adapter) (&(adapter)->rf_ctl)
#define GET_HAL_DATA(adapter) (&(adapter)->hal_data)
#define IS_HARDWARE_TYPE_JAGUAR_ALL(adapter) ((adapter)->jaguar)

#define RTW_ERR(...) do { } while (0)

static inline void _rtw_init_listhead(_list *list)
{
	list->next = list;
	list->prev = list;
}

static inline void rtw_list_insert_tail(_list *n, _list *head)
{
	_list *prev = head->prev;

	n->next = head;
	n->prev = prev;
	prev->next = n;
	head->prev = n;
}

static inline _list *get_next(_list *list)
{
	return list->next;
}

#define LIST_CONTAINOR(ptr, type, member) \
	((type *)((char *)(ptr) - (size_t)(&((type *)0)->member)))

static inline u32 rtw_end_of_queue_search(_list *queue, _list *pelement)
{
	return (queue == pelement) ? _TRUE : _FALSE;
}

static inline void _enter_critical_mutex(_mutex *pmutex, _irqL *pirqL)
{
	(void)pmutex;
	(void)pirqL;
}

static inline void _exit_critical_mutex(_mutex *pmutex, _irqL *pirqL)
{
	(void)pmutex;
	(void)pirqL;
}

static inline int _rtw_memcmp(const void *dst, const void *src, u32 sz)
{
	return memcmp(dst, src, sz) == 0 ? _TRUE : _FALSE;
}

void _dump_regd_exc_list(void *sel, struct rf_ctl_t *rfctl);
void rtw_txpwr_lmt_add(struct rf_ctl_t *rfctl, const char *regd_name, u8 band,
		       u8 bw, u8 tlrs, u8 ntx_idx, u8 ch_idx, s8 lmt);
struct txpwr_lmt_ent *_rtw_txpwr_lmt_get_by_name(struct rf_ctl_t *rfctl,
						const char *regd_name);

bool hal_is_band_support(struct _adapter *adapter, u8 band);
s8 phy_get_txpwr_lmt(struct _adapter *adapter, const char *regd_name, u8 band,
		     u8 bw, u8 tlrs, u8 ntx_idx, u8 ch, u8 lock);
s8 phy_get_txpwr_lmt_diff(struct _adapter *adapter, const char *regd_name,
			  u8 band, u8 bw, u8 rfpath, u8 rs, u8 tlrs,
			  u8 ntx_idx, u8 ch, u8 lock);
u8 phy_get_target_txpwr(struct _adapter *adapter, u8 band, u8 rfpath, u8 rs);

u8 center_chs_5g_num(u8 bw);
u8 center_chs_5g(u8 bw, u8 id);

void dump_txpwr_lmt(void *sel, struct _adapter *adapter);

void host_rf_dump_txpwr_lmt_reset(struct _adapter *adapter);
void host_rf_dump_txpwr_lmt_set_target_txpwr(u8 target);

void *rtw_zvmalloc(u32 sz);
void rtw_vmfree(u8 *p, u32 sz);
void *rtw_malloc(u32 sz);
void rtw_mfree(void *p, u32 sz);

#endif /* HOST_RF_DUMP_TXPWR_LMT_TYPES_H */
