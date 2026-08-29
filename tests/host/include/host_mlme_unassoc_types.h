/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MLME_UNASSOC_TYPES_H
#define HOST_MLME_UNASSOC_TYPES_H

#include <stddef.h>
#include <stdbool.h>
#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define ETH_ALEN 6
#define UNASOC_STA_SRC_NUM 2
#define UNASOC_STA_DEL_CHK_SKIP 0
#define UNASOC_STA_DEL_CHK_ALIVE 1
#define UNASOC_STA_DEL_CHK_DELETED 2
#define UNASOC_STA_MODE_INTERESTED 1
#define UNASOC_STA_MODE_ALL 2
#define UNASSOC_STA_LIFETIME_MS 60000

typedef unsigned long systime, _irqL;
typedef int _lock;
struct _list { struct _list *next, *prev; };
typedef struct _list _list;
struct __queue { _list queue; _lock lock; };
typedef struct __queue _queue;
struct unassoc_sta_info {
	_list list; u8 addr[ETH_ALEN]; u8 interested; s8 recv_signal_power; systime time;
};
struct mlme_priv {
	u8 unassoc_sta_mode_of_stype[UNASOC_STA_SRC_NUM];
	_queue unassoc_sta_queue, free_unassoc_sta_queue;
	u8 *free_unassoc_sta_buf; u32 interested_unassoc_sta_cnt, max_unassoc_sta_cnt;
};
struct _adapter { struct mlme_priv mlmepriv; };
typedef struct _adapter _adapter;

#define GET_PRIMARY_ADAPTER(a) (a)
#define mlme_to_adapter(m) \
	((struct _adapter *)((char *)(m) - offsetof(struct _adapter, mlmepriv)))
#define LIST_CONTAINOR(p, t, m) ((t *)((char *)(p) - offsetof(t, m)))

static inline void _enter_critical_bh(_lock *l, _irqL *i) { (void)l; (void)i; }
static inline void _exit_critical_bh(_lock *l, _irqL *i) { (void)l; (void)i; }
static inline _list *get_list_head(_queue *q) { return &q->queue; }
static inline _list *get_next(_list *l) { return l->next; }
static inline int rtw_end_of_queue_search(_list *h, _list *l) { return h == l; }
static inline void rtw_list_insert_tail(_list *n, _list *h)
{
	n->next = h; n->prev = h->prev; h->prev->next = n; h->prev = n;
}
static inline void rtw_list_delete(_list *e)
{
	e->next->prev = e->prev; e->prev->next = e->next; e->next = e->prev = e;
}

static inline int _rtw_queue_empty(_queue *q) { return q->queue.next == &q->queue; }

systime rtw_get_current_time(void);
systime rtw_ms_to_systime(int ms);
bool rtw_time_before(systime a, systime b);
bool rtw_time_after(systime a, systime b);

int _rtw_memcmp(const void *a, const void *b, size_t n);
void rtw_run_in_thread_cmd(_adapter *a, void *f, _adapter *c);
void rtw_hal_rcr_set_chk_bssid_act_non(_adapter *a);

#endif
