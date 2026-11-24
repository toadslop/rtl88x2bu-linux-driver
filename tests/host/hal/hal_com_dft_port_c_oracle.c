// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-131 default port H2C (hal_com_rest.c layout). */

#include <string.h>
#include "host_hal_com_dft_port_types.h"

struct dvobj_priv;
struct _adapter;

extern u8 rtw_hal_get_port(struct _adapter *adapter);
extern s32 rtw_hal_fill_h2c_cmd(struct _adapter *adapter, u8 cmd, unsigned int len, u8 *buf);
extern struct dvobj_priv *rtw_rust_hal_com_adapter_to_dvobj(struct _adapter *adapter);
extern u8 rtw_rust_hal_com_dft_port_id(struct dvobj_priv *dvobj);
extern u8 rtw_rust_hal_com_dft_mac_id(struct dvobj_priv *dvobj);
extern void rtw_rust_hal_com_dft_set(struct dvobj_priv *dvobj, u8 port_id, u8 mac_id);
extern s32 rtw_rust_hal_com_is_client_associated_to_ap(struct _adapter *adapter);
extern u8 rtw_rust_hal_com_mlmepriv_is_ap(struct _adapter *adapter);
extern u8 rtw_rust_hal_com_assoc_sta_mac_id(struct _adapter *adapter, u8 *found);

s32 rtw_hal_set_default_port_id_cmd_c_oracle(struct _adapter *adapter, u8 mac_id)
{
	s32 ret = _SUCCESS;
	u8 parm[H2C_DEFAULT_PORT_ID_LEN] = {0};
	struct dvobj_priv *dvobj = rtw_rust_hal_com_adapter_to_dvobj(adapter);
	u8 port_id;

	port_id = rtw_hal_get_port(adapter);
	if ((rtw_rust_hal_com_dft_port_id(dvobj) == port_id) &&
	    (rtw_rust_hal_com_dft_mac_id(dvobj) == mac_id))
		return ret;

	parm[0] = port_id;
	parm[1] = mac_id;
	ret = rtw_hal_fill_h2c_cmd(adapter, H2C_DEFAULT_PORT_ID, H2C_DEFAULT_PORT_ID_LEN, parm);
	rtw_rust_hal_com_dft_set(dvobj, port_id, mac_id);
	return ret;
}

s32 rtw_set_default_port_id_c_oracle(struct _adapter *adapter)
{
	s32 ret = _SUCCESS;
	u8 found = 0;
	u8 mac_id;

	if (rtw_rust_hal_com_is_client_associated_to_ap(adapter)) {
		mac_id = rtw_rust_hal_com_assoc_sta_mac_id(adapter, &found);
		if (found)
			ret = rtw_hal_set_default_port_id_cmd_c_oracle(adapter, mac_id);
	} else if (rtw_rust_hal_com_mlmepriv_is_ap(adapter)) {
	} else {
	}

	return ret;
}