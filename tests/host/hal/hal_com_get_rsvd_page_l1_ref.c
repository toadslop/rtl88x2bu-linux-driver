// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: rtw_hal_get_rsvd_page symbol surface (W4-121). */

struct _adapter;

int rtw_hal_get_rsvd_page(struct _adapter *adapter, unsigned int page_offset,
			  unsigned int page_num, unsigned char *buffer,
			  unsigned int buffer_size)
{
	(void)adapter;
	(void)page_offset;
	(void)page_num;
	(void)buffer;
	(void)buffer_size;
	return 0;
}