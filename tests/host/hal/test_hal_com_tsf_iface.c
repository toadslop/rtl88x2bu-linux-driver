// SPDX-License-Identifier: GPL-2.0
/* L2 differential tests for W4-129 TSF iface enable/disable. */

#include "host_hal_com_tsf_types.h"
#include <stdio.h>
#include <string.h>

void host_hal_com_tsf_reset(void);
struct _adapter *host_hal_com_tsf_adapter(void);
extern struct host_hal_com_tsf_trace host_hal_com_tsf_trace;

void rtw_iface_enable_tsf_update_c_oracle(struct _adapter *adapter);
void rtw_iface_disable_tsf_update_c_oracle(struct _adapter *adapter);

#ifdef RUST_HAL_COM_TSF_ORACLE
void rtw_iface_enable_tsf_update(struct _adapter *adapter);
void rtw_iface_disable_tsf_update(struct _adapter *adapter);
#endif

static int failures;

static void fail(const char *tag)
{
	fprintf(stderr, "FAIL %s\n", tag);
	failures++;
}

static void eq_u8(const char *tag, u8 got, u8 want)
{
	if (got != want)
		fail(tag);
}

static void eq_u32(const char *tag, u32 got, u32 want)
{
	if (got != want)
		fail(tag);
}

static void expect_trace(const char *tag, u8 want_count, u8 want_last_en)
{
	if (host_hal_com_tsf_trace.set_tsf_update_count != want_count ||
	    host_hal_com_tsf_trace.last_set_tsf_en != want_last_en)
		fail(tag);
}

static void expect_state(const char *tag, const struct _adapter *a, systime pause,
			 u8 required, u8 en_hw)
{
	eq_u32(tag, a->mlmeextpriv.tsf_update_pause_stime, pause);
	eq_u8(tag, a->mlmeextpriv.tsf_update_required, required);
	eq_u8(tag, a->mlmeextpriv.en_hw_update_tsf, en_hw);
}

static void run_enable_case(void (*under_test)(struct _adapter *),
			    void (*oracle)(struct _adapter *), u8 prime_en_hw)
{
	struct _adapter s, o;
	const char *tag;

	host_hal_com_tsf_reset();
	s = *host_hal_com_tsf_adapter();
	o = s;
	s.mlmeextpriv.tsf_update_pause_stime = 99;
	s.mlmeextpriv.en_hw_update_tsf = prime_en_hw;
	s.mlmeextpriv.tsf_update_required = 0;
	o = s;

	tag = prime_en_hw ? "enable_keep_en_hw" : "enable_from_zero_en_hw";
	memset(&host_hal_com_tsf_trace, 0, sizeof(host_hal_com_tsf_trace));
	under_test(&s);
#ifndef CONFIG_MI_WITH_MBSSID_CAM
	expect_trace(tag, 1, 1);
#else
	expect_trace(tag, 0, 0);
#endif
	expect_state(tag, &s, 0, 1, prime_en_hw);

	memset(&host_hal_com_tsf_trace, 0, sizeof(host_hal_com_tsf_trace));
	oracle(&o);
#ifndef CONFIG_MI_WITH_MBSSID_CAM
	expect_trace("enable_oracle", 1, 1);
#else
	expect_trace("enable_oracle", 0, 0);
#endif
	expect_state("enable_oracle", &o, 0, 1, prime_en_hw);

	if (memcmp(&s, &o, sizeof(s)) != 0)
		fail("enable_pair_memcmp");
}

static void run_disable_case(void (*under_test)(struct _adapter *),
			     void (*oracle)(struct _adapter *))
{
	struct _adapter s, o;

	host_hal_com_tsf_reset();
	s = *host_hal_com_tsf_adapter();
	o = s;
	s.mlmeextpriv.tsf_update_pause_stime = 99;
	s.mlmeextpriv.en_hw_update_tsf = 1;
	s.mlmeextpriv.tsf_update_required = 1;
	o = s;

	memset(&host_hal_com_tsf_trace, 0, sizeof(host_hal_com_tsf_trace));
	under_test(&s);
#ifndef CONFIG_MI_WITH_MBSSID_CAM
	expect_trace("disable", 1, 0);
#else
	expect_trace("disable", 0, 0);
#endif
	expect_state("disable", &s, 0, 0, 0);

	memset(&host_hal_com_tsf_trace, 0, sizeof(host_hal_com_tsf_trace));
	oracle(&o);
#ifndef CONFIG_MI_WITH_MBSSID_CAM
	expect_trace("disable_oracle", 1, 0);
#else
	expect_trace("disable_oracle", 0, 0);
#endif
	expect_state("disable_oracle", &o, 0, 0, 0);

	if (memcmp(&s, &o, sizeof(s)) != 0)
		fail("disable_pair_memcmp");
}

int main(void)
{
	run_enable_case(rtw_iface_enable_tsf_update, rtw_iface_enable_tsf_update_c_oracle, 1);
	run_enable_case(rtw_iface_enable_tsf_update, rtw_iface_enable_tsf_update_c_oracle, 0);
	run_disable_case(rtw_iface_disable_tsf_update, rtw_iface_disable_tsf_update_c_oracle);

	if (failures)
		return 1;
	puts("OK hal_com_tsf_iface L2");
	return 0;
}