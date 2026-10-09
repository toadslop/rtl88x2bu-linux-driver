/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-132 debug manufacturing/test hook globals. */
#ifndef HOST_RTW_DEBUG_TEST_HOOKS_TYPES_H
#define HOST_RTW_DEBUG_TEST_HOOKS_TYPES_H

#include <stdint.h>
#include <stdio.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef unsigned long systime;

#define _TRUE 1
#define _FALSE 0

#define RTW_PRINT(fmt, ...) ((void)0)

#endif /* HOST_RTW_DEBUG_TEST_HOOKS_TYPES_H */
