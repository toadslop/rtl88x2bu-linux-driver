// SPDX-License-Identifier: GPL-2.0
#define _HAL_USB_REST_C_
#define HAL_USB_SET_INTF_OPS_IN_RUST 1
#define HAL_USB_VENDOR_WVALUE_IN_RUST 1
#define HAL_USB_SYNC_READ_IN_RUST 1
#define HAL_USB_SYNC_WRITE_IN_RUST 1
#define HAL_USB_ASYNC_WRITE_IN_RUST 1
#define HAL_USB_FREE_RECV_PRIV_IN_RUST 1
#define HAL_USB_C2H_HISR_IN_RUST 1
#define HAL_USB_INIT_RECV_PRIV_P1_IN_RUST 1
#define HAL_USB_INIT_RECV_PRIV_IN_RUST 1

#include "hal_usb.c"

#include <linux/skbuff.h>

/* Layout anchors for rust/hal_usb.rs usb_free_recv_priv (W4-43). */
const size_t rtw_rust_hal_usb_off_adapter_registrypriv = offsetof(_adapter, registrypriv);
const size_t rtw_rust_hal_usb_off_registrypriv_recvbuf_nr =
	offsetof(struct registry_priv, recvbuf_nr);
const size_t rtw_rust_hal_usb_off_adapter_recvpriv = offsetof(_adapter, recvpriv);
const size_t rtw_rust_hal_usb_off_recvpriv_pallocated_recv_buf =
	offsetof(struct recv_priv, pallocated_recv_buf);
const size_t rtw_rust_hal_usb_off_recvpriv_precv_buf =
	offsetof(struct recv_priv, precv_buf);
#ifdef CONFIG_USB_INTERRUPT_IN_PIPE
const size_t rtw_rust_hal_usb_off_recvpriv_int_in_urb =
	offsetof(struct recv_priv, int_in_urb);
const size_t rtw_rust_hal_usb_off_recvpriv_int_in_buf =
	offsetof(struct recv_priv, int_in_buf);
#endif
const size_t rtw_rust_hal_usb_off_recvpriv_free_recv_skb_queue =
	offsetof(struct recv_priv, free_recv_skb_queue);
const size_t rtw_rust_hal_usb_off_recvpriv_rx_skb_queue =
	offsetof(struct recv_priv, rx_skb_queue);
const size_t rtw_rust_hal_usb_size_recv_buf = sizeof(struct recv_buf);

#ifdef CONFIG_FW_C2H_REG
const size_t rtw_rust_hal_usb_off_adapter_evtpriv = offsetof(_adapter, evtpriv);
const size_t rtw_rust_hal_usb_off_evtpriv_c2h_queue = offsetof(struct evt_priv, c2h_queue);
const size_t rtw_rust_hal_usb_off_evtpriv_c2h_wk = offsetof(struct evt_priv, c2h_wk);

void rtw_rust_hal_usb_err_c2h_cbuf_push_fail(void)
{
	RTW_ERR("usb_c2h_hisr_hdl rtw_cbuf_push fail\n");
}
#endif /* CONFIG_FW_C2H_REG */

u32 rtw_rust_hal_usb_skb_queue_len(struct sk_buff_head *list)
{
	return skb_queue_len(list);
}

struct sk_buff *rtw_rust_hal_usb_skb_dequeue(struct sk_buff_head *list)
{
	return skb_dequeue(list);
}

void rtw_rust_hal_usb_warn_rx_skb_queue_not_empty(void)
{
	RTW_WARN("rx_skb_queue not empty\n");
}

void rtw_rust_hal_usb_warn_free_recv_skb_queue_not_empty(u32 len)
{
	RTW_WARN("free_recv_skb_queue not empty, %u\n", len);
}

/* W4-45: helpers for rtw_rust_hal_usb_init_recv_priv_p1 in rust/hal_usb.rs */
void rtw_rust_hal_usb_recv_tasklets_init(_adapter *padapter)
{
	struct recv_priv *precvpriv = &padapter->recvpriv;

#ifdef PLATFORM_LINUX
	tasklet_init(&precvpriv->recv_tasklet,
		     usb_recv_tasklet,
		     (unsigned long)padapter);
#endif /* PLATFORM_LINUX */

#ifdef PLATFORM_FREEBSD
#ifdef CONFIG_RX_INDICATE_QUEUE
	TASK_INIT(&precvpriv->rx_indicate_tasklet, 0, rtw_rx_indicate_tasklet, padapter);
#endif /* CONFIG_RX_INDICATE_QUEUE */
#endif /* PLATFORM_FREEBSD */
}

void rtw_rust_hal_usb_recv_queues_init(_adapter *padapter)
{
	struct recv_priv *precvpriv = &padapter->recvpriv;

	_rtw_init_queue(&precvpriv->free_recv_buf_queue);
	_rtw_init_queue(&precvpriv->recv_buf_pending_queue);
#ifndef CONFIG_USE_USB_BUFFER_ALLOC_RX
	skb_queue_head_init(&precvpriv->free_recv_skb_queue);
#endif
}

int rtw_rust_hal_usb_int_in_pipe_init(_adapter *padapter, u16 ini_in_buf_sz)
{
	struct recv_priv *precvpriv = &padapter->recvpriv;

#ifdef CONFIG_USB_INTERRUPT_IN_PIPE
#ifdef PLATFORM_LINUX
	precvpriv->int_in_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (precvpriv->int_in_urb == NULL) {
		RTW_INFO("alloc_urb for interrupt in endpoint fail !!!!\n");
		return _FAIL;
	}
#endif /* PLATFORM_LINUX */
	precvpriv->int_in_buf = rtw_zmalloc(ini_in_buf_sz);
	if (precvpriv->int_in_buf == NULL) {
		RTW_INFO("alloc_mem for interrupt in endpoint fail !!!!\n");
		return _FAIL;
	}
#endif /* CONFIG_USB_INTERRUPT_IN_PIPE */

	return _SUCCESS;
}

