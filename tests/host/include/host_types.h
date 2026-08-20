/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal userspace types for host crypto differential tests (T2).
 * Replaces kernel drv_types.h / basic_types.h — no kernel headers required.
 */
#ifndef HOST_TYPES_H
#define HOST_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#ifndef __must_check
#define __must_check __attribute__((__warn_unused_result__))
#endif

static inline void *_rtw_memset(void *s, int c, size_t n)
{
	return memset(s, c, n);
}

static inline void *_rtw_memcpy(void *dest, const void *src, size_t n)
{
	return memcpy(dest, src, n);
}

#endif /* HOST_TYPES_H */
