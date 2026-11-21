// SPDX-License-Identifier: GPL-2.0
/* Link rtw_hal_get_rsvd_page to the C oracle for dump harnesses only. */

#include <stddef.h>

#include "host_hal_com_get_rsvd_page_types.h"

int rtw_hal_get_rsvd_page_c_oracle(struct _adapter *adapter, u32 page_offset,
				   u32 page_num, u8 *buffer, u32 buffer_size);

int rtw_hal_get_rsvd_page(struct _adapter *adapter, u32 page_offset,
			  u32 page_num, u8 *buffer, u32 buffer_size)
{
	return rtw_hal_get_rsvd_page_c_oracle(adapter, page_offset, page_num,
					      buffer, buffer_size);
}