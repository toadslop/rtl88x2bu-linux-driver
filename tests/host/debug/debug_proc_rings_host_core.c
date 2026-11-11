// SPDX-License-Identifier: GPL-2.0
/* C oracle core for W3-233 TRX ring proc getters (host L2). */

#include "host_rtw_debug_proc_rings_types.h"

void host_rings_test_reset(void)
{
}

void *rtw_rust_debug_proc_adapter(void *m)
{
	struct seq_file *s = m;

	if (!s)
		return NULL;
	return rtw_netdev_priv((struct net_device *)s->private);
}

void rtw_rust_debug_proc_emit_rx_ring(void *sel, void *adapter)
{
	struct _adapter *padapter = adapter;
	struct dvobj_priv *pdvobjpriv = adapter_to_dvobj(padapter);
	struct recv_priv *precvpriv = &padapter->recvpriv;
	struct rtw_rx_ring *rx_ring = &precvpriv->rx_ring[RX_MPDU_QUEUE];
	_irqL irqL;
	int i, j;

	RTW_PRINT_SEL(sel, "rx ring (%p)\n", (void *)rx_ring);
	RTW_PRINT_SEL(sel, "  dma: 0x%08x\n", (int)rx_ring->dma);
	RTW_PRINT_SEL(sel, "  idx: %d\n", rx_ring->idx);

	_enter_critical(&pdvobjpriv->irq_th_lock, &irqL);
	for (i = 0; i < precvpriv->rxringcount; i++) {
		struct recv_stat *entry = &rx_ring->desc[i];
		struct sk_buff *skb = rx_ring->rx_buf[i];

		RTW_PRINT_SEL(sel, "  desc[%03d]: %p, rx_buf[%03d]: 0x%08x\n", i,
			      (void *)entry, i,
			      cpu_to_le32(*((dma_addr_t *)skb->cb)));

		for (j = 0; j < (int)(sizeof(*entry) / 4); j++) {
			if ((j % 4) == 0)
				RTW_PRINT_SEL(sel, "  0x%03x", j);

			RTW_PRINT_SEL(sel, " 0x%08x ", ((int *)entry)[j]);

			if ((j % 4) == 3)
				RTW_PRINT_SEL(sel, "\n");
		}
	}
	_exit_critical(&pdvobjpriv->irq_th_lock, &irqL);
}

void rtw_rust_debug_proc_emit_tx_ring(void *sel, void *adapter)
{
	struct _adapter *padapter = adapter;
	struct dvobj_priv *pdvobjpriv = adapter_to_dvobj(padapter);
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	_irqL irqL;
	int i, j, k;

	_enter_critical(&pdvobjpriv->irq_th_lock, &irqL);
	for (i = 0; i < PCI_MAX_TX_QUEUE_COUNT; i++) {
		struct rtw_tx_ring *tx_ring = &pxmitpriv->tx_ring[i];

		RTW_PRINT_SEL(sel, "tx ring[%d] (%p)\n", i, (void *)tx_ring);
		RTW_PRINT_SEL(sel, "  dma: 0x%08x\n", (int)tx_ring->dma);
		RTW_PRINT_SEL(sel, "  idx: %d\n", tx_ring->idx);
		RTW_PRINT_SEL(sel, "  entries: %d\n", tx_ring->entries);
		RTW_PRINT_SEL(sel, "  qlen: %d\n", tx_ring->qlen);

		for (j = 0; j < pxmitpriv->txringcount[i]; j++) {
			struct tx_desc *entry = &tx_ring->desc[j];

			RTW_PRINT_SEL(sel, "  desc[%03d]: %p\n", j, (void *)entry);

			for (k = 0; k < (int)(sizeof(*entry) / 4); k++) {
				if ((k % 4) == 0)
					RTW_PRINT_SEL(sel, "  0x%03x", k);

				RTW_PRINT_SEL(sel, " 0x%08x ", ((int *)entry)[k]);

				if ((k % 4) == 3)
					RTW_PRINT_SEL(sel, "\n");
			}
		}
	}
	_exit_critical(&pdvobjpriv->irq_th_lock, &irqL);
}

void rtw_rust_debug_proc_emit_tx_ring_ext(void *sel, void *adapter)
{
	struct _adapter *padapter = adapter;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	RTW_PRINT_SEL(sel, "<<<< tx ring ext dump settings >>>>\n");
	RTW_PRINT_SEL(sel, " - backup frame num: %d\n", TX_BAK_FRMAE_CNT);
	RTW_PRINT_SEL(sel, " - backup max. desc size: %d bytes\n",
		      TX_BAK_DESC_LEN);
	RTW_PRINT_SEL(sel, " - backup data size: %d bytes\n\n", TX_BAK_DATA_LEN);

	if (!pxmitpriv->dump_txbd_desc)
		RTW_PRINT_SEL(sel, "Dump function is disabled.\n");
}
