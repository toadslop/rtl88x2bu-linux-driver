/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_RM_FSM_TYPES_H
#define HOST_RM_FSM_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include "host_types.h"

#define _TRUE 1
#define _FAIL 0
#define _SUCCESS 1
#define RM_TIMER_NUM 32
#define CLOCK_UNIT 10
#define RM_ST_IDLE 0

enum RM_EV_ID {
	RM_EV_state_in,
	RM_EV_busy_timer_expire,
	RM_EV_delay_timer_expire,
	RM_EV_meas_timer_expire,
	RM_EV_retry_timer_expire,
	RM_EV_repeat_delay_expire,
	RM_EV_request_timer_expire,
	RM_EV_wait_report,
	RM_EV_start_meas,
	RM_EV_survey_done,
	RM_EV_recv_rep,
	RM_EV_cancel,
	RM_EV_state_out,
	RM_EV_max,
};

typedef int ATOMIC_T, _lock;
typedef unsigned long _irqL;

struct _list { struct _list *next, *prev; };
typedef struct _list _list;
struct __queue { _list queue; _lock lock; };
typedef struct __queue _queue;

struct bcn_req_opt { u8 *req_start; u8 req_len; };
struct rm_meas_req { u8 *pssid; struct { struct bcn_req_opt bcn; } opt; };
struct rm_clock { struct rm_obj *prm; ATOMIC_T counter; enum RM_EV_ID evid; };
struct rm_obj {
	u32 rmid;
	u8 state;
	struct rm_meas_req q;
	struct rm_clock *pclock;
	_list list;
};
struct rm_event { u32 rmid; enum RM_EV_ID evid; _list list; };
struct rm_priv { _queue ev_queue, rm_queue; struct rm_clock clock[RM_TIMER_NUM]; };
struct _adapter { struct rm_priv rmpriv; };
typedef struct _adapter _adapter;

#define ATOMIC_SET(v, x) (*(v) = (x))
#define ATOMIC_READ(v) (*(v))

static inline void _rtw_init_listhead(_list *l) { l->next = l->prev = l; }
static inline void _enter_critical(_lock *l, _irqL *i) { (void)l; (void)i; }
static inline void _exit_critical(_lock *l, _irqL *i) { (void)l; (void)i; }
static inline int rtw_is_list_empty(_list *h) { return h->next == h; }
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
static inline void _rtw_init_queue(_queue *q) { _rtw_init_listhead(&q->queue); }

void host_rm_fsm_adapter_init(_adapter *a);
void *rtw_malloc(u32 sz);
void rtw_mfree(u8 *p, u32 sz);
int is_list_linked(const struct _list *head);
void rm_set_clock(struct rm_obj *prm, u32 ms, enum RM_EV_ID evid);
struct rm_clock *rm_alloc_clock(_adapter *padapter, struct rm_obj *prm);
void rm_cancel_clock(struct rm_obj *prm);
void rm_free_clock(struct rm_clock *pclock);
int rm_enqueue_ev(_queue *queue, struct rm_event *obj, bool to_head);
void rm_free_rmobj(struct rm_obj *prm);
struct rm_obj *rm_alloc_rmobj(_adapter *padapter);
int rm_enqueue_rmobj(_adapter *padapter, struct rm_obj *prm, bool to_head);

#endif /* HOST_RM_FSM_TYPES_H */
