/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Full-width systime helpers for host pwrctrl L2 (matches kernel unsigned long jiffies).
 */
#ifndef HOST_SYSTIME_H
#define HOST_SYSTIME_H

#include <limits.h>
#include <stdint.h>

typedef unsigned long systime;

#ifndef HZ
#define HZ 1000
#endif

/* Rough INITIAL_JIFFIES for CONFIG_HZ=1000 host vectors (see FX-01). */
#define HOST_INITIAL_JIFFIES ((systime)(UINT32_MAX - 300UL * (unsigned long)HZ))

static inline systime host_ms_to_systime(unsigned int ms)
{
	return (systime)ms * (1000UL / (unsigned long)HZ);
}

static inline unsigned int host_systime_to_ms(systime delta)
{
	return (unsigned int)(delta * (unsigned long)HZ / 1000UL);
}

/* include/linux/jiffies.h time_after() */
static inline int host_time_after(systime a, systime b)
{
	return ((long)(b - a) < 0);
}

static inline unsigned int host_jiffies_to_msecs_hz300(unsigned long delta)
{
	return (unsigned int)((delta * 10ULL + 2ULL) / 3ULL);
}

#endif /* HOST_SYSTIME_H */
