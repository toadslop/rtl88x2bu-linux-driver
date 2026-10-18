// SPDX-License-Identifier: GPL-2.0
#define _HAL_USB_REST_C_
#define HAL_USB_SET_INTF_OPS_IN_RUST 1
#define HAL_USB_VENDOR_WVALUE_IN_RUST 1
#define HAL_USB_SYNC_READ_IN_RUST 1
#define HAL_USB_SYNC_WRITE_IN_RUST 1
#define HAL_USB_ASYNC_WRITE_IN_RUST 1

#include "hal_usb.c"

/* usb_set_intf_ops — rust/hal_usb.rs (W4-25) */

#if CONFIG_USB_SUPPORT_ASYNC_VDN_REQ
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
