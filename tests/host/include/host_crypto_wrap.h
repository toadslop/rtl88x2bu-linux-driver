/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Userspace shim for core/crypto when HOST_CRYPTO_TEST is defined (T2).
 * Included from core/crypto/rtw_crypto_wrap.h — do not include directly.
 *
 * Extend this header and tests/host/shim/ as each crypto unit is added for
 * L2 differential tests. See core/crypto/rtw_crypto_wrap.h for the kernel-side
 * symbol checklist. Host GCMP/aes-gcm tests link host_crypto_shim.c only —
 * do not link core/crypto/rtw_crypto_wrap.c (duplicate wpa_* / os_memcmp_const).
 */
#ifndef HOST_CRYPTO_WRAP_H
#define HOST_CRYPTO_WRAP_H

#include "host_types.h"
#include "host_wifi_types.h"

#define TEST_FAIL() 0

enum {
	_MSG_EXCESSIVE_,
	_MSG_MSGDUMP_,
	_MSG_DEBUG_,
	_MSG_INFO_,
	_MSG_WARNING_,
	_MSG_ERROR_,
};

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#define os_memset _rtw_memset
#define os_memcpy _rtw_memcpy

void *rtw_malloc(size_t sz);
void *os_malloc(size_t sz);
void rtw_mfree(void *ptr, size_t sz);

u8 rtw_registrypriv_amsdu_mode(const _adapter *padapter);

void wpa_printf(int level, const char *fmt, ...);
void wpa_hexdump(int level, const char *title, const void *buf, size_t len);
void wpa_hexdump_key(int level, const char *title, const void *buf, size_t len);

#endif /* HOST_CRYPTO_WRAP_H */
