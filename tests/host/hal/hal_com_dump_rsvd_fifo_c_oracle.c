// SPDX-License-Identifier: GPL-2.0
#include <stdarg.h>
#include <stdio.h>

#include "host_hal_com_get_rsvd_page_types.h"

#define CONFIG_RTW_DEBUG 1

#define RTW_BUFDUMP_BSIZE 16

struct dvobj_priv *adapter_to_dvobj(struct _adapter *adapter);
void rtw_hal_get_def_var(struct _adapter *adapter, int def, u8 *val);
int rtw_hal_get_rsvd_page(struct _adapter *adapter, u32 page_offset, u32 page_num,
			  u8 *buffer, u32 buffer_size);
void *rtw_zvmalloc(u32 sz);
void rtw_vmfree(void *p, u32 sz);

static void print_sel(void *sel, const char *fmt, ...)
{
	struct host_sel_capture *cap = (struct host_sel_capture *)sel;
	va_list ap;
	int n;
	size_t room;

	if (cap->len >= sizeof(cap->buf) - 1) {
		host_sel_mark_overflow();
		return;
	}
	room = sizeof(cap->buf) - cap->len - 1;
	va_start(ap, fmt);
	n = vsnprintf(cap->buf + cap->len, room + 1, fmt, ap);
	va_end(ap);
	if (n < 0)
		return;
	if ((size_t)n > room) {
		cap->len = sizeof(cap->buf) - 1;
		cap->buf[cap->len] = '\0';
		host_sel_mark_overflow();
		return;
	}
	if (n > 0)
		cap->len += (size_t)n;
}

#define RTW_PRINT_SEL(sel, fmt, ...) print_sel(sel, fmt, ##__VA_ARGS__)

/* Match RTW_BUF_DUMP_SEL (_DRV_ALWAYS_ / no title / no index) in rtw_debug_rest.c */
static void host_rtw_dump_sel(void *sel, const u8 *hexdata, int hexdatalen)
{
	int i;

	for (i = 0; i < hexdatalen; i++) {
		RTW_PRINT_SEL(sel, "%02X%s", hexdata[i],
			      (((i + 1) % 4) == 0) ? "  " : " ");
		if ((i + 1 < hexdatalen) &&
		    ((i + 1) % RTW_BUFDUMP_BSIZE) == 0)
			RTW_PRINT_SEL(sel, "\n");
	}
	RTW_PRINT_SEL(sel, "\n");
}

void rtw_dump_rsvd_page_c_oracle(void *sel, struct _adapter *adapter,
				 u8 page_offset, u8 page_num)
{
	u32 page_size, buf_size;
	u8 *buffer;

	if (page_num == 0)
		return;

	RTW_PRINT_SEL(sel, "======= RSVD PAGE DUMP =======\n");
	RTW_PRINT_SEL(sel, "page_offset:%d, page_num:%d\n", page_offset, page_num);
	rtw_hal_get_def_var(adapter, HAL_DEF_TX_PAGE_SIZE, (u8 *)&page_size);
	if (!page_size) {
		RTW_PRINT_SEL(sel, "ERROR - Tx page size is zero ??\n");
		goto done;
	}
	buf_size = page_size * page_num;
	buffer = rtw_zvmalloc(buf_size);
	if (!buffer) {
		RTW_PRINT_SEL(sel, "ERROR - rsvd_buf mem allocate failed\n");
		goto done;
	}
	rtw_hal_get_rsvd_page(adapter, page_offset, page_num, buffer, buf_size);
	host_rtw_dump_sel(sel, buffer, (int)buf_size);
	rtw_vmfree(buffer, buf_size);
done:
	RTW_PRINT_SEL(sel, "==========================\n");
}