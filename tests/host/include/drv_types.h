/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal drv_types.h for compiling core/crypto/rtw_crypto_wrap.c as the L2 C
 * oracle (W2-06d). Not used for HOST_CRYPTO_TEST builds.
 */
#ifndef DRV_TYPES_H
#define DRV_TYPES_H

#include "host_types.h"

#include <stdlib.h>
#include <string.h>

enum rtw_amsdu_mode {
	RTW_AMSDU_MODE_NON_SPP = 0,
	RTW_AMSDU_MODE_SPP = 1,
	RTW_AMSDU_MODE_ALL_DROP = 2,
};

struct registry_priv {
	u8 amsdu_mode;
};

typedef struct {
	struct registry_priv registrypriv;
} _adapter;

#define RTW_PUT_LE16(a, val)                     \
	do {                                     \
		(a)[1] = (u8)(((u16)(val)) >> 8); \
		(a)[0] = (u8)(((u16)(val)) & 0xff); \
	} while (0)
#define RTW_GET_LE16(a) ((((u16)(a)[1]) << 8) | (u16)(a)[0])
#define RTW_PUT_LE32(a, val)                                          \
	do {                                                          \
		(a)[3] = (u8)(((u32)(val)) & 0xff);                  \
		(a)[2] = (u8)(((u32)(val)) >> 8) & 0xff);            \
		(a)[1] = (u8)(((u32)(val)) >> 16) & 0xff);           \
		(a)[0] = (u8)(((u32)(val)) >> 24) & 0xff);           \
	} while (0)
#define RTW_GET_LE32(a)                                       \
	(((u32)(((const u8 *)(a))[0]) << 24) |                \
	 ((u32)(((const u8 *)(a))[1]) << 16) |                \
	 ((u32)(((const u8 *)(a))[2]) << 8) |                 \
	 ((u32)(((const u8 *)(a))[3])))
#define RTW_PUT_LE64(a, val)                                  \
	do {                                                  \
		RTW_PUT_LE32((a), (u32)((u64)(val)));            \
		RTW_PUT_LE32((a) + 4, (u32)((u64)(val) >> 32)); \
	} while (0)
#define RTW_GET_LE64(a) 0ULL
#define RTW_PUT_BE16(a, val)                     \
	do {                                     \
		(a)[0] = (u8)(((u16)(val)) >> 8); \
		(a)[1] = (u8)(((u16)(val)) & 0xff); \
	} while (0)
#define RTW_GET_BE16(a) ((((u16)(a)[0]) << 8) | (u16)(a)[1])
#define RTW_PUT_BE32(a, val)                                          \
	do {                                                          \
		(a)[0] = (u8)(((u32)(val)) >> 24) & 0xff);            \
		(a)[1] = (u8)(((u32)(val)) >> 16) & 0xff);           \
		(a)[2] = (u8)(((u32)(val)) >> 8) & 0xff);            \
		(a)[3] = (u8)(((u32)(val)) & 0xff);                  \
	} while (0)
#define RTW_GET_BE32(a)                                       \
	(((u32)(((const u8 *)(a))[0]) << 24) |                \
	 ((u32)(((const u8 *)(a))[1]) << 16) |                \
	 ((u32)(((const u8 *)(a))[2]) << 8) |                 \
	 ((u32)(((const u8 *)(a))[3])))
#define RTW_PUT_BE64(a, val)                                  \
	do {                                                  \
		RTW_PUT_BE32((a), (u32)((u64)(val) >> 32));    \
		RTW_PUT_BE32((a) + 4, (u32)((u64)(val)));     \
	} while (0)
#define RTW_GET_BE64(a) 0ULL

static inline u16 le_to_host16(u16 val)
{
	return val;
}

#define cpu_to_le16(x) ((__u16)(x))
#define le16_to_cpu(x) ((__u16)(x))

#define RTW_INFO(fmt, ...) ((void)0)
#define RTW_INFO_DUMP(title, buf, len) ((void)0)

void *rtw_malloc(size_t sz);
void rtw_mfree(void *ptr, size_t sz);
int _rtw_memcmp2(const void *dst, const void *src, u32 sz);

#endif /* DRV_TYPES_H */
