// SPDX-License-Identifier: GPL-2.0
#define _HAL_USB_REST_C_
#define HAL_USB_SET_INTF_OPS_IN_RUST 1
#define HAL_USB_VENDOR_WVALUE_IN_RUST 1
#define HAL_USB_SYNC_READ_IN_RUST 1
#define HAL_USB_SYNC_WRITE_IN_RUST 1
#define HAL_USB_ASYNC_WRITE_IN_RUST 1
#define HAL_USB_FREE_RECV_PRIV_IN_RUST 1

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
