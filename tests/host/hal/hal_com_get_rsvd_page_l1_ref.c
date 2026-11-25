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

/* rtw_dump_fifo — PR4 / rust/hal_com_dump_rsvd_page.rs (leave out of this L1 leaf). */

typedef struct {
	unsigned char LocProbeRsp;
	unsigned char LocPsPoll;
	unsigned char LocNullData;
	unsigned char LocQosNull;
	unsigned char LocBTQosNull;
} RSVDPAGE_LOC;

void rtw_hal_set_FwRsvdPage_cmd(void *adapter, RSVDPAGE_LOC *rsvdpageloc)
{
	(void)adapter;
	(void)rsvdpageloc;
}