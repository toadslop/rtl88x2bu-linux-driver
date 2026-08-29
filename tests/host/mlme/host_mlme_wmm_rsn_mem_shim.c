// SPDX-License-Identifier: GPL-2.0
#include <string.h>

void *_rtw_memcpy(void *d, const void *s, size_t n)
{
	return memcpy(d, s, n);
}
