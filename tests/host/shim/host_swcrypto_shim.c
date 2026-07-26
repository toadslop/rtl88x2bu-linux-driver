/* SPDX-License-Identifier: GPL-2.0 */
/* Kernel-style allocator symbols for rust/rtw_swcrypto.rs host L2 tests (W3-01). */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void _rtw_mfree(void *ptr, uint32_t sz)
{
	(void)sz;
	free(ptr);
}

void *_rtw_memcpy(void *dst, const void *src, size_t n)
{
	return memcpy(dst, src, n);
}
