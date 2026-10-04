// SPDX-License-Identifier: GPL-2.0
/* C oracle part 1 — clock/list helpers (W3-112 PR1). */
#include <stdlib.h>
#include <string.h>
#include "host_rm_fsm_types.h"

void host_rm_fsm_adapter_init(_adapter *a)
{
	_rtw_memset(a, 0, sizeof(*a));
	_rtw_init_queue(&a->rmpriv.ev_queue);
	_rtw_init_queue(&a->rmpriv.rm_queue);
}

int is_list_linked(const struct _list *head)
{
	return head->prev != NULL;
}

void rm_set_clock(struct rm_obj *prm, u32 ms, enum RM_EV_ID evid)
{
	ATOMIC_SET(&(prm->pclock->counter), (ms / CLOCK_UNIT));
	prm->pclock->evid = evid;
}

struct rm_clock *rm_alloc_clock(_adapter *padapter, struct rm_obj *prm)
{
	int i;
	struct rm_priv *prmpriv = &padapter->rmpriv;
	struct rm_clock *pclock = NULL;

	for (i = 0; i < RM_TIMER_NUM; i++) {
		pclock = &prmpriv->clock[i];
		if (pclock->prm == NULL) {
			pclock->prm = prm;
			ATOMIC_SET(&(pclock->counter), 0);
			pclock->evid = RM_EV_max;
			break;
		}
	}
	return pclock;
}

void rm_cancel_clock(struct rm_obj *prm)
{
	ATOMIC_SET(&(prm->pclock->counter), 0);
	prm->pclock->evid = RM_EV_max;
}

void rm_free_clock(struct rm_clock *pclock)
{
	pclock->prm = NULL;
	ATOMIC_SET(&(pclock->counter), 0);
	pclock->evid = RM_EV_max;
}
