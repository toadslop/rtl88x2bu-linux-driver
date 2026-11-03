/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-212 proc TDLS enable (PR1); display fields in PR2. */
#ifndef HOST_RTW_DEBUG_PROC_TDLS_TYPES_H
#define HOST_RTW_DEBUG_PROC_TDLS_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef long ssize_t;

#define _TRUE 1
#define _FALSE 0

struct registry_priv {
	u8 en_tdls;
};

struct _adapter {
	struct registry_priv registrypriv;
};

struct net_device {
	void *priv;
};

struct seq_file {
	void *private;
	FILE *out;
};

#define RTW_PRINT_SEL(sel, fmt, ...)                                           \
	fprintf(((struct seq_file *)(sel))->out ? ((struct seq_file *)(sel))->out \
					       : stdout,                           \
		fmt, ##__VA_ARGS__)

#define rtw_warn_on(c) ((void)(c))

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

u8 rtw_is_tdls_enabled(struct _adapter *padapter);
void rtw_enable_tdls_func(struct _adapter *padapter);
void rtw_disable_tdls_func(struct _adapter *padapter, u8 enqueue_cmd);

int proc_get_tdls_enable(struct seq_file *m, void *v);
ssize_t proc_set_tdls_enable(void *file, const void *buffer, size_t count,
			     long *pos, void *data);

#endif /* HOST_RTW_DEBUG_PROC_TDLS_TYPES_H */
