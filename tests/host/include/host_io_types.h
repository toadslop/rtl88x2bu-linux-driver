/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal types for host L2 rtw_io_rest tests (W3-18 PR1).
 */
#ifndef HOST_IO_TYPES_H
#define HOST_IO_TYPES_H

#include "host_types.h"
#include "host_autoconf.h"

#include <stdbool.h>
#include <stdio.h>

#define _TRUE 1
#define _FALSE 0

#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80
#define BIT(i) (1U << (i))

#define MAX_CONTINUAL_IO_ERR 4
#define MAX_CHIP_TYPE 255
#define MAX_RF_PATH 4
#define bRFRegOffsetMask 0xfffff

#define RTW_SDIO BIT0
#define RTW_USB BIT1
#define RTW_PCIE BIT2

typedef int ATOMIC_T;
#define ATOMIC_INC_RETURN(v) __sync_add_and_fetch((v), 1)
#define ATOMIC_SET(v, x) (*(v) = (x))

#define RTW_INFO(...) do { } while (0)
#define rtw_warn_on(c) do { (void)(c); } while (0)

struct dvobj_priv {
	u8 chip_type;
	u8 interface_type;
	ATOMIC_T continual_io_error;
};

struct _adapter {
	struct dvobj_priv *dvobj;
};

typedef struct _adapter _adapter;
typedef struct _adapter *PADAPTER;

#define rtw_get_chip_type(adapter) (((PADAPTER)(adapter))->dvobj->chip_type)
#define rtw_get_intf_type(adapter) (((PADAPTER)(adapter))->dvobj->interface_type)

static inline u32 bitshift(u32 bitmask)
{
	u32 i;

	for (i = 0; i <= 31; i++)
		if (((bitmask >> i) & 0x1) == 1)
			break;
	return i;
}

#endif /* HOST_IO_TYPES_H */
