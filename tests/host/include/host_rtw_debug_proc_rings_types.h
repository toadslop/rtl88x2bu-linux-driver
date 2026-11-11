/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-233 TRX ring proc getters (CONFIG_PCI_HCI). */
#ifndef HOST_RTW_DEBUG_PROC_RINGS_TYPES_H
#define HOST_RTW_DEBUG_PROC_RINGS_TYPES_H

#include <stdio.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef unsigned long dma_addr_t;

#define RX_MPDU_QUEUE 0
#define PCI_MAX_TX_QUEUE_COUNT 8
#define TX_BAK_FRMAE_CNT 10
#define TX_BAK_DESC_LEN 48
#define TX_BAK_DATA_LEN 30

struct recv_stat {
	u32 w[4];
};

struct tx_desc {
	u32 w[8];
};

struct sk_buff {
	u8 cb[8];
};

struct rtw_rx_ring {
	struct recv_stat *desc;
	dma_addr_t dma;
	unsigned int idx;
	struct sk_buff *rx_buf[1];
};

struct rtw_tx_ring {
	struct tx_desc *desc;
	dma_addr_t dma;
	unsigned int idx;
	unsigned int entries;
	u32 qlen;
};

struct recv_priv {
	struct rtw_rx_ring rx_ring[1];
	int rxringcount;
};

struct xmit_priv {
	struct rtw_tx_ring tx_ring[PCI_MAX_TX_QUEUE_COUNT];
	int txringcount[PCI_MAX_TX_QUEUE_COUNT];
	int dump_txbd_desc;
};

struct rings_lock {
	int dummy;
};

struct dvobj_priv {
	struct rings_lock irq_th_lock;
};

struct _adapter {
	struct dvobj_priv dvobjpriv;
	struct recv_priv recvpriv;
	struct xmit_priv xmitpriv;
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

typedef unsigned long _irqL;

static inline void _enter_critical(struct rings_lock *plock, _irqL *pirqL)
{
	(void)plock;
	(void)pirqL;
}

static inline void _exit_critical(struct rings_lock *plock, _irqL *pirqL)
{
	(void)plock;
	(void)pirqL;
}

static inline u32 cpu_to_le32(u32 v)
{
	return v;
}

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

#define adapter_to_dvobj(a) (&(a)->dvobjpriv)

void host_rings_test_reset(void);
void *rtw_rust_debug_proc_adapter(void *m);
void rtw_rust_debug_proc_emit_rx_ring(void *sel, void *adapter);
void rtw_rust_debug_proc_emit_tx_ring(void *sel, void *adapter);
void rtw_rust_debug_proc_emit_tx_ring_ext(void *sel, void *adapter);

int proc_get_rx_ring(struct seq_file *m, void *v);
int proc_get_tx_ring(struct seq_file *m, void *v);
int proc_get_tx_ring_ext(struct seq_file *m, void *v);

#endif /* HOST_RTW_DEBUG_PROC_RINGS_TYPES_H */
