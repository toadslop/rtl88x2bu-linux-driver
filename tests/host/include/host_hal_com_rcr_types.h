/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_RCR_TYPES_H
#define HOST_HAL_COM_RCR_TYPES_H

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1

typedef unsigned char u8;
typedef unsigned int u32;

/* HW_VAR_RCR in hal_intf.h (_HW_VARIABLES). */
#define HW_VAR_RCR 8

struct _adapter;
struct hal_data;

struct hal_data *host_hal_rcr_hal_data(struct _adapter *adapter);
u32 host_hal_rcr_receive_config(struct _adapter *adapter);

#define GET_HAL_DATA(adapter) (host_hal_rcr_hal_data(adapter))

struct host_hal_rcr_trace {
	unsigned int get_count;
	unsigned int set_count;
	u32 last_get_rcr;
	u32 last_set_rcr;
};

#endif