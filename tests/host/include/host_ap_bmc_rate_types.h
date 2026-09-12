/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_BMC_RATE_TYPES_H
#define HOST_AP_BMC_RATE_TYPES_H

#include <stddef.h>

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long _irqL;
typedef int _lock;

#define BAND_ON_5G 1
#define _TRUE 1
#define _FALSE 0
#define HOST_BMC_MAX_STA 4

struct _list {
	struct _list *next;
	struct _list *prev;
};

struct host_ra_sta_info {
	u8 curr_tx_rate;
};

struct host_cmn_sta_info {
	struct host_ra_sta_info ra_info;
};

struct sta_info {
	struct host_cmn_sta_info cmn;
	struct _list asoc_list;
};

struct sta_priv {
	struct _list asoc_list;
	_lock asoc_list_lock;
};

typedef struct {
	u8 current_band_type;
} HAL_DATA_TYPE, *PHAL_DATA_TYPE;

struct _adapter {
	HAL_DATA_TYPE hal_data;
	struct sta_priv stapriv;
};

#define GET_HAL_DATA(a) (&((a)->hal_data))

#define LIST_CONTAINOR(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))

static inline void _enter_critical_bh(_lock *plock, _irqL *pirqL)
{
	(void)plock;
	(void)pirqL;
}

static inline void _exit_critical_bh(_lock *plock, _irqL *pirqL)
{
	(void)plock;
	(void)pirqL;
}

static inline struct _list *get_next(struct _list *list)
{
	return list->next;
}

static inline u32 rtw_end_of_queue_search(struct _list *queue, struct _list *pelement)
{
	return (queue == pelement) ? _TRUE : _FALSE;
}

static inline void _rtw_init_listhead(struct _list *list)
{
	list->next = list;
	list->prev = list;
}

static inline void rtw_list_insert_tail(struct _list *n, struct _list *head)
{
	struct _list *prev = head->prev;

	n->next = head;
	n->prev = prev;
	prev->next = n;
	head->prev = n;
}

u8 rtw_ap_find_bmc_rate(struct _adapter *adapter, u8 tx_rate);
u8 rtw_ap_find_mini_tx_rate(struct _adapter *adapter);

#endif /* HOST_AP_BMC_RATE_TYPES_H */
