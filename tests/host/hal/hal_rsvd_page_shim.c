// SPDX-License-Identifier: GPL-2.0
#include <stdlib.h>
#include <string.h>

#include "host_hal_com_rsvd_types.h"

void *rtw_malloc(u32 sz)
{
	return malloc(sz);
}

void *rtw_zmalloc(u32 sz)
{
	return calloc(1, sz);
}

void rtw_mfree(u8 *p, u32 sz)
{
	(void)sz;
	free(p);
}

int _rtw_memcmp(const void *a, const void *b, u32 sz)
{
	return memcmp(a, b, sz) ? _FALSE : _TRUE;
}

void rtw_warn_on(int cond)
{
	(void)cond;
}
