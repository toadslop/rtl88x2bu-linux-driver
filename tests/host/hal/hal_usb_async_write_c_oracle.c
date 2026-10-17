// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-37 async usb_write* (extracted from hal/hal_hci/hal_usb.c). */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct usb_device;
struct dvobj_priv {
	struct usb_device *pusbdev;
};
struct intf_hdl {
	void *pintf_dev;
};

#define VENDOR_WRITE 0x00
#define REALTEK_USB_VENQT_CMD_REQ 0x05
#define REALTEK_USB_VENQT_CMD_IDX 0x00

int _usbctrl_vendorreq_async_write(struct usb_device *udev, u8 request,
				   u16 value, u16 index, void *pdata, u16 len,
				   u8 requesttype);

int usb_write_async(struct usb_device *udev, u32 addr, void *pdata, u16 len)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	int ret;

	requesttype = VENDOR_WRITE;
	request = REALTEK_USB_VENQT_CMD_REQ;
	index = REALTEK_USB_VENQT_CMD_IDX;

	wvalue = (u16)(addr & 0x0000ffff);

	ret = _usbctrl_vendorreq_async_write(udev, request, wvalue, index, pdata,
					     len, requesttype);

	return ret;
}

int usb_async_write8(struct intf_hdl *pintfhdl, u32 addr, u8 val)
{
	u8 data;
	int ret;
	struct dvobj_priv *pdvobjpriv = (struct dvobj_priv *)pintfhdl->pintf_dev;
	struct usb_device *udev = pdvobjpriv->pusbdev;

	data = val;
	ret = usb_write_async(udev, addr, &data, 1);

	return ret;
}

int usb_async_write16(struct intf_hdl *pintfhdl, u32 addr, u16 val)
{
	u16 data;
	int ret;
	struct dvobj_priv *pdvobjpriv = (struct dvobj_priv *)pintfhdl->pintf_dev;
	struct usb_device *udev = pdvobjpriv->pusbdev;

	data = val;
	ret = usb_write_async(udev, addr, &data, 2);

	return ret;
}

int usb_async_write32(struct intf_hdl *pintfhdl, u32 addr, u32 val)
{
	u32 data;
	int ret;
	struct dvobj_priv *pdvobjpriv = (struct dvobj_priv *)pintfhdl->pintf_dev;
	struct usb_device *udev = pdvobjpriv->pusbdev;

	data = val;
	ret = usb_write_async(udev, addr, &data, 4);

	return ret;
}
