/* SPDX-License-Identifier: GPL-2.0 */
/* Host allocator stubs for core/crypto when built userspace (T2). */

#include <stdlib.h>

#include "host_crypto_wrap.h"

void *rtw_malloc(size_t sz)
{
	return malloc(sz);
}

void rtw_mfree(void *ptr, size_t sz)
{
	(void)sz;
	free(ptr);
}
