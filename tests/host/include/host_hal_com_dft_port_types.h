/* SPDX-License-Identifier: GPL-2.0 */
/* Host types for W4-131 default port H2C L2 harness. */

#ifndef HOST_HAL_COM_DFT_PORT_TYPES_H
#define HOST_HAL_COM_DFT_PORT_TYPES_H

#include <stddef.h>

typedef unsigned char u8;
typedef int s32;

struct _adapter;
struct dvobj_priv;

#define H2C_DEFAULT_PORT_ID 0x2c
#define H2C_DEFAULT_PORT_ID_LEN 2
#define _SUCCESS 1

s32 rtw_hal_set_default_port_id_cmd(struct _adapter *adapter, u8 mac_id);
s32 rtw_set_default_port_id(struct _adapter *adapter);

#endif