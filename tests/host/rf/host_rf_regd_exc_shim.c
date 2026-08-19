// SPDX-License-Identifier: GPL-2.0
/* Host fixtures for W3-51 regd_exc L2 oracles. */

#include <stdlib.h>
#include <string.h>

#include "host_rf_regd_exc_types.h"

void *rtw_zmalloc(u32 sz)
{
	return calloc(1, sz);
}

void rtw_mfree(u8 *p, u32 sz)
{
	(void)sz;
	free(p);
}

void host_rf_regd_exc_reset(struct rf_ctl_t *rfctl)
{
	memset(rfctl, 0, sizeof(*rfctl));
	_rtw_init_listhead(&rfctl->reg_exc_list);
}
