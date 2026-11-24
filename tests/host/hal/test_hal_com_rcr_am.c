// SPDX-License-Identifier: GPL-2.0
/* L2 vectors for W4-137 HW_VAR RCR_AM enable/disable delegate. */

#include "host_hal_com_rcr_am_types.h"
#include <stdio.h>

void host_hal_rcr_am_reset(void);
struct _adapter *host_hal_rcr_am_adapter(void);
extern struct host_hal_rcr_am_trace host_hal_rcr_am_trace;

u8 rtw_hal_set_hwreg(struct _adapter *adapter, u32 variable, u8 *val);

static int failures;

static void fail(const char *tag)
{
	fprintf(stderr, "FAIL %s\n", tag);
	failures++;
}

static void expect_u32(const char *tag, u32 a, u32 b)
{
	if (a != b)
		fail(tag);
}

static void expect_count(const char *tag, unsigned int a, unsigned int b)
{
	if (a != b)
		fail(tag);
}

int main(void)
{
	struct _adapter *a = host_hal_rcr_am_adapter();
	u8 dummy;

	host_hal_rcr_am_reset();
	if (rtw_hal_set_hwreg(a, HW_VAR_ON_RCR_AM, &dummy) != _SUCCESS)
		fail("on_ret");
	expect_count("on_add", host_hal_rcr_am_trace.add_count, 1u);
	expect_count("on_clear", host_hal_rcr_am_trace.clear_count, 0u);
	expect_u32("on_mask", host_hal_rcr_am_trace.last_add_mask, RCR_AM);

	host_hal_rcr_am_reset();
	if (rtw_hal_set_hwreg(a, HW_VAR_OFF_RCR_AM, &dummy) != _SUCCESS)
		fail("off_ret");
	expect_count("off_add", host_hal_rcr_am_trace.add_count, 0u);
	expect_count("off_clear", host_hal_rcr_am_trace.clear_count, 1u);
	expect_u32("off_mask", host_hal_rcr_am_trace.last_clear_mask, RCR_AM);

	/* Repeated enable must invoke add each time (no idempotency guard in C). */
	host_hal_rcr_am_reset();
	rtw_hal_set_hwreg(a, HW_VAR_ON_RCR_AM, &dummy);
	rtw_hal_set_hwreg(a, HW_VAR_ON_RCR_AM, &dummy);
	expect_count("twice_add", host_hal_rcr_am_trace.add_count, 2u);
	expect_u32("twice_mask", host_hal_rcr_am_trace.last_add_mask, RCR_AM);

	if (failures) {
		fprintf(stderr, "%d failure(s)\n", failures);
		return 1;
	}
	puts("OK hal_com_rcr_am L2");
	return 0;
}