// SPDX-License-Identifier: GPL-2.0
/*
 * Host shims for rtw_mlme_rest L2 tests (W3-53).
 */

#include "host_mlme_types.h"

static u32 host_mlme_random32;

void host_mlme_set_random32(u32 val)
{
	host_mlme_random32 = val;
}

int _rtw_memcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1, s2, n) == 0 ? _TRUE : _FALSE;
}

int rtw_bug_check(void *parg1, void *parg2, void *parg3, void *parg4)
{
	(void)parg1;
	(void)parg2;
	(void)parg3;
	(void)parg4;
	return _TRUE;
}

int is_all_null(char *c, int len)
{
	for (; len > 0; len--)
		if (c[len - 1] != '\0')
			return _FALSE;
	return _TRUE;
}

u32 rtw_random32(void)
{
	return host_mlme_random32;
}