void rtw_rust_hal_usb_int_in_pipe_undo(_adapter *padapter, u16 ini_in_buf_sz)
{
	struct recv_priv *precvpriv = &padapter->recvpriv;

#ifdef CONFIG_USB_INTERRUPT_IN_PIPE
#ifdef PLATFORM_LINUX
	if (precvpriv->int_in_urb) {
		usb_free_urb(precvpriv->int_in_urb);
		precvpriv->int_in_urb = NULL;
	}
#endif /* PLATFORM_LINUX */
	if (precvpriv->int_in_buf) {
		rtw_mfree(precvpriv->int_in_buf, ini_in_buf_sz);
		precvpriv->int_in_buf = NULL;
	}
#endif /* CONFIG_USB_INTERRUPT_IN_PIPE */
}

void rtw_rust_hal_usb_log_recvbuf_alloc(_adapter *padapter)
{
	struct registry_priv *regsty = adapter_to_regsty(padapter);

	RTW_INFO("NR_RECVBUFF: %d, recvbuf_nr: %d\n", NR_RECVBUFF, regsty->recvbuf_nr);
	RTW_INFO("MAX_RECVBUF_SZ: %d\n", MAX_RECVBUF_SZ);
}

/* W4-46: second half of usb_init_recv_priv (rust/hal_usb.rs usb_init_recv_priv). */
int rtw_rust_hal_usb_init_recv_priv_p2(_adapter *padapter)
{
	struct registry_priv *regsty = adapter_to_regsty(padapter);
	struct recv_priv *precvpriv = &padapter->recvpriv;
	int i, res = _SUCCESS;
	struct recv_buf *precvbuf;

	precvbuf = (struct recv_buf *)precvpriv->precv_buf;

	for (i = 0; i < regsty->recvbuf_nr; i++) {
		_rtw_init_listhead(&precvbuf->list);

#ifdef PLATFORM_WINDOWS
		_rtw_spinlock_init(&precvbuf->recvbuf_lock);
#endif

		precvbuf->alloc_sz = MAX_RECVBUF_SZ;

		res = rtw_os_recvbuf_resource_alloc(padapter, precvbuf, precvbuf->alloc_sz);
		if (res == _FAIL)
			break;

		precvbuf->ref_cnt = 0;
		precvbuf->adapter = padapter;

		precvbuf++;
	}

	precvpriv->free_recv_buf_queue_cnt = regsty->recvbuf_nr;

#if defined(PLATFORM_LINUX) || defined(PLATFORM_FREEBSD)

	skb_queue_head_init(&precvpriv->rx_skb_queue);

#ifdef CONFIG_RX_INDICATE_QUEUE
	memset(&precvpriv->rx_indicate_queue, 0, sizeof(struct ifqueue));
	mtx_init(&precvpriv->rx_indicate_queue.ifq_mtx, "rx_indicate_queue", NULL, MTX_DEF);
#endif /* CONFIG_RX_INDICATE_QUEUE */

#ifdef CONFIG_PREALLOC_RECV_SKB
	{
		int j;
		SIZE_PTR tmpaddr = 0;
		SIZE_PTR alignment = 0;
		struct sk_buff *pskb = NULL;

		RTW_INFO("NR_PREALLOC_RECV_SKB: %d\n", NR_PREALLOC_RECV_SKB);
#ifdef CONFIG_FIX_NR_BULKIN_BUFFER
		RTW_INFO("Enable CONFIG_FIX_NR_BULKIN_BUFFER\n");
#endif

		for (j = 0; j < NR_PREALLOC_RECV_SKB; j++) {
#ifdef CONFIG_PREALLOC_RX_SKB_BUFFER
			pskb = rtw_alloc_skb_premem(MAX_RECVBUF_SZ);
#else
			pskb = rtw_skb_alloc(MAX_RECVBUF_SZ + RECVBUFF_ALIGN_SZ);
#endif /* CONFIG_PREALLOC_RX_SKB_BUFFER */

			if (pskb) {
#ifdef PLATFORM_FREEBSD
				pskb->dev = padapter->pifp;
#else
				pskb->dev = padapter->pnetdev;
#endif /* PLATFORM_FREEBSD */

#ifndef CONFIG_PREALLOC_RX_SKB_BUFFER
				tmpaddr = (SIZE_PTR)pskb->data;
				alignment = tmpaddr & (RECVBUFF_ALIGN_SZ - 1);
				skb_reserve(pskb, (RECVBUFF_ALIGN_SZ - alignment));
#endif
				skb_queue_tail(&precvpriv->free_recv_skb_queue, pskb);
			}
		}
	}
#endif /* CONFIG_PREALLOC_RECV_SKB */

#endif /* defined(PLATFORM_LINUX) || defined(PLATFORM_FREEBSD) */

	return res;
}

/* usb_set_intf_ops — rust/hal_usb.rs (W4-25) */

#ifdef CONFIG_USB_SUPPORT_ASYNC_VDN_REQ
void *rtw_rust_hal_usb_pusbdev_from_intf(struct intf_hdl *pintfhdl)
{
	struct dvobj_priv *pdvobjpriv;

	if (!pintfhdl)
		return NULL;
	pdvobjpriv = (struct dvobj_priv *)pintfhdl->pintf_dev;
	if (!pdvobjpriv)
		return NULL;
	return pdvobjpriv->pusbdev;
}
#endif /* CONFIG_USB_SUPPORT_ASYNC_VDN_REQ */
