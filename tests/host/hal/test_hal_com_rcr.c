// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-127 rtw_hal_rcr_check/add/clear. */

#include "host_hal_com_rcr_types.h"
#include <stdio.h>

void host_hal_rcr_reset(void);
struct _adapter *host_hal_rcr_adapter(void);
void host_hal_rcr_set_cached(u32 rcr);
extern struct host_hal_rcr_trace host_hal_rcr_trace;

u8 rtw_hal_rcr_check(struct _adapter *adapter, u32 check_bit);
u8 rtw_hal_rcr_add(struct _adapter *adapter, u32 add);
u8 rtw_hal_rcr_clear(struct _adapter *adapter, u32 clear);
u8 rtw_hal_rcr_check_c_oracle(struct _adapter *adapter, u32 check_bit);
u8 rtw_hal_rcr_add_c_oracle(struct _adapter *adapter, u32 add);
u8 rtw_hal_rcr_clear_c_oracle(struct _adapter *adapter, u32 clear);

static int failures;

static void fail(const char *tag)
{
	fprintf(stderr, "FAIL %s\n", tag);
	failures++;
}

static void expect_u8(const char *tag, u8 a, u8 b)
{
	if (a != b)
		fail(tag);
}

static void expect_u32(const char *tag, u32 a, u32 b)
{
	if (a != b)
		fail(tag);
}

int main(void)
{
	struct _adapter *a = host_hal_rcr_adapter();

	host_hal_rcr_reset();
	host_hal_rcr_set_cached(0x0F00u);
	expect_u8("check", rtw_hal_rcr_check(a, 0x0F00u), 1);
	expect_u8("check0", rtw_hal_rcr_check(a, 1u), 0);

	host_hal_rcr_reset();
	host_hal_rcr_set_cached(0x10u);
	host_hal_rcr_trace.set_count = 0;
	expect_u8("add", rtw_hal_rcr_add(a, 0x20u), _SUCCESS);
	expect_u32("add_set", host_hal_rcr_trace.last_set_rcr, 0x30u);

	host_hal_rcr_reset();
	host_hal_rcr_set_cached(0xF0u);
	host_hal_rcr_trace.set_count = 0;
	expect_u8("clear", rtw_hal_rcr_clear(a, 0x30u), _SUCCESS);
	expect_u32("clr_set", host_hal_rcr_trace.last_set_rcr, 0xC0u);

#ifdef RUST_HAL_COM_RCR_ORACLE
	host_hal_rcr_reset();
	host_hal_rcr_set_cached(0x55u);
	expect_u8("add_eq", rtw_hal_rcr_add(a, 0x2u), rtw_hal_rcr_add_c_oracle(a, 0x2u));
	host_hal_rcr_reset();
	host_hal_rcr_set_cached(0x55u);
	expect_u8("clr_eq", rtw_hal_rcr_clear(a, 5u), rtw_hal_rcr_clear_c_oracle(a, 5u));
#endif

	if (failures)
		return 1;
	puts("OK hal_com_rcr L2");
	return 0;
}