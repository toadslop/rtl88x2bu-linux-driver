/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-215 proc btcoex debug handlers. */
#ifndef HOST_RTW_DEBUG_PROC_BTCOEX_TYPES_H
#define HOST_RTW_DEBUG_PROC_BTCOEX_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef long ssize_t;

struct _adapter {
	u8 _pad;
};

struct net_device {
	void *priv;
};

struct seq_file {
	void *private;
	FILE *out;
};

#define _RTW_PRINT_SEL(sel, fmt, ...) RTW_PRINT_SEL(sel, fmt, ##__VA_ARGS__)

#define RTW_PRINT_SEL(sel, fmt, ...)                                           \
	fprintf(((struct seq_file *)(sel))->out ? ((struct seq_file *)(sel))->out \
					       : stdout,                           \
		fmt, ##__VA_ARGS__)

#define RTW_INFO(fmt, ...) ((void)0)
#define FUNC_ADPT_FMT "%s"
#define FUNC_ADPT_ARG(a) "adapter"

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

static inline void _rtw_memset(void *p, int c, size_t n)
{
	memset(p, c, n);
}

u32 rtw_btcoex_GetDBG(struct _adapter *padapter, u8 *pStrBuf, u32 bufSize);
void rtw_btcoex_SetDBG(struct _adapter *padapter, u32 *pDbgModule);
void rtw_btcoex_DisplayBtCoexInfo(struct _adapter *padapter, u8 *pbuf,
				  u32 bufsize);
void *rtw_zmalloc(u32 sz);
void rtw_mfree(void *p, u32 sz);

void host_debug_proc_btcoex_reset_mocks(void);
void host_debug_proc_btcoex_set_dbg(u32 m0, u32 m1);
void host_debug_proc_btcoex_get_dbg(u32 *m0, u32 *m1);

int proc_get_btcoex_dbg(struct seq_file *m, void *v);
ssize_t proc_set_btcoex_dbg(void *file, const void *buffer, size_t count,
			     long *pos, void *data);
int proc_get_btcoex_info(struct seq_file *m, void *v);

#endif /* HOST_RTW_DEBUG_PROC_BTCOEX_TYPES_H */
