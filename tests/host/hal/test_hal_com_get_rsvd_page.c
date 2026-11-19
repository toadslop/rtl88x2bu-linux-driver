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

static void run_halmac_case(struct _adapter *adap, u32 page_offset, u32 page_num,
			    u8 *buf, u32 buf_sz, u8 *ref, const char *tag)
{
	int ret_subj;
	int ret_orc;
	u32 fifo_subj;
	u32 size_subj;
	u32 addr_subj;
	u32 fifo_orc;
	u32 size_orc;
	u32 addr_orc;

	memset(buf, 0, buf_sz);
	memset(ref, 0, buf_sz);
	ret_subj = rtw_hal_get_rsvd_page(adap, page_offset, page_num, buf, buf_sz);
	fifo_subj = host_get_rsvd_last_fifo_sel_is_rsvd();
	size_subj = host_get_rsvd_last_size();
	addr_subj = host_get_rsvd_last_addr();

	ret_orc = rtw_hal_get_rsvd_page_c_oracle(adap, page_offset, page_num, ref, buf_sz);
	fifo_orc = host_get_rsvd_last_fifo_sel_is_rsvd();
	size_orc = host_get_rsvd_last_size();
	addr_orc = host_get_rsvd_last_addr();

	eq_i(tag, ret_subj, ret_orc);
	eq_u32("fifo_rsvd", fifo_subj, 1);
	eq_u32("fifo", fifo_subj, fifo_orc);
	eq_u32("dump_size", size_subj, size_orc);
	eq_u32("dump_addr", addr_subj, addr_orc);
	if (ret_subj == _TRUE)
		buf_eq(tag, buf, ref, size_subj);
}

int main(void)
{
	u8 buf[512];
	u8 ref[512];
	struct _adapter *adap;
	int ret;

	host_get_rsvd_page_reset();
	adap = host_get_rsvd_page_adapter();
	host_get_rsvd_page_set_page_size(128);

	run_halmac_case(adap, 2, 1, buf, 128, ref, "single_page_ok");

	ret = rtw_hal_get_rsvd_page(adap, 0, 2, buf, 64);
	eq_i("buf_too_small", ret, _FALSE);

	host_get_rsvd_page_set_page_size(256);
	run_halmac_case(adap, 1, 2, buf, 512, ref, "two_pages");

	if (failures)
		return 1;
	puts("OK hal_com_get_rsvd_page L2");
	return 0;
}