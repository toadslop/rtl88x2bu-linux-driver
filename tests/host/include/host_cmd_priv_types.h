/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_CMD_PRIV_TYPES_H
#define HOST_CMD_PRIV_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0
#define CONFIG_EVENT_THREAD_MODE 1
#define CONFIG_C2H_WK 1
#define MAX_CMDSZ 1536
#define MAX_RSPSZ 512
#define MAX_EVTSZ 1024
#define CMDBUFF_ALIGN_SZ 512
#define C2H_QUEUE_MAX_LEN 10

typedef int sint;
typedef unsigned long SIZE_PTR;
typedef int ATOMIC_T, _sema, _mutex, _lock;
typedef struct { int s; } _workitem;
struct _list { struct _list *next, *prev; };
typedef struct _list _list;
struct __queue { _list queue; _lock lock; };
typedef struct __queue _queue;
struct rtw_cbuf { u32 size, write, read; void **bufs; };
struct cmd_priv {
	_sema cmd_queue_sema, start_cmdthread_sema;
	_queue cmd_queue;
	u8 cmd_seq;
	u8 *cmd_buf, *cmd_allocated_buf, *rsp_buf, *rsp_allocated_buf;
	u32 cmd_issued_cnt, cmd_done_cnt, rsp_cnt;
	ATOMIC_T cmdthd_running;
	void *padapter;
	_mutex sctx_mutex;
};
struct evt_priv {
	_sema evt_notify;
	_queue evt_queue;
	_workitem c2h_wk;
	bool c2h_wk_alive;
	struct rtw_cbuf *c2h_queue;
	ATOMIC_T event_seq;
	u8 *evt_buf, *evt_allocated_buf;
	u32 evt_done_cnt;
};
#define ATOMIC_SET(v, x) (*(v) = (x))
#define ATOMIC_READ(v) (*(v))
#define RTW_INFO(...) ((void)0)
#define rtw_warn_on(c) ((void)0)
static inline void _rtw_init_listhead(_list *l) { l->next = l->prev = l; }
static inline void _rtw_spinlock_init(_lock *x) { (void)x; }
static inline void _rtw_spinlock_free(_lock *x) { (void)x; }
static inline void _rtw_init_queue(_queue *q)
{
	_rtw_init_listhead(&q->queue);
	_rtw_spinlock_init(&q->lock);
}

void host_cmd_priv_set_malloc_fail_after(int n);
void _rtw_init_sema(_sema *s, int v);
void _rtw_free_sema(_sema *s);
void _rtw_mutex_init(_mutex *m);
void _rtw_mutex_free(_mutex *m);
void _init_workitem(_workitem *w, void *f, void *c);
void _cancel_workitem_sync(_workitem *w);
void rtw_msleep_os(int ms);
void *rtw_zmalloc(u32 sz);
void rtw_mfree(u8 *p, u32 sz);
bool rtw_cbuf_empty(struct rtw_cbuf *c);
struct rtw_cbuf *rtw_cbuf_alloc(u32 n);
void rtw_cbuf_free(struct rtw_cbuf *c);
void *rtw_cbuf_pop(struct rtw_cbuf *c);
void c2h_wk_callback(_workitem *work);
sint _rtw_init_cmd_priv(struct cmd_priv *p);
sint _rtw_init_evt_priv(struct evt_priv *p);
void _rtw_free_cmd_priv(struct cmd_priv *p);
void _rtw_free_evt_priv(struct evt_priv *p);

#endif
