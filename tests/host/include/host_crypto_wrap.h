/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Userspace shim for core/crypto when HOST_CRYPTO_TEST is defined (T2).
 * Included from core/crypto/rtw_crypto_wrap.h — do not include directly.
 */
#ifndef HOST_CRYPTO_WRAP_H
#define HOST_CRYPTO_WRAP_H

#include "host_types.h"

/*
 * Minimal shim for aes-ctr host tests (T2). Wave 2 crypto ports will need
 * additional symbols from core/crypto/rtw_crypto_wrap.h — use that file as
 * the checklist (endian macros, os_memcmp, wpa_printf, …) and extend this
 * header + tests/host/shim/ as each unit is added.
 */

#define TEST_FAIL() 0

#define os_memset _rtw_memset
#define os_memcpy _rtw_memcpy

void *rtw_malloc(size_t sz);
void rtw_mfree(void *ptr, size_t sz);
#define os_malloc rtw_malloc

#endif /* HOST_CRYPTO_WRAP_H */
