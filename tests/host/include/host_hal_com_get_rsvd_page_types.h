/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_GET_RSVD_PAGE_TYPES_H
#define HOST_HAL_COM_GET_RSVD_PAGE_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;

#define _FALSE 0
#define _TRUE 1
#define RTW_HALMAC 1
#define HAL_DEF_TX_PAGE_SIZE 26

struct dvobj_priv {
	u8 pad;
};

struct _adapter {
	struct dvobj_priv *dvobj;
};

void host_get_rsvd_page_reset(void);
struct _adapter *host_get_rsvd_page_adapter(void);
void host_get_rsvd_page_set_page_size(u32 psz);
u32 host_get_rsvd_page_fixture_page_size(void);
u32 host_get_rsvd_last_fifo_sel(void);
u32 host_get_rsvd_last_addr(void);
u32 host_get_rsvd_last_size(void);
int host_get_rsvd_last_fifo_sel_is_rsvd(void);

#endif