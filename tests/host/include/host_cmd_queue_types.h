/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_CMD_QUEUE_TYPES_H
#define HOST_CMD_QUEUE_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0
#define CONFIG_EVENT_THREAD_MODE 1
#define MAX_CMDSZ 1536
#define CMD_SET_DRV_EXTRA 12
#define CMD_SET_CHANPLAN 13

typedef int sint, ATOMIC_T, _sema, _mutex, _lock;
typedef unsigned long _irqL;

struct _list { struct _list *next, *prev; };
typedef struct _list _list;
struct __queue { _list queue; _lock lock; };
typedef struct __queue _queue;

struct cmd_obj {
	void *padapter;
	u16 cmdcode;
	u8 res, no_io;
	u8 *parmbuf;
	u32 cmdsz;
	u8 *rsp;
	u32 rspsz;
	void *sctx;
	_list list;
};

struct evt_obj { u16 evtcode; u8 res; u8 *parmbuf; u32 evtsz; _list list; };

struct drvextra_cmd_parm { int ec_id, type, size; u8 *pbuf; };

struct hal_data_t { u8 hw_init_completed; };

struct cmd_priv {
	_sema cmd_queue_sema, start_cmdthread_sema;
	_queue cmd_queue;
	ATOMIC_T cmdthd_running;
	void *padapter;
};

struct evt_priv { _sema evt_notify; _queue evt_queue; u32 evt_done_cnt; };

struct _adapter { struct hal_data_t hal; struct cmd_priv cmdpriv; };
typedef struct _adapter *PADAPTER;

#define ATOMIC_SET(v, x) (*(v) = (x))
#define ATOMIC_READ(v) (*(v))
#define GET_HAL_DATA(a) (&((struct _adapter *)(a))->hal)
#define rtw_is_hw_init_completed(a) (GET_HAL_DATA(a)->hw_init_completed == _TRUE)
#define rtw_get_hw_init_completed(a) (GET_HAL_DATA(a)->hw_init_completed)
#define RTW_INFO(...) ((void)0)
#define rtw_warn_on(c) ((void)0)

static inline void _rtw_init_listhead(_list *l) { l->next = l->prev = l; }
static inline void _enter_critical(_lock *l, _irqL *i) { (void)l; (void)i; }
static inline void _exit_critical(_lock *l, _irqL *i) { (void)l; (void)i; }
static inline void _enter_critical_bh(_lock *l, _irqL *i) { (void)l; (void)i; }
static inline void _exit_critical_bh(_lock *l, _irqL *i) { (void)l; (void)i; }
static inline int rtw_is_list_empty(_list *h) { return h->next == h; }
static inline _list *get_next(_list *l) { return l->next; }
#define LIST_CONTAINOR(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))
static inline void rtw_list_insert_head(_list *n, _list *h)
{
	n->next = h->next; n->prev = h; h->next->prev = n; h->next = n;
}
static inline void rtw_list_insert_tail(_list *n, _list *h)
{
	n->next = h; n->prev = h->prev; h->prev->next = n; h->prev = n;
}
static inline void rtw_list_delete(_list *e)
{
	e->next->prev = e->prev; e->prev->next = e->next; e->next = e->prev = e;
}

void host_cmd_queue_reset(void);
void host_cmd_queue_set_hw_init(int v);
void host_cmd_queue_set_cmdthd_running(int v);
int host_cmd_queue_sema_up_count(void);
void *rtw_zmalloc(u32 sz);
void rtw_mfree(u8 *p, u32 sz);
void _rtw_up_sema(_sema *s);

sint _rtw_enqueue_cmd(_queue *q, struct cmd_obj *obj, bool to_head);
struct cmd_obj *_rtw_dequeue_cmd(_queue *q);
int rtw_cmd_filter(struct cmd_priv *p, struct cmd_obj *obj);
u32 rtw_enqueue_cmd(struct cmd_priv *p, struct cmd_obj *obj);
struct cmd_obj *rtw_dequeue_cmd(struct cmd_priv *p);
void rtw_free_cmd_obj(struct cmd_obj *pcmd);
u32 rtw_enqueue_evt(struct evt_priv *p, struct evt_obj *obj);
void rtw_free_evt_obj(struct evt_obj *obj);
void rtw_evt_notify_isr(struct evt_priv *p);

extern struct _adapter g_adapter;

#endif
