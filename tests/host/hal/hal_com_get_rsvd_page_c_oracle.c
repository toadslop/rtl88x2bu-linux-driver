// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-121 rtw_hal_get_rsvd_page (HALMAC path). */

#include <stdio.h>
#include <string.h>

#include "host_hal_com_get_rsvd_page_types.h"

struct dvobj_priv *adapter_to_dvobj(struct _adapter *adapter);
void rtw_hal_get_def_var(struct _adapter *adapter, int def, u8 *val);
int rtw_halmac_dump_fifo(struct dvobj_priv *d, u8 fifo_sel, u32 addr,
			 u32 size, u8 *buffer);
#define RTW_ERR(fmt, ...) ((void)0)

int rtw_hal_get_rsvd_page_c_oracle(struct _adapter *adapter, u32 page_offset,
				   u32 page_num, u8 *buffer, u32 buffer_size)
{
	u32 page_size = 0;
	u32 addr;
	u32 size;
	int rst = _FALSE;

	rtw_hal_get_def_var(adapter, HAL_DEF_TX_PAGE_SIZE, (u8 *)&page_size);

	addr = page_offset * page_size;
	size = page_num * page_size;

	if (buffer_size < size) {
		RTW_ERR("%s buffer_size(%d) < get page total size(%d)\n",
			__func__, buffer_size, size);
		return rst;
	}

#ifdef RTW_HALMAC
	if (rtw_halmac_dump_fifo(adapter_to_dvobj(adapter), 2, addr, size,
				 buffer) < 0)
		rst = _FALSE;
	else
		rst = _TRUE;
#endif

	return rst;
}