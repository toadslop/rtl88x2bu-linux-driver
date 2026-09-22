/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_RM_FSM_TYPES_H
#define HOST_RM_FSM_TYPES_H

#include <stddef.h>
#include "host_types.h"

#define _SUCCESS 1
#define RM_TIMER_NUM 32
#define CLOCK_UNIT 10

enum RM_EV_ID {
	RM_EV_meas_timer_expire = 3,
	RM_EV_max,
};

typedef int ATOMIC_T;

struct _list {
	struct _list *next, *prev;
};

struct rm_clock {
	struct rm_obj *prm;
	ATOMIC_T counter;
	enum RM_EV_ID evid;
};

struct rm_obj {
	struct rm_clock *pclock;
	struct _list list;
};

struct rm_priv {
	struct rm_clock clock[RM_TIMER_NUM];
	struct {
		struct _list queue;
	} ev_queue, rm_queue;
};

struct _adapter {
	struct rm_priv rmpriv;
};

typedef struct _adapter _adapter;

#define ATOMIC_SET(v, x) (*(v) = (x))
#define ATOMIC_READ(v) (*(v))

static inline void _rtw_init_listhead(struct _list *l)
{
	l->next = l->prev = l;
}

static inline void _rtw_init_queue(struct rm_priv *p, int which)
{
	(void)p;
	(void)which;
}

void host_rm_fsm_adapter_init(_adapter *a);

int is_list_linked(const struct _list *head);
void rm_set_clock(struct rm_obj *prm, u32 ms, enum RM_EV_ID evid);
struct rm_clock *rm_alloc_clock(_adapter *padapter, struct rm_obj *prm);
void rm_cancel_clock(struct rm_obj *prm);
void rm_free_clock(struct rm_clock *pclock);

#endif /* HOST_RM_FSM_TYPES_H */
