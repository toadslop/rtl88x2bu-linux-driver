/* SPDX-License-Identifier: GPL-2.0 */
/* Allocator and memcmp stubs for rtw_crypto_wrap.c C-oracle builds (W2-06d). */

#include <stdlib.h>
#include <string.h>

#include "drv_types.h"

static int g_expect_zero_on_free;
static int g_zero_check_failed;

void host_rtw_wrap_enable_bin_clear_free_check(int on)
{
	if (on)
		g_zero_check_failed = 0;
	g_expect_zero_on_free = on;
}

int host_rtw_wrap_zero_check_failed(void)
{
	return g_zero_check_failed;
}

void *rtw_malloc(size_t sz)
{
	return malloc(sz);
}

void rtw_mfree(void *ptr, size_t sz)
{
	if (g_expect_zero_on_free && ptr && sz > 0) {
		const u8 *p = ptr;

		for (size_t i = 0; i < sz; i++) {
			if (p[i]) {
				g_zero_check_failed = 1;
				break;
			}
		}
	}
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
