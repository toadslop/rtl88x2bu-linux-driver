/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-234 RTKM info proc (CONFIG_PREALLOC_RX_SKB_BUFFER + USB). */
#ifndef HOST_RTW_DEBUG_PROC_RTKM_TYPES_H
#define HOST_RTW_DEBUG_PROC_RTKM_TYPES_H

#include <stdio.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define HOST_RTKM_NR_PREALLOC_RECV_SKB 16
#define HOST_RTKM_RECVBUF_SZ 15360
#define NR_PREALLOC_RECV_SKB (HOST_RTKM_NR_PREALLOC_RECV_SKB >> 1)

struct recv_buf {
	u32 alloc_sz;
};

struct recv_priv {
	u8 *precv_buf;
};

struct _adapter {
	struct recv_priv recvpriv;
};

struct net_device {
	void *priv;
};

struct seq_file {
	void *private;
	FILE *out;
};

#define RTW_PRINT_SEL(sel, fmt, ...) fprintf(((struct seq_file *)(sel))->out, fmt, ##__VA_ARGS__)

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev->priv;
}

u8 rtw_rtkm_get_nr_recv_skb(void);
u16 rtw_rtkm_get_buff_size(void);
void *rtw_rust_debug_proc_adapter(void *m);
void rtw_rust_debug_proc_emit_rtkm_info(void *sel, void *adapter);

int proc_get_rtkm_info(struct seq_file *m, void *v);

#endif /* HOST_RTW_DEBUG_PROC_RTKM_TYPES_H */
