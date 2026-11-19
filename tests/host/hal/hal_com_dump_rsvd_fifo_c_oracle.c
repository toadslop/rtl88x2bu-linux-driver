// SPDX-License-Identifier: GPL-2.0
#include <stdarg.h>
#include <stdio.h>

#include "host_hal_com_get_rsvd_page_types.h"

#define CONFIG_RTW_DEBUG 1

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

	va_start(ap, fmt);
	n = vsnprintf(cap->buf + cap->len,
		      sizeof(cap->buf) - cap->len - 1, fmt, ap);
	va_end(ap);
	if (n > 0)
		cap->len += (size_t)n;
}

#define RTW_PRINT_SEL(sel, fmt, ...) print_sel(sel, fmt, ##__VA_ARGS__)
#define RTW_DUMP_SEL(sel, d, n)                                        \
	do {                                                           \
		u32 _i;                                                \
		for (_i = 0; _i < (n); _i++) {                         \
			RTW_PRINT_SEL(sel, "%02x ", (d)[_i]);          \
			if ((_i & 15) == 15)                           \
				RTW_PRINT_SEL(sel, "\n");              \
		}                                                      \
		if ((n) & 15)                                          \
			RTW_PRINT_SEL(sel, "\n");                      \
	} while (0)

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
	RTW_DUMP_SEL(sel, buffer, buf_size);
	rtw_vmfree(buffer, buf_size);
done:
	RTW_PRINT_SEL(sel, "==========================\n");
}