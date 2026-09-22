// SPDX-License-Identifier: GPL-2.0
#include <stdlib.h>
#include <string.h>
#include "host_rm_fsm_types.h"

void *rtw_malloc(u32 sz)
{
	return malloc(sz);
}

void host_rm_fsm_adapter_init(_adapter *a)
{
	_rtw_memset(a, 0, sizeof(*a));
	_rtw_init_queue(&a->rmpriv.ev_queue);
	_rtw_init_queue(&a->rmpriv.rm_queue);
}
