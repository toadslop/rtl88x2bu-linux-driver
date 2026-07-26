/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Linker-visible shims for Rust IE oracle host tests.
 */
#include <stddef.h>
#include <string.h>

#define _TRUE 1

void *_rtw_memcpy(void *dest, const void *src, size_t n)
{
	return memcpy(dest, src, n);
}

int _rtw_memcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1, s2, n) == 0 ? _TRUE : 0;
}

void *_rtw_memmove(void *dest, const void *src, size_t n)
{
	return memmove(dest, src, n);
}
