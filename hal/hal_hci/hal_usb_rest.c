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
