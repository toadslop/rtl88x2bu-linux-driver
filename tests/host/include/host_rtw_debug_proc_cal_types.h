/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-230 proc IQK/LCK cal test get/set. */
#ifndef HOST_RTW_DEBUG_PROC_CAL_TYPES_H
#define HOST_RTW_DEBUG_PROC_CAL_TYPES_H

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef long ssize_t;

struct _adapter {
	u8 pad;
};

struct net_device {
	void *priv;
};

struct seq_file {
	void *private;
	FILE *out;
};

#define RTW_INFO(fmt, ...) ((void)0)
#define rtw_warn_on(x) ((void)(x))

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

void rtw_hal_iqk_test(struct _adapter *adapter, bool recovery, bool clear,
		      bool segment);
void rtw_hal_lck_test(struct _adapter *adapter);

void host_debug_proc_cal_reset_log(void);
unsigned host_debug_proc_cal_iqk_calls(void);
bool host_debug_proc_cal_iqk_last_recovery(void);
bool host_debug_proc_cal_iqk_last_clear(void);
bool host_debug_proc_cal_iqk_last_segment(void);
unsigned host_debug_proc_cal_lck_calls(void);

int proc_get_iqk_info(struct seq_file *m, void *v);
ssize_t proc_set_iqk(void *file, const void *buffer, size_t count, long *pos,
		     void *data);
int proc_get_lck_info(struct seq_file *m, void *v);
ssize_t proc_set_lck(void *file, const void *buffer, size_t count, long *pos,
		     void *data);

#endif /* HOST_RTW_DEBUG_PROC_CAL_TYPES_H */
