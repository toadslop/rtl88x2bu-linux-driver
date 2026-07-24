/* SPDX-License-Identifier: GPL-2.0 */
/* Host allocator and driver stubs for core/crypto when built userspace (T2). */

#include <stdarg.h>
#include <stdlib.h>

#include "host_crypto_wrap.h"

void *rtw_malloc(size_t sz)
{
	return malloc(sz);
}

void *os_malloc(size_t sz)
{
	return rtw_malloc(sz);
}

void rtw_mfree(void *ptr, size_t sz)
{
	(void)sz;
	free(ptr);
}

u8 rtw_registrypriv_amsdu_mode(const _adapter *padapter)
{
	if (!padapter)
		return RTW_AMSDU_MODE_NON_SPP;
	return padapter->registrypriv.amsdu_mode;
}

void wpa_printf(int level, const char *fmt, ...)
{
	(void)level;
	(void)fmt;
}

void wpa_hexdump(int level, const char *title, const void *buf, size_t len)
{
	(void)level;
	(void)title;
	(void)buf;
	(void)len;
}

void wpa_hexdump_key(int level, const char *title, const void *buf, size_t len)
{
	wpa_hexdump(level, title, buf, len);
}
