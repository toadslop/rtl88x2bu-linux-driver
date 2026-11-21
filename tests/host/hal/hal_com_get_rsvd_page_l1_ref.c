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

#if defined(CONFIG_RTW_DEBUG) || defined(CONFIG_PROC_DEBUG)
void rtw_dump_rsvd_page(void *sel, struct _adapter *adapter,
			unsigned char page_offset, unsigned char page_num)
{
	(void)sel;
	(void)adapter;
	(void)page_offset;
	(void)page_num;
}
#endif

#ifdef CONFIG_SUPPORT_FIFO_DUMP
void rtw_dump_fifo(void *sel, struct _adapter *adapter, unsigned char fifo_sel,
		   unsigned int fifo_addr, unsigned int fifo_size)
{
	(void)sel;
	(void)adapter;
	(void)fifo_sel;
	(void)fifo_addr;
	(void)fifo_size;
}
#endif