// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-121 rtw_hal_get_rsvd_page. */

#include <stdio.h>
#include <string.h>

#include "host_hal_com_get_rsvd_page_types.h"

int rtw_hal_get_rsvd_page_c_oracle(struct _adapter *adapter, u32 page_offset,
				   u32 page_num, u8 *buffer, u32 buffer_size);

#ifdef RUST_HAL_COM_GET_RSVD_PAGE_ORACLE
int rtw_hal_get_rsvd_page(struct _adapter *adapter, u32 page_offset,
			  u32 page_num, u8 *buffer, u32 buffer_size);
#else
#define rtw_hal_get_rsvd_page rtw_hal_get_rsvd_page_c_oracle
#endif

static int failures;

static void eq_i(const char *tag, int a, int b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s a=%d b=%d\n", tag, a, b);
		failures++;
	}
}

static void eq_u32(const char *tag, u32 a, u32 b)
{
	if (a != b) {
		fprintf(stderr, "FAIL %s a=%u b=%u\n", tag, a, b);
		failures++;
	}
}

static void buf_eq(const char *tag, u8 *a, u8 *b, u32 n)
{
	if (memcmp(a, b, n) != 0) {
		fprintf(stderr, "FAIL %s buffer mismatch\n", tag);
		failures++;
	}
}

int main(void)
{
	u8 buf[512];
	u8 ref[512];
	struct _adapter *adap;
	u32 psz;
	int ret;
	u32 want_size;

	host_get_rsvd_page_reset();
	adap = host_get_rsvd_page_adapter();
	host_get_rsvd_page_set_page_size(128);

	memset(buf, 0, sizeof(buf));
	memset(ref, 0, sizeof(ref));
	ret = rtw_hal_get_rsvd_page(adap, 2, 1, buf, 128);
	eq_i("single_page_ok", ret, rtw_hal_get_rsvd_page_c_oracle(adap, 2, 1, ref, 128));
	eq_u32("fifo_rsvd", host_get_rsvd_last_fifo_sel_is_rsvd(), 1);
	psz = host_get_rsvd_page_fixture_page_size();
	want_size = 1 * psz;
	eq_u32("dump_size", host_get_rsvd_last_size(), want_size);
	eq_u32("dump_addr", host_get_rsvd_last_addr(), 2 * psz);
	buf_eq("fill", buf, ref, want_size);

	ret = rtw_hal_get_rsvd_page(adap, 0, 2, buf, 64);
	eq_i("buf_too_small", ret, _FALSE);

	host_get_rsvd_page_set_page_size(256);
	memset(buf, 0, sizeof(buf));
	memset(ref, 0, sizeof(ref));
	ret = rtw_hal_get_rsvd_page(adap, 1, 2, buf, 512);
	eq_i("two_pages", ret, rtw_hal_get_rsvd_page_c_oracle(adap, 1, 2, ref, 512));
	eq_u32("dump_addr2", host_get_rsvd_last_addr(), 256);

	if (failures)
		return 1;
	puts("OK hal_com_get_rsvd_page L2");
	return 0;
}