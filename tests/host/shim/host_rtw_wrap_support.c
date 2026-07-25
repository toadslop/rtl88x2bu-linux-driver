/* SPDX-License-Identifier: GPL-2.0 */
/* Allocator and memcmp stubs for rtw_crypto_wrap.c C-oracle builds (W2-06d). */

#include <stdlib.h>
#include <string.h>

#include "drv_types.h"

void *rtw_malloc(size_t sz)
{
	return malloc(sz);
}

void rtw_mfree(void *ptr, size_t sz)
{
	(void)sz;
	free(ptr);
}

int _rtw_memcmp2(const void *dst, const void *src, u32 sz)
{
	return memcmp(dst, src, sz);
}

void host_adapter_set_amsdu_mode(_adapter *padapter, enum rtw_amsdu_mode mode)
{
	if (padapter)
		padapter->registrypriv.amsdu_mode = (u8)mode;
}